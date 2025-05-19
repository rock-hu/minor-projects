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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_ACCESSIBILITY_SESSION_ADAPTER_UI_EXTENSION_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_UI_EXTENSION_ACCESSIBILITY_SESSION_ADAPTER_UI_EXTENSION_H

#include "core/accessibility/accessibility_session_adapter.h"

namespace OHOS::Ace::NG {
class SessionWrapper;

class AccessibilitySessionAdapterUIExtension : public AccessibilitySessionAdapter {
    DECLARE_ACE_TYPE(AccessibilitySessionAdapterUIExtension, AceType);
public:
    explicit AccessibilitySessionAdapterUIExtension(const WeakPtr<SessionWrapper> sessionWrapper)
        : sessionWrapper_(sessionWrapper) { }
    ~AccessibilitySessionAdapterUIExtension() override = default;

    void TransferHoverEvent(const PointF& point, SourceType source,
        AccessibilityHoverEventType eventType, TimeStamp time) override;
    bool IgnoreHostNode() const override;

    bool IgnoreTransformMouseEvent() const override
    {
        // already send mouse event by self, no need send in barrierfree hover
        return true;
    }

private:
    WeakPtr<SessionWrapper> sessionWrapper_;
};
} // namespace OHOS::Ace::NG

#endif
