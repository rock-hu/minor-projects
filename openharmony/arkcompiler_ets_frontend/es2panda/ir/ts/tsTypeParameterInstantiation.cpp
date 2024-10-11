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

#include "tsTypeParameterInstantiation.h"

#include <ir/astDump.h>
#include <ir/expression.h>

namespace panda::es2panda::ir {

void TSTypeParameterInstantiation::Iterate(const NodeTraverser &cb) const
{
    for (auto *it : params_) {
        cb(it);
    }
}

void TSTypeParameterInstantiation::Dump(ir::AstDumper *dumper) const
{
    dumper->Add({{"type", "TSTypeParameterInstantiation"}, {"params", params_}});
}

void TSTypeParameterInstantiation::Compile([[maybe_unused]] compiler::PandaGen *pg) const {}

checker::Type *TSTypeParameterInstantiation::Check([[maybe_unused]] checker::Checker *checker) const
{
    return nullptr;
}

void TSTypeParameterInstantiation::UpdateSelf(const NodeUpdater &cb, [[maybe_unused]] binder::Binder *binder)
{
    for (auto iter = params_.begin(); iter != params_.end(); iter++) {
        *iter = std::get<ir::AstNode *>(cb(*iter))->AsExpression();
    }
}

}  // namespace panda::es2panda::ir
