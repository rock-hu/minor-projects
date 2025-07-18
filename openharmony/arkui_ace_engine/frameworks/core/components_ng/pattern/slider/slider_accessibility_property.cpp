/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/slider/slider_accessibility_property.h"

#include "core/components_ng/pattern/slider/slider_pattern.h"
#include "core/components_ng/pattern/slider/slider_paint_property.h"

#if defined(OHOS_STANDARD_SYSTEM) and !defined(ACE_UNITTEST)
#include "accessibility_element_info.h"
#endif

namespace OHOS::Ace::NG {
#if defined(OHOS_STANDARD_SYSTEM) and !defined(ACE_UNITTEST)
namespace {
constexpr const char* AXIS_VERTICAL = "vertical";
constexpr const char* AXIS_HORIZONTAL = "horizontal";
constexpr const char* AXIS_FREE = "free";
constexpr const char* AXIS_NONE = "none";
} // namespace
#endif
std::string SliderAccessibilityProperty::GetText() const
{
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, "");
    auto sliderProperty = frameNode->GetPaintProperty<SliderPaintProperty>();
    CHECK_NULL_RETURN(sliderProperty, "");
    auto value = sliderProperty->GetValue().value_or(0);
    if (sliderProperty->GetValidSlideRange().has_value()) {
        auto range = sliderProperty->GetValidSlideRange().value();
        auto rangeFromValue = range->GetFromValue();
        auto rangeToValue = range->GetToValue();
        value = std::clamp(value, rangeFromValue, rangeToValue);
    }
    return std::to_string(value);
}

AccessibilityValue SliderAccessibilityProperty::GetAccessibilityValue() const
{
    AccessibilityValue result;
    auto frameNode = host_.Upgrade();
    CHECK_NULL_RETURN(frameNode, result);
    auto sliderProperty = frameNode->GetPaintProperty<SliderPaintProperty>();
    CHECK_NULL_RETURN(sliderProperty, result);
    result.min = sliderProperty->GetMin().value_or(0);
    result.max = sliderProperty->GetMax().value_or(100.0f);
    result.current = sliderProperty->GetValue().value_or(0);
    return result;
}

void SliderAccessibilityProperty::SetSpecificSupportAction()
{
    auto value = GetAccessibilityValue();
    if (value.current < value.max) {
        AddSupportAction(AceAction::ACTION_SCROLL_FORWARD);
    }
    if (value.min < value.current) {
        AddSupportAction(AceAction::ACTION_SCROLL_BACKWARD);
    }
}

void SliderAccessibilityProperty::GetExtraElementInfo(Accessibility::ExtraElementInfo& extraElementInfo)
{
#if defined(OHOS_STANDARD_SYSTEM) and !defined(ACE_UNITTEST)
    auto frameNode = host_.Upgrade();
    CHECK_NULL_VOID(frameNode);
    auto sliderPattern = frameNode->GetPattern<SliderPattern>();
    CHECK_NULL_VOID(sliderPattern);
    std::string axisStr = AXIS_NONE;
    switch (sliderPattern->GetDirection()) {
        case Axis::VERTICAL:
            axisStr = AXIS_VERTICAL;
            break;
        case Axis::HORIZONTAL:
            axisStr = AXIS_HORIZONTAL;
            break;
        case Axis::FREE:
            axisStr = AXIS_FREE;
            break;
        case Axis::NONE:
            axisStr = AXIS_NONE;
            break;
        default:
            axisStr = AXIS_NONE;
            break;
    }
    extraElementInfo.SetExtraElementInfo("direction", axisStr);
#endif
}
} // namespace OHOS::Ace::NG
