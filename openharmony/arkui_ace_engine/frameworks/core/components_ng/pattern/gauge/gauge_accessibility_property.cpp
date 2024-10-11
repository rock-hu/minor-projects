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

#include "core/components_ng/pattern/gauge/gauge_accessibility_property.h"

#include "core/components_ng/pattern/gauge/gauge_pattern.h"

namespace OHOS::Ace::NG {
AccessibilityValue GaugeAccessibilityProperty::GetAccessibilityValue() const
{
    AccessibilityValue accessibilityValue;
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, accessibilityValue);
    auto gaugePaintProperty = frameNode->GetPaintProperty<NG::GaugePaintProperty>();
    CHECK_NULL_RETURN(gaugePaintProperty, accessibilityValue);
    accessibilityValue.min = gaugePaintProperty->GetMin().value_or(0);
    accessibilityValue.max = gaugePaintProperty->GetMax().value_or(0);
    accessibilityValue.current = gaugePaintProperty->GetValue().value_or(0);
    return accessibilityValue;
}

std::string GaugeAccessibilityProperty::GetText() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, "");
    auto gaugePaintProperty = frameNode->GetPaintProperty<NG::GaugePaintProperty>();
    CHECK_NULL_RETURN(gaugePaintProperty, "");
    return std::to_string(gaugePaintProperty->GetValue().value_or(0));
}
} // namespace OHOS::Ace::NG