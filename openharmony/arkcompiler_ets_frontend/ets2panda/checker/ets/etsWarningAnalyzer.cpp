
/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

#include "etsWarningAnalyzer.h"

#include "generated/diagnostic.h"
#include "parser/program/program.h"
#include "util/options.h"
#include "ir/expressions/binaryExpression.h"
#include "ir/base/methodDefinition.h"
#include "ir/base/scriptFunction.h"
#include "ir/statements/classDeclaration.h"
#include "ir/statements/expressionStatement.h"
#include "ir/statements/blockStatement.h"
#include "ir/expressions/assignmentExpression.h"
#include "ir/expressions/callExpression.h"
#include "ir/expressions/identifier.h"
#include "ir/expressions/memberExpression.h"
#include "ir/ets/etsTypeReferencePart.h"
#include "ir/ets/etsTypeReference.h"
#include "ir/base/classDefinition.h"
#include "ir/statements/forOfStatement.h"
#include "ir/statements/variableDeclarator.h"
#include "ir/statements/variableDeclaration.h"
#include "ir/expressions/updateExpression.h"

namespace ark::es2panda::checker {

void ETSWarningAnalyzer::AnalyzeClassDefForFinalModifier(const ir::ClassDefinition *classDef)
{
    ES2PANDA_ASSERT(classDef != nullptr);

    if (program_ == nullptr || classDef->IsFinal() || classDef->IsAbstract() || classDef->IsStatic() ||
        classDef->IsGlobal() || classDef->IsExported()) {
        return;
    }

    const auto statements = program_->Ast()->Statements();
    for (const auto *it : statements) {
        if (!it->IsClassDeclaration() ||
            classDef->Ident()->Name() == it->AsClassDeclaration()->Definition()->Ident()->Name()) {
            continue;
        }

        const auto *itAsClassDef = it->AsClassDeclaration()->Definition();

        if (!itAsClassDef->IsGlobal()) {
            const auto *superClass = itAsClassDef->Super();

            if (superClass == nullptr) {
                continue;
            }

            if (superClass->IsETSTypeReference() &&
                superClass->AsETSTypeReference()->Part()->GetIdent()->Name() == classDef->Ident()->Name()) {
                return;
            }
        }
    }

    LogWarning(diagnostic::SUGGEST_FINAL_MODIFIER_FOR_CLASS, classDef->Ident()->Start());
}

void ETSWarningAnalyzer::AnalyzeClassMethodForFinalModifier(const ir::MethodDefinition *methodDef,
                                                            const ir::ClassDefinition *classDef)
{
    ES2PANDA_ASSERT(methodDef != nullptr && classDef != nullptr);

    if (methodDef->IsAbstract() || methodDef->IsStatic() || classDef->IsFinal() || program_ == nullptr ||
        methodDef->IsFinal() || methodDef->IsConstructor() || classDef->IsGlobal()) {
        return;
    }

    bool suggestFinal = true;

    const auto statements = program_->Ast()->Statements();
    for (const auto *it : statements) {
        if (!it->IsClassDeclaration() || it->AsClassDeclaration()->Definition()->IsGlobal() ||
            classDef->Ident()->Name() == it->AsClassDeclaration()->Definition()->Ident()->Name()) {
            continue;
        }

        const auto *statementDef = it->AsClassDeclaration()->Definition();
        for (const auto *bodyPart : statementDef->Body()) {
            if (!bodyPart->IsMethodDefinition()) {
                continue;
            }
            static auto classAsETSObject = classDef->TsType()->AsETSObjectType();
            static auto potentialDescendant = statementDef->TsType()->AsETSObjectType();
            if (!potentialDescendant->IsDescendantOf(classAsETSObject)) {
                continue;
            }
            const util::StringView bodyMethodName =
                ETSChecker::GetSignatureFromMethodDefinition(bodyPart->AsMethodDefinition())->Function()->Id()->Name();
            if (bodyPart->IsOverride() && bodyMethodName != compiler::Signatures::CTOR &&
                bodyMethodName == methodDef->Function()->Id()->Name()) {
                suggestFinal = false;
                break;
            }
        }
    }

    if (suggestFinal) {
        LogWarning(diagnostic::SUGGEST_FINAL_MODIFIER_FOR_METHOD, classDef->Ident()->Start());
    }
}

void ETSWarningAnalyzer::ETSWarningSuggestFinal(const ir::AstNode *node)
{
    if (node->IsClassDeclaration() && !program_->NodeContainsETSNolint(node, ETSWarnings::ETS_SUGGEST_FINAL)) {
        if (node->AsClassDeclaration()->Definition()->IsClassDefinition()) {
            AnalyzeClassDefForFinalModifier(node->AsClassDeclaration()->Definition());
        }

        const auto classBody = node->AsClassDeclaration()->Definition()->Body();
        for (const auto *it : classBody) {
            if (it->IsMethodDefinition()) {
                AnalyzeClassMethodForFinalModifier(it->AsMethodDefinition(), node->AsClassDeclaration()->Definition());
            }
        }
    }
    node->Iterate([&](auto *childNode) { ETSWarningSuggestFinal(childNode); });
}

void ETSWarningAnalyzer::CheckTopLevelExpressions(const ir::Expression *expression)
{
    if (expression->IsCallExpression()) {
        const auto exprCallee = expression->AsCallExpression()->Callee();
        lexer::SourcePosition pos = exprCallee->Start();
        if (exprCallee->IsMemberExpression()) {
            pos = exprCallee->AsMemberExpression()->Object()->Start();
            LogWarning(diagnostic::PROHIBIT_TOP_LEVEL_STATEMENTS, pos);
        }
    } else if (expression->IsAssignmentExpression()) {
        const auto assignmentExpr = expression->AsAssignmentExpression();
        LogWarning(diagnostic::PROHIBIT_TOP_LEVEL_STATEMENTS, assignmentExpr->Left()->Start());
    }
}

void ETSWarningAnalyzer::CheckProhibitedTopLevelStatements(const ir::Statement *statement)
{
    switch (statement->Type()) {
        case ir::AstNodeType::ARROW_FUNCTION_EXPRESSION:
        case ir::AstNodeType::FUNCTION_DECLARATION:
        case ir::AstNodeType::SCRIPT_FUNCTION:
        case ir::AstNodeType::ETS_FUNCTION_TYPE:
        case ir::AstNodeType::IMPORT_NAMESPACE_SPECIFIER:
        case ir::AstNodeType::CLASS_DECLARATION:
        case ir::AstNodeType::CLASS_EXPRESSION:
        case ir::AstNodeType::VARIABLE_DECLARATION:
        case ir::AstNodeType::CLASS_DEFINITION:
        case ir::AstNodeType::CLASS_PROPERTY:
            break;
        default:
            LogWarning(diagnostic::PROHIBIT_TOP_LEVEL_STATEMENTS, statement->Start());
            break;
    }
}

void ETSWarningAnalyzer::ETSWarningAnnotationUnusedGenericAliasWarn(const ir::AstNode *node)
{
    if (!node->IsTSTypeAliasDeclaration()) {
        node->Iterate([&](auto *childNode) { ETSWarningAnnotationUnusedGenericAliasWarn(childNode); });
        return;
    }

    auto st = node->AsTSTypeAliasDeclaration();
    for (auto *const param : st->TypeParams()->Params()) {
        const auto *const res = st->TypeAnnotation()->FindChild([&param](const ir::AstNode *const astNode) {
            if (!astNode->IsIdentifier()) {
                return false;
            }

            return param->Name()->AsIdentifier()->Variable() == astNode->AsIdentifier()->Variable();
        });

        if (res == nullptr) {
            util::DiagnosticMessageParams diagnosticParams = {param->Name()->Name()};
            LogWarning(diagnostic::ANNOTATION_UNUSED_GENERIC_ALIAS_WARN, diagnosticParams, param->Start());
            return;
        }
    }
}

void ETSWarningAnalyzer::ETSWarningsProhibitTopLevelStatements(const ir::AstNode *node)
{
    if (!node->IsClassDeclaration() ||
        program_->NodeContainsETSNolint(node, ETSWarnings::ETS_PROHIBIT_TOP_LEVEL_STATEMENTS)) {
        node->Iterate([&](auto *childNode) { ETSWarningsProhibitTopLevelStatements(childNode); });
        return;
    }

    const auto *classDef = node->AsClassDeclaration()->Definition();
    if (!classDef->IsGlobal()) {
        node->Iterate([&](auto *childNode) { ETSWarningsProhibitTopLevelStatements(childNode); });
        return;
    }

    for (const auto *itBody : classDef->Body()) {
        if (!itBody->IsMethodDefinition() ||
            itBody->AsMethodDefinition()->Id()->Name() != compiler::Signatures::INIT_METHOD) {
            continue;
        }

        for (const auto *statement :
             itBody->AsMethodDefinition()->Function()->Body()->AsBlockStatement()->Statements()) {
            if (program_->NodeContainsETSNolint(statement, ETSWarnings::ETS_PROHIBIT_TOP_LEVEL_STATEMENTS)) {
                continue;
            }

            if (!statement->IsExpressionStatement()) {
                CheckProhibitedTopLevelStatements(statement);
                continue;
            }

            // Rewrite this part after fixing AST issue about tiop-level
            CheckTopLevelExpressions(statement->AsExpressionStatement()->GetExpression());
        }
    }
}

void ETSWarningAnalyzer::ETSWarningBoostEqualityStatement(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);

