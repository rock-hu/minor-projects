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
#include "node_toggle_modifier.h"

#include "core/components/checkable/checkable_theme.h"
#include "core/components_ng/pattern/toggle/toggle_model_ng.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
const int32_t ERROR_UINT_CODE = -1;
namespace {
constexpr uint32_t RESPONSE_REGION_STEP = 4;
constexpr uint32_t OFFSET_0 = 0;
constexpr uint32_t OFFSET_1 = 1;
constexpr uint32_t OFFSET_2 = 2;
constexpr uint32_t OFFSET_3 = 3;
constexpr uint32_t INDEX_DIMENSION_TOP_0 = 0;
constexpr uint32_t INDEX_DIMENSION_RIGHT_1 = 1;
constexpr uint32_t INDEX_DIMENSION_BOTTOM_2 = 2;
constexpr uint32_t INDEX_DIMENSION_LEFT_3 = 3;
constexpr uint32_t DIMENSION_STEP = 3;

bool SetToggleDimension(
    const ArkUI_Float32* dimensionArray, ArkUI_Uint32 offset, const size_t dataCount,
    std::optional<CalcDimension>& optDimension)
{
    CHECK_NULL_RETURN(dimensionArray, false);
    auto hasValue = dimensionArray[offset];
    if (!static_cast<bool>(hasValue)) {
        return false;
    }
    uint32_t valueIndex = offset + 1;
    uint32_t unitIndex = offset + 2;
    if (unitIndex >= dataCount) {
        return false;
    }
    auto value = dimensionArray[valueIndex];
    auto unit = dimensionArray[unitIndex];
    DimensionUnit unitValue = static_cast<DimensionUnit>(unit);
    CalcDimension dimensionValue = CalcDimension(value, unitValue);
    optDimension = dimensionValue;
    return true;
}

void SetToggleSelectedColor(ArkUINodeHandle node, ArkUI_Uint32 selectedColor)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetSelectedColor(frameNode, Color(selectedColor));
}

void ResetToggleSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::optional<Color> selectedColor;
    ToggleModelNG::SetSelectedColor(frameNode, selectedColor);
}

void SetToggleSwitchPointColor(ArkUINodeHandle node, ArkUI_Uint32 switchPointColor)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetSwitchPointColor(frameNode, Color(switchPointColor));
}

void ResetToggleSwitchPointColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto theme = themeManager->GetTheme<SwitchTheme>();
    Color color;
    if (theme) {
        color = theme->GetPointColor();
    }
    ToggleModelNG::SetSwitchPointColor(frameNode, Color(color));
}

ArkUI_Uint32 GetToggleSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return ToggleModelNG::GetSelectedColor(frameNode).GetValue();
}

ArkUI_Uint32 GetToggleSwitchPointColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return ToggleModelNG::GetSwitchPointColor(frameNode).GetValue();
}

void SetToggleHeight(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    DimensionUnit unitValue = static_cast<DimensionUnit>(unit);
    CalcDimension dimensionValue = CalcDimension(value, unitValue);
    ToggleModelNG::SetHeight(frameNode, dimensionValue);
}

void ResetToggleHeight(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto switchTheme = GetTheme<SwitchTheme>();
    CHECK_NULL_VOID(switchTheme);
    auto defaultHeight = switchTheme->GetHeight();
    auto verticalPadding = switchTheme->GetHotZoneVerticalPadding();
    auto height = defaultHeight - verticalPadding * 2;
    CalcDimension value(height);
    ToggleModelNG::SetHeight(frameNode, value);
}

void SetToggleResponseRegion(ArkUINodeHandle node, const ArkUI_Float32* values,
    const ArkUI_Int32* units, ArkUI_Uint32 length)
{
    CHECK_NULL_VOID(values);
    CHECK_NULL_VOID(units);
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<DimensionRect> region;
    for (uint32_t i = 0; i + OFFSET_3 < length; i = i + RESPONSE_REGION_STEP) {
        CalcDimension xDimen = CalcDimension(values[i + OFFSET_0], static_cast<DimensionUnit>(units[i + OFFSET_0]));
        CalcDimension yDimen = CalcDimension(values[i + OFFSET_1], static_cast<DimensionUnit>(units[i + OFFSET_1]));
        CalcDimension widthDimen = CalcDimension(values[i + OFFSET_2], static_cast<DimensionUnit>(units[i + OFFSET_2]));
        CalcDimension heightDimen =
            CalcDimension(values[i + OFFSET_3], static_cast<DimensionUnit>(units[i + OFFSET_3]));
        DimensionOffset offsetDimen(xDimen, yDimen);
        DimensionRect dimenRect(widthDimen, heightDimen, offsetDimen);
        region.emplace_back(dimenRect);
    }
    ToggleModelNG::SetResponseRegion(frameNode, region);
}

