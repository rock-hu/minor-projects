/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DIALOG_DIALOG_ACCESSIBILITY_PROPERTY_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_PATTERNS_DIALOG_DIALOG_ACCESSIBILITY_PROPERTY_H

#include <string>

#include "base/utils/utils.h"
#include "core/components_ng/property/accessibility_property.h"

namespace OHOS::Ace::NG {
class DialogAccessibilityProperty : public AccessibilityProperty {
    DECLARE_ACE_TYPE(DialogAccessibilityProperty, AccessibilityProperty);

public:
    DialogAccessibilityProperty() = default;

    ~DialogAccessibilityProperty() override = default;

    std::string GetText() const override;

    void GetExtraElementInfo(Accessibility::ExtraElementInfo& extraElementInfo) override;

private:
    ACE_DISALLOW_COPY_AND_MOVE(DialogAccessibilityProperty);
};
} // namespace OHOS::Ace::NG

#endif