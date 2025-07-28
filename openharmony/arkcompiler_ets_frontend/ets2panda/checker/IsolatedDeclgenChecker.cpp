/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "checker/IsolatedDeclgenChecker.h"
#include "clang.h"
#include "utils/logger.h"

namespace ark::es2panda::checker {
using AstNodePtr = ir::AstNode *;

void IsolatedDeclgenChecker::LogError(const diagnostic::DiagnosticKind &diagnostic,
                                      const util::DiagnosticMessageParams &diagnosticParams,
                                      const lexer::SourcePosition &pos)
{
    diagnosticEngine_.LogDiagnostic(diagnostic, diagnosticParams, pos);
}

void IsolatedDeclgenChecker::Check(ir::ClassProperty *ast)
{
    if (ast->Parent()->IsAnnotationUsage()) {
        return;
    }
    if (ast->TypeAnnotation() == nullptr) {
        LogError(diagnostic::PROPERTY_MUST_HAVE_EXPLICIT_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {}, ast->Start());
    }
}

void IsolatedDeclgenChecker::Check(ir::ObjectExpression *ast)
{
    for (auto property : ast->Properties()) {
        if (property->IsSpreadElement()) {
            LogError(diagnostic::OBJECTS_THAT_CONTAIN_SPREAD_ASSIGNMENTS_CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {},
                     property->Start());
        }
    }
}

static bool IsEnumArray(ir::ArrayExpression *ast)
{
    if (ast->Elements().empty()) {
        return false;
    }
    auto *arrayParent = ast->Parent();
    if (arrayParent == nullptr || !arrayParent->IsClassProperty()) {
        return false;
    }
    auto *classProperty = arrayParent->AsClassProperty();
    if (classProperty == nullptr || classProperty->Parent() == nullptr) {
        return false;
    }
    auto *classDecl = classProperty->Parent()->AsClassDefinition();
    return !(classDecl == nullptr || !classDecl->InternalName().StartsWith("enum."));
}

void IsolatedDeclgenChecker::Check(ir::ArrayExpression *ast)
{
    if (IsEnumArray(ast)) {
        return;
    }
    for (auto element : ast->Elements()) {
        if (!element->IsStringLiteral() && !element->IsNumberLiteral() && !element->IsBooleanLiteral()) {
            LogError(diagnostic::ONLY_CONST_ARRAYS_CAN_BE_INFERRED_WITH_ISOLATED_DECL, {}, element->Start());
        }
    }
}

void IsolatedDeclgenChecker::Check(ir::ETSParameterExpression *ast)
{
    if (ast->TypeAnnotation() == nullptr) {
        LogError(diagnostic::PARAMETER_MUST_HAVE_EXPLICIT_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {}, ast->Start());
    }
}

void IsolatedDeclgenChecker::Check(ir::ExportDefaultDeclaration *ast)
{
    auto *decl = ast->Decl();
    if (decl == nullptr) {
        LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        return;
    }
    if (decl->IsClassDeclaration()) {
        auto *classDecl = decl->AsClassDeclaration();
        if (classDecl == nullptr) {
            LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        }
        return;
    }
    if (decl->IsTSInterfaceDeclaration()) {
        auto *interfaceDecl = decl->AsTSInterfaceDeclaration();
        if (interfaceDecl == nullptr) {
            LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        }
        return;
    }
    if (decl->IsTSEnumDeclaration()) {
        auto *enumDecl = decl->AsTSEnumDeclaration();
        if (enumDecl == nullptr) {
            LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        }
        return;
    }
    if (decl->IsFunctionDeclaration()) {
        auto *funcDecl = decl->AsFunctionDeclaration();
        if (funcDecl == nullptr) {
            LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        }
        return;
    }
    if (decl->IsClassProperty()) {
        auto *classProperty = decl->AsClassProperty();
        if (classProperty == nullptr) {
            LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
        }
        return;
    }
    LogError(diagnostic::DEFAULT_EXPORTS__CANNOT_BE_INFERRED_WITH_ISOLATED_DECL, {}, ast->Start());
}

std::string GetLiteralType(const ir::Literal *literal)
{
    if (literal->IsStringLiteral()) {
        return "String";
    }
    if (literal->IsNumberLiteral()) {
        return "Number";
    }
    if (literal->IsBooleanLiteral()) {
        return "Boolean";
    }
    return "";
}

std::string IsolatedDeclgenChecker::ProcessLiteralArgument(ir::Literal *literal, ir::ReturnStatement *returnStatement)
{
    std::string literalType = GetLiteralType(literal);
    if (!literalType.empty()) {
        return literalType;
    }
    LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
             returnStatement->Start());
    return "";
}

std::string IsolatedDeclgenChecker::ProcessIdentifierArgument(ir::Identifier *identifier,
                                                              ir::ReturnStatement *returnStatement)
{
    auto idVar = identifier->Variable();
    if (idVar == nullptr) {
        LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
                 returnStatement->Start());
        return "";
    }
    auto decl = idVar->Declaration();
    if (decl == nullptr) {
        LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
                 returnStatement->Start());
        return "";
    }

    if (decl->IsLetOrConstDecl()) {
        return decl->Node()
            ->AsClassProperty()
            ->TypeAnnotation()
            ->AsETSTypeReference()
            ->Part()
            ->Name()
            ->AsIdentifier()
            ->Name()
            .Mutf8();
    }
    LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
             returnStatement->Start());
    return "";
}

