/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "tsTypeAssertion.h"

#include "checker/TSchecker.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "ir/typeNode.h"

namespace ark::es2panda::ir {
void TSTypeAssertion::TransformChildren(const NodeTransformer &cb, std::string_view transformationName)
{
    if (auto *typeAnnotation = TypeAnnotation(); typeAnnotation != nullptr) {
        if (auto *transformedNode = cb(typeAnnotation); typeAnnotation != transformedNode) {
            typeAnnotation->SetTransformedNode(transformationName, transformedNode);
            SetTsTypeAnnotation(static_cast<TypeNode *>(transformedNode));
        }
    }

    if (auto *transformedNode = cb(expression_); expression_ != transformedNode) {
        expression_->SetTransformedNode(transformationName, transformedNode);
        expression_ = transformedNode->AsExpression();
    }
}

void TSTypeAssertion::Iterate(const NodeTraverser &cb) const
{
    cb(TypeAnnotation());
    cb(expression_);
}

void TSTypeAssertion::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSTypeAssertion"}, {"typeAnnotation", TypeAnnotation()}, {"expression", expression_}});
}

void TSTypeAssertion::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("TSTypeAssertion");
}

void TSTypeAssertion::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void TSTypeAssertion::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSTypeAssertion::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSTypeAssertion::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}
}  // namespace ark::es2panda::ir
