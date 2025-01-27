/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/text_picker/textpicker_model_ng.h"

#include <securec.h>

#include "base/geometry/dimension.h"
#include "base/geometry/ng/size_t.h"
#include "base/utils/utils.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/button/button_pattern.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/picker/picker_type_define.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_column_pattern.h"
#include "core/components_ng/pattern/text_picker/textpicker_event_hub.h"
#include "core/components_ng/pattern/text_picker/textpicker_layout_property.h"
#include "core/components_ng/pattern/text_picker/textpicker_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float PICKER_MAXFONTSCALE = 1.0f;
constexpr bool DEFAULT_ENABLE_HAPTIC_FEEDBACK = true;
const int32_t BUFFER_NODE_NUMBER = 2;

void SetDialogProperties(DialogProperties& properties, TextPickerDialog& textPickerDialog,
                         const RefPtr<DialogTheme>& theme)
{
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        properties.alignment = theme->GetAlignment();
    }
    if (textPickerDialog.alignment.has_value()) {
        properties.alignment = textPickerDialog.alignment.value();
    }

    if (textPickerDialog.backgroundColor.has_value()) {
        properties.backgroundColor = textPickerDialog.backgroundColor.value();
    }
    if (textPickerDialog.backgroundBlurStyle.has_value()) {
        properties.backgroundBlurStyle = textPickerDialog.backgroundBlurStyle.value();
    }
    if (textPickerDialog.shadow.has_value()) {
        properties.shadow = textPickerDialog.shadow.value();
    }
    properties.customStyle = false;
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        properties.offset = DimensionOffset(Offset(0, -theme->GetMarginBottom().ConvertToPx()));
    }
    if (textPickerDialog.offset.has_value()) {
        properties.offset = textPickerDialog.offset.value();
    }

    properties.maskRect = textPickerDialog.maskRect;
    properties.enableHoverMode = textPickerDialog.enableHoverMode;
    if (textPickerDialog.hoverModeArea.has_value()) {
        properties.hoverModeArea = textPickerDialog.hoverModeArea.value();
    }
}

using TextPickerGetTextStyleFunc = const std::unique_ptr<FontStyle>& (TextPickerLayoutProperty::*)() const;
void ResetTextPickerTextStyleColor(FrameNode* frameNode, TextPickerGetTextStyleFunc getTextStyleFunc)
{
    CHECK_NULL_VOID(frameNode);
    auto property = frameNode->GetLayoutPropertyPtr<TextPickerLayoutProperty>();
    CHECK_NULL_VOID(property);
    auto& textStyle = (property->*getTextStyleFunc)();
    CHECK_NULL_VOID(textStyle);
    if (textStyle->HasTextColor()) {
        textStyle->ResetTextColor();
        property->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE_SELF);
    }
}
}

void TextPickerModelNG::Create(RefPtr<PickerTheme> pickerTheme, uint32_t columnKind)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::TEXT_PICKER_ETS_TAG, nodeId);
    auto textPickerNode = FrameNode::GetOrCreateFrameNode(
        V2::TEXT_PICKER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TextPickerPattern>(); });
    auto textPickerPattern = textPickerNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetColumnsKind(columnKind);
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto dialogTheme = pipeline->GetTheme<DialogTheme>();
    CHECK_NULL_VOID(dialogTheme);
    textPickerPattern->SetBackgroundColor(dialogTheme->GetBackgroundColor());
    CHECK_NULL_VOID(pickerTheme);
    uint32_t showCount = pickerTheme->GetShowOptionCount() + BUFFER_NODE_NUMBER;

    if (textPickerNode->GetChildren().empty()) {
        auto columnNode = CreateColumnNode(columnKind, showCount);
        auto stackNode = CreateStackNode();
        auto buttonNode = CreateButtonNode();
        auto columnBlendNode = CreateColumnNode();
        buttonNode->MountToParent(stackNode);
        columnNode->MountToParent(columnBlendNode);
        columnBlendNode->MountToParent(stackNode);
        columnNode->MarkModifyDone();
        columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
        layoutProperty->UpdateAlignment(Alignment::CENTER);
        stackNode->MountToParent(textPickerNode);
    }
    stack->Push(textPickerNode);
    options_.clear();
}

void TextPickerModelNG::SetDefaultAttributes(const RefPtr<PickerTheme>& pickerTheme)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto selectedStyle = pickerTheme->GetOptionStyle(true, false);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontSize,
        ConvertFontScaleValue(selectedStyle.GetFontSize()));
    ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetSelectedTextStyle);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedWeight, selectedStyle.GetFontWeight());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontFamily, selectedStyle.GetFontFamilies());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontStyle, selectedStyle.GetFontStyle());

    auto disappearStyle = pickerTheme->GetDisappearOptionStyle();
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontSize,
        ConvertFontScaleValue(disappearStyle.GetFontSize()));
    ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetDisappearTextStyle);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearWeight, disappearStyle.GetFontWeight());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontFamily, disappearStyle.GetFontFamilies());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontStyle, disappearStyle.GetFontStyle());

    auto normalStyle = pickerTheme->GetOptionStyle(false, false);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
        ConvertFontScaleValue(normalStyle.GetFontSize()));
    ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetTextStyle);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Weight, normalStyle.GetFontWeight());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontFamily, normalStyle.GetFontFamilies());
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontStyle, normalStyle.GetFontStyle());

    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, CanLoop, true);
}

