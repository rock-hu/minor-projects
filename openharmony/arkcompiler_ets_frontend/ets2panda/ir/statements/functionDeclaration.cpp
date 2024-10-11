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

#include "functionDeclaration.h"

#include "varbinder/variable.h"
#include "compiler/core/ETSGen.h"
#include "checker/TSchecker.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"
#include "compiler/core/pandagen.h"

namespace ark::es2panda::ir {
void FunctionDeclaration::TransformChildren(const NodeTransformer &cb, std::string_view transformationName)
{
    for (auto *&it : decorators_) {
        if (auto *transformedNode = cb(it); it != transformedNode) {
            it->SetTransformedNode(transformationName, transformedNode);
            it = transformedNode->AsDecorator();
        }
    }

    if (auto *transformedNode = cb(func_); func_ != transformedNode) {
        func_->SetTransformedNode(transformationName, transformedNode);
        func_ = transformedNode->AsScriptFunction();
    }
}

void FunctionDeclaration::Iterate(const NodeTraverser &cb) const
{
    for (auto *it : decorators_) {
        cb(it);
    }

    cb(func_);
}

void FunctionDeclaration::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", func_->IsOverload() ? "TSDeclareFunction" : "FunctionDeclaration"},
                 {"decorators", AstDumper::Optional(decorators_)},
                 {"function", func_}});
}

void FunctionDeclaration::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("function ");
    func_->Id()->Dump(dumper);
    func_->Dump(dumper);
}

void FunctionDeclaration::Compile(compiler::PandaGen *pg) const
{
    pg->GetAstCompiler()->Compile(this);
}

void FunctionDeclaration::Compile(compiler::ETSGen *etsg) const
{
    etsg->GetAstCompiler()->Compile(this);
}

checker::Type *FunctionDeclaration::Check(checker::TSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}

checker::Type *FunctionDeclaration::Check(checker::ETSChecker *checker)
{
    return checker->GetAnalyzer()->Check(this);
}
}  // namespace ark::es2panda::ir
