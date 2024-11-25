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

#include "stringConstantsLowering.h"

namespace ark::es2panda::compiler {

std::string_view StringConstantsLowering::Name() const
{
    return "StringConstantsLowering";
}

static ir::AstNode *FoldConcat(public_lib::Context *ctx, ir::BinaryExpression *const concat)
{
    auto const lhs = concat->Left()->AsStringLiteral();
    auto const rhs = concat->Right()->AsStringLiteral();
    auto const resStr = util::UString(lhs->Str().Mutf8() + rhs->Str().Mutf8(), ctx->allocator).View();

    auto resNode = util::NodeAllocator::Alloc<ir::StringLiteral>(ctx->allocator, resStr);
    resNode->SetParent(concat->Parent());
    resNode->SetRange({lhs->Range().start, rhs->Range().end});
    return resNode;
}

bool StringConstantsLowering::Perform(public_lib::Context *ctx, parser::Program *program)
{
    for (auto &[_, ext_programs] : program->ExternalSources()) {
        (void)_;
        for (auto *extProg : ext_programs) {
            Perform(ctx, extProg);
        }
    }

    program->Ast()->TransformChildrenRecursivelyPostorder(
        // CC-OFFNXT(G.FMT.14-CPP) project code style
        [ctx](ir::AstNode *const node) -> ir::AstNode * {
            if (node->IsBinaryExpression()) {
                auto const binOp = node->AsBinaryExpression();
                if (binOp->OperatorType() == lexer::TokenType::PUNCTUATOR_PLUS && binOp->Left()->IsStringLiteral() &&
                    binOp->Right()->IsStringLiteral()) {
                    return FoldConcat(ctx, binOp);
                }
            }
            return node;
        },
        Name());

    return true;
}

}  // namespace ark::es2panda::compiler
