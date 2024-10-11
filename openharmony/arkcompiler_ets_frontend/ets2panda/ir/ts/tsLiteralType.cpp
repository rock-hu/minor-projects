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

#include "tsLiteralType.h"

#include "compiler/core/ETSGen.h"
#include "compiler/core/pandagen.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "checker/TSchecker.h"

namespace ark::es2panda::ir {
void TSLiteralType::TransformChildren(const NodeTransformer &cb, std::string_view transformationName)
{
    if (auto *transformedNode = cb(literal_); literal_ != transformedNode) {
        literal_->SetTransformedNode(transformationName, transformedNode);
        literal_ = transformedNode->AsExpression();
    }
}

void TSLiteralType::Iterate(const NodeTraverser &cb) const
{
    cb(literal_);
}

void TSLiteralType::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSLiteralType"}, {"literal", literal_}});
}

void TSLiteralType::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("TSLiteralType");
}

void TSLiteralType::Compile([[maybe_unused]] compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void TSLiteralType::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *TSLiteralType::Check([[maybe_unused]] checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *TSLiteralType::GetType([[maybe_unused]] checker::TSChecker *checker)
{
    if (TsType() != nullptr) {
        return TsType();
    }

    SetTsType(literal_->Check(checker));
    return TsType();
}

checker::Type *TSLiteralType::Check([[maybe_unused]] checker::ETSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}
}  // namespace ark::es2panda::ir