std::string IsolatedDeclgenChecker::ProcessConstArrayArgument(ir::ArrayExpression *arrayExpr,
                                                              ir::ReturnStatement *returnStatement)
{
    if (arrayExpr->Elements().empty() || !arrayExpr->Elements()[0]->IsLiteral()) {
        LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
                 returnStatement->Start());
        return "";
    }

    return "Array<" + GetLiteralType((arrayExpr->Elements()[0])->AsLiteral()) + ">";
}

std::string IsolatedDeclgenChecker::ProcessNewClassInstanceExpressionArgument(
    ir::ETSNewClassInstanceExpression *newClassInstance, ir::ReturnStatement *returnStatement)
{
    auto *classType = newClassInstance->GetTypeRef();
    if (classType == nullptr) {
        LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
                 returnStatement->Start());
        return "";
    }
    return classType->AsETSTypeReference()->Part()->Name()->AsIdentifier()->Name().Mutf8();
}

std::string IsolatedDeclgenChecker::InferReturnTypeForArgument(ir::ReturnStatement *returnStatement)
{
    if (returnStatement->Argument() == nullptr) {
        return "void";
    }
    auto *returnType = returnStatement->ReturnType();
    auto returnTypeStr = returnStatement->ReturnType()->ToString();
    if (returnType != nullptr && returnTypeStr.find(ERROR_TYPE) == std::string::npos) {
        return returnTypeStr;
    }
    auto *argument = returnStatement->Argument();
    if (argument->IsLiteral()) {
        auto *literal = argument->AsLiteral();
        return ProcessLiteralArgument(literal, returnStatement);
    }
    if (argument->IsETSNewClassInstanceExpression()) {
        auto *newClassInstance = argument->AsETSNewClassInstanceExpression();
        return ProcessNewClassInstanceExpressionArgument(newClassInstance, returnStatement);
    }
    if (argument->IsETSParameterExpression()) {
        auto *etsParam = argument->AsETSParameterExpression();
        return etsParam->TypeAnnotation()->AsETSTypeReference()->Part()->Name()->AsIdentifier()->Name().Mutf8();
    }
    if (argument->IsArrayExpression()) {
        auto *arrayExpr = argument->AsArrayExpression();
        return ProcessConstArrayArgument(arrayExpr, returnStatement);
    }
    if (argument->IsIdentifier()) {
        auto *identifier = argument->AsIdentifier();
        return ProcessIdentifierArgument(identifier, returnStatement);
    }

    LogError(diagnostic::FUNCTION_MUST_HAVE_AN_EXPLICIT_RETURN_TYPE_ANNOTATION_WITH_ISOLATED_DECL, {},
             returnStatement->Start());
    return "";
}

std::string IsolatedDeclgenChecker::Check(ir::ScriptFunction *ast)
{
    auto &returnStatements = ast->ReturnStatements();
    if (returnStatements.empty()) {
        return "void";
    }
    if (returnStatements.size() == 1) {
        return InferReturnTypeForArgument(returnStatements[0]);
    }

    std::set<std::string> returnTypes;
    for (const auto &returnStatement : returnStatements) {
        auto returnType = InferReturnTypeForArgument(returnStatement);
        if (!returnType.empty()) {
            returnTypes.insert(returnType);
        } else {
            return "";
        }
    }

    if (returnTypes.size() == 1) {
        return *returnTypes.begin();
    }
    std::string returnTypeStr;
    for (const auto &returnType : returnTypes) {
        if (returnTypeStr.empty()) {
            returnTypeStr = returnType;
        } else {
            returnTypeStr += " | " + returnType;
        }
    }

    return returnTypeStr;
}

void IsolatedDeclgenChecker::CheckBeforeChecker()
{
    program_.Ast()->TransformChildrenRecursively(
        [&](ir::AstNode *ast) -> AstNodePtr {
            if (ast->IsClassProperty()) {
                Check(ast->AsClassProperty());
                return ast;
            }

            if (ast->IsETSParameterExpression()) {
                this->Check(ast->AsETSParameterExpression());
                return ast;
            }

            if (ast->IsArrayExpression()) {
                Check(ast->AsArrayExpression());
                return ast;
            }

            if (ast->IsObjectExpression()) {
                Check(ast->AsObjectExpression());
                return ast;
            }
            if (ast->IsExportDefaultDeclaration()) {
                Check(ast->AsExportDefaultDeclaration());
                return ast;
            }

            return ast;
        },
        "CheckIsolatedDeclBeforeChecker");
}

void IsolatedDeclgenChecker::CheckAfterChecker()
{
    program_.Ast()->TransformChildrenRecursively(
        [&](ir::AstNode *ast) -> AstNodePtr {
            if (!ast->IsScriptFunction()) {
                return ast;
            }

            auto *scriptFunction = ast->AsScriptFunction();
            auto *returnType = scriptFunction->Signature()->ReturnType();
            if (returnType == nullptr) {
                return ast;
            }
            std::string returnTypeStr = Check(scriptFunction);
            if (returnType->ToString().find(ERROR_TYPE) == std::string::npos) {
                scriptFunction->SetIsolatedDeclgenReturnType(returnType->ToString());
                return ast;
            }
            scriptFunction->SetIsolatedDeclgenReturnType(returnTypeStr);

            return ast;
        },
        "CheckIsolatedDeclAfterChecker");
}
}  // namespace ark::es2panda::checker