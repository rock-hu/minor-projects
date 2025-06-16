/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef PANDA_GUARD_CONFIGS_VERSION_H
#define PANDA_GUARD_CONFIGS_VERSION_H

#include <string_view>

namespace panda::guard {

constexpr std::string_view ARK_GUARD_DYNAMIC_VERSION = "ArkGuardDynamicVersion";
constexpr std::string_view OBFUSCATION_TOOL_VERSION = "1.0.0.300";

}  // namespace panda::guard

#endif  // PANDA_GUARD_CONFIGS_VERSION_H
