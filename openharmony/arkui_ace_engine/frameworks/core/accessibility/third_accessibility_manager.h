/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_THIRD_ACCESSIBILITY_PROVIDER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_THIRD_ACCESSIBILITY_PROVIDER_H

#include <map>
#include <vector>

#include "base/memory/ace_type.h"
#include "core/accessibility/native_interface_accessibility_impl.h"

namespace OHOS::Ace {
class ThirdAccessibilityManager : public AceType {
    DECLARE_ACE_TYPE(ThirdAccessibilityManager, AceType);
public:
    virtual int32_t SendAccessibilityAsyncEvent(
        const ArkUI_AccessibilityEventInfo& accessibilityEvent,
        void (*callback)(int32_t errorCode)) { return 0; };
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_ACCESSIBILITY_THIRD_ACCESSIBILITY_PROVIDER_H
