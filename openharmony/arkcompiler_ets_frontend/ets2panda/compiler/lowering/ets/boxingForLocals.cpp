/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "boxingForLocals.h"

#include "varbinder/ETSBinder.h"
#include "checker/ETSchecker.h"

namespace ark::es2panda::compiler {

static constexpr std::string_view LOWERING_NAME = "boxing-for-locals";

static ArenaSet<varbinder::Variable *> FindCaptured(public_lib::Context *ctx, ir::ScriptFunction *func)
{
    auto *allocator = ctx->allocator;
    auto captured = ArenaSet<varbinder::Variable *>(allocator->Adapter());
    bool withinLambda = false;

    auto innermostArrowScopes = ArenaSet<varbinder::Scope *>(allocator->Adapter());
    innermostArrowScopes.insert(func->Scope());
    innermostArrowScopes.insert(func->Scope()->ParamScope());

    std::function<void(ir::AstNode *)> walker = [&](ir::AstNode *ast) {
        if (ast->IsArrowFunctionExpression() || ast->IsClassDeclaration()) {
            auto savedWL = withinLambda;
            auto savedScopes = ArenaSet<varbinder::Scope *>(allocator->Adapter());
            std::swap(innermostArrowScopes, savedScopes);
            withinLambda = true;

            ast->Iterate(walker);

            withinLambda = savedWL;
            std::swap(innermostArrowScopes, savedScopes);

            return;
        }

        if (withinLambda && ast->IsScopeBearer()) {
            innermostArrowScopes.insert(ast->Scope());
            if (ast->Scope()->IsFunctionScope()) {
                innermostArrowScopes.insert(ast->Scope()->AsFunctionScope()->ParamScope());
            }
            if (ast->Scope()->IsCatchScope()) {
                innermostArrowScopes.insert(ast->Scope()->AsCatchScope()->ParamScope());
            }
            if (ast->Scope()->IsLoopScope()) {
                innermostArrowScopes.insert(ast->Scope()->AsLoopScope()->DeclScope());
            }
        } else if (withinLambda && ast->IsIdentifier()) {
            auto *var = ast->AsIdentifier()->Variable();
            if (var == nullptr) {
                return;
            }
            auto *scope = var->GetScope();
            if (scope != nullptr && !scope->IsClassScope() && !scope->IsGlobalScope() &&
                innermostArrowScopes.count(scope) == 0) {
                captured.insert(var);
            }
        }

        ast->Iterate(walker);
    };

    func->Iterate(walker);

    auto varsToBox = ArenaSet<varbinder::Variable *>(allocator->Adapter());
    return captured;
}

static ArenaSet<varbinder::Variable *> FindModified(public_lib::Context *ctx, ir::ScriptFunction *func)
{
    auto *allocator = ctx->allocator;
    auto modified = ArenaSet<varbinder::Variable *>(allocator->Adapter());

    std::function<void(ir::AstNode *)> walker = [&](ir::AstNode *ast) {
        if (ast->IsAssignmentExpression()) {
            auto assignment = ast->AsAssignmentExpression();
            if (assignment->Left()->IsIdentifier()) {
                ASSERT(assignment->Left()->Variable() != nullptr);
                auto *var = assignment->Left()->Variable();
                var->AddFlag(varbinder::VariableFlags::INITIALIZED);
                modified.insert(var);
            }
        }
    };

    func->IterateRecursively(walker);
    return modified;
}

static ArenaSet<varbinder::Variable *> FindVariablesToBox(public_lib::Context *ctx, ir::ScriptFunction *func)
{
    auto *allocator = ctx->allocator;
    auto captured = FindCaptured(ctx, func);
    auto modified = FindModified(ctx, func);

    auto varsToBox = ArenaSet<varbinder::Variable *>(allocator->Adapter());
    std::set_intersection(captured.cbegin(), captured.cend(), modified.cbegin(), modified.cend(),
                          std::inserter(varsToBox, varsToBox.begin()));

    return varsToBox;
}

static void HandleFunctionParam(public_lib::Context *ctx, ir::ETSParameterExpression *param,
                                ArenaMap<varbinder::Variable *, varbinder::Variable *> *varsMap)
{
    auto *allocator = ctx->allocator;
    auto *checker = ctx->checker->AsETSChecker();
    auto *varBinder = checker->VarBinder();

    auto *id = param->Ident()->AsIdentifier();
    auto *oldVar = id->Variable();
    auto *oldType = oldVar->TsType();
    auto *func = param->Parent()->AsScriptFunction();
    ASSERT(func->Body()->IsBlockStatement());  // guaranteed after expressionLambdaLowering
    auto *body = func->Body()->AsBlockStatement();
    auto &bodyStmts = body->Statements();
    auto *scope = body->Scope();

    auto *initId = allocator->New<ir::Identifier>(id->Name(), allocator);
    initId->SetVariable(id->Variable());
    initId->SetTsType(oldType);

    // The new variable will have the same name as the parameter. This is not representable in source code.
    auto *boxedType = checker->GlobalBuiltinBoxType(oldType);
    ArenaVector<ir::Expression *> newInitArgs {allocator->Adapter()};
    newInitArgs.push_back(initId);
    auto *newInit = util::NodeAllocator::ForceSetParent<ir::ETSNewClassInstanceExpression>(
        allocator, allocator->New<ir::OpaqueTypeNode>(boxedType), std::move(newInitArgs), nullptr);
    auto *newDeclarator = util::NodeAllocator::ForceSetParent<ir::VariableDeclarator>(
        allocator, ir::VariableDeclaratorFlag::CONST, allocator->New<ir::Identifier>(id->Name(), allocator), newInit);
    ArenaVector<ir::VariableDeclarator *> declVec {allocator->Adapter()};
    declVec.push_back(newDeclarator);

    auto *newDecl = allocator->New<varbinder::ConstDecl>(id->Name(), newDeclarator);
    auto *newVar = allocator->New<varbinder::LocalVariable>(newDecl, oldVar->Flags());
    newVar->SetTsType(boxedType);

    newDeclarator->Id()->AsIdentifier()->SetVariable(newVar);
    newVar->AddFlag(varbinder::VariableFlags::INITIALIZED);
    newVar->SetScope(scope);
    scope->EraseBinding(newVar->Name());
    scope->InsertBinding(newVar->Name(), newVar);

    auto *newDeclaration = util::NodeAllocator::ForceSetParent<ir::VariableDeclaration>(
        allocator, ir::VariableDeclaration::VariableDeclarationKind::CONST, allocator, std::move(declVec));
    newDeclaration->SetParent(body);
    bodyStmts.insert(bodyStmts.begin(), newDeclaration);

    auto lexScope = varbinder::LexicalScope<varbinder::Scope>::Enter(varBinder, scope);
    auto savedContext = checker::SavedCheckerContext(checker, checker::CheckerStatus::NO_OPTS);
    auto scopeContext = checker::ScopeContext(checker, scope);

    newDeclaration->Check(checker);

    varsMap->emplace(oldVar, newVar);
}

static ir::AstNode *HandleVariableDeclarator(public_lib::Context *ctx, ir::VariableDeclarator *declarator,
                                             ArenaMap<varbinder::Variable *, varbinder::Variable *> *varsMap)
{
    auto *allocator = ctx->allocator;
    auto *checker = ctx->checker->AsETSChecker();
    auto *varBinder = checker->VarBinder();

    auto *id = declarator->Id()->AsIdentifier();
    auto *oldVar = id->Variable();
    auto *scope = oldVar->GetScope();
    auto *type = oldVar->TsType();
    auto *boxedType = checker->GlobalBuiltinBoxType(type);

    auto initArgs = ArenaVector<ir::Expression *>(allocator->Adapter());
    if (declarator->Init() != nullptr) {
        auto *arg = declarator->Init();
        if (arg->TsType() != type) {
            arg = util::NodeAllocator::ForceSetParent<ir::TSAsExpression>(
                allocator, arg, allocator->New<ir::OpaqueTypeNode>(type), false);
        }
        initArgs.push_back(arg);
    }
    auto *newInit = util::NodeAllocator::ForceSetParent<ir::ETSNewClassInstanceExpression>(
        allocator, allocator->New<ir::OpaqueTypeNode>(boxedType), std::move(initArgs), nullptr);
    auto *newDeclarator = util::NodeAllocator::ForceSetParent<ir::VariableDeclarator>(
        allocator, declarator->Flag(), allocator->New<ir::Identifier>(id->Name(), allocator), newInit);
    newDeclarator->SetParent(declarator->Parent());

    auto *newDecl = allocator->New<varbinder::ConstDecl>(oldVar->Name(), newDeclarator);
    auto *newVar = allocator->New<varbinder::LocalVariable>(newDecl, oldVar->Flags());
    newDeclarator->Id()->AsIdentifier()->SetVariable(newVar);
    newVar->AddFlag(varbinder::VariableFlags::INITIALIZED);
    newVar->SetScope(scope);

    scope->EraseBinding(oldVar->Name());
    scope->InsertBinding(newVar->Name(), newVar);

    auto lexScope = varbinder::LexicalScope<varbinder::Scope>::Enter(varBinder, scope);
    auto savedContext = checker::SavedCheckerContext(checker, checker::CheckerStatus::NO_OPTS);
    auto scopeContext = checker::ScopeContext(checker, scope);

    newDeclarator->Check(checker);

    varsMap->emplace(oldVar, newVar);

    return newDeclarator;
}

static bool IsBeingDeclared(ir::AstNode *ast)
{
    ASSERT(ast->IsIdentifier());
    return (ast->Parent()->IsVariableDeclarator() && ast == ast->Parent()->AsVariableDeclarator()->Id()) ||
           (ast->Parent()->IsETSParameterExpression() && ast == ast->Parent()->AsETSParameterExpression()->Ident());
}

static bool IsPartOfBoxInitializer(public_lib::Context *ctx, ir::AstNode *ast)
{
    ASSERT(ast->IsIdentifier());
    auto *checker = ctx->checker->AsETSChecker();
    auto *id = ast->AsIdentifier();

    // NOTE(gogabr): rely on caching for type instantiations, so we can use pointer comparison.
    return id->Parent()->IsETSNewClassInstanceExpression() &&
           id->Parent()->AsETSNewClassInstanceExpression()->GetTypeRef()->TsType() ==
               checker->GlobalBuiltinBoxType(id->TsType());
}

static bool OnLeftSideOfAssignment(ir::AstNode *ast)
{
    return ast->Parent()->IsAssignmentExpression() && ast->Parent()->AsAssignmentExpression()->Left() == ast;
}

static ir::AstNode *HandleReference(public_lib::Context *ctx, ir::Identifier *id, varbinder::Variable *var)
{
    auto *parser = ctx->parser->AsETSParser();
    auto *checker = ctx->checker->AsETSChecker();

    // `as` is needed to account for smart types
    auto *res = parser->CreateFormattedExpression("@@I1.get() as @@T2", var->Name(), id->TsType());
    res->SetParent(id->Parent());
    res->AsTSAsExpression()
        ->Expr()
        ->AsCallExpression()
        ->Callee()
        ->AsMemberExpression()
        ->Object()
        ->AsIdentifier()
        ->SetVariable(var);

    // NOTE(gogabr) -- The `get` property remains without variable; this is OK for the current checker, but may need
    // adjustment later.
    res->Check(checker);

    ASSERT(res->TsType() == id->TsType());
    res->SetBoxingUnboxingFlags(id->GetBoxingUnboxingFlags());

    return res;
}

static ir::AstNode *HandleAssignment(public_lib::Context *ctx, ir::AssignmentExpression *ass,
                                     ArenaMap<varbinder::Variable *, varbinder::Variable *> const &varsMap)
{
    // Should be true after opAssignment lowering
    ASSERT(ass->OperatorType() == lexer::TokenType::PUNCTUATOR_SUBSTITUTION);

    auto *parser = ctx->parser->AsETSParser();
    auto *varBinder = ctx->checker->VarBinder()->AsETSBinder();
    auto *checker = ctx->checker->AsETSChecker();

    auto *oldVar = ass->Left()->AsIdentifier()->Variable();
    auto *newVar = varsMap.find(oldVar)->second;
    auto *scope = newVar->GetScope();
    newVar->AddFlag(varbinder::VariableFlags::INITIALIZED);

    auto *res = parser->CreateFormattedExpression("@@I1.set(@@E2 as @@T3) as @@T4", newVar->Name(), ass->Right(),
                                                  oldVar->TsType(), ass->TsType());
    res->SetParent(ass->Parent());

    // NOTE(gogabr) -- The `get` and `set` properties remain without variable; this is OK for the current checker, but
    // may need adjustment later.
    auto lexScope = varbinder::LexicalScope<varbinder::Scope>::Enter(varBinder, scope);
    auto savedContext = checker::SavedCheckerContext(checker, checker::CheckerStatus::NO_OPTS);
    auto scopeContext = checker::ScopeContext(checker, scope);

    varBinder->ResolveReferencesForScopeWithContext(res, scope);
    res->Check(checker);

    ASSERT(res->TsType() == ass->TsType());
    res->SetBoxingUnboxingFlags(ass->GetBoxingUnboxingFlags());

    return res;
}

static void HandleScriptFunction(public_lib::Context *ctx, ir::ScriptFunction *func)
{
    auto *allocator = ctx->allocator;
    auto varsToBox = FindVariablesToBox(ctx, func);
    if (varsToBox.empty()) {
        return;
    }
    auto varsMap = ArenaMap<varbinder::Variable *, varbinder::Variable *>(allocator->Adapter());

    /*
      The function relies on the following facts:
      - TransformChildrenRecursively handles children in order
      - local variables are never used before declaration.
      This ensures that varsToMap has the appropriate record by the time the variable reference is processed.
    */
    auto handleNode = [ctx, &varsToBox, &varsMap](ir::AstNode *ast) {
        if (ast->IsETSParameterExpression() && varsToBox.count(ast->AsETSParameterExpression()->Variable()) > 0) {
            HandleFunctionParam(ctx, ast->AsETSParameterExpression(), &varsMap);
            return ast;  // modifications occur in the containing function
        }
        if (ast->IsVariableDeclarator() && ast->AsVariableDeclarator()->Id()->IsIdentifier() &&
            varsToBox.count(ast->AsVariableDeclarator()->Id()->AsIdentifier()->Variable()) > 0) {
            return HandleVariableDeclarator(ctx, ast->AsVariableDeclarator(), &varsMap);
        }
        if (ast->IsAssignmentExpression() && ast->AsAssignmentExpression()->Left()->IsIdentifier() &&
            varsToBox.count(ast->AsAssignmentExpression()->Left()->AsIdentifier()->Variable()) > 0) {
            return HandleAssignment(ctx, ast->AsAssignmentExpression(), varsMap);
        }
        if (ast->IsIdentifier() && !IsBeingDeclared(ast) && !IsPartOfBoxInitializer(ctx, ast) &&
            !OnLeftSideOfAssignment(ast) && varsToBox.count(ast->AsIdentifier()->Variable()) > 0) {
            return HandleReference(ctx, ast->AsIdentifier(), varsMap.find(ast->AsIdentifier()->Variable())->second);
        }

        return ast;
    };

    func->TransformChildrenRecursivelyPostorder(handleNode, LOWERING_NAME);
}

bool BoxingForLocals::Perform(public_lib::Context *ctx, parser::Program *program)
{
    parser::SavedFormattingFileName savedFormattingName(ctx->parser->AsETSParser(), "boxing-for-lambdas");

    if (ctx->config->options->CompilerOptions().compilationMode == CompilationMode::GEN_STD_LIB) {
        for (auto &[_, ext_programs] : program->ExternalSources()) {
            (void)_;
            for (auto *extProg : ext_programs) {
                Perform(ctx, extProg);
            }
        }
    }

    std::function<void(ir::AstNode *)> searchForFunctions = [&](ir::AstNode *ast) {
        if (ast->IsScriptFunction()) {
            HandleScriptFunction(ctx, ast->AsScriptFunction());  // no recursion
        } else {
            ast->Iterate(searchForFunctions);
        }
    };
    program->Ast()->Iterate(searchForFunctions);
    return true;
}

bool BoxingForLocals::Postcondition([[maybe_unused]] public_lib::Context *ctx, parser::Program const *program)
{
    for (auto &[_, ext_programs] : program->ExternalSources()) {
        (void)_;
        for (auto *extProg : ext_programs) {
            if (!Postcondition(ctx, extProg)) {
                return false;
            }
        }
    }

    return !program->Ast()->IsAnyChild([](const ir::AstNode *node) {
        if (node->IsAssignmentExpression() && node->AsAssignmentExpression()->Left()->IsIdentifier()) {
            auto asExpr = node->AsAssignmentExpression();
            auto var = asExpr->Left()->AsIdentifier()->Variable();
            if (var != nullptr && var->IsLocalVariable() && !var->HasFlag(varbinder::VariableFlags::INITIALIZED)) {
                return true;
            }
        }
        return false;
    });
}

}  // namespace ark::es2panda::compiler
