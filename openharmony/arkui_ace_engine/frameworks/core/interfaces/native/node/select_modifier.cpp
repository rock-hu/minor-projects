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
#include "core/interfaces/native/node/select_modifier.h"

#include "core/components_ng/base/view_abstract_model_ng.h"
#include "frameworks/bridge/common/utils/utils.h"

namespace OHOS::Ace::NG {
constexpr int32_t DEFAULT_GROUP_DIVIDER_VALUES_COUNT = 3;
const char DELIMITER = '|';
const char* ERR_CODE = "-1";
const int32_t SIZE_OF_FONT_INFO = 3;
const int32_t SIZE_OF_TWO = 2;
const int32_t DEFAULT_SELECT = 0;

void SetSpace(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension space = Dimension(value, static_cast<DimensionUnit>(unit));
    SelectModelNG::SetSpace(frameNode, space);
}

void SetValue(ArkUINodeHandle node, ArkUI_CharPtr value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetValue(frameNode, value);
}

void SetSelected(ArkUINodeHandle node, ArkUI_Int32 idx)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (idx < -1) {
        idx = -1;
    }
    SelectModelNG::SetSelected(frameNode, idx);
}

void SetSelectFontColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetFontColor(frameNode, Color(color));
}

void SetSelectedOptionBgColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetSelectedOptionBgColor(frameNode, Color(color));
}

void SetOptionBgColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetOptionBgColor(frameNode, Color(color));
}

void SetOptionFontColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetOptionFontColor(frameNode, Color(color));
}

void SetSelectedOptionFontColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetSelectedOptionFontColor(frameNode, Color(color));
}

void SetArrowPosition(ArkUINodeHandle node, ArkUI_Int32 arrowPosition)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (static_cast<OHOS::Ace::ArrowPosition>(arrowPosition) != OHOS::Ace::ArrowPosition::START &&
        static_cast<OHOS::Ace::ArrowPosition>(arrowPosition) != OHOS::Ace::ArrowPosition::END) {
        arrowPosition = 0;
    }
    SelectModelNG::SetArrowPosition(frameNode, static_cast<ArrowPosition>(arrowPosition));
}

void SetMenuAlign(
    ArkUINodeHandle node, ArkUI_Int32 alignType, const ArkUI_Float32* values,
    const ArkUI_Int32* units, ArkUI_Int32 size)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    MenuAlign menuAlignObj;
    menuAlignObj.alignType = static_cast<MenuAlignType>(alignType);
    if (values != nullptr && units != nullptr && size == SIZE_OF_TWO) {
        Dimension dx = Dimension(values[0], static_cast<OHOS::Ace::DimensionUnit>(units[0]));
        Dimension dy = Dimension(values[1], static_cast<OHOS::Ace::DimensionUnit>(units[1]));

        menuAlignObj.offset = DimensionOffset(dx, dy);
    }
    SelectModelNG::SetMenuAlign(frameNode, menuAlignObj);
}

void SetFont(ArkUINodeHandle node, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, DELIMITER, res);
    if (res.size() != SIZE_OF_FONT_INFO) {
        return;
    }

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);

    CalcDimension fontSize = selectTheme->GetFontSize();
    if (res[0] != ERR_CODE) { // 0: index of font size data
        fontSize = StringUtils::StringToCalcDimension(res[0], false, DimensionUnit::FP);
    }
    SelectModelNG::SetFontSize(frameNode, fontSize);

    FontWeight weight = FontWeight::MEDIUM;
    if (res[1] != ERR_CODE) { // 1: index of font weight data
        weight = StringUtils::StringToFontWeight(res[1], FontWeight::MEDIUM);
    }
    SelectModelNG::SetFontWeight(frameNode, weight);

    std::vector<std::string> fontFamilies = textTheme->GetTextStyle().GetFontFamilies();
    if (res[2] != ERR_CODE) { // 2: index of font family data
        fontFamilies = Framework::ConvertStrToFontFamilies(res[2]);
    }
    SelectModelNG::SetFontFamily(frameNode, fontFamilies);

    auto style = static_cast<Ace::FontStyle>(styleVal);
    SelectModelNG::SetItalicFontStyle(frameNode, style);
}