RefPtr<FrameNode> TextPickerModelNG::CreateColumnNode(uint32_t columnKind, uint32_t showCount)
{
    auto columnNode =
        FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<TextPickerColumnPattern>(); });
    if (columnKind == ICON) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto row = FrameNode::CreateFrameNode(
                V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<LinearLayoutPattern>(false));
            CHECK_NULL_RETURN(row, nullptr);
            auto layoutProps = row->GetLayoutProperty<LinearLayoutProperty>();
            CHECK_NULL_RETURN(layoutProps, nullptr);
            layoutProps->UpdateMainAxisAlign(FlexAlign::CENTER);
            layoutProps->UpdateCrossAxisAlign(FlexAlign::CENTER);

            auto imageNode = FrameNode::CreateFrameNode(
                V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<ImagePattern>());
            CHECK_NULL_RETURN(imageNode, nullptr);
            imageNode->MountToParent(row);
            row->MountToParent(columnNode);
        }
    } else if (columnKind == TEXT) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto textNode = FrameNode::CreateFrameNode(
                V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<TextPattern>());
            CHECK_NULL_RETURN(textNode, nullptr);
            textNode->MountToParent(columnNode);
        }
    } else if (columnKind == MIXTURE) {
        for (uint32_t index = 0; index < showCount; index++) {
            auto row = FrameNode::CreateFrameNode(
                V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<LinearLayoutPattern>(false));
            CHECK_NULL_RETURN(row, nullptr);

            auto imageNode = FrameNode::CreateFrameNode(
                V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<ImagePattern>());
            CHECK_NULL_RETURN(imageNode, nullptr);
            imageNode->MountToParent(row);

            auto textNode = FrameNode::CreateFrameNode(
                V2::TEXT_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                AceType::MakeRefPtr<TextPattern>());
            CHECK_NULL_RETURN(textNode, nullptr);
            textNode->MountToParent(row);
            row->MountToParent(columnNode);
        }
    }
    return columnNode;
}

RefPtr<FrameNode> TextPickerModelNG::CreateStackNode()
{
    auto stackId = ElementRegister::GetInstance()->MakeUniqueId();
    return FrameNode::GetOrCreateFrameNode(
        V2::STACK_ETS_TAG, stackId, []() { return AceType::MakeRefPtr<StackPattern>(); });
}

RefPtr<FrameNode> TextPickerModelNG::CreateColumnNode()
{
    auto columnId = ElementRegister::GetInstance()->MakeUniqueId();
    return FrameNode::GetOrCreateFrameNode(
        V2::COLUMN_ETS_TAG, columnId, []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });
}

RefPtr<FrameNode> TextPickerModelNG::CreateButtonNode()
{
    auto buttonId = ElementRegister::GetInstance()->MakeUniqueId();
    return FrameNode::GetOrCreateFrameNode(
        V2::BUTTON_ETS_TAG, buttonId, []() { return AceType::MakeRefPtr<ButtonPattern>(); });
}

RefPtr<FrameNode> TextPickerModelNG::CreateFrameNode(int32_t nodeId)
{
    auto textPickerNode = FrameNode::GetOrCreateFrameNode(
        V2::TEXT_PICKER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TextPickerPattern>(); });
    auto textPickerPattern = textPickerNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetColumnsKind(TEXT);
    auto pipeline = PipelineBase::GetCurrentContextSafely();
    CHECK_NULL_RETURN(pipeline, textPickerNode);
    auto pickerTheme = pipeline->GetTheme<PickerTheme>(textPickerNode->GetThemeScopeId());
    CHECK_NULL_RETURN(pickerTheme, textPickerNode);
    showCount_ = BUFFER_NODE_NUMBER + pickerTheme->GetShowOptionCount();
    rangeValue_.clear();
    SetDefaultAttributes(textPickerNode, pickerTheme);
    return textPickerNode;
}

void TextPickerModelNG::SetSelected(uint32_t value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetSelected(value);
    std::vector<uint32_t> values;
    values.emplace_back(value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Selected, value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedIndex, values);
}

void TextPickerModelNG::SetColumnWidths(const std::vector<Dimension>& widths)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetColumnWidths(widths);
}

void TextPickerModelNG::SetColumnWidths(FrameNode* frameNode, const std::vector<Dimension>& widths)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetColumnWidths(widths);
}

std::vector<Dimension> TextPickerModelNG::GetColumnWidths(FrameNode* frameNode)
{
    std::vector<Dimension> columnWidths;
    CHECK_NULL_RETURN(frameNode, columnWidths);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    columnWidths = textPickerPattern->GetColumnWidths();
    return columnWidths;
}

void TextPickerModelNG::SetRange(const std::vector<NG::RangeContent>& value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetRange(value);
    for (auto& range : value) {
        rangeValue_.emplace_back(std::move(range));
    }
}

void TextPickerModelNG::SetDefaultPickerItemHeight(const Dimension& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultPickerItemHeight, value);
}

void TextPickerModelNG::SetGradientHeight(const Dimension& value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetGradientHeight(value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, GradientHeight, value);
}

void TextPickerModelNG::SetCanLoop(const bool value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetCanLoop(value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, CanLoop, value);
}

void TextPickerModelNG::SetBackgroundColor(const Color& color)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetBackgroundColor(color);
}

void TextPickerModelNG::SetDisableTextStyleAnimation(const bool value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetDisableTextStyleAnimation(value);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisableTextStyleAnimation, value);
}

void TextPickerModelNG::SetDisappearTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto disappearStyle = pickerTheme->GetDisappearOptionStyle();
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontSize,
            ConvertFontScaleValue(value.fontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearFontSize,
            ConvertFontScaleValue(disappearStyle.GetFontSize()));
    }
    if (value.textColor.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisappearColor, value.textColor.value());
    } else {
        ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetDisappearTextStyle);
    }
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearWeight, value.fontWeight.value_or(disappearStyle.GetFontWeight()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontFamily, value.fontFamily.value_or(disappearStyle.GetFontFamilies()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontStyle, value.fontStyle.value_or(disappearStyle.GetFontStyle()));
}

