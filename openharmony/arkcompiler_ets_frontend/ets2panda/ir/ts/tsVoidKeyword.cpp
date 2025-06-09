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

#include "tsVoidKeyword.h"

#include "checker/TSchecker.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"

namespace ark::es2panda::ir {
void TSVoidKeyword::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                      [[maybe_unused]] std::string_view const transformationName)
{
    for (auto *&it : VectorIterationGuard(Annotations())) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsAnnotationUsage();
        }
    }
}

void TSVoidKeyword::Iterate([[maybe_unused]] const NodeTraverser &cb) const
{
    for (auto *it : VectorIterationGuard(Annotations())) {
        cb(it);
    }
}

void TSVoidKeyword::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSVoidKeyword"}, {"annotations", AstDumper::Optional(Annotations())}});
}

void TSVoidKeyword::Dump(ir::SrcDumper *dumper) const
{
    for (auto *anno : Annotations()) {
        anno->Dump(dumper);
    }
    dumper->Add("TSVoidKeyword");
}

void TSVoidKeyword::Compile(compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void TSVoidKeyword::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSVoidKeyword::Check(checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSVoidKeyword::GetType([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GlobalVoidType();
}

checker::VerifiedType TSVoidKeyword::Check(checker::ETSChecker *checker)
{
    return {this, checker->GetAnalyzer()->Check(this)};
}
}  // namespace ark::es2panda::ir
