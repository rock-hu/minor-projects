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
#include "core/interfaces/native/node/alphabet_indexer_modifier.h"
#include "ui/base/ace_type.h"

#include "core/common/resource/resource_object.h"
#include "core/components_ng/pattern/indexer/indexer_model.h"
#include "core/components_ng/pattern/indexer/indexer_model_ng.h"
#include "frameworks/bridge/common/utils/utils.h"
#include "core/components/indexer/indexer_theme.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int FONT_SIZE_INDEX = 0;
constexpr int FONT_WEIGHT_INDEX = 1;
constexpr int FONT_FAMILY_INDEX = 2;
const std::vector<Ace::FontStyle> FONT_STYLES = { Ace::FontStyle::NORMAL, Ace::FontStyle::ITALIC };
constexpr bool DEFAULT_USINGPOPUP = false;
constexpr int32_t DEFAULT_SELECTED = 0;
constexpr Dimension DEFAULT_POPUPHORIZONTALSPACE = -1.0_vp;
constexpr int32_t DEFAULT_ALIGN_STYLE = static_cast<int32_t>(NG::AlignStyle::RIGHT);
constexpr double DEFAULT_POPUP_POSITION_X = 60.0;
constexpr double DEFAULT_POPUP_POSITION_Y = 48.0;
} // namespace

void SetPopupItemFont(ArkUINodeHandle node, ArkUI_Float32 size, int unit, const char* weight)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    Dimension fontSize = Dimension(size, static_cast<OHOS::Ace::DimensionUnit>(unit));
    IndexerModelNG::SetFontSize(frameNode, fontSize);
    IndexerModelNG::SetFontWeight(frameNode, StringUtils::StringToFontWeight(std::string(weight), FontWeight::MEDIUM));
}

void ResetPopupItemFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    auto fontSize = indexerTheme->GetPopupTextSize();
    IndexerModelNG::SetFontSize(frameNode, fontSize);
    auto fontWeight = indexerTheme->GetPopupTextStyle().GetFontWeight();
    IndexerModelNG::SetFontWeight(frameNode, fontWeight);
}

