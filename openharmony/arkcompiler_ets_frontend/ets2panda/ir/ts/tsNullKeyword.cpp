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

#include "tsNullKeyword.h"

#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "checker/TSchecker.h"

namespace ark::es2panda::ir {
void TSNullKeyword::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                      [[maybe_unused]] std::string_view const transformationName)
{
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void TSNullKeyword::Iterate([[maybe_unused]] const NodeTraverser &cb) const
{
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void TSNullKeyword::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSNullKeyword"}, {"annotations", AstDumper::Optional(Annotations())}});
}

void TSNullKeyword::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("TSNullKeyword");
}

void TSNullKeyword::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}
void TSNullKeyword::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSNullKeyword::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSNullKeyword::GetType([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GlobalNullType();
}

checker::VerifiedType TSNullKeyword::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}
}  // namespace ark::es2panda::ir