    if (node->IsBinaryExpression() &&
        !program_->NodeContainsETSNolint(node, ETSWarnings::ETS_BOOST_EQUALITY_STATEMENT)) {
        const auto binExpr = node->AsBinaryExpression();
        if (binExpr->OperatorType() == lexer::TokenType::PUNCTUATOR_EQUAL ||
            binExpr->OperatorType() == lexer::TokenType::PUNCTUATOR_NOT_EQUAL) {
            if (binExpr->Right()->IsNullLiteral() && !binExpr->Left()->IsNullLiteral()) {
                LogWarning(diagnostic::BOOST_EQUALITY_STATEMENT, node->Start());
            }
        }
    }
    node->Iterate([&](auto *childNode) { ETSWarningBoostEqualityStatement(childNode); });
}

void ETSWarningAnalyzer::ETSWarningRemoveAsync(const ir::AstNode *node)
{
    if (node->IsMethodDefinition() && !program_->NodeContainsETSNolint(node, ETSWarnings::ETS_REMOVE_ASYNC)) {
        const auto methodDefinition = node->AsMethodDefinition();
        if (methodDefinition->IsAsync()) {
            LogWarning(diagnostic::REPLACE_ASYNC_FUNCTION_WITH_COROUTINE, methodDefinition->Start());
        }
    }
    node->Iterate([&](auto *childNode) { ETSWarningRemoveAsync(childNode); });
}

