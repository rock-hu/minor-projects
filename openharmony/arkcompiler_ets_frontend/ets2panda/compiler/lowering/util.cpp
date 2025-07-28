/**
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "util.h"

#include "compiler/lowering/scopesInit/scopesInitPhase.h"
#include "ir/expressions/identifier.h"
#include "checker/checker.h"
#include "checker/ETSAnalyzer.h"

namespace ark::es2panda::compiler {

bool HasGlobalClassParent(const ir::AstNode *node)
{
    auto parentClass = util::Helpers::FindAncestorGivenByType(node, ir::AstNodeType::CLASS_DEFINITION);
    return parentClass != nullptr && parentClass->AsClassDefinition()->IsGlobal();
}

varbinder::Scope *NearestScope(const ir::AstNode *ast)
{
    while (ast != nullptr && !ast->IsScopeBearer()) {
        ast = ast->Parent();
    }

    return ast == nullptr ? nullptr : ast->Scope();
}

checker::ETSObjectType const *ContainingClass(const ir::AstNode *ast)
{
    ast = util::Helpers::FindAncestorGivenByType(ast, ir::AstNodeType::CLASS_DEFINITION);
    return ast == nullptr ? nullptr : ast->AsClassDefinition()->TsType()->AsETSObjectType();
}

ir::Identifier *Gensym(ArenaAllocator *const allocator)
{
    util::UString const s = GenName(allocator);
    return allocator->New<ir::Identifier>(s.View(), allocator);
}

util::UString GenName(ArenaAllocator *const allocator)
{
    static std::size_t gensymCounter = 0U;
    return util::UString {std::string(GENSYM_CORE) + std::to_string(++gensymCounter), allocator};
}

void SetSourceRangesRecursively(ir::AstNode *node, const lexer::SourceRange &range)
{
    ES2PANDA_ASSERT(node != nullptr);
    node->SetRange(range);
    node->IterateRecursively([](ir::AstNode *n) { n->SetRange(n->Parent()->Range()); });
}

ir::AstNode *RefineSourceRanges(ir::AstNode *node)
{
    auto const isDummyLoc = [](lexer::SourceRange const &range, ir::AstNode *ast) {
        return (range.start.index == 0 && range.start.line == 0) || (range.end.index < range.start.index) ||
               (range.start.index < ast->Parent()->Start().index) || (range.end.index > ast->Parent()->End().index);
    };

    auto const refine = [isDummyLoc](ir::AstNode *ast) {
        if (isDummyLoc(ast->Range(), ast) && ast->Parent() != nullptr) {
            ast->SetRange(ast->Parent()->Range());
        }
    };

    refine(node);
    node->IterateRecursively(refine);
    return node;
}

// Function to clear expression node types and identifier node variables (for correct re-binding and re-checking)
void ClearTypesVariablesAndScopes(ir::AstNode *node) noexcept
{
    std::function<void(ir::AstNode *)> doNode = [&](ir::AstNode *nn) {
        if (nn->IsScopeBearer()) {
            nn->ClearScope();
        }
        if (nn->IsTyped() && !(nn->IsExpression() && nn->AsExpression()->IsTypeNode())) {
            nn->AsTyped()->SetTsType(nullptr);
        }
        if (nn->IsIdentifier()) {
            nn->AsIdentifier()->SetVariable(nullptr);
        }
        if (!nn->IsETSTypeReference()) {
            nn->Iterate([&](ir::AstNode *child) { doNode(child); });
        }
    };

    doNode(node);
}

ArenaSet<varbinder::Variable *> FindCaptured(ArenaAllocator *allocator, ir::AstNode *scopeBearer) noexcept
{
    auto result = ArenaSet<varbinder::Variable *> {allocator->Adapter()};
    auto scopes = ArenaSet<varbinder::Scope *> {allocator->Adapter()};
    scopeBearer->IterateRecursivelyPreorder([&result, &scopes](ir::AstNode *ast) {
        if (ast->IsScopeBearer() && ast->Scope() != nullptr) {
            scopes.insert(ast->Scope());
            if (ast->Scope()->IsFunctionScope()) {
                scopes.insert(ast->Scope()->AsFunctionScope()->ParamScope());
            } else if (ast->IsForUpdateStatement() || ast->IsForInStatement() || ast->IsForOfStatement() ||
                       ast->IsCatchClause()) {
                // NOTE(gogabr) LoopScope _does not_ currently respond to IsLoopScope().
                // For now, this is the way to reach LoopDeclarationScope.
                scopes.insert(ast->Scope()->Parent());
            }
        }
        if (ast->IsIdentifier() && !ast->Parent()->IsLabelledStatement()) {
            auto *var = ast->AsIdentifier()->Variable();
            if (var == nullptr || !var->HasFlag(varbinder::VariableFlags::LOCAL)) {
                return;
            }
            auto *sc = var->GetScope();
            if (sc != nullptr && !sc->IsClassScope() && !sc->IsGlobalScope() && scopes.count(var->GetScope()) == 0) {
                result.insert(var);
            }
        }
    });
    return result;
}

static void ResetGlobalClass(parser::Program *prog)
{
    for (auto *statement : prog->Ast()->Statements()) {
        if (statement->IsClassDeclaration() && statement->AsClassDeclaration()->Definition()->IsGlobal()) {
            prog->SetGlobalClass(statement->AsClassDeclaration()->Definition());
            break;
        }
    }
}

static bool IsGeneratedForUtilityType(ir::AstNode const *ast)
{
    if (ast->IsClassDeclaration()) {
        auto &name = ast->AsClassDeclaration()->Definition()->Ident()->Name();
        return name.EndsWith(checker::PARTIAL_CLASS_SUFFIX);
    }
    if (ast->IsTSInterfaceDeclaration()) {
        auto &name = ast->AsTSInterfaceDeclaration()->Id()->Name();
        return name.EndsWith(checker::PARTIAL_CLASS_SUFFIX);
    }
    return false;
}

static bool IsGeneratedDynamicClass(ir::AstNode const *ast)
{
    if (ast->IsClassDeclaration()) {
        auto &name = ast->AsClassDeclaration()->Definition()->Ident()->Name();
        return name.Is(Signatures::JSNEW_CLASS) || name.Is(Signatures::JSCALL_CLASS);
    }

    return false;
}

static void ClearHelper(parser::Program *prog)
{
    ResetGlobalClass(prog);
    prog->ClearASTCheckedStatus();
    // #24256 Should be removed when code refactoring on checker is done and no ast node allocated in checker.
    auto &stmts = prog->Ast()->Statements();
    // clang-format off
    stmts.erase(std::remove_if(stmts.begin(), stmts.end(),
        [](ir::AstNode *ast) -> bool {
            return !ast->HasAstNodeFlags(ir::AstNodeFlags::NOCLEANUP) ||
                IsGeneratedForUtilityType(ast) || IsGeneratedDynamicClass(ast);
        }),
        stmts.end());
    // clang-format on

    prog->Ast()->IterateRecursively([](ir::AstNode *ast) -> void { ast->CleanUp(); });
    prog->Ast()->ClearScope();
}

// Rerun varbinder on the node. (First clear typesVariables and scopes)
varbinder::Scope *Rebind(PhaseManager *phaseManager, varbinder::ETSBinder *varBinder, ir::AstNode *node)
{
    if (node->IsProgram()) {
        auto program = node->AsETSModule()->Program();
        if (program->IsPackage()) {
            return nullptr;
        }

        for (auto [_, program_list] : program->ExternalSources()) {
            for (auto prog : program_list) {
                ClearHelper(prog);
            }
        }

        ClearHelper(program);

        varBinder->CleanUp();
        for (auto *phase : phaseManager->RebindPhases()) {
            phase->Apply(varBinder->GetContext(), program);
        }

        return varBinder->TopScope();
    }

    auto *scope = NearestScope(node->Parent());
    auto bscope = varbinder::LexicalScope<varbinder::Scope>::Enter(varBinder, scope);

    ClearTypesVariablesAndScopes(node);
    InitScopesPhaseETS::RunExternalNode(node, varBinder);
    varBinder->ResolveReferencesForScopeWithContext(node, scope);

    return scope;
}

// Rerun varbinder and checker on the node.
void Recheck(PhaseManager *phaseManager, varbinder::ETSBinder *varBinder, checker::ETSChecker *checker,
             ir::AstNode *node)
{
    RefineSourceRanges(node);
    if (node->IsProgram()) {
        auto program = node->AsETSModule()->Program();
        if (program->IsPackage()) {
            return;
        }

        for (auto [_, program_list] : program->ExternalSources()) {
            for (auto prog : program_list) {
                ClearHelper(prog);
            }
        }

        ClearHelper(program);

        varBinder->CleanUp();
        varBinder->GetContext()->checker->CleanUp();

        for (auto *phase : phaseManager->RecheckPhases()) {
            phase->Apply(varBinder->GetContext(), program);
        }
        return;
    }

    auto *scope = Rebind(phaseManager, varBinder, node);

    // NOTE(gogabr: should determine checker status more finely.
    auto *containingClass = ContainingClass(node);
    checker::CheckerStatus newStatus =
        (containingClass == nullptr) ? checker::CheckerStatus::NO_OPTS : checker::CheckerStatus::IN_CLASS;
    if ((checker->Context().Status() & checker::CheckerStatus::IN_EXTENSION_ACCESSOR_CHECK) != 0) {
        newStatus |= checker::CheckerStatus::IN_EXTENSION_ACCESSOR_CHECK;
    }
    auto checkerCtx = checker::SavedCheckerContext(checker, newStatus, containingClass);
    auto scopeCtx = checker::ScopeContext(checker, scope);

    node->Check(checker);
}

// NOTE: used to get the declaration from identifier in Plugin API and LSP
ir::AstNode *DeclarationFromIdentifier(const ir::Identifier *node)
{
    if (node == nullptr) {
        return nullptr;
    }

    auto idVar = node->Variable();
    if (idVar == nullptr) {
        return nullptr;
    }
    auto decl = idVar->Declaration();
    if (decl == nullptr) {
        return nullptr;
    }
    return decl->Node();
}

// Note: run varbinder and checker on the new node generated in lowering phases (without ClearTypesVariablesAndScopes)
void CheckLoweredNode(varbinder::ETSBinder *varBinder, checker::ETSChecker *checker, ir::AstNode *node)
{
    RefineSourceRanges(node);
    InitScopesPhaseETS::RunExternalNode(node, varBinder);
    auto *scope = NearestScope(node);
    varBinder->ResolveReferencesForScopeWithContext(node, scope);

    checker::CheckerStatus newStatus = checker::CheckerStatus::NO_OPTS;
    auto *containingClass = util::Helpers::GetContainingClassDefinition(node);

    if (containingClass != nullptr) {
        if (containingClass->IsAbstract()) {
            newStatus = checker::CheckerStatus::IN_ABSTRACT;
        } else {
            newStatus = checker::CheckerStatus::IN_CLASS;
        }
    }

    if ((checker->Context().Status() & checker::CheckerStatus::IN_EXTENSION_ACCESSOR_CHECK) != 0) {
        newStatus |= checker::CheckerStatus::IN_EXTENSION_ACCESSOR_CHECK;
    }
    auto checkerCtx = checker::SavedCheckerContext(
        checker, newStatus, containingClass != nullptr ? containingClass->TsType()->AsETSObjectType() : nullptr);
    auto scopeCtx = checker::ScopeContext(checker, scope);

    node->Check(checker);
}

bool IsAnonymousClassType(const checker::Type *type)
{
    if (type == nullptr || !type->IsETSObjectType()) {
        return false;
    }

    auto declNode = type->AsETSObjectType()->GetDeclNode();
    return declNode != nullptr && declNode->IsClassDefinition() && declNode->AsClassDefinition()->IsAnonymous();
}

bool ClassDefinitionIsEnumTransformed(const ir::AstNode *node)
{
    return node != nullptr && node->IsClassDefinition() && node->AsClassDefinition()->IsEnumTransformed();
}
}  // namespace ark::es2panda::compiler