void SetSelectedFont(ArkUINodeHandle node, const char* fontInfo, int32_t style)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, '|', res);
    std::optional<Dimension> fontSize =
        StringUtils::StringToCalcDimension(res[FONT_SIZE_INDEX], false, DimensionUnit::FP);
    std::optional<FontWeight> fontWeight = StringUtils::StringToFontWeight(res[FONT_WEIGHT_INDEX]);
    std::optional<std::vector<std::string>> fontFamily = Framework::ConvertStrToFontFamilies(res[FONT_FAMILY_INDEX]);
    std::optional<Ace::FontStyle> fontStyle = FONT_STYLES[0];
    if (style >= 0 && style < static_cast<int32_t>(FONT_STYLES.size())) {
        fontStyle = FONT_STYLES[style];
    };
    IndexerModelNG::SetSelectedFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void ResetSelectedFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    TextStyle selectTextStyle = indexerTheme->GetSelectTextStyle();
    std::optional<Dimension> fontSize = selectTextStyle.GetFontSize();
    std::optional<FontWeight> fontWeight = selectTextStyle.GetFontWeight();
    std::optional<std::vector<std::string>> fontFamily = selectTextStyle.GetFontFamilies();
    std::optional<Ace::FontStyle> fontStyle = selectTextStyle.GetFontStyle();
    IndexerModelNG::SetSelectedFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void SetPopupFont(ArkUINodeHandle node, const char* fontInfo, int32_t style)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, '|', res);
    std::optional<Dimension> fontSize =
        StringUtils::StringToCalcDimension(res[FONT_SIZE_INDEX], false, DimensionUnit::FP);
    std::optional<FontWeight> fontWeight = StringUtils::StringToFontWeight(res[FONT_WEIGHT_INDEX]);
    std::optional<std::vector<std::string>> fontFamily = Framework::ConvertStrToFontFamilies(res[FONT_FAMILY_INDEX]);
    std::optional<Ace::FontStyle> fontStyle = FONT_STYLES[0];
    if (style >= 0 && style < static_cast<int32_t>(FONT_STYLES.size())) {
        fontStyle = FONT_STYLES[style];
    };
    IndexerModelNG::SetPopupFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void ResetPopupFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    TextStyle popupTextStyle = indexerTheme->GetPopupTextStyle();
    std::optional<Dimension> fontSize = popupTextStyle.GetFontSize();
    std::optional<FontWeight> fontWeight = popupTextStyle.GetFontWeight();
    std::optional<std::vector<std::string>> fontFamily = popupTextStyle.GetFontFamilies();
    std::optional<Ace::FontStyle> fontStyle = popupTextStyle.GetFontStyle();
    IndexerModelNG::SetPopupFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void SetAlphabetIndexerFont(ArkUINodeHandle node, const char* fontInfo, int32_t style)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> res;
    std::string fontValues = std::string(fontInfo);
    StringUtils::StringSplitter(fontValues, '|', res);
    std::optional<Dimension> fontSize =
        StringUtils::StringToCalcDimension(res[FONT_SIZE_INDEX], false, DimensionUnit::FP);
    std::optional<FontWeight> fontWeight = StringUtils::StringToFontWeight(res[FONT_WEIGHT_INDEX]);
    std::optional<std::vector<std::string>> fontFamily = Framework::ConvertStrToFontFamilies(res[FONT_FAMILY_INDEX]);
    std::optional<Ace::FontStyle> fontStyle = FONT_STYLES[0];
    if (style >= 0 && style < static_cast<int32_t>(FONT_STYLES.size())) {
        fontStyle = FONT_STYLES[style];
    };
    IndexerModelNG::SetFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void ResetAlphabetIndexerFont(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    TextStyle defaultTextStyle = indexerTheme->GetDefaultTextStyle();
    std::optional<Dimension> fontSize = defaultTextStyle.GetFontSize();
    std::optional<FontWeight> fontWeight = defaultTextStyle.GetFontWeight();
    std::optional<std::vector<std::string>> fontFamily = defaultTextStyle.GetFontFamilies();
    std::optional<Ace::FontStyle> fontStyle = defaultTextStyle.GetFontStyle();
    IndexerModelNG::SetFont(frameNode, fontSize, fontWeight, fontFamily, fontStyle);
}

void SetPopupItemBackgroundColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupItemBackground(frameNode, Color(color));
    IndexerModelNG::SetPopupItemBackgroundByUser(frameNode, true);
}

void ResetPopupItemBackgroundColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupUnclickedBgAreaColor();
    IndexerModelNG::SetPopupItemBackground(frameNode, color);
    IndexerModelNG::SetPopupItemBackgroundByUser(frameNode, false);
}

void SetAlphabetIndexerColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetColor(frameNode, Color(color));
    IndexerModelNG::SetColorByUser(frameNode, true);
}

void ResetAlphabetIndexerColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetDefaultTextColor();
    IndexerModelNG::SetColor(frameNode, color);
    IndexerModelNG::SetColorByUser(frameNode, false);
}

void SetPopupColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupColor(frameNode, Color(color));
    IndexerModelNG::SetPopupColorByUser(frameNode, true);
}

void ResetPopupColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupTextColor();
    IndexerModelNG::SetPopupColor(frameNode, color);
    IndexerModelNG::SetPopupColorByUser(frameNode, false);
}

void SetAlphabetIndexerSelectedColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetSelectedColor(frameNode, Color(color));
    IndexerModelNG::SetSelectedColorByUser(frameNode, true);
}

void ResetAlphabetIndexerSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetSelectedTextColor();
    IndexerModelNG::SetSelectedColor(frameNode, color);
    IndexerModelNG::SetSelectedColorByUser(frameNode, false);
}

void SetPopupBackground(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupBackground(frameNode, Color(color));
    IndexerModelNG::SetPopupBackgroundByUser(frameNode, true);
}

void ResetPopupBackground(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupBackgroundColor();
    IndexerModelNG::SetPopupBackground(frameNode, color);
    IndexerModelNG::SetPopupBackgroundByUser(frameNode, false);
}

void SetSelectedBackgroundColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetSelectedBackgroundColor(frameNode, Color(color));
    IndexerModelNG::SetSelectedBGColorByUser(frameNode, true);
}

void ResetSelectedBackgroundColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetSelectedBackgroundColor();
    IndexerModelNG::SetSelectedBackgroundColor(frameNode, color);
    IndexerModelNG::SetSelectedBGColorByUser(frameNode, false);
}

void SetPopupUnselectedColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupUnselectedColor(frameNode, Color(color));
    IndexerModelNG::SetPopupUnselectedColorByUser(frameNode, true);
}

void ResetPopupUnselectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupUnselectedTextColor();
    IndexerModelNG::SetPopupUnselectedColor(frameNode, color);
    IndexerModelNG::SetPopupUnselectedColorByUser(frameNode, false);
}

void SetAlignStyle(ArkUINodeHandle node, int32_t value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAlignStyle(frameNode, value);
}

void ResetAlignStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAlignStyle(frameNode, DEFAULT_ALIGN_STYLE);
}

void SetPopupHorizontalSpace(ArkUINodeHandle node, ArkUI_Float32 value, int unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupHorizontalSpace(frameNode, Dimension(value, static_cast<OHOS::Ace::DimensionUnit>(unit)));
}

void ResetPopupHorizontalSpace(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupHorizontalSpace(frameNode, DEFAULT_POPUPHORIZONTALSPACE);
}

void SetUsingPopup(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetUsingPopup(frameNode, value);
}

void ResetUsingPopup(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetUsingPopup(frameNode, DEFAULT_USINGPOPUP);
}

void SetAlphabetIndexerSelected(ArkUINodeHandle node, int32_t value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetSelected(frameNode, value);
}

void ResetAlphabetIndexerSelected(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetSelected(frameNode, DEFAULT_SELECTED);
}

void SetPopupSelectedColor(ArkUINodeHandle node, uint32_t color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupSelectedColor(frameNode, Color(color));
    IndexerModelNG::SetPopupSelectedColorByUser(frameNode, true);
}
void ResetPopupSelectedColor(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupSelectedTextColor();
    IndexerModelNG::SetPopupSelectedColor(frameNode, color);
    IndexerModelNG::SetPopupSelectedColorByUser(frameNode, false);
}

void SetItemSize(ArkUINodeHandle node, ArkUI_Float32 value, int unit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetItemSize(frameNode, Dimension(value, static_cast<OHOS::Ace::DimensionUnit>(unit)));
}

void ResetItemSize(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetItemSize(frameNode, DEFAULT_ITEM_SIZE);
}

void SetPopupPosition(ArkUINodeHandle node, ArkUI_Float32 xValue, int xUnit, ArkUI_Float32 yValue, int yUnit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupPositionX(frameNode, Dimension(xValue, static_cast<OHOS::Ace::DimensionUnit>(xUnit)));
    IndexerModelNG::SetPopupPositionY(frameNode, Dimension(yValue, static_cast<OHOS::Ace::DimensionUnit>(yUnit)));
}

void ResetPopupPosition(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupPositionX(frameNode, Dimension(DEFAULT_POPUP_POSITION_X, DimensionUnit::VP));
    IndexerModelNG::SetPopupPositionY(frameNode, Dimension(DEFAULT_POPUP_POSITION_Y, DimensionUnit::VP));
}