void TextPickerModelNG::SetNormalTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto normalStyle = pickerTheme->GetOptionStyle(false, false);
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
            ConvertFontScaleValue(value.fontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
            ConvertFontScaleValue(normalStyle.GetFontSize()));
    }
    if (value.textColor.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Color, value.textColor.value());
    } else {
        ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetTextStyle);
    }
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, Weight, value.fontWeight.value_or(normalStyle.GetFontWeight()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, FontFamily, value.fontFamily.value_or(normalStyle.GetFontFamilies()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, FontStyle, value.fontStyle.value_or(normalStyle.GetFontStyle()));
}

void TextPickerModelNG::SetSelectedTextStyle(const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto selectedStyle = pickerTheme->GetOptionStyle(true, false);
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontSize,
            ConvertFontScaleValue(value.fontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontSize,
            ConvertFontScaleValue(selectedStyle.GetFontSize()));
    }
    if (value.textColor.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedColor, value.textColor.value());
    } else {
        ResetTextPickerTextStyleColor(frameNode, &TextPickerLayoutProperty::GetSelectedTextStyle);
    }
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedWeight, value.fontWeight.value_or(selectedStyle.GetFontWeight()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontFamily, value.fontFamily.value_or(selectedStyle.GetFontFamilies()));
    ACE_UPDATE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontStyle, value.fontStyle.value_or(selectedStyle.GetFontStyle()));
}

void TextPickerModelNG::SetDefaultTextStyle(const RefPtr<TextTheme>& textTheme, const NG::PickerTextStyle& value)
{
    CHECK_NULL_VOID(textTheme);
    auto textStyle = textTheme->GetTextStyle();

    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontSize,
            ConvertFontScaleValue(value.fontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontSize,
            ConvertFontScaleValue(textStyle.GetFontSize()));
    }
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultColor,
        value.textColor.value_or(textStyle.GetTextColor()));
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultWeight,
        value.fontWeight.value_or(textStyle.GetFontWeight()));
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontFamily,
        value.fontFamily.value_or(textStyle.GetFontFamilies()));
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontStyle,
        value.fontStyle.value_or(textStyle.GetFontStyle()));
    if (value.minFontSize.has_value() && value.minFontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMinFontSize,
            ConvertFontScaleValue(value.minFontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMinFontSize, Dimension());
    }
    if (value.maxFontSize.has_value() && value.maxFontSize->IsValid()) {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMaxFontSize,
            ConvertFontScaleValue(value.maxFontSize.value()));
    } else {
        ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMaxFontSize, Dimension());
    }
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultTextOverflow,
        value.textOverflow.value_or(textStyle.GetTextOverflow()));
}

void TextPickerModelNG::HasUserDefinedDisappearFontFamily(bool isUserDefined)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->HasUserDefinedDisappearFontFamily(isUserDefined);
}

void TextPickerModelNG::HasUserDefinedNormalFontFamily(bool isUserDefined)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->HasUserDefinedNormalFontFamily(isUserDefined);
}

void TextPickerModelNG::HasUserDefinedSelectedFontFamily(bool isUserDefined)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->HasUserDefinedSelectedFontFamily(isUserDefined);
}

void TextPickerModelNG::SetOnCascadeChange(TextCascadeChangeEvent&& onChange)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(std::move(onChange));
}

void TextPickerModelNG::SetOnScrollStop(TextCascadeChangeEvent&& onScrollStop)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnScrollStop(std::move(onScrollStop));
}

void TextPickerModelNG::SetOnEnterSelectedArea(TextCascadeChangeEvent&& onEnterSelectedArea)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnEnterSelectedArea(std::move(onEnterSelectedArea));
}

void TextPickerModelNG::SetValue(const std::string& value)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Value, value);
}

PickerTextStyle TextPickerModelNG::getDisappearTextStyle(FrameNode* frameNode)
{
    PickerTextStyle pickerTextStyle;
    CHECK_NULL_RETURN(frameNode, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_RETURN(context, pickerTextStyle);
    auto theme = context->GetTheme<PickerTheme>();
    CHECK_NULL_RETURN(theme, pickerTextStyle);
    auto style = theme->GetDisappearOptionStyle();
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, DisappearFontSize, pickerTextStyle.fontSize, frameNode, style.GetFontSize());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, DisappearColor, pickerTextStyle.textColor, frameNode, style.GetTextColor());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, DisappearWeight, pickerTextStyle.fontWeight, frameNode, style.GetFontWeight());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, DisappearFontFamily,
        pickerTextStyle.fontFamily, frameNode, style.GetFontFamilies());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, DisappearFontStyle,
        pickerTextStyle.fontStyle, frameNode, style.GetFontStyle());
    return pickerTextStyle;
}

PickerTextStyle TextPickerModelNG::getNormalTextStyle(FrameNode* frameNode)
{
    PickerTextStyle pickerTextStyle;
    CHECK_NULL_RETURN(frameNode, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_RETURN(context, pickerTextStyle);
    auto theme = context->GetTheme<PickerTheme>();
    CHECK_NULL_RETURN(theme, pickerTextStyle);
    auto style = theme->GetOptionStyle(false, false);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, FontSize, pickerTextStyle.fontSize, frameNode, style.GetFontSize());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, Color, pickerTextStyle.textColor, frameNode, style.GetTextColor());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, Weight, pickerTextStyle.fontWeight, frameNode, style.GetFontWeight());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, FontFamily,
        pickerTextStyle.fontFamily, frameNode, style.GetFontFamilies());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, FontStyle,
        pickerTextStyle.fontStyle, frameNode, style.GetFontStyle());
    return pickerTextStyle;
}