void ResetToggleResponseRegion(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<DimensionRect> region;
    CalcDimension xDimen = CalcDimension(0.0, DimensionUnit::VP);
    CalcDimension yDimen = CalcDimension(0.0, DimensionUnit::VP);
    CalcDimension widthDimen = CalcDimension(1, DimensionUnit::PERCENT);
    CalcDimension heightDimen = CalcDimension(1, DimensionUnit::PERCENT);
    DimensionOffset offsetDimen(xDimen, yDimen);
    DimensionRect dimenRect(widthDimen, heightDimen, offsetDimen);
    region.emplace_back(dimenRect);
    ToggleModelNG::SetResponseRegion(frameNode, region);
}

void SetTogglePadding(ArkUINodeHandle node, const ArkUI_Float32* values, const ArkUI_Uint32 length)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(values);
    CalcLength topDimen;
    CalcLength rightDimen;
    CalcLength bottomDimen;
    CalcLength leftDimen;
    uint32_t step = DIMENSION_STEP;
    uint32_t topIndex = INDEX_DIMENSION_TOP_0;
    std::optional<CalcDimension> topOptional = std::nullopt;
    if (SetToggleDimension(values, topIndex, length, topOptional)) {
        topDimen = CalcLength(
            CalcDimension(topOptional.value().Value(), static_cast<DimensionUnit>(topOptional.value().Unit())));
    }
    uint32_t rightIndex = INDEX_DIMENSION_RIGHT_1 * step;
    std::optional<CalcDimension> rightOptional = std::nullopt;
    if (SetToggleDimension(values, rightIndex, length, rightOptional)) {
        rightDimen = CalcLength(
            CalcDimension(rightOptional.value().Value(), static_cast<DimensionUnit>(rightOptional.value().Unit())));
    }
    uint32_t bottomIndex = INDEX_DIMENSION_BOTTOM_2 * step;
    std::optional<CalcDimension> bottomOptional = std::nullopt;
    if (SetToggleDimension(values, bottomIndex, length, bottomOptional)) {
        bottomDimen = CalcLength(
            CalcDimension(bottomOptional.value().Value(), static_cast<DimensionUnit>(bottomOptional.value().Unit())));
    }
    uint32_t leftIndex = INDEX_DIMENSION_LEFT_3 * step;
    std::optional<CalcDimension> leftOptional = std::nullopt;
    if (SetToggleDimension(values, leftIndex, length, leftOptional)) {
        leftDimen = CalcLength(
            CalcDimension(leftOptional.value().Value(), static_cast<DimensionUnit>(leftOptional.value().Unit())));
    }
    NG::PaddingPropertyF oldPaddings;
    NG::PaddingProperty paddings;
    paddings.top = std::optional<CalcLength>(topDimen);
    paddings.bottom = std::optional<CalcLength>(bottomDimen);
    paddings.left = std::optional<CalcLength>(leftDimen);
    paddings.right = std::optional<CalcLength>(rightDimen);
    ToggleModelNG::SetPadding(frameNode, oldPaddings, paddings);
}

void ResetTogglePadding(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    NG::PaddingPropertyF oldPaddings;
    NG::PaddingProperty paddings;
    paddings.top = std::optional<CalcLength>(CalcLength(0.0, DimensionUnit::VP));
    paddings.bottom = std::optional<CalcLength>(CalcLength(0.0, DimensionUnit::VP));
    paddings.left = std::optional<CalcLength>(CalcLength(0.0, DimensionUnit::VP));
    paddings.right = std::optional<CalcLength>(CalcLength(0.0, DimensionUnit::VP));
    ToggleModelNG::SetPadding(frameNode, oldPaddings, paddings);
}

void SetToggleBackgroundColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetBackgroundColor(frameNode, Color(color));
}

void ResetToggleBackgroundColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetBackgroundColor(frameNode, Color(Color::TRANSPARENT));
}

void SetToggleHoverEffect(ArkUINodeHandle node, ArkUI_Int32 hoverEffectValue)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    OHOS::Ace::HoverEffectType hoverEffect = OHOS::Ace::HoverEffectType::AUTO;
    hoverEffect = static_cast<OHOS::Ace::HoverEffectType>(hoverEffectValue);
    ToggleModelNG::SetHoverEffect(frameNode, hoverEffect);
}

void ResetToggleHoverEffect(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetHoverEffect(frameNode, OHOS::Ace::HoverEffectType::AUTO);
}