void SetPopupItemBorderRadius(ArkUINodeHandle node, ArkUI_Float32 radiusValue, ArkUI_Int32 radiusUnit,
    ArkUI_Float32 popupValue, ArkUI_Int32 popupUnit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupItemBorderRadius(
        frameNode, Dimension(radiusValue, static_cast<OHOS::Ace::DimensionUnit>(radiusUnit)));
    IndexerModelNG::SetPopupBorderRadius(
        frameNode, Dimension(popupValue, static_cast<OHOS::Ace::DimensionUnit>(popupUnit)));
}
void ResetPopupItemBorderRadius(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto radius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    auto popupRadius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    radius.SetValue(POPUP_ITEM_DEFAULT_RADIUS);
    radius.SetUnit(DimensionUnit::VP);
    popupRadius.SetValue(radius.Value() + RADIUS_OFFSET);
    popupRadius.SetUnit(DimensionUnit::VP);
    IndexerModelNG::SetPopupItemBorderRadius(frameNode, radius);
    IndexerModelNG::SetPopupBorderRadius(frameNode, popupRadius);
}
void SetItemBorderRadius(ArkUINodeHandle node, ArkUI_Float32 radiusValue, ArkUI_Int32 radiusUnit,
    ArkUI_Float32 indexerValue, ArkUI_Int32 indexerUnit)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetItemBorderRadius(
        frameNode, Dimension(radiusValue, static_cast<OHOS::Ace::DimensionUnit>(radiusUnit)));
    IndexerModelNG::SetIndexerBorderRadius(
        frameNode, Dimension(indexerValue, static_cast<OHOS::Ace::DimensionUnit>(indexerUnit)));
}
void ResetItemBorderRadius(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto radius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    auto indexerRadius = Dimension(ZERO_RADIUS, DimensionUnit::VP);
    radius.SetValue(ITEM_DEFAULT_RADIUS);
    radius.SetUnit(DimensionUnit::VP);
    indexerRadius.SetValue(radius.Value() + RADIUS_OFFSET);
    indexerRadius.SetUnit(DimensionUnit::VP);
    IndexerModelNG::SetIndexerBorderRadius(frameNode, indexerRadius);
    IndexerModelNG::SetItemBorderRadius(frameNode, radius);
}
void SetPopupBackgroundBlurStyle(ArkUINodeHandle node, ArkUI_Uint32 value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    BlurStyleOption styleOption;
    if (value >= static_cast<ArkUI_Uint32>(BlurStyle::NO_MATERIAL) &&
        value <= static_cast<ArkUI_Uint32>(BlurStyle::COMPONENT_ULTRA_THICK)) {
        styleOption.blurStyle = static_cast<BlurStyle>(value);
    }
    IndexerModelNG::SetPopupBackgroundBlurStyle(frameNode, styleOption);
}
void ResetPopupBackgroundBlurStyle(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    BlurStyleOption styleOption;
    styleOption.blurStyle = BlurStyle::COMPONENT_REGULAR;
    IndexerModelNG::SetPopupBackgroundBlurStyle(frameNode, styleOption);
}
void SetPopupTitleBackground(ArkUINodeHandle node, ArkUI_Uint32 color)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetPopupTitleBackground(frameNode, Color(color));
    IndexerModelNG::SetPopupTitleBackgroundByUser(frameNode, true);
}
void ResetPopupTitleBackground(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto indexerTheme = pipeline->GetTheme<IndexerTheme>();
    CHECK_NULL_VOID(indexerTheme);
    Color color = indexerTheme->GetPopupTitleBackground();
    IndexerModelNG::SetPopupTitleBackground(frameNode, color);
    IndexerModelNG::SetPopupTitleBackgroundByUser(frameNode, false);
}

void SetAdaptiveWidth(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAdaptiveWidth(frameNode, true);
}

void ResetAdaptiveWidth(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAdaptiveWidth(frameNode, false);
}

void SetArrayValue(ArkUINodeHandle node, ArkUI_CharPtr* value, ArkUI_Uint32 length)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(value);
    std::vector<std::string> valueVector;
    for (uint32_t i = 0; i < length; i++) {
        valueVector.emplace_back(value[i]);
    }
    IndexerModelNG::SetArrayValue(frameNode, valueVector);
}

void ResetArrayValue(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    std::vector<std::string> valueVector;
    IndexerModelNG::SetArrayValue(frameNode, valueVector);
}

void SetAutoCollapse(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAutoCollapse(frameNode, value);
}

void ResetAutoCollapse(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetAutoCollapse(frameNode, true);
}

void SetEnableHapticFeedback(ArkUINodeHandle node, ArkUI_Bool value)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetEnableHapticFeedback(frameNode, value);
}

void ResetEnableHapticFeedback(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetEnableHapticFeedback(frameNode, true);
}

