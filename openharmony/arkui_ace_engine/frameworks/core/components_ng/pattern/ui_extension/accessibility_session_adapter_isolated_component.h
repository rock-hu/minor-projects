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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_ACCESSIBILITY_SESSION_ADAPTER_ISOLATED_COMPONENT_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERN_ACCESSIBILITY_SESSION_ADAPTER_ISOLATED_COMPONENT_H

#include "core/accessibility/accessibility_session_adapter.h"
#include "core/common/dynamic_component_renderer.h"

namespace OHOS::Ace::NG {
class SessionWrapper;

class AccessibilitySessionAdapterIsolatedComponent : public AccessibilitySessionAdapter {
    DECLARE_ACE_TYPE(AccessibilitySessionAdapterIsolatedComponent, AceType);
public:
    explicit AccessibilitySessionAdapterIsolatedComponent(
        const WeakPtr<DynamicComponentRenderer> dynamicComponentRenderer)
        : dynamicComponentRenderer_(dynamicComponentRenderer) { }
    ~AccessibilitySessionAdapterIsolatedComponent() override = default;

    void TransferHoverEvent(const PointF& point, SourceType source,
        AccessibilityHoverEventType eventType, TimeStamp time) override;
    bool IgnoreHostNode() const override;
private:
    WeakPtr<DynamicComponentRenderer> dynamicComponentRenderer_;
};
} // namespace OHOS::Ace::NG

#endif