void SetOptionFont(ArkUINodeHandle node, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, DELIMITER, res);
    if (res.size() != SIZE_OF_FONT_INFO) {
        return;
    }

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);

    CalcDimension fontSize = selectTheme->GetMenuFontSize();
    if (res[0] != ERR_CODE) { // 0: index of font size data
        fontSize = StringUtils::StringToCalcDimension(res[0], false, DimensionUnit::FP);
    }
    SelectModelNG::SetOptionFontSize(frameNode, fontSize);

    FontWeight weight = textTheme->GetTextStyle().GetFontWeight();
    if (res[1] != ERR_CODE) { // 1: index of font weight data
        weight = StringUtils::StringToFontWeight(res[1], FontWeight::REGULAR);
    }
    SelectModelNG::SetOptionFontWeight(frameNode, weight);

    std::vector<std::string> fontFamilies = textTheme->GetTextStyle().GetFontFamilies();
    if (res[2] != ERR_CODE) { // 2: index of font family data
        fontFamilies = Framework::ConvertStrToFontFamilies(res[2]);
    }
    SelectModelNG::SetOptionFontFamily(frameNode, fontFamilies);

    auto style = static_cast<Ace::FontStyle>(styleVal);
    SelectModelNG::SetOptionItalicFontStyle(frameNode, style);
}

void SetSelectedOptionFont(ArkUINodeHandle node, ArkUI_CharPtr fontInfo, ArkUI_Int32 styleVal)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, DELIMITER, res);
    if (res.size() != SIZE_OF_FONT_INFO) {
        return;
    }

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);

    CalcDimension fontSize = selectTheme->GetFontSize();
    if (res[0] != ERR_CODE) { // 0: index of font size data
        fontSize = StringUtils::StringToCalcDimension(res[0], false, DimensionUnit::FP);
    }
    SelectModelNG::SetSelectedOptionFontSize(frameNode, fontSize);

    FontWeight weight = textTheme->GetTextStyle().GetFontWeight();
    if (res[1] != ERR_CODE) { // 1: index of font weight data
        weight = StringUtils::StringToFontWeight(res[1], FontWeight::REGULAR);
    }
    SelectModelNG::SetSelectedOptionFontWeight(frameNode, weight);

    std::vector<std::string> fontFamilies = textTheme->GetTextStyle().GetFontFamilies();
    if (res[2] != ERR_CODE) { // 2: index of font family data
        fontFamilies = Framework::ConvertStrToFontFamilies(res[2]);
    }
    SelectModelNG::SetSelectedOptionFontFamily(frameNode, fontFamilies);

    auto style = static_cast<Ace::FontStyle>(styleVal);
    SelectModelNG::SetSelectedOptionItalicFontStyle(frameNode, style);
}

void ResetSpace(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    Dimension space = selectTheme->GetContentSpinnerPadding();
    SelectModelNG::SetSpace(frameNode, space);
}

void ResetValue(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetValue(frameNode, "");
}

void ResetSelected(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetSelected(frameNode, DEFAULT_SELECT);
}

void ResetSelectFontColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetFontColor(frameNode, selectTheme->GetFontColor());
}

void ResetSelectedOptionBgColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetSelectedOptionBgColor(frameNode, selectTheme->GetSelectedColor());
}

void ResetOptionBgColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetOptionBgColor(frameNode, selectTheme->GetBackgroundColor());
}

void ResetOptionFontColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetOptionFontColor(frameNode, selectTheme->GetMenuFontColor());
}

void ResetSelectedOptionFontColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetSelectedOptionFontColor(frameNode, selectTheme->GetSelectedColorText());
}

void ResetArrowPosition(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetArrowPosition(frameNode, ArrowPosition::END);
}

void ResetMenuAlign(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    MenuAlign menuAlignObj;
    SelectModelNG::SetMenuAlign(frameNode, menuAlignObj);
}

void ResetFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);
    
    auto controlSize = SelectModelNG::GetControlSize(frameNode);
    SelectModelNG::SetFontSize(frameNode, selectTheme->GetFontSize(controlSize));
    SelectModelNG::SetFontWeight(frameNode, FontWeight::MEDIUM);
    SelectModelNG::SetFontFamily(frameNode, textTheme->GetTextStyle().GetFontFamilies());
    SelectModelNG::SetItalicFontStyle(frameNode, textTheme->GetTextStyle().GetFontStyle());
}

void ResetOptionFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);

    SelectModelNG::SetOptionFontSize(frameNode, selectTheme->GetMenuFontSize());
    SelectModelNG::SetOptionFontWeight(frameNode, textTheme->GetTextStyle().GetFontWeight());
    SelectModelNG::SetOptionFontFamily(frameNode, textTheme->GetTextStyle().GetFontFamilies());
    SelectModelNG::SetOptionItalicFontStyle(frameNode, textTheme->GetTextStyle().GetFontStyle());
}

void ResetSelectedOptionFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    auto textTheme = GetTheme<TextTheme>();
    CHECK_NULL_VOID(textTheme);

    SelectModelNG::SetSelectedOptionFontSize(frameNode, selectTheme->GetFontSize());
    SelectModelNG::SetSelectedOptionFontWeight(frameNode, textTheme->GetTextStyle().GetFontWeight());
    SelectModelNG::SetSelectedOptionFontFamily(frameNode, textTheme->GetTextStyle().GetFontFamilies());
    SelectModelNG::SetSelectedOptionItalicFontStyle(frameNode, textTheme->GetTextStyle().GetFontStyle());
}

void SetSelectOptionWidthFitTrigger(ArkUINodeHandle node, ArkUI_Bool trigger)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetOptionWidthFitTrigger(frameNode, trigger);
}

void SetSelectOptionWidth(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension optionWidth = Dimension(value, static_cast<DimensionUnit>(unit));
    SelectModelNG::SetHasOptionWidth(frameNode, true);
    SelectModelNG::SetOptionWidth(frameNode, optionWidth);
}

void ResetSelectOptionWidth(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension optionWidth;
    SelectModelNG::SetHasOptionWidth(frameNode, false);
    SelectModelNG::SetOptionWidth(frameNode, optionWidth);
}

void SetSelectOptionHeight(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension optionHeight = Dimension(value, static_cast<DimensionUnit>(unit));
    SelectModelNG::SetOptionHeight(frameNode, optionHeight);
}

void ResetSelectOptionHeight(ArkUINodeHandle node) {}

void SetSelectWidth(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit, ArkUI_CharPtr calcValue)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto unitEnum = static_cast<OHOS::Ace::DimensionUnit>(unit);
    if (unitEnum == DimensionUnit::CALC) {
        ViewAbstract::SetWidth(
            frameNode, CalcLength(CalcLength(std::string(calcValue))));
    } else {
        ViewAbstract::SetWidth(frameNode, CalcLength(value, unitEnum));
    }
}

void ResetSelectWidth(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    ViewAbstract::ClearWidthOrHeight(frameNode, true);
}

void SetSelectHeight(ArkUINodeHandle node, ArkUI_Float32 value, ArkUI_Int32 unit, ArkUI_CharPtr calcValue)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    auto unitEnum = static_cast<OHOS::Ace::DimensionUnit>(unit);
    if (unitEnum == DimensionUnit::CALC) {
        ViewAbstract::SetHeight(
            frameNode, CalcLength(CalcLength(std::string(calcValue))));
    } else {
        ViewAbstract::SetHeight(frameNode, CalcLength(value, unitEnum));
    }
}

void ResetSelectHeight(ArkUINodeHandle node)
{
    auto *frameNode = reinterpret_cast<FrameNode *>(node);
    CHECK_NULL_VOID(frameNode);
    ViewAbstract::ClearWidthOrHeight(frameNode, false);
}