void SetTogglePointRadius(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    DimensionUnit unitValue = static_cast<DimensionUnit>(unit);
    CalcDimension switchPointRadius = CalcDimension(value, unitValue);
    ToggleModelNG::SetPointRadius(frameNode, switchPointRadius);
}

void ResetTogglePointRadius(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::ResetPointRadius(frameNode);
}

void SetToggleUnselectedColor(ArkUINodeHandle node, ArkUI_Uint32 unselectedColor)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetUnselectedColor(frameNode, Color(unselectedColor));
}

void ResetToggleUnselectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto switchTheme = themeManager->GetTheme<SwitchTheme>();
    CHECK_NULL_VOID(switchTheme);
    Color unselectedColor;
    if (switchTheme) {
        unselectedColor = switchTheme->GetInactiveColor();
    }
    ToggleModelNG::SetUnselectedColor(frameNode, unselectedColor);
}

void SetToggleTrackBorderRadius(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    DimensionUnit unitValue = static_cast<DimensionUnit>(unit);
    CalcDimension borderRadius = CalcDimension(value, unitValue);
    ToggleModelNG::SetTrackBorderRadius(frameNode, borderRadius);
}

void ResetToggleTrackBorderRadius(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::ResetTrackBorderRadius(frameNode);
}

void SetToggleIsOn(ArkUINodeHandle node, ArkUI_Bool switchPointColor)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetSwitchIsOn(frameNode, static_cast<bool>(switchPointColor));
}

void ResetToggleIsOn(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetSwitchIsOn(frameNode, false);
}

ArkUI_Bool GetToggleIsOn(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return static_cast<ArkUI_Bool>(ToggleModelNG::GetSwitchIsOn(frameNode));
}

ArkUI_Uint32 GetToggleUnselectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_RETURN(frameNode, ERROR_UINT_CODE);
    return ToggleModelNG::GetUnselectedColor(frameNode).GetValue();
}

void SetToggleState(ArkUINodeHandle node, ArkUI_Bool isOn)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::SetToggleState(frameNode, static_cast<bool>(isOn));
}
} // namespace
namespace NodeModifier {
const ArkUIToggleModifier* GetToggleModifier()
{
    static const ArkUIToggleModifier modifier = {
        SetToggleSelectedColor,
        ResetToggleSelectedColor,
        SetToggleSwitchPointColor,
        ResetToggleSwitchPointColor,
        SetToggleHeight,
        ResetToggleHeight,
        SetToggleResponseRegion,
        ResetToggleResponseRegion,
        SetTogglePadding,
        ResetTogglePadding,
        SetToggleBackgroundColor,
        ResetToggleBackgroundColor,
        SetToggleHoverEffect,
        ResetToggleHoverEffect,
        GetToggleSelectedColor,
        GetToggleSwitchPointColor,
        SetToggleIsOn,
        ResetToggleIsOn,
        GetToggleIsOn,
        SetTogglePointRadius,
        ResetTogglePointRadius,
        SetToggleUnselectedColor,
        ResetToggleUnselectedColor,
        SetToggleTrackBorderRadius,
        ResetToggleTrackBorderRadius,
        GetToggleUnselectedColor,
        SetToggleState,
    };

    return &modifier;
}

const CJUIToggleModifier* GetCJUIToggleModifier()
{
    static const CJUIToggleModifier modifier = {
        SetToggleSelectedColor,
        ResetToggleSelectedColor,
        SetToggleSwitchPointColor,
        ResetToggleSwitchPointColor,
        SetToggleHeight,
        ResetToggleHeight,
        SetToggleResponseRegion,
        ResetToggleResponseRegion,
        SetTogglePadding,
        ResetTogglePadding,
        SetToggleBackgroundColor,
        ResetToggleBackgroundColor,
        SetToggleHoverEffect,
        ResetToggleHoverEffect,
        GetToggleSelectedColor,
        GetToggleSwitchPointColor,
        SetToggleIsOn,
        ResetToggleIsOn,
        GetToggleIsOn,
        SetTogglePointRadius,
        ResetTogglePointRadius,
        SetToggleUnselectedColor,
        ResetToggleUnselectedColor,
        SetToggleTrackBorderRadius,
        ResetToggleTrackBorderRadius,
        GetToggleUnselectedColor,
    };

    return &modifier;
}

void SetOnToggleChange(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onChange = [node, extraParam](const bool isOn) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_TOGGLE_CHANGE;
        event.componentAsyncEvent.data[0].u32 = isOn;
        SendArkUIAsyncEvent(&event);
    };
    ToggleModelNG::OnChange(frameNode, std::move(onChange));
}

void ResetOnToggleChange(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ToggleModelNG::OnChange(frameNode, nullptr);
}
} // namespace NodeModifier
} // namespace OHOS::Ace::NG
