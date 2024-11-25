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

#include "undefinedLiteral.h"

#include "compiler/core/pandagen.h"
#include "compiler/core/ETSGen.h"
#include "checker/TSchecker.h"
#include "checker/ETSchecker.h"
#include "ir/astDump.h"
#include "ir/srcDump.h"

namespace ark::es2panda::ir {
void UndefinedLiteral::TransformChildren([[maybe_unused]] const NodeTransformer &cb,
                                         [[maybe_unused]] std::string_view const transformationName)
{
}

void UndefinedLiteral::Iterate([[maybe_unused]] const NodeTraverser &cb) const {}

void UndefinedLiteral::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "UndefinedLiteral"}, {"value", AstDumper::Property::Constant::PROP_UNDEFINED}});
}

void UndefinedLiteral::Dump(ir::SrcDumper *dumper) const
{
    dumper->Add("undefined");
}

void UndefinedLiteral::Compile(compiler::PandaGen *pg) const
{
    pg->LoadConst(this, compiler::Constant::JS_UNDEFINED);
}

void UndefinedLiteral::Compile(compiler::ETSGen *etsg) const
{
    etsg->LoadAccumulatorUndefined(this);
}

checker::Type *UndefinedLiteral::Check(checker::TSChecker *checker)
{
    return checker->GlobalUndefinedType();
}

checker::Type *UndefinedLiteral::Check(checker::ETSChecker *checker)
{
    SetTsType(checker->GlobalETSUndefinedType());
    return TsType();
}

UndefinedLiteral *UndefinedLiteral::Clone(ArenaAllocator *allocator, AstNode *parent)
{
    if (auto *const clone = allocator->New<UndefinedLiteral>(); clone != nullptr) {
        if (parent != nullptr) {
            clone->SetParent(parent);
        }
        clone->SetTsType(TsType());
        clone->SetRange(Range());
        return clone;
    }

    throw Error(ErrorType::GENERIC, "", CLONE_ALLOCATION_ERROR);
}
}  // namespace ark::es2panda::ir