void SetOnIndexerSelect(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto onSelectEvent = reinterpret_cast<std::function<void(int32_t)>*>(extraParam);
        IndexerModelNG::SetOnSelected(frameNode, std::move(*onSelectEvent));
    } else {
        IndexerModelNG::SetOnSelected(frameNode, nullptr);
    }
}

void ResetOnIndexerSelect(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetOnSelected(frameNode, nullptr);
}

void SetOnRequestPopupData(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto onRequestPopupData = reinterpret_cast<std::function<std::vector<std::string>(int32_t)>*>(extraParam);
        IndexerModelNG::SetOnRequestPopupData(frameNode, std::move(*onRequestPopupData));
    } else {
        IndexerModelNG::SetOnRequestPopupData(frameNode, nullptr);
    }
}

void ResetOnRequestPopupData(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetOnRequestPopupData(frameNode, nullptr);
}

void SetOnPopupSelected(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    if (extraParam) {
        auto onSelectEvent = reinterpret_cast<std::function<void(int32_t)>*>(extraParam);
        IndexerModelNG::SetOnPopupSelected(frameNode, std::move(*onSelectEvent));
    } else {
        IndexerModelNG::SetOnPopupSelected(frameNode, nullptr);
    }
}

void ResetOnPopupSelected(ArkUINodeHandle node)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    IndexerModelNG::SetOnPopupSelected(frameNode, nullptr);
}

void CreateWithResourceObj(ArkUINodeHandle node, ArkUI_Int32 jsType, void* resObj)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto* resourceObj = reinterpret_cast<ResourceObject*>(resObj);
    IndexerModelNG::CreateWithResourceObj(
        frameNode, static_cast<IndexerJsResourceType>(jsType), AceType::Claim(resourceObj));
}