void ETSWarningAnalyzer::ETSWarningRemoveLambda(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);

    if (node->IsArrowFunctionExpression() && !program_->NodeContainsETSNolint(node, ETSWarnings::ETS_REMOVE_LAMBDA)) {
        LogWarning(diagnostic::REPLACE_LAMBDA_FUNCTION_WITH_REGULAR_FUNCTION, node->Start());
    }
    node->Iterate([&](auto *childNode) { ETSWarningRemoveLambda(childNode); });
}

void ETSWarningAnalyzer::CheckTypeOfBoxing(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);
    const auto flags = node->GetBoxingUnboxingFlags();
    if ((flags & ir::BoxingUnboxingFlags::BOXING_FLAG) != 0) {
        std::string diagnosticParam;
        switch (static_cast<ir::BoxingUnboxingFlags>(flags & ir::BoxingUnboxingFlags::BOXING_FLAG)) {
            case ir::BoxingUnboxingFlags::BOX_TO_INT:
                diagnosticParam = "Int";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_BOOLEAN:
                diagnosticParam = "Boolean";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_BYTE:
                diagnosticParam = "Byte";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_CHAR:
                diagnosticParam = "Char";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_DOUBLE:
                diagnosticParam = "Double";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_FLOAT:
                diagnosticParam = "Float";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_LONG:
                diagnosticParam = "Long";
                break;
            case ir::BoxingUnboxingFlags::BOX_TO_SHORT:
                diagnosticParam = "Short";
                break;
            default:
                break;
        }

        if (!diagnosticParam.empty()) {
            util::DiagnosticMessageParams diagnosticParams = {diagnosticParam, GetBoxingUnboxingType(node)};
            LogWarning(diagnostic::IMPLICIT_BOXING_TO, diagnosticParams, node->Start());
        }
    }
}

