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

#include "thisExpression.h"

#include "compiler/core/pandagen.h"
#include "compiler/core/ETSGen.h"
#include "checker/TSchecker.h"
#include "checker/ETSchecker.h"

namespace ark::es2panda::ir {
void ThisExpression::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                       [[maybe_unused]] std::string_view const transformationName)
{
}

void ThisExpression::Iterate([[maybe_unused]] const NodeTraverser &cb) const {}

void ThisExpression::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "ThisExpression"}});
}

void ThisExpression::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("this");
}

void ThisExpression::Compile(compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void ThisExpression::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *ThisExpression::Check(checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::VerifiedType ThisExpression::Check(checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}

ThisExpression *ThisExpression::Clone(ArenaAllocator *const allocator, AstNode *const parent)
{
    auto *const clone = allocator->New<ThisExpression>();
    ES2PANDA_ASSERT(clone != nullptr);
    if (parent != nullptr) {
        clone->SetParent(parent);
    }
    clone->SetRange(Range());
    return clone;
}
}  // namespace ark::es2panda::ir
