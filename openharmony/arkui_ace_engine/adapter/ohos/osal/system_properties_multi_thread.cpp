/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "base/utils/system_properties.h"

#include "parameter.h"
#include "parameters.h"

namespace OHOS::Ace {
constexpr char ENABLE_DEBUG_THREAD_SAFE_NODE_KEY[] = "persist.ace.debug.thread_safe_node.enabled";
bool IsDebugThreadSafeNodeEnabled()
{
    return system::GetBoolParameter(ENABLE_DEBUG_THREAD_SAFE_NODE_KEY, false);
}
bool SystemProperties::debugThreadSafeNodeEnable_ = IsDebugThreadSafeNodeEnabled();
} // namespace OHOS::Ace