namespace NodeModifier {
const ArkUIAlphabetIndexerModifier* GetAlphabetIndexerModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const ArkUIAlphabetIndexerModifier modifier = {
        .setPopupItemFont = SetPopupItemFont,
        .resetPopupItemFont = ResetPopupItemFont,
        .setSelectedFont = SetSelectedFont,
        .resetSelectedFont = ResetSelectedFont,
        .setPopupFont = SetPopupFont,
        .resetPopupFont = ResetPopupFont,
        .setAlphabetIndexerFont = SetAlphabetIndexerFont,
        .resetAlphabetIndexerFont = ResetAlphabetIndexerFont,
        .setPopupItemBackgroundColor = SetPopupItemBackgroundColor,
        .resetPopupItemBackgroundColor = ResetPopupItemBackgroundColor,
        .setAlphabetIndexerColor = SetAlphabetIndexerColor,
        .resetAlphabetIndexerColor = ResetAlphabetIndexerColor,
        .setPopupColor = SetPopupColor,
        .resetPopupColor = ResetPopupColor,
        .setAlphabetIndexerSelectedColor = SetAlphabetIndexerSelectedColor,
        .resetAlphabetIndexerSelectedColor = ResetAlphabetIndexerSelectedColor,
        .setPopupBackground = SetPopupBackground,
        .resetPopupBackground = ResetPopupBackground,
        .setSelectedBackgroundColor = SetSelectedBackgroundColor,
        .resetSelectedBackgroundColor = ResetSelectedBackgroundColor,
        .setPopupUnselectedColor = SetPopupUnselectedColor,
        .resetPopupUnselectedColor = ResetPopupUnselectedColor,
        .setAlignStyle = SetAlignStyle,
        .resetAlignStyle = ResetAlignStyle,
        .setUsingPopup = SetUsingPopup,
        .resetUsingPopup = ResetUsingPopup,
        .setAlphabetIndexerSelected = SetAlphabetIndexerSelected,
        .resetAlphabetIndexerSelected = ResetAlphabetIndexerSelected,
        .setPopupHorizontalSpace = SetPopupHorizontalSpace,
        .resetPopupHorizontalSpace = ResetPopupHorizontalSpace,
        .setPopupSelectedColor = SetPopupSelectedColor,
        .resetPopupSelectedColor = ResetPopupSelectedColor,
        .setItemSize = SetItemSize,
        .resetItemSize = ResetItemSize,
        .setPopupPosition = SetPopupPosition,
        .resetPopupPosition = ResetPopupPosition,
        .setPopupItemBorderRadius = SetPopupItemBorderRadius,
        .resetPopupItemBorderRadius = ResetPopupItemBorderRadius,
        .setItemBorderRadius = SetItemBorderRadius,
        .resetItemBorderRadius = ResetItemBorderRadius,
        .setPopupBackgroundBlurStyle = SetPopupBackgroundBlurStyle,
        .resetPopupBackgroundBlurStyle = ResetPopupBackgroundBlurStyle,
        .setPopupTitleBackground = SetPopupTitleBackground,
        .resetPopupTitleBackground = ResetPopupTitleBackground,
        .setAdaptiveWidth = SetAdaptiveWidth,
        .resetAdaptiveWidth = ResetAdaptiveWidth,
        .setArrayValue = SetArrayValue,
        .resetArrayValue = ResetArrayValue,
        .setAutoCollapse = SetAutoCollapse,
        .resetAutoCollapse = ResetAutoCollapse,
        .setEnableHapticFeedback = SetEnableHapticFeedback,
        .resetEnableHapticFeedback = ResetEnableHapticFeedback,
        .setOnIndexerSelect = SetOnIndexerSelect,
        .resetOnIndexerSelect = ResetOnIndexerSelect,
        .setOnRequestPopupData = SetOnRequestPopupData,
        .resetOnRequestPopupData = ResetOnRequestPopupData,
        .setOnPopupSelected = SetOnPopupSelected,
        .resetOnPopupSelected = ResetOnPopupSelected,
        .createWithResourceObj = CreateWithResourceObj
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line

    return &modifier;
}

const CJUIAlphabetIndexerModifier* GetCJUIAlphabetIndexerModifier()
{
    CHECK_INITIALIZED_FIELDS_BEGIN(); // don't move this line
    static const CJUIAlphabetIndexerModifier modifier = {
        .setPopupItemFont = SetPopupItemFont,
        .resetPopupItemFont = ResetPopupItemFont,
        .setSelectedFont = SetSelectedFont,
        .resetSelectedFont = ResetSelectedFont,
        .setPopupFont = SetPopupFont,
        .resetPopupFont = ResetPopupFont,
        .setAlphabetIndexerFont = SetAlphabetIndexerFont,
        .resetAlphabetIndexerFont = ResetAlphabetIndexerFont,
        .setPopupItemBackgroundColor = SetPopupItemBackgroundColor,
        .resetPopupItemBackgroundColor = ResetPopupItemBackgroundColor,
        .setAlphabetIndexerColor = SetAlphabetIndexerColor,
        .resetAlphabetIndexerColor = ResetAlphabetIndexerColor,
        .setPopupColor = SetPopupColor,
        .resetPopupColor = ResetPopupColor,
        .setAlphabetIndexerSelectedColor = SetAlphabetIndexerSelectedColor,
        .resetAlphabetIndexerSelectedColor = ResetAlphabetIndexerSelectedColor,
        .setPopupBackground = SetPopupBackground,
        .resetPopupBackground = ResetPopupBackground,
        .setSelectedBackgroundColor = SetSelectedBackgroundColor,
        .resetSelectedBackgroundColor = ResetSelectedBackgroundColor,
        .setPopupUnselectedColor = SetPopupUnselectedColor,
        .resetPopupUnselectedColor = ResetPopupUnselectedColor,
        .setAlignStyle = SetAlignStyle,
        .resetAlignStyle = ResetAlignStyle,
        .setUsingPopup = SetUsingPopup,
        .resetUsingPopup = ResetUsingPopup,
        .setAlphabetIndexerSelected = SetAlphabetIndexerSelected,
        .resetAlphabetIndexerSelected = ResetAlphabetIndexerSelected,
        .setPopupHorizontalSpace = SetPopupHorizontalSpace,
        .resetPopupHorizontalSpace = ResetPopupHorizontalSpace,
        .setPopupSelectedColor = SetPopupSelectedColor,
        .resetPopupSelectedColor = ResetPopupSelectedColor,
        .setItemSize = SetItemSize,
        .resetItemSize = ResetItemSize,
        .setPopupPosition = SetPopupPosition,
        .resetPopupPosition = ResetPopupPosition,
        .setPopupItemBorderRadius = SetPopupItemBorderRadius,
        .resetPopupItemBorderRadius = ResetPopupItemBorderRadius,
        .setItemBorderRadius = SetItemBorderRadius,
        .resetItemBorderRadius = ResetItemBorderRadius,
        .setPopupBackgroundBlurStyle = SetPopupBackgroundBlurStyle,
        .resetPopupBackgroundBlurStyle = ResetPopupBackgroundBlurStyle,
        .setPopupTitleBackground = SetPopupTitleBackground,
        .resetPopupTitleBackground = ResetPopupTitleBackground,
        .setAdaptiveWidth = SetAdaptiveWidth,
        .resetAdaptiveWidth = ResetAdaptiveWidth,
        .setAutoCollapse = SetAutoCollapse,
        .resetAutoCollapse = ResetAutoCollapse,
        .setEnableHapticFeedback = SetEnableHapticFeedback,
        .resetEnableHapticFeedback = ResetEnableHapticFeedback,
        .setArrayValue = SetArrayValue,
        .resetArrayValue = ResetArrayValue,
    };
    CHECK_INITIALIZED_FIELDS_END(modifier, 0, 0, 0); // don't move this line

    return &modifier;
}

void SetOnIndexerSelected(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](const int32_t selected) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_ALPHABET_INDEXER_SELECTED;
        event.componentAsyncEvent.data[0].i32 = selected;
        SendArkUISyncEvent(&event);
    };
    IndexerModelNG::SetOnSelected(frameNode, std::move(onEvent));
}