PickerTextStyle TextPickerModelNG::getSelectedTextStyle(FrameNode* frameNode)
{
    PickerTextStyle pickerTextStyle;
    CHECK_NULL_RETURN(frameNode, pickerTextStyle);
    auto context = frameNode->GetContext();
    CHECK_NULL_RETURN(context, pickerTextStyle);
    auto theme = context->GetTheme<PickerTheme>();
    CHECK_NULL_RETURN(theme, pickerTextStyle);
    auto style = theme->GetOptionStyle(true, false);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, SelectedFontSize, pickerTextStyle.fontSize, frameNode, style.GetFontSize());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, SelectedColor, pickerTextStyle.textColor, frameNode, style.GetTextColor());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        TextPickerLayoutProperty, SelectedWeight, pickerTextStyle.fontWeight, frameNode, style.GetFontWeight());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, SelectedFontFamily,
        pickerTextStyle.fontFamily, frameNode, style.GetFontFamilies());
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(TextPickerLayoutProperty, SelectedFontStyle,
        pickerTextStyle.fontStyle, frameNode, style.GetFontStyle());
    return pickerTextStyle;
}

int32_t TextPickerModelNG::getTextPickerSelectedIndex(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    return frameNode->GetLayoutProperty<TextPickerLayoutProperty>()->GetSelectedValue(0);
}

void TextPickerModelNG::MultiInit(const RefPtr<PickerTheme> pickerTheme)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    auto textPickerNode = FrameNode::GetOrCreateFrameNode(
        V2::TEXT_PICKER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TextPickerPattern>(); });
    auto textPickerPattern = textPickerNode->GetPattern<TextPickerPattern>();

    CHECK_NULL_VOID(pickerTheme);
    showCount_ = pickerTheme->GetShowOptionCount() + BUFFER_NODE_NUMBER;
    stack->Push(textPickerNode);
    rangeValue_.clear();
}

void TextPickerModelNG::SetIsCascade(bool isCascade)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    isCascade_ = isCascade;
    textPickerPattern->SetIsCascade(isCascade_);
}

void TextPickerModelNG::SetHasSelectAttr(bool value)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetHasSelectAttr(value);
}

void TextPickerModelNG::SetUnCascadeColumns(const std::vector<NG::TextCascadePickerOptions>& options)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    if (frameNode->GetChildren().empty()) {
        for (uint32_t i = 0; i < options.size(); i++) {
            auto columnNode = CreateColumnNode(NG::TEXT, showCount_);
            auto stackNode = CreateStackNode();
            auto buttonNode = CreateButtonNode();
            auto columnBlendNode = CreateColumnNode();
            buttonNode->MountToParent(stackNode);
            columnNode->MountToParent(columnBlendNode);
            columnBlendNode->MountToParent(stackNode);
            columnNode->MarkModifyDone();
            columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
            layoutProperty->UpdateAlignment(Alignment::CENTER);
            stackNode->MountToParent(AceType::Claim(frameNode));
        }
    }

    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetCascadeOptions(options, options);
}

void TextPickerModelNG::SetCascadeColumns(const std::vector<NG::TextCascadePickerOptions>& options)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    std::vector<NG::TextCascadePickerOptions> reOptions;
    // Caculate max depth
    size_t columnCount = options.empty()? 0 : 1;
    for (size_t i = 0; i < options.size(); i++) {
        size_t tmp  = textPickerPattern->ProcessCascadeOptionDepth(options[i]);
        if (tmp > columnCount) {
            columnCount = tmp;
        }
    }

    // Create Node
    if (frameNode->GetChildren().empty()) {
        for (size_t i = 0; i < columnCount; i++) {
            auto columnNode = CreateColumnNode(NG::TEXT, showCount_);
            auto stackNode = CreateStackNode();
            auto buttonNode = CreateButtonNode();
            auto columnBlendNode = CreateColumnNode();
            buttonNode->MountToParent(stackNode);
            columnNode->MountToParent(columnBlendNode);
            columnBlendNode->MountToParent(stackNode);
            columnNode->MarkModifyDone();
            columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
            layoutProperty->UpdateAlignment(Alignment::CENTER);
            stackNode->MountToParent(AceType::Claim<NG::FrameNode>(frameNode));
        }
    }

    textPickerPattern->ProcessCascadeOptions(options, reOptions, 0);
    if (reOptions.size() < columnCount) {
        auto differ = columnCount - reOptions.size();
        for (uint32_t i = 0; i < differ; i++) {
            NG::TextCascadePickerOptions differOption;
            memset_s(&differOption, sizeof(differOption), 0, sizeof(differOption));
            reOptions.emplace_back(differOption);
        }
    }
    textPickerPattern->SetCascadeOptions(options, reOptions);
}

void TextPickerModelNG::SetColumns(const std::vector<NG::TextCascadePickerOptions>& options)
{
    options_.clear();
    for (auto& option : options) {
        options_.emplace_back(std::move(option));
    }
    if (!isCascade_) {
        SetUnCascadeColumns(options);
    } else {
        SetCascadeColumns(options);
    }
}

bool TextPickerModelNG::IsSingle()
{
    return rangeValue_.size() > 0;
}

bool TextPickerModelNG::IsSingle(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, false);
    return textPickerPattern->GetRange().size() > 0;
}

bool TextPickerModelNG::GetSingleRange(std::vector<NG::RangeContent>& rangeValue)
{
    rangeValue.clear();
    for (auto& item : rangeValue_) {
        rangeValue.emplace_back(std::move(item));
    }
    return true;
}