void SetSelectSize(ArkUINodeHandle node, const ArkUI_Float32* number, const ArkUI_Int32* unit, ArkUI_CharPtr* calc)
{
    CHECK_NULL_VOID(number);
    CHECK_NULL_VOID(unit);
    int32_t widthIndex = 0;
    int32_t heightIndex = 1;
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (*(unit + widthIndex) == static_cast<int8_t>(DimensionUnit::CALC) && *(calc + widthIndex) != nullptr) {
        ViewAbstract::SetWidth(frameNode, CalcLength(std::string(*(calc + widthIndex))));
    } else {
        ViewAbstract::SetWidth(
            frameNode, CalcLength(*(number + widthIndex), static_cast<DimensionUnit>(*(unit + widthIndex))));
    }
    if (*(unit + heightIndex) == static_cast<int8_t>(DimensionUnit::CALC) && *(calc + heightIndex) != nullptr) {
        ViewAbstract::SetHeight(frameNode, CalcLength(std::string(*(calc + heightIndex))));
    } else {
        ViewAbstract::SetHeight(
            frameNode, CalcLength(*(number + heightIndex), static_cast<DimensionUnit>(*(unit + heightIndex))));
    }
}

void ResetSelectSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    ViewAbstract::ClearWidthOrHeight(frameNode, true);
    ViewAbstract::ClearWidthOrHeight(frameNode, false);
}

void SetControlSize(ArkUINodeHandle node, ArkUI_Int32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto controlSize = static_cast<ControlSize>(value);
    SelectModelNG::SetControlSize(frameNode, controlSize);
}

void ResetControlSize(ArkUINodeHandle node) {}

void SetSelectValue(ArkUINodeHandle node, ArkUI_CharPtr* values, ArkUI_CharPtr* icons, ArkUI_Uint32 length)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(values);
    CHECK_NULL_VOID(icons);
    std::vector<SelectParam> params;
    for (uint32_t i = 0; i < length; i++) {
        if (!values[i]) {
            return;
        }
        SelectParam param;
        param.text = values[i];
        param.icon = icons[i];
        params.emplace_back(param);
    }
    SelectModelNG::InitSelect(frameNode, params);
}

void ResetSelectValue(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<SelectParam> params;
    SelectModelNG::InitSelect(frameNode, params);
}

void SetMenuBgColor(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    SelectModelNG::SetMenuBackgroundColor(frameNode, Color(color));
}

void ResetMenuBgColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto selectTheme = GetTheme<SelectTheme>();
    CHECK_NULL_VOID(selectTheme);
    SelectModelNG::SetMenuBackgroundColor(frameNode, selectTheme->GetBackgroundColor());
}

void SetMenuBgBlurStyle(ArkUINodeHandle node, ArkUI_Int32 style)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    BlurStyleOption styleOption;
    styleOption.blurStyle = static_cast<OHOS::Ace::BlurStyle>(style);
    SelectModelNG::SetMenuBackgroundBlurStyle(frameNode, styleOption);
}

void ResetMenuBgBlurStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    BlurStyleOption styleOption;
    SelectModelNG::SetMenuBackgroundBlurStyle(frameNode, styleOption);
}

void SetSelectDivider(ArkUINodeHandle node, ArkUI_Uint32 color, const ArkUI_Float32* values,
    const ArkUI_Int32* units, ArkUI_Int32 length)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);

    if (length != DEFAULT_GROUP_DIVIDER_VALUES_COUNT) {
        return;
    }

    NG::SelectDivider divider;
    divider.color = Color(color);
    divider.strokeWidth = Dimension(values[0], static_cast<OHOS::Ace::DimensionUnit>(units[0]));
    divider.startMargin = Dimension(values[1], static_cast<OHOS::Ace::DimensionUnit>(units[1]));
    divider.endMargin = Dimension(values[2], static_cast<OHOS::Ace::DimensionUnit>(units[2]));

    SelectModelNG::SetDivider(frameNode, divider);
}

void ResetSelectDivider(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto selectTheme = themeManager->GetTheme<SelectTheme>();
    Dimension defaultStrokeWidth = 0.0_vp;
    Dimension defaultMargin = -1.0_vp;
    Color defaultColor = Color::TRANSPARENT;
    NG::SelectDivider divider;
    if (selectTheme) {
        defaultStrokeWidth = selectTheme->GetDefaultDividerWidth();
        defaultColor = selectTheme->GetLineColor();
        divider.strokeWidth = defaultStrokeWidth;
        divider.color = defaultColor;
        divider.startMargin = defaultMargin;
        divider.endMargin = defaultMargin;
    }
    SelectModelNG::SetDivider(frameNode, divider);
}

