/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "yieldExpression.h"

#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "compiler/function/generatorFunctionBuilder.h"
#include "checker/TSchecker.h"

namespace ark::es2panda::ir {
void YieldExpression::TransformChildren(const NodeTransformer &cb, std::string_view const transformationName)
{
    if (argument_ != nullptr) {
        if (auto *transformedNode = cb(argument_); argument_ != transformedNode) {
            argument_->SetTransformedNode(transformationName, transformedNode);
            argument_ = transformedNode->AsExpression();
        }
    }
}

void YieldExpression::Iterate(const NodeTraverser &cb) const
{
    if (argument_ != nullptr) {
        cb(argument_);
    }
}

void YieldExpression::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "YieldExpression"}, {"delegate", delegate_}, {"argument", AstDumper::Nullish(argument_)}});
}

void YieldExpression::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("YieldExpression");
}

void YieldExpression::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void YieldExpression::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *YieldExpression::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::VerifiedType YieldExpression::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

YieldExpression *YieldExpression::Clone(ArenaAllocator *const allocator, AstNode *const parent)
{
    auto *const argument = argument_ != nullptr ? argument_->Clone(allocator, nullptr)->AsExpression() : nullptr;
    auto *const clone = allocator->New<YieldExpression>(argument, delegate_);

    if (argument != nullptr) {
        argument->SetParent(clone);
    }

    if (parent != nullptr) {
        clone->SetParent(parent);
    }

    clone->SetRange(Range());
    return clone;
}
}  // namespace ark::es2panda::ir