void ETSWarningAnalyzer::CheckTypeOfUnboxing(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);
    const auto flags = node->GetBoxingUnboxingFlags();
    if ((flags & ir::BoxingUnboxingFlags::UNBOXING_FLAG) != 0) {
        std::string diagnosticParam;
        switch (static_cast<ir::BoxingUnboxingFlags>(flags & ir::BoxingUnboxingFlags::UNBOXING_FLAG)) {
            case ir::BoxingUnboxingFlags::UNBOX_TO_INT:
                diagnosticParam = "Int";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_BOOLEAN:
                diagnosticParam = "Boolean";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_BYTE:
                diagnosticParam = "Byte";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_CHAR:
                diagnosticParam = "Char";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_DOUBLE:
                diagnosticParam = "Double";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_FLOAT:
                diagnosticParam = "Float";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_LONG:
                diagnosticParam = "Long";
                break;
            case ir::BoxingUnboxingFlags::UNBOX_TO_SHORT:
                diagnosticParam = "Short";
                break;
            default:
                break;
        }

        if (!diagnosticParam.empty()) {
            util::DiagnosticMessageParams diagnosticParams = {diagnosticParam, GetBoxingUnboxingType(node)};
            LogWarning(diagnostic::IMPLICIT_BOXING_TO, diagnosticParams, node->Start());
        }
    }
}

void ETSWarningAnalyzer::CheckTypeOfBoxingUnboxing(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);

    CheckTypeOfBoxing(node);
    CheckTypeOfUnboxing(node);
}

std::string ETSWarningAnalyzer::GetBoxingUnboxingType(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node->Parent() != nullptr);
    switch (node->Parent()->Type()) {
        case ir::AstNodeType::VARIABLE_DECLARATOR: {
            return " in Variable Declaration";
        }
        case ir::AstNodeType::CALL_EXPRESSION: {
            return " in Call Method/Function";
        }
        case ir::AstNodeType::SWITCH_STATEMENT: {
            return " in Switch-case Statement";
        }
        case ir::AstNodeType::ASSIGNMENT_EXPRESSION: {
            return " in Assignment Expression";
        }
        case ir::AstNodeType::BINARY_EXPRESSION: {
            return " in Binary Expression";
        }
        case ir::AstNodeType::UNARY_EXPRESSION: {
            return " in Unary Expression";
        }
        case ir::AstNodeType::UPDATE_EXPRESSION: {
            return " in Update Expression";
        }
        case ir::AstNodeType::MEMBER_EXPRESSION: {
            return " in Member Expression";
        }
        default:
            return "";
    }
}

void ETSWarningAnalyzer::ETSWarningImplicitBoxingUnboxing(const ir::AstNode *node)
{
    ES2PANDA_ASSERT(node != nullptr);

    switch (node->Type()) {
        case ir::AstNodeType::VARIABLE_DECLARATOR:
        case ir::AstNodeType::SWITCH_STATEMENT:
        case ir::AstNodeType::CALL_EXPRESSION:
        case ir::AstNodeType::BINARY_EXPRESSION:
        case ir::AstNodeType::ASSIGNMENT_EXPRESSION:
        case ir::AstNodeType::UNARY_EXPRESSION:
        case ir::AstNodeType::UPDATE_EXPRESSION:
        case ir::AstNodeType::MEMBER_EXPRESSION: {
            if (!program_->NodeContainsETSNolint(node, ETSWarnings::ETS_IMPLICIT_BOXING_UNBOXING)) {
                node->Iterate([this](auto *childNode) { CheckTypeOfBoxingUnboxing(childNode); });
            }
            break;
        }
        default: {
            break;
        }
    }

    node->Iterate([&](auto *childNode) { ETSWarningImplicitBoxingUnboxing(childNode); });
}

void ETSWarningAnalyzer::LogWarning(const std::string &message, const lexer::SourcePosition &position)
{
    diagnosticEngine_.LogWarning(message, position);
}

void ETSWarningAnalyzer::LogWarning(const diagnostic::DiagnosticKind &diagnostic,
                                    const lexer::SourcePosition &position) const
{
    util::DiagnosticMessageParams diagnosticParams;
    this->LogWarning(diagnostic, diagnosticParams, position);
}

void ETSWarningAnalyzer::LogWarning(const diagnostic::DiagnosticKind &diagnostic,
                                    util::DiagnosticMessageParams &diagnosticParams,
                                    const lexer::SourcePosition &position) const
{
    diagnosticEngine_.LogWarning(diagnostic, std::move(diagnosticParams), position);
}

}  // namespace ark::es2panda::checker
