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

#include "variableHasEnclosingScope.h"
#include "variableHasScope.h"
#include "helpers.h"
#include "ir/expressions/functionExpression.h"
#include "ir/base/scriptFunction.h"
#include "ir/expressions/identifier.h"
#include "ir/typeNode.h"

namespace ark::es2panda::compiler::ast_verifier {

[[nodiscard]] CheckResult VariableHasEnclosingScope::operator()(CheckContext &ctx, const ir::AstNode *ast)
{
    const auto maybeVar = VariableHasScope::GetLocalScopeVariable(allocator_, ctx, ast);
    if (!maybeVar) {
        return {CheckDecision::CORRECT, CheckAction::CONTINUE};
    }
    const auto var = *maybeVar;
    const auto scope = var->GetScope();
    if (scope == nullptr) {
        // already checked
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }
    const auto encloseScope = scope->EnclosingVariableScope();
    if (encloseScope == nullptr) {
        ctx.AddCheckMessage("NO_ENCLOSING_VAR_SCOPE", *ast, ast->Start());
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }
    const auto node = scope->Node();
    auto result = std::make_tuple(CheckDecision::CORRECT, CheckAction::CONTINUE);
    if (!IsContainedIn(ast, node)) {
        if (CheckCatchClause(ast, node)) {
            return {CheckDecision::CORRECT, CheckAction::CONTINUE};
        }

        if (CheckAstExceptions(ast)) {
            return {CheckDecision::CORRECT, CheckAction::CONTINUE};
        }

        result = {CheckDecision::INCORRECT, CheckAction::CONTINUE};
        ctx.AddCheckMessage("VARIABLE_NOT_ENCLOSE_SCOPE", *ast, ast->Start());
    }

    if (!IsContainedIn<varbinder::Scope>(scope, encloseScope)) {
        result = {CheckDecision::INCORRECT, CheckAction::CONTINUE};
        ctx.AddCheckMessage("VARIABLE_NOT_ENCLOSE_SCOPE", *ast, ast->Start());
    }

    return result;
}

bool VariableHasEnclosingScope::CheckCatchClause(const ir::AstNode *ast, const ir::AstNode *node) const
{
    if (node == nullptr) {
        return false;
    }

    // Check that ast node is contained within node parent for Catch Clause:
    // Catch Clause {
    //      Catch Body {
    //          AST that we need to check
    //      }
    //      Param (Scope Node) {
    //      }
    // }
    if (node->Parent() != nullptr && node->Parent()->IsCatchClause()) {
        return IsContainedIn(ast, node->Parent());
    }

    return false;
}

bool VariableHasEnclosingScope::CheckAstExceptions(const ir::AstNode *ast) const
{
    // NOTE(kkonkuznetsov): skip parameter expression inside arrow function expression
    auto parent = ast->Parent();
    while (parent != nullptr) {
        if (parent->IsETSParameterExpression()) {
            return true;
        }

        parent = parent->Parent();
    }

    // Labels are attached to loop scopes,
    // however label identifier is outside of loop.
    // Example:
    //
    // loop: for (let i = 0; i < 10; i++) {
    // }
    return (ast->Parent()->IsLabelledStatement());
}

}  // namespace ark::es2panda::compiler::ast_verifier