bool TextPickerModelNG::GetSingleRange(FrameNode* frameNode, std::vector<NG::RangeContent>& rangeValue)
{
    rangeValue.clear();
    CHECK_NULL_RETURN(frameNode, false);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, false);
    for (auto& item : textPickerPattern->GetRange()) {
        rangeValue.emplace_back(std::move(item));
    }
    return true;
}

bool TextPickerModelNG::IsCascade(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, false);
    return textPickerPattern->GetIsCascade();
}

bool TextPickerModelNG::GetMultiOptions(std::vector<NG::TextCascadePickerOptions>& options)
{
    options.clear();
    for (auto& item : options_) {
        options.emplace_back(std::move(item));
    }
    return true;
}

bool TextPickerModelNG::GetMultiOptions(FrameNode* frameNode, std::vector<NG::TextCascadePickerOptions>& options)
{
    options.clear();
    CHECK_NULL_RETURN(frameNode, false);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, false);
    for (auto& item : textPickerPattern->GetMultiOptions()) {
        options.emplace_back(std::move(item));
    }
    return true;
}

uint32_t TextPickerModelNG::GetMaxCount(FrameNode* frameNode)
{
    return 1;
}

void TextPickerModelNG::SetValues(const std::vector<std::string>& values)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetValues(values);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Values, values);
}

void TextPickerModelNG::SetSelecteds(const std::vector<uint32_t>& values)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetSelecteds(values);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Selecteds, values);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedIndex, values);
}

void TextPickerModelNG::SetOnValueChangeEvent(TextCascadeValueChangeEvent&& onValueChangeEvent)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnValueChangeEvent(std::move(onValueChangeEvent));
}

void TextPickerModelNG::SetOnSelectedChangeEvent(TextCascadeSelectedChangeEvent&& onSelectedChangeEvent)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnSelectedChangeEvent(std::move(onSelectedChangeEvent));
}

RefPtr<AceType> TextPickerDialogModelNG::CreateObject()
{
    return nullptr;
}

void TextPickerDialogModelNG::SetTextPickerDialogShow(RefPtr<AceType>& PickerText,
    NG::TextPickerSettingData& settingData, std::function<void()>&& onCancel,
    std::function<void(const std::string&)>&& onAccept, std::function<void(const std::string&)>&& onChange,
    std::function<void(const std::string&)>&& onScrollStop,
    std::function<void(const std::string&)>&& onEnterSelectedArea, TextPickerDialog& textPickerDialog,
    TextPickerDialogEvent& textPickerDialogEvent, const std::vector<ButtonInfo>& buttonInfos)
{
    auto container = Container::Current();
    if (!container) {
        return;
    }
    auto pipelineContext = AccessibilityManager::DynamicCast<NG::PipelineContext>(container->GetPipelineContext());
    if (!pipelineContext) {
        return;
    }
    auto executor = pipelineContext->GetTaskExecutor();
    if (!executor) {
        return;
    }
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<DialogTheme>();
    CHECK_NULL_VOID(theme);

    std::map<std::string, NG::DialogTextEvent> dialogEvent;
    std::map<std::string, NG::DialogCancelEvent> dialogLifeCycleEvent;
    std::map<std::string, NG::DialogGestureEvent> dialogCancelEvent;
    dialogEvent["acceptId"] = onAccept;
    dialogEvent["changeId"] = onChange;
    dialogEvent["scrollStopId"] = onScrollStop;
    dialogEvent["enterSelectedAreaId"] = onEnterSelectedArea;
    auto func = [onCancel](const GestureEvent& /* info */) {
        if (onCancel) {
            onCancel();
        }
    };
    dialogCancelEvent["cancelId"] = func;
    dialogLifeCycleEvent["didAppearId"] = textPickerDialogEvent.onDidAppear;
    dialogLifeCycleEvent["didDisappearId"] = textPickerDialogEvent.onDidDisappear;
    dialogLifeCycleEvent["willAppearId"] = textPickerDialogEvent.onWillAppear;
    dialogLifeCycleEvent["willDisappearId"] = textPickerDialogEvent.onWillDisappear;
    DialogProperties properties;
    SetDialogProperties(properties, textPickerDialog, theme);

    auto context = AccessibilityManager::DynamicCast<NG::PipelineContext>(pipelineContext);
    auto overlayManager = context ? context->GetOverlayManager() : nullptr;
    executor->PostTask(
        [properties, settingData, dialogEvent, dialogCancelEvent, dialogLifeCycleEvent, buttonInfos,
            weak = WeakPtr<NG::OverlayManager>(overlayManager)] {
            auto overlayManager = weak.Upgrade();
            CHECK_NULL_VOID(overlayManager);
            overlayManager->ShowTextDialog(
                properties, settingData, dialogEvent, dialogCancelEvent, dialogLifeCycleEvent, buttonInfos);
        },
        TaskExecutor::TaskType::UI, "ArkUITextPickerShowTextDialog",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

void TextPickerModelNG::SetCanLoop(FrameNode* frameNode, const bool value)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetCanLoop(value);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, CanLoop, value, frameNode);
}

int32_t TextPickerModelNG::GetCanLoop(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 1);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, 1);
    return textPickerPattern->GetCanLoop();
}

