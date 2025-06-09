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

#include "tsConditionalType.h"

#include "checker/TSchecker.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"

namespace ark::es2panda::ir {
void TSConditionalType::TransformChildren(const NodeTransformer &cb, std::string_view transformationName)
{
    if (auto *transformedNode = cb(checkType_); checkType_ != transformedNode) {
        checkType_->SetTransformedNode(transformationName, transformedNode);
        checkType_ = transformedNode->AsExpression();
    }

    if (auto *transformedNode = cb(extendsType_); extendsType_ != transformedNode) {
        extendsType_->SetTransformedNode(transformationName, transformedNode);
        extendsType_ = transformedNode->AsExpression();
    }

    if (auto *transformedNode = cb(trueType_); trueType_ != transformedNode) {
        trueType_->SetTransformedNode(transformationName, transformedNode);
        trueType_ = transformedNode->AsExpression();
    }

    if (auto *transformedNode = cb(falseType_); falseType_ != transformedNode) {
        falseType_->SetTransformedNode(transformationName, transformedNode);
        falseType_ = transformedNode->AsExpression();
    }
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void TSConditionalType::Iterate(const NodeTraverser &cb) const
{
    cb(checkType_);
    cb(extendsType_);
    cb(trueType_);
    cb(falseType_);
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void TSConditionalType::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSConditionalType"},
                 {"checkType", checkType_},
                 {"extendsType", extendsType_},
                 {"trueType", trueType_},
                 {"falseType", falseType_},
                 {"annotations", AstDumper::Optional(Annotations())}});
}

void TSConditionalType::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("TSConditionalType");
}

void TSConditionalType::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}
void TSConditionalType::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSConditionalType::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSConditionalType::GetType([[maybe_unused]] checker::TSChecker *checker)
{
    return nullptr;
}

checker::VerifiedType TSConditionalType::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}
}  // namespace ark::es2panda::ir
