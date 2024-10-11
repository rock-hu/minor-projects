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

#include "referenceTypeAnnotationIsNull.h"

#include "ir/expressions/identifier.h"

namespace ark::es2panda::compiler::ast_verifier {

CheckResult ReferenceTypeAnnotationIsNull::operator()(CheckContext &ctx, const ir::AstNode *ast)
{
    auto result = std::make_tuple(CheckDecision::CORRECT, CheckAction::CONTINUE);
    if (ast->IsIdentifier()) {
        if (ast->AsIdentifier()->IsReference() && ast->AsIdentifier()->TypeAnnotation() != nullptr) {
            ctx.AddCheckMessage("TYPE_ANNOTATION_NOT_NULLPTR", *ast, ast->Start());
            result = {CheckDecision::INCORRECT, CheckAction::CONTINUE};
        }
    }
    return result;
}

}  // namespace ark::es2panda::compiler::ast_verifier
