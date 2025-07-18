/*
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

#include "forLoopCorrectlyInitialized.h"
#include "ir/statements/forInStatement.h"
#include "ir/statements/forOfStatement.h"
#include "ir/statements/forUpdateStatement.h"

namespace ark::es2panda::compiler::ast_verifier {

[[nodiscard]] CheckResult ForLoopCorrectlyInitialized::operator()(const ir::AstNode *ast)
{
    if (ast->IsForInStatement()) {
        return HandleForInStatement(ast);
    }

    if (ast->IsForOfStatement()) {
        return HandleForOfStatement(ast);
    }

    if (ast->IsForUpdateStatement()) {
        return HandleForUpdateStatement(ast);
    }
    return {CheckDecision::CORRECT, CheckAction::CONTINUE};
}

[[nodiscard]] CheckResult ForLoopCorrectlyInitialized::HandleForInStatement(const ir::AstNode *ast)
{
    auto const *left = ast->AsForInStatement()->Left();
    if (left == nullptr) {
        AddCheckMessage("NULL FOR-IN-LEFT", *ast);
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }

    if (!left->IsIdentifier() && !left->IsVariableDeclaration()) {
        AddCheckMessage("INCORRECT FOR-IN-LEFT", *ast);
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }

    return {CheckDecision::CORRECT, CheckAction::CONTINUE};
}

[[nodiscard]] CheckResult ForLoopCorrectlyInitialized::HandleForOfStatement(const ir::AstNode *ast)
{
    auto const *left = ast->AsForOfStatement()->Left();
    if (left == nullptr) {
        AddCheckMessage("NULL FOR-OF-LEFT", *ast);
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }

    if (!left->IsIdentifier() && !left->IsVariableDeclaration()) {
        AddCheckMessage("INCORRECT FOR-OF-LEFT", *ast);
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }

    return {CheckDecision::CORRECT, CheckAction::CONTINUE};
}

[[nodiscard]] CheckResult ForLoopCorrectlyInitialized::HandleForUpdateStatement(const ir::AstNode *ast)
{
    auto const *test = ast->AsForUpdateStatement()->Test();
    if (test == nullptr) {
        auto const *body = ast->AsForUpdateStatement()->Body();
        if (body == nullptr) {
            AddCheckMessage("NULL FOR-TEST AND FOR-BODY", *ast);
            return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
        }

        return {CheckDecision::CORRECT, CheckAction::CONTINUE};
    }

    if (!test->IsExpression()) {
        AddCheckMessage("NULL FOR VAR", *ast);
        return {CheckDecision::INCORRECT, CheckAction::CONTINUE};
    }

    return {CheckDecision::CORRECT, CheckAction::CONTINUE};
}

}  // namespace ark::es2panda::compiler::ast_verifier
