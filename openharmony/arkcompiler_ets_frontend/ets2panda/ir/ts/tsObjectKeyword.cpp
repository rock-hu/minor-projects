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

#include "tsObjectKeyword.h"

#include "checker/TSchecker.h"
#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"

namespace ark::es2panda::ir {
void TSObjectKeyword::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                        [[maybe_unused]] std::string_view const transformationName)
{
}

void TSObjectKeyword::Iterate([[maybe_unused]] const NodeTraverser &cb) const {}

void TSObjectKeyword::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSObjectKeyword"}});
}

void TSObjectKeyword::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("TSObjectKeyword");
}

void TSObjectKeyword::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void TSObjectKeyword::Compile([[maybe_unused]] compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSObjectKeyword::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSObjectKeyword::GetType([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GlobalNonPrimitiveType();
}

checker::Type *TSObjectKeyword::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}
}  // namespace ark::es2panda::ir
