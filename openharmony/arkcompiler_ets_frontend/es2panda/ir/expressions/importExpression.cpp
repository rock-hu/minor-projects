/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "importExpression.h"

#include <compiler/core/pandagen.h>
#include <ir/astDump.h>

namespace panda::es2panda::ir {

void ImportExpression::Iterate(const NodeTraverser &cb) const
{
    cb(source_);

    if (importAssertion_) {
        cb(importAssertion_);
    }
}

void ImportExpression::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "ImportExpression"},
                 {"source", source_},
                 {"importAssertion", AstDumper::Optional(importAssertion_)}});
}

void ImportExpression::Compile(compiler::PandaGen *pg) const
{
    source_->Compile(pg);
    pg->DynamicImportCall(this);
}

checker::Type *ImportExpression::Check([[maybe_unused]] checker::Checker *checker) const
{
    return nullptr;
}

void ImportExpression::UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder)
{
    source_ = std::get<ir::AstNode *>(cb(source_))->AsExpression();

    if (importAssertion_) {
        importAssertion_ = std::get<ir::AstNode *>(cb(importAssertion_))->AsObjectExpression();
    }
}

}  // namespace panda::es2panda::ir