void SetOnIndexerRequestPopupData(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](const int32_t selected) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_ALPHABET_INDEXER_REQUEST_POPUP_DATA;
        event.componentAsyncEvent.data[0].i32 = selected;
        SendArkUISyncEvent(&event);
        char** valueArray = reinterpret_cast<char**>(event.textArrayEvent.nativeStringArrayPtr);
        ArkUI_Int32 length = event.textArrayEvent.length;
        std::vector<std::string> valueVector;
        if (length != 0 && valueArray != nullptr) {
            for (ArkUI_Int32 i = 0; i < length; i++) {
                valueVector.emplace_back(valueArray[i]);
                free(valueArray[i]);
            }
            free(valueArray);
        }
        return valueVector;
    };
    IndexerModelNG::SetOnRequestPopupData(frameNode, std::move(onEvent));
}

void SetOnIndexerPopupSelected(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](const int32_t selected) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_ALPHABET_INDEXER_POPUP_SELECTED;
        event.componentAsyncEvent.data[0].i32 = selected;
        SendArkUISyncEvent(&event);
    };
    IndexerModelNG::SetOnPopupSelected(frameNode, std::move(onEvent));
}

void SetIndexerChangeEvent(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](const int32_t selected) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_ALPHABET_INDEXER_CHANGE_EVENT;
        event.componentAsyncEvent.data[0].i32 = selected;
        SendArkUISyncEvent(&event);
    };
    IndexerModelNG::SetChangeEvent(frameNode, std::move(onEvent));
}

void SetIndexerCreatChangeEvent(ArkUINodeHandle node, void* extraParam)
{
    auto* frameNode = reinterpret_cast<FrameNode*>(node);
    CHECK_NULL_VOID(frameNode);
    auto onEvent = [node, extraParam](const int32_t selected) {
        ArkUINodeEvent event;
        event.kind = COMPONENT_ASYNC_EVENT;
        event.extraParam = reinterpret_cast<intptr_t>(extraParam);
        event.componentAsyncEvent.subKind = ON_ALPHABET_INDEXER_CREAT_CHANGE_EVENT;
        event.componentAsyncEvent.data[0].i32 = selected;
        SendArkUISyncEvent(&event);
    };
    IndexerModelNG::SetCreatChangeEvent(frameNode, std::move(onEvent));
}
}
} // namespace OHOS::Ace::NG
