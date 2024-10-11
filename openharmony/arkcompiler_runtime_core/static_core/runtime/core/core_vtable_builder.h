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

#ifndef PANDA_RUNTIME_CORE_CORE_VTABLE_BUILDER_H_
#define PANDA_RUNTIME_CORE_CORE_VTABLE_BUILDER_H_

#include "runtime/include/vtable_builder_standard.h"

namespace ark {

struct CoreVTableOverridePred {
    bool operator()([[maybe_unused]] const MethodInfo *info1, [[maybe_unused]] const MethodInfo *info2) const
    {
        return true;
    }
};

using CoreVTableBuilder = StandardVTableBuilder<CoreVTableOverridePred>;

}  // namespace ark

#endif  // PANDA_RUNTIME_CORE_CORE_VTABLE_BUILDER_H_
