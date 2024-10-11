/**
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "tsClassImplements.h"

#include <ir/astDump.h>
#include <ir/ts/tsTypeParameter.h>
#include <ir/ts/tsTypeParameterInstantiation.h>

namespace panda::es2panda::ir {

void TSClassImplements::Iterate(const NodeTraverser &cb) const
{
    cb(expression_);

    if (typeParameters_) {
        cb(typeParameters_);
    }
}

void TSClassImplements::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSClassImplements"},
                 {"expression", expression_},
                 {"typeParameters", AstDumper::Optional(typeParameters_)}});
}

void TSClassImplements::Compile([[maybe_unused]] compiler::PandaGen *pg) const {}

checker::Type *TSClassImplements::Check([[maybe_unused]] checker::Checker *checker) const
{
    return nullptr;
}

void TSClassImplements::UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder)
{
    expression_ = std::get<ir::AstNode *>(cb(expression_))->AsExpression();

    if (typeParameters_) {
        typeParameters_ = std::get<ir::AstNode *>(cb(typeParameters_))->AsTSTypeParameterInstantiation();
    }
}

}  // namespace panda::es2panda::ir