void ResetSelectDividerNull(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContext();
    CHECK_NULL_VOID(context);
    auto themeManager = context->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto selectTheme = themeManager->GetTheme<SelectTheme>();
    Dimension defaultStrokeWidth = 0.0_vp;
    Dimension defaultMargin = -1.0_vp;
    Color defaultColor = Color::TRANSPARENT;
    NG::SelectDivider divider;
    if (selectTheme) {
        defaultColor = selectTheme->GetLineColor();
        divider.strokeWidth = defaultStrokeWidth;
        divider.color = defaultColor;
        divider.startMargin = defaultMargin;
        divider.endMargin = defaultMargin;
    }
    SelectModelNG::SetDivider(frameNode, divider);
}

namespace NodeModifier {
const ArkUISelectModifier* GetSelectModifier()
{
    static const ArkUISelectModifier modifier = { SetSpace, SetValue, SetSelected, SetSelectFontColor,
        SetSelectedOptionBgColor, SetOptionBgColor, SetOptionFontColor, SetSelectedOptionFontColor, SetArrowPosition,
        SetMenuAlign, SetFont, SetOptionFont, SetSelectedOptionFont, ResetSpace, ResetValue, ResetSelected,
        ResetSelectFontColor, ResetSelectedOptionBgColor, ResetOptionBgColor, ResetOptionFontColor,
        ResetSelectedOptionFontColor, ResetArrowPosition, ResetMenuAlign, ResetFont, ResetOptionFont,
        ResetSelectedOptionFont, SetSelectWidth, ResetSelectWidth, SetSelectHeight, ResetSelectHeight, SetSelectSize,
        ResetSelectSize, SetSelectOptionWidthFitTrigger, SetSelectOptionWidth, ResetSelectOptionWidth,
        SetSelectOptionHeight, ResetSelectOptionHeight, SetControlSize, ResetControlSize, SetSelectValue,
        ResetSelectValue, SetMenuBgColor, ResetMenuBgColor, SetMenuBgBlurStyle, ResetMenuBgBlurStyle, SetSelectDivider,
        ResetSelectDivider, ResetSelectDividerNull };

    return &modifier;
}

const CJUISelectModifier* GetCJUISelectModifier()
{
    static const CJUISelectModifier modifier = { SetSpace, SetValue, SetSelected, SetSelectFontColor,
        SetSelectedOptionBgColor, SetOptionBgColor, SetOptionFontColor, SetSelectedOptionFontColor, SetArrowPosition,
        SetMenuAlign, SetFont, SetOptionFont, SetSelectedOptionFont, ResetSpace, ResetValue, ResetSelected,
        ResetSelectFontColor, ResetSelectedOptionBgColor, ResetOptionBgColor, ResetOptionFontColor,
        ResetSelectedOptionFontColor, ResetArrowPosition, ResetMenuAlign, ResetFont, ResetOptionFont,
        ResetSelectedOptionFont, SetSelectWidth, ResetSelectWidth, SetSelectHeight, ResetSelectHeight, SetSelectSize,
        ResetSelectSize, SetSelectOptionWidthFitTrigger, SetSelectOptionWidth, ResetSelectOptionWidth,
        SetSelectOptionHeight, ResetSelectOptionHeight, SetControlSize, ResetControlSize, SetMenuBgColor,
        ResetMenuBgColor, SetMenuBgBlurStyle, ResetMenuBgBlurStyle, SetSelectValue, ResetSelectValue, SetSelectDivider,
        ResetSelectDivider, ResetSelectDividerNull };

    return &modifier;
}

void SetOnSelectSelect(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](int32_t index, const std::string& value) {
        ArkUINodeEvent event;
        event.kind = TEXT_ARRAY;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        auto indexStr = std::to_string(index);
        const char* arrayValue[] = {value.c_str(), indexStr.c_str()};
        event.textArrayEvent.subKind = ON_SELECT_SELECT;
        event.textArrayEvent.nativeStringArrayPtr = reinterpret_cast<intptr_t>(arrayValue);
        SendArkUISyncEvent(&event);
    };
    SelectModelNG::SetOnSelect(frameNode, std::move(onEvent));
}
}
} // namespace OHOS::Ace::NG