void TextPickerModelNG::SetSelecteds(FrameNode* frameNode, const std::vector<uint32_t>& values)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetSelecteds(values);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Selecteds, values, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedIndex, values, frameNode);
}
void TextPickerModelNG::SetSelected(FrameNode* frameNode, uint32_t value)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetSelected(value);
    std::vector<uint32_t> values;
    values.emplace_back(value);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Selected, value, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedIndex, values, frameNode);
}
void TextPickerModelNG::SetHasSelectAttr(FrameNode* frameNode, bool value)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetHasSelectAttr(value);
}
void TextPickerModelNG::SetNormalTextStyle(
    FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto normalStyle = pickerTheme->GetOptionStyle(false, false);
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
            ConvertFontScaleValue(value.fontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
            ConvertFontScaleValue(normalStyle.GetFontSize()), frameNode);
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, Color, value.textColor.value_or(normalStyle.GetTextColor()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, Weight, value.fontWeight.value_or(normalStyle.GetFontWeight()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, FontFamily, value.fontFamily.value_or(normalStyle.GetFontFamilies()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, FontStyle, value.fontStyle.value_or(normalStyle.GetFontStyle()), frameNode);
}

void TextPickerModelNG::SetSelectedTextStyle(
    FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto selectedStyle = pickerTheme->GetOptionStyle(true, false);
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            TextPickerLayoutProperty, SelectedFontSize,
            ConvertFontScaleValue(value.fontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            TextPickerLayoutProperty, SelectedFontSize,
            ConvertFontScaleValue(selectedStyle.GetFontSize()), frameNode);
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedColor,
        value.textColor.value_or(selectedStyle.GetTextColor()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedWeight,
        value.fontWeight.value_or(selectedStyle.GetFontWeight()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontFamily,
        value.fontFamily.value_or(selectedStyle.GetFontFamilies()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontStyle, value.fontStyle.value_or(selectedStyle.GetFontStyle()), frameNode);
}

void TextPickerModelNG::SetDisappearTextStyle(
    FrameNode* frameNode, const RefPtr<PickerTheme>& pickerTheme, const NG::PickerTextStyle& value)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(pickerTheme);
    auto disappearStyle = pickerTheme->GetDisappearOptionStyle();
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            TextPickerLayoutProperty, DisappearFontSize,
            ConvertFontScaleValue(value.fontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            TextPickerLayoutProperty, DisappearFontSize,
            ConvertFontScaleValue(disappearStyle.GetFontSize()), frameNode);
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearColor, value.textColor.value_or(disappearStyle.GetTextColor()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearWeight,
        value.fontWeight.value_or(disappearStyle.GetFontWeight()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontFamily,
        value.fontFamily.value_or(disappearStyle.GetFontFamilies()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontStyle,
        value.fontStyle.value_or(disappearStyle.GetFontStyle()), frameNode);
}

void TextPickerModelNG::SetDefaultPickerItemHeight(FrameNode* frameNode, const Dimension& value)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultPickerItemHeight, value, frameNode);
}

Dimension TextPickerModelNG::GetDefaultPickerItemHeight(FrameNode* frameNode)
{
    Dimension value = Dimension(0.0f);
    CHECK_NULL_RETURN(frameNode, value);
    auto layoutProperty = frameNode->GetLayoutProperty<TextPickerLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, value);
    return layoutProperty->HasDefaultPickerItemHeight() ? layoutProperty->GetDefaultPickerItemHeightValue() : value;
}

void TextPickerModelNG::SetBackgroundColor(FrameNode* frameNode, const Color& color)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetBackgroundColor(color);
}

void TextPickerModelNG::SetRange(FrameNode* frameNode, const std::vector<NG::RangeContent>& value)
{
    CHECK_NULL_VOID(frameNode);
    if (frameNode->GetChildren().empty()) {
        auto columnNode = CreateColumnNode(TEXT, showCount_);
        auto stackNode = CreateStackNode();
        auto buttonNode = CreateButtonNode();
        auto columnBlendNode = CreateColumnNode();
        buttonNode->MountToParent(stackNode);
        columnNode->MountToParent(columnBlendNode);
        columnBlendNode->MountToParent(stackNode);
        columnNode->MarkModifyDone();
        columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
        layoutProperty->UpdateAlignment(Alignment::CENTER);
        stackNode->MountToParent(AceType::Claim(frameNode));
    }
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    textPickerPattern->SetRange(value);
    rangeValue_.clear();
    for (auto& range : value) {
        rangeValue_.emplace_back(std::move(range));
    }
}

void TextPickerModelNG::SetColumns(FrameNode* frameNode, const std::vector<NG::TextCascadePickerOptions>& options)
{
    options_.clear();
    for (auto& option : options) {
        options_.emplace_back(std::move(option));
    }
    if (!isCascade_) {
        SetUnCascadeColumnsNode(frameNode, options);
    } else {
        SetCascadeColumnsNode(frameNode, options);
    }
}

void TextPickerModelNG::SetUnCascadeColumnsNode(FrameNode* frameNode,
    const std::vector<NG::TextCascadePickerOptions>& options)
{
    CHECK_NULL_VOID(frameNode);
    if (frameNode->GetChildren().empty()) {
        for (uint32_t i = 0; i < options.size(); i++) {
            auto columnNode = CreateColumnNode(TEXT, showCount_);
            auto stackNode = CreateStackNode();
            auto buttonNode = CreateButtonNode();
            auto columnBlendNode = CreateColumnNode();
            buttonNode->MountToParent(stackNode);
            columnNode->MountToParent(columnBlendNode);
            columnBlendNode->MountToParent(stackNode);
            columnNode->MarkModifyDone();
            columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
            layoutProperty->UpdateAlignment(Alignment::CENTER);
            stackNode->MountToParent(AceType::Claim(frameNode));
        }
    }

    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetCascadeOptions(options, options);
}

void TextPickerModelNG::SetCascadeColumnsNode(FrameNode* frameNode,
    const std::vector<NG::TextCascadePickerOptions>& options)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    std::vector<NG::TextCascadePickerOptions> reOptions;
    // Caculate max depth
    size_t columnCount = options.empty()? 0 : 1;
    for (size_t i = 0; i < options.size(); i++) {
        size_t tmp  = textPickerPattern->ProcessCascadeOptionDepth(options[i]);
        if (tmp > columnCount) {
            columnCount = tmp;
        }
    }

    // Create Node
    if (frameNode->GetChildren().empty()) {
        for (size_t i = 0; i < columnCount; i++) {
            auto columnNode = CreateColumnNode(NG::TEXT, showCount_);
            auto stackNode = CreateStackNode();
            auto buttonNode = CreateButtonNode();
            auto columnBlendNode = CreateColumnNode();
            buttonNode->MountToParent(stackNode);
            columnNode->MountToParent(columnBlendNode);
            columnBlendNode->MountToParent(stackNode);
            columnNode->MarkModifyDone();
            columnNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            auto layoutProperty = stackNode->GetLayoutProperty<LayoutProperty>();
            layoutProperty->UpdateAlignment(Alignment::CENTER);
            stackNode->MountToParent(AceType::Claim(frameNode));
        }
    }

    textPickerPattern->ProcessCascadeOptions(options, reOptions, 0);
    if (reOptions.size() < columnCount) {
        auto differ = columnCount - reOptions.size();
        for (uint32_t i = 0; i < differ; i++) {
            NG::TextCascadePickerOptions differOption;
            memset_s(&differOption, sizeof(differOption), 0, sizeof(differOption));
            reOptions.emplace_back(differOption);
        }
    }
    textPickerPattern->SetCascadeOptions(options, reOptions);
}

void TextPickerModelNG::SetValue(FrameNode* frameNode, const std::string& value)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Value, value, frameNode);
    auto valueIterator = std::find_if(rangeValue_.begin(), rangeValue_.end(),
        [&value](const NG::RangeContent& range) { return range.text_ == value; });
    if (valueIterator != rangeValue_.end()) {
        TextPickerModelNG::SetSelected(frameNode, std::distance(rangeValue_.begin(), valueIterator));
    }
}

void TextPickerModelNG::SetValues(FrameNode* frameNode, const std::vector<std::string>& values)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    std::vector<std::string> selectedValues;
    std::vector<uint32_t> valuesIndex;
    for (uint32_t i = 0; i < options_.size(); i++) {
        if (values.size() > 0 && values.size() < i + 1) {
            if (options_[i].rangeResult.size() > 0) {
                selectedValues.emplace_back(options_[i].rangeResult[0]);
            } else {
                selectedValues.emplace_back("");
            }
            valuesIndex.emplace_back(0);
        } else {
            auto valueIterator = std::find(options_[i].rangeResult.begin(), options_[i].rangeResult.end(), values[i]);
            if (valueIterator == options_[i].rangeResult.end()) {
                selectedValues[i] = options_[i].rangeResult.front();
                valuesIndex.emplace_back(0);
            } else {
                selectedValues.emplace_back(values[i]);
                valuesIndex.emplace_back(std::distance(options_[i].rangeResult.begin(), valueIterator));
            }
        }
    }
    TextPickerModelNG::SetSelecteds(frameNode, valuesIndex);
    textPickerPattern->SetValues(selectedValues);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Values, selectedValues, frameNode);
}

void TextPickerModelNG::SetDefaultAttributes(RefPtr<FrameNode>& frameNode, const RefPtr<PickerTheme>& pickerTheme)
{
    auto selectedStyle = pickerTheme->GetOptionStyle(true, false);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedFontSize,
        ConvertFontScaleValue(selectedStyle.GetFontSize()), frameNode);
    ResetTextPickerTextStyleColor(frameNode.GetRawPtr(), &TextPickerLayoutProperty::GetSelectedTextStyle);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, SelectedWeight, selectedStyle.GetFontWeight(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontFamily, selectedStyle.GetFontFamilies(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, SelectedFontStyle, selectedStyle.GetFontStyle(), frameNode);

    auto disappearStyle = pickerTheme->GetDisappearOptionStyle();
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontSize,
        ConvertFontScaleValue(disappearStyle.GetFontSize()), frameNode);
    ResetTextPickerTextStyleColor(frameNode.GetRawPtr(), &TextPickerLayoutProperty::GetDisappearTextStyle);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearWeight, disappearStyle.GetFontWeight(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontFamily, disappearStyle.GetFontFamilies(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(
        TextPickerLayoutProperty, DisappearFontStyle, disappearStyle.GetFontStyle(), frameNode);

    auto normalStyle = pickerTheme->GetOptionStyle(false, false);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontSize,
        ConvertFontScaleValue(normalStyle.GetFontSize()), frameNode);
    ResetTextPickerTextStyleColor(frameNode.GetRawPtr(), &TextPickerLayoutProperty::GetTextStyle);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Weight, normalStyle.GetFontWeight(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontFamily, normalStyle.GetFontFamilies(), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, FontStyle, normalStyle.GetFontStyle(), frameNode);

    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, CanLoop, true, frameNode);
}

void TextPickerModelNG::SetDefaultTextStyle(
    FrameNode* frameNode, const RefPtr<TextTheme>& textTheme, const NG::PickerTextStyle& value)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(textTheme);
    auto textStyle = textTheme->GetTextStyle();
    if (value.fontSize.has_value() && value.fontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontSize,
            ConvertFontScaleValue(value.fontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontSize,
            ConvertFontScaleValue(textStyle.GetFontSize()), frameNode);
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultColor,
        value.textColor.value_or(textStyle.GetTextColor()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultWeight,
        value.fontWeight.value_or(textStyle.GetFontWeight()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontFamily,
        value.fontFamily.value_or(textStyle.GetFontFamilies()), frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultFontStyle,
        value.fontStyle.value_or(textStyle.GetFontStyle()), frameNode);
    if (value.minFontSize.has_value() && value.minFontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMinFontSize,
            ConvertFontScaleValue(value.minFontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMinFontSize, Dimension(), frameNode);
    }
    if (value.maxFontSize.has_value() && value.maxFontSize->IsValid()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMaxFontSize,
            ConvertFontScaleValue(value.maxFontSize.value()), frameNode);
    } else {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultMaxFontSize, Dimension(), frameNode);
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DefaultTextOverflow,
        value.textOverflow.value_or(textStyle.GetTextOverflow()), frameNode);
}

std::string TextPickerModelNG::getTextPickerValue(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    return frameNode->GetLayoutProperty<TextPickerLayoutProperty>()->GetValueValue("");
}

std::string TextPickerModelNG::getTextPickerRange(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    std::string result;
    if (isSingleRange_) {
        for (auto range : rangeValue_) {
            result.append(range.text_ + ";");
        }
        if (result.length() > 0) {
            result = result.substr(0, result.length() > 0 ? result.length() - 1 : 0);
        }
    } else {
        for (auto option : options_) {
            for (auto range : option.rangeResult) {
                result.append(range + ",");
            }
            result = result.substr(0, result.length() > 0 ? result.length() - 1 : 0);
            result.append(";");
        }
        if (result.length() > 0) {
            result = result.substr(0, result.length() - 1);
        }
    }
    return result;
}

void TextPickerModelNG::SetDivider(const ItemDivider& divider)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetDivider(divider);
    textPickerPattern->SetCustomDividerFlag(true);
    ACE_UPDATE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Divider, divider);
}

void TextPickerModelNG::SetDivider(FrameNode* frameNode, const ItemDivider& divider)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetDivider(divider);
    textPickerPattern->SetCustomDividerFlag(true);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, Divider, divider, frameNode);
}

void TextPickerModelNG::SetGradientHeight(FrameNode* frameNode, const Dimension& value)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetGradientHeight(value);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, GradientHeight, value, frameNode);
}

void TextPickerModelNG::SetDisableTextStyleAnimation(FrameNode* frameNode, const bool value)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetDisableTextStyleAnimation(value);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TextPickerLayoutProperty, DisableTextStyleAnimation, value, frameNode);
}

void TextPickerModelNG::SetOnCascadeChange(FrameNode* frameNode, TextCascadeChangeEvent&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnChange(std::move(onChange));
}

void TextPickerModelNG::SetOnScrollStop(FrameNode* frameNode, TextCascadeChangeEvent&& onScrollStop)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<TextPickerEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetOnScrollStop(std::move(onScrollStop));
}

int32_t TextPickerModelNG::GetSelectedSize(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, 0);
    return textPickerPattern->GetSelecteds().size();
}

int32_t TextPickerModelNG::GetColumnWidthsSize(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, 0);
    return textPickerPattern->GetColumnWidths().size();
}

std::string TextPickerModelNG::getTextPickerValues(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, "");
    auto values = textPickerPattern->GetValues();
    std::string result;
    for (auto& valueRet : values) {
        result.append(valueRet + ';');
    }
    const size_t length = result.length();
    result = result.substr(0, length > 0 ? length - 1 : 0);
    return result;
}

std::vector<uint32_t> TextPickerModelNG::getTextPickerSelecteds(FrameNode* frameNode)
{
    std::vector<uint32_t> defaultValue = { 0 };
    CHECK_NULL_RETURN(frameNode, defaultValue);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, defaultValue);
    return textPickerPattern->GetSelecteds();
}

void TextPickerModelNG::SetTextPickerRangeType(FrameNode* frameNode, int32_t rangeType)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetRangeType(rangeType);
}

int32_t TextPickerModelNG::GetTextPickerRangeType(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, 0);
    return textPickerPattern->GetRangeType();
}

const Dimension TextPickerModelNG::ConvertFontScaleValue(const Dimension& fontSizeValue)
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, fontSizeValue);
    auto maxAppFontScale = pipeline->GetMaxAppFontScale();
    auto follow = pipeline->IsFollowSystem();
    float fontScale = pipeline->GetFontScale();
    if (NearZero(fontScale) || (fontSizeValue.Unit() == DimensionUnit::VP)) {
        return fontSizeValue;
    }
    if (GreatOrEqualCustomPrecision(fontScale, PICKER_MAXFONTSCALE) && follow) {
        fontScale = std::clamp(fontScale, 0.0f, maxAppFontScale);
        if (fontScale != 0.0f) {
            return Dimension(fontSizeValue / fontScale);
        }
    }
    return fontSizeValue;
}

void TextPickerModelNG::HasUserDefinedOpacity()
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    auto renderContext = frameNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    textPickerPattern->SetUserDefinedOpacity(renderContext->GetOpacityValue(1.0));
}

void TextPickerModelNG::SetEnableHapticFeedback(bool isEnableHapticFeedback)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    SetEnableHapticFeedback(frameNode, isEnableHapticFeedback);
}

void TextPickerModelNG::SetEnableHapticFeedback(FrameNode* frameNode, bool isEnableHapticFeedback)
{
    CHECK_NULL_VOID(frameNode);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_VOID(textPickerPattern);
    textPickerPattern->SetIsEnableHaptic(isEnableHapticFeedback);
}

bool TextPickerModelNG::GetEnableHapticFeedback(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, DEFAULT_ENABLE_HAPTIC_FEEDBACK);
    auto textPickerPattern = frameNode->GetPattern<TextPickerPattern>();
    CHECK_NULL_RETURN(textPickerPattern, DEFAULT_ENABLE_HAPTIC_FEEDBACK);
    return textPickerPattern->GetIsEnableHaptic();
}
} // namespace OHOS::Ace::NG
