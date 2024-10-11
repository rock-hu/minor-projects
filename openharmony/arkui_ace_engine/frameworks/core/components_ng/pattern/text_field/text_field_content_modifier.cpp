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

#include "core/components_ng/pattern/text_field/text_field_content_modifier.h"

#include "base/utils/utils.h"
#include "core/components_ng/base/modifier.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/pattern/text_field/text_field_pattern.h"
#include "core/components_ng/property/calc_length.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_ng/render/image_painter.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
namespace {
const FontWeight FONT_WEIGHT_CONVERT_MAP[] = {
    FontWeight::W100,
    FontWeight::W200,
    FontWeight::W300,
    FontWeight::W400,
    FontWeight::W500,
    FontWeight::W600,
    FontWeight::W700,
    FontWeight::W800,
    FontWeight::W900,
    FontWeight::W700,
    FontWeight::W400,
    FontWeight::W900,
    FontWeight::W100,
    FontWeight::W500,
    FontWeight::W400,
};
constexpr Dimension ERROR_TEXT_UNDERLINE_MARGIN = 8.0_vp;
constexpr Dimension ERROR_TEXT_CAPSULE_MARGIN = 8.0_vp;
constexpr float ROUND_VALUE = 0.5f;

inline FontWeight ConvertFontWeight(FontWeight fontWeight)
{
    return FONT_WEIGHT_CONVERT_MAP[(int)fontWeight];
}

float CalCounterWidth(const RefPtr<TextFieldPattern>& textFieldPattern)
{
    float counterWidth = 0.0f;
    RefPtr<LayoutWrapper> counterNode = textFieldPattern->GetCounterNode().Upgrade();
    auto counterFrameNode = counterNode->GetHostNode();
    CHECK_NULL_RETURN(counterFrameNode, 0.0f);
    auto counterTextPattern = counterFrameNode->GetPattern<TextPattern>();
    CHECK_NULL_RETURN(counterTextPattern, 0.0f);
    auto counterParagraphs = counterTextPattern->GetParagraphs();
    for (auto &&info : counterParagraphs) {
        float width = info.paragraph->GetLongestLine();
        counterWidth = std::max(counterWidth, width);
    }
    return counterWidth;
}
} // namespace

TextFieldContentModifier::TextFieldContentModifier(const WeakPtr<OHOS::Ace::NG::Pattern>& pattern) : pattern_(pattern)
{
    SetDefaultAnimatablePropertyValue();
    SetDefaultPropertyValue();
}

void TextFieldContentModifier::onDraw(DrawingContext& context)
{
    auto& canvas = context.canvas;
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    auto paragraph = textFieldPattern->GetParagraph();
    CHECK_NULL_VOID(paragraph);
    auto contentOffset = contentOffset_->Get();
    auto contentRect = textFieldPattern->GetContentRect();
    auto clipRectHeight = 0.0f;
    auto errorMargin = 0.0f;
    auto frameNode = textFieldPattern->GetHost();
    CHECK_NULL_VOID(frameNode);
    auto layoutProperty = frameNode->GetLayoutProperty<TextFieldLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    if (layoutProperty->GetShowUnderlineValue(false) && showErrorState_->Get()) {
        errorMargin = ERROR_TEXT_UNDERLINE_MARGIN.ConvertToPx();
    } else if (textFieldPattern->NeedShowPasswordIcon() && showErrorState_->Get()) {
        errorMargin = ERROR_TEXT_CAPSULE_MARGIN.ConvertToPx();
    } else if (showErrorState_->Get()) {
        errorMargin = ERROR_TEXT_CAPSULE_MARGIN.ConvertToPx();
    } else {
        errorMargin = 0;
    }
    ProcessErrorParagraph(context, errorMargin);
    clipRectHeight = contentRect.GetY() + contentRect.Height();
    canvas.Save();
    RSRect clipInnerRect = RSRect(contentRect.GetX(), contentRect.GetY(),
        contentRect.Width() + contentRect.GetX() + textFieldPattern->GetInlinePadding(), clipRectHeight);
    canvas.ClipRect(clipInnerRect, RSClipOp::INTERSECT);
    if (paragraph) {
        auto textField = textFieldPattern->IsTextArea() ? "TextArea" : "TextInput";
        ACE_LAYOUT_SCOPED_TRACE("[%s][id:%d] [Rect:%s]", textField, frameNode->GetId(), contentRect.ToString().c_str());
        if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
            canvas.Save();
            RSRect clipRect;
            std::vector<RSPoint> clipRadius;
            GetFrameRectClip(clipRect, clipRadius);
            canvas.ClipRoundRect(clipRect, clipRadius, true);
            paragraph->Paint(canvas, textFieldPattern->GetTextRect().GetX(),
                textFieldPattern->IsTextArea() ? textFieldPattern->GetTextRect().GetY() : contentOffset.GetY());
            canvas.Restore();
        } else {
            paragraph->Paint(canvas, textFieldPattern->GetTextRect().GetX(),
                textFieldPattern->IsTextArea() ? textFieldPattern->GetTextRect().GetY() : contentOffset.GetY());
        }
    }
    canvas.Restore();
}

void TextFieldContentModifier::GetFrameRectClip(RSRect& clipRect, std::vector<RSPoint>& clipRadius)
{
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    auto host = textFieldPattern->GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto textFrameRect = textFieldPattern->GetFrameRect();
    clipRect = RSRect(0.0f, 0.0f, textFrameRect.Width(), textFrameRect.Height());
    auto radius = renderContext->GetBorderRadius().value_or(BorderRadiusProperty());
    auto radiusTopLeft = RSPoint(static_cast<float>(radius.radiusTopLeft.value_or(0.0_vp).ConvertToPx()),
        static_cast<float>(radius.radiusTopLeft.value_or(0.0_vp).ConvertToPx()));
    clipRadius.emplace_back(radiusTopLeft);
    auto radiusTopRight = RSPoint(static_cast<float>(radius.radiusTopRight.value_or(0.0_vp).ConvertToPx()),
        static_cast<float>(radius.radiusTopRight.value_or(0.0_vp).ConvertToPx()));
    clipRadius.emplace_back(radiusTopRight);
    auto radiusBottomRight = RSPoint(static_cast<float>(radius.radiusBottomRight.value_or(0.0_vp).ConvertToPx()),
        static_cast<float>(radius.radiusBottomRight.value_or(0.0_vp).ConvertToPx()));
    clipRadius.emplace_back(radiusBottomRight);
    auto radiusBottomLeft = RSPoint(static_cast<float>(radius.radiusBottomLeft.value_or(0.0_vp).ConvertToPx()),
        static_cast<float>(radius.radiusBottomLeft.value_or(0.0_vp).ConvertToPx()));
    clipRadius.emplace_back(radiusBottomLeft);
}

void TextFieldContentModifier::SetDefaultAnimatablePropertyValue()
{
    RefPtr<TextTheme> theme;
    RefPtr<TextFieldLayoutProperty> textFieldLayoutProperty;
    RefPtr<PipelineContext> pipelineContext;
    auto textPartten = pattern_.Upgrade();
    CHECK_NULL_VOID(textPartten);
    auto frameNode = textPartten->GetHost();
    CHECK_NULL_VOID(frameNode);
    pipelineContext = frameNode->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    theme = pipelineContext->GetTheme<TextTheme>();
    textFieldLayoutProperty = frameNode->GetLayoutProperty<TextFieldLayoutProperty>();
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    TextStyle textStyle;
    if (!textFieldPattern->GetTextValue().empty()) {
        textStyle = CreateTextStyleUsingTheme(
            textFieldLayoutProperty->GetFontStyle(), textFieldLayoutProperty->GetTextLineStyle(), theme);
    } else {
        textStyle = CreateTextStyleUsingTheme(textFieldLayoutProperty->GetPlaceholderFontStyle(),
            textFieldLayoutProperty->GetPlaceholderTextLineStyle(), theme);
    }
    SetDefaultFontSize(textStyle);
    SetDefaultAdaptMinFontSize(textStyle);
    SetDefaultAdaptMaxFontSize(textStyle);
    SetDefaultFontWeight(textStyle);
    SetDefaultTextColor(textStyle);
    SetDefaultFontStyle(textStyle);
    SetDefaultTextOverflow(textStyle);
    SetDefaultTextDecoration(textStyle);
}

void TextFieldContentModifier::SetDefaultPropertyValue()
{
    RefPtr<TextFieldTheme> theme;
    RefPtr<PipelineContext> pipelineContext;
    auto textPattern = pattern_.Upgrade();
    CHECK_NULL_VOID(textPattern);
    auto frameNode = textPattern->GetHost();
    CHECK_NULL_VOID(frameNode);
    pipelineContext = frameNode->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    theme = pipelineContext->GetTheme<TextFieldTheme>();
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());

    textObscured_ = AceType::MakeRefPtr<PropertyBool>(textFieldPattern->GetTextObscured());
    dragStatus_ = AceType::MakeRefPtr<PropertyBool>(false);
    contentOffset_ = AceType::MakeRefPtr<PropertyOffsetF>(
        OffsetF(textFieldPattern->GetTextRect().GetX(), textFieldPattern->GetTextRect().GetY()));
    contentSize_ = AceType::MakeRefPtr<PropertySizeF>(SizeF());
    textValue_ = AceType::MakeRefPtr<PropertyString>("");
    errorTextValue_ = AceType::MakeRefPtr<PropertyString>("");
    placeholderValue_ = AceType::MakeRefPtr<PropertyString>("");
    textRectY_ = AceType::MakeRefPtr<PropertyFloat>(textFieldPattern->GetTextRect().GetY());
    textRectX_ = AceType::MakeRefPtr<PropertyFloat>(textFieldPattern->GetTextRect().GetX());
    textAlign_ = AceType::MakeRefPtr<PropertyInt>(static_cast<int32_t>(TextAlign::START));
    showErrorState_ = AceType::MakeRefPtr<PropertyBool>(false);
    fontFamilyString_ = AceType::MakeRefPtr<PropertyString>("");
    fontReady_ = AceType::MakeRefPtr<PropertyBool>(false);
    contentChange_ = AceType::MakeRefPtr<PropertyBool>(false);
    AttachProperty(contentOffset_);
    AttachProperty(contentSize_);
    AttachProperty(textValue_);
    AttachProperty(errorTextValue_);
    AttachProperty(placeholderValue_);
    AttachProperty(textRectY_);
    AttachProperty(textObscured_);
    AttachProperty(dragStatus_);
    AttachProperty(textRectX_);
    AttachProperty(textAlign_);
    AttachProperty(showErrorState_);
    AttachProperty(showUnderline_);
    AttachProperty(fontFamilyString_);
    AttachProperty(fontReady_);
    AttachProperty(contentChange_);
}

void TextFieldContentModifier::SetDefaultFontSize(const TextStyle& textStyle)
{
    float fontSizeValue;
    auto pipelineContext = PipelineContext::GetCurrentContext();
    if (pipelineContext) {
        fontSizeValue = pipelineContext->NormalizeToPx(textStyle.GetFontSize());
        if (textStyle.IsAllowScale() && textStyle.GetFontSize().Unit() == DimensionUnit::FP) {
            fontSizeValue = pipelineContext->NormalizeToPx(textStyle.GetFontSize() * pipelineContext->GetFontScale());
        }
    } else {
        fontSizeValue = textStyle.GetFontSize().ConvertToPx();
    }

    fontSizeFloat_ = AceType::MakeRefPtr<AnimatablePropertyFloat>(fontSizeValue);
    AttachProperty(fontSizeFloat_);
}

void TextFieldContentModifier::SetDefaultAdaptMinFontSize(const TextStyle& textStyle)
{
    float minFontSizeValue = 0.0f;
    adaptMinFontSizeFloat_ = AceType::MakeRefPtr<AnimatablePropertyFloat>(minFontSizeValue);
    AttachProperty(adaptMinFontSizeFloat_);
}

void TextFieldContentModifier::SetDefaultAdaptMaxFontSize(const TextStyle& textStyle)
{
    float maxFontSizeValue = 0.0f;
    adaptMaxFontSizeFloat_ = AceType::MakeRefPtr<AnimatablePropertyFloat>(maxFontSizeValue);
    AttachProperty(adaptMaxFontSizeFloat_);
}

void TextFieldContentModifier::SetDefaultFontWeight(const TextStyle& textStyle)
{
    fontWeightFloat_ =
        AceType::MakeRefPtr<AnimatablePropertyFloat>(static_cast<float>(ConvertFontWeight(textStyle.GetFontWeight())));
    AttachProperty(fontWeightFloat_);
}

void TextFieldContentModifier::SetDefaultTextColor(const TextStyle& textStyle)
{
    animatableTextColor_ = AceType::MakeRefPtr<AnimatablePropertyColor>(LinearColor(textStyle.GetTextColor()));
    AttachProperty(animatableTextColor_);
}

void TextFieldContentModifier::SetDefaultFontStyle(const TextStyle& textStyle)
{
    fontStyle_ = AceType::MakeRefPtr<PropertyInt>(static_cast<int32_t>(textStyle.GetFontStyle()));
    AttachProperty(fontStyle_);
}

void TextFieldContentModifier::SetDefaultTextOverflow(const TextStyle& textStyle)
{
    textOverflow_ = AceType::MakeRefPtr<PropertyInt>(static_cast<int32_t>(textStyle.GetTextOverflow()));
    AttachProperty(textOverflow_);
}

void TextFieldContentModifier::SetDefaultTextDecoration(const TextStyle& textStyle)
{
    textDecoration_ = textStyle.GetTextDecoration();
    textDecorationStyle_ = textStyle.GetTextDecorationStyle();
    textDecorationColor_ = textStyle.GetTextDecorationColor();
    textDecorationColorAlpha_ = MakeRefPtr<AnimatablePropertyFloat>(
        textDecoration_ == TextDecoration::NONE ? 0.0f : textDecorationColor_->GetAlpha());
    AttachProperty(textDecorationColorAlpha_);
}

void TextFieldContentModifier::ModifyTextStyle(TextStyle& textStyle)
{
    if (fontSize_.has_value() && fontSizeFloat_) {
        textStyle.SetFontSize(Dimension(fontSizeFloat_->Get(), DimensionUnit::PX));
    }
    if (textStyle.GetAdaptTextSize()) {
        if (adaptMinFontSize_.has_value() && adaptMinFontSizeFloat_) {
            textStyle.SetAdaptMinFontSize(Dimension(adaptMinFontSizeFloat_->Get(), DimensionUnit::PX));
        }
        if (adaptMaxFontSize_.has_value() && adaptMaxFontSizeFloat_) {
            textStyle.SetAdaptMaxFontSize(Dimension(adaptMaxFontSizeFloat_->Get(), DimensionUnit::PX));
        }
    }
    if (fontWeight_.has_value() && fontWeightFloat_) {
        textStyle.SetFontWeight(static_cast<FontWeight>(std::floor(fontWeightFloat_->Get() + 0.5f)));
    }
    if (textColor_.has_value() && animatableTextColor_) {
        textStyle.SetTextColor(Color(animatableTextColor_->Get().GetValue()));
    }
    ModifyDecorationInTextStyle(textStyle);
}

void TextFieldContentModifier::SetFontFamilies(const std::vector<std::string>& value)
{
    CHECK_NULL_VOID(fontFamilyString_);
    fontFamilyString_->Set(V2::ConvertFontFamily(value));
}

void TextFieldContentModifier::SetFontSize(const Dimension& value)
{
    auto valPx = static_cast<float>(value.ConvertToPx());
    fontSize_ = Dimension(valPx);
    CHECK_NULL_VOID(fontSizeFloat_);
    fontSizeFloat_->Set(valPx);
}

void TextFieldContentModifier::SetAdaptMinFontSize(const Dimension& value)
{
    auto valPx = static_cast<float>(value.ConvertToPx());
    adaptMinFontSize_ = Dimension(valPx);
    CHECK_NULL_VOID(adaptMinFontSizeFloat_);
    adaptMinFontSizeFloat_->Set(valPx);
}

void TextFieldContentModifier::SetAdaptMaxFontSize(const Dimension& value)
{
    auto valPx = static_cast<float>(value.ConvertToPx());
    adaptMaxFontSize_ = Dimension(valPx);
    CHECK_NULL_VOID(adaptMaxFontSizeFloat_);
    adaptMaxFontSizeFloat_->Set(valPx);
}

void TextFieldContentModifier::SetFontWeight(const FontWeight& value)
{
    fontWeight_ = ConvertFontWeight(value);
    CHECK_NULL_VOID(fontWeightFloat_);
    fontWeightFloat_->Set(static_cast<int>(ConvertFontWeight(value)));
}

void TextFieldContentModifier::SetTextColor(const Color& value)
{
    textColor_ = value;
    CHECK_NULL_VOID(animatableTextColor_);
    animatableTextColor_->Set(LinearColor(value));
}

void TextFieldContentModifier::SetTextOverflow(const TextOverflow value)
{
    if (textOverflow_->Get() != static_cast<int32_t>(value)) {
        textOverflow_->Set(static_cast<int32_t>(value));
    }
}

void TextFieldContentModifier::SetFontStyle(const OHOS::Ace::FontStyle& value)
{
    if (fontStyle_->Get() != static_cast<int32_t>(value)) {
        fontStyle_->Set(static_cast<int32_t>(value));
    }
}

void TextFieldContentModifier::SetContentOffset(OffsetF& value)
{
    if (contentOffset_) {
        contentOffset_->Set(value);
    }
}

float TextFieldContentModifier::GetContentOffsetY()
{
    return contentOffset_->Get().GetY();
}

void TextFieldContentModifier::SetContentSize(SizeF& value)
{
    if (contentSize_) {
        contentSize_->Set(value);
    }
}

void TextFieldContentModifier::SetTextValue(std::string& value)
{
    if (textValue_->Get() != value) {
        textValue_->Set(value);
    }
}

void TextFieldContentModifier::SetErrorTextValue(const std::string& value)
{
    if (errorTextValue_->Get() != value) {
        errorTextValue_->Set(value);
    }
}

void TextFieldContentModifier::SetPlaceholderValue(std::string&& value)
{
    if (placeholderValue_->Get() != value) {
        placeholderValue_->Set(value);
    }
}

void TextFieldContentModifier::SetTextRectY(const float value)
{
    if (textRectY_->Get() != value) {
        textRectY_->Set(value);
    }
}

float TextFieldContentModifier::GetTextRectY()
{
    return textRectY_->Get();
}

void TextFieldContentModifier::SetTextObscured(bool value)
{
    if (textObscured_) {
        textObscured_->Set(value);
    }
}

void TextFieldContentModifier::ChangeDragStatus()
{
    dragStatus_->Set(!dragStatus_->Get());
}

void TextFieldContentModifier::SetTextRectX(const float value)
{
    if (textRectX_->Get() != value) {
        textRectX_->Set(value);
    }
}

float TextFieldContentModifier::GetTextRectX()
{
    return textRectX_->Get();
}

void TextFieldContentModifier::SetTextAlign(const TextAlign value)
{
    if (textAlign_->Get() != static_cast<int32_t>(value)) {
        textAlign_->Set(static_cast<int32_t>(value));
    }
}

void TextFieldContentModifier::SetShowErrorState(bool value)
{
    if (showErrorState_) {
        showErrorState_->Set(value);
    }
}

void TextFieldContentModifier::SetShowUnderlineState(bool value)
{
    if (showUnderline_) {
        showUnderline_->Set(value);
    }
}

void TextFieldContentModifier::SetFontReady(bool value)
{
    if (fontReady_) {
        fontReady_->Set(value);
    }
}

void TextFieldContentModifier::ContentChange()
{
    CHECK_NULL_VOID(contentChange_);
    contentChange_->Set(!contentChange_->Get());
}

bool TextFieldContentModifier::NeedMeasureUpdate(PropertyChangeFlag& flag)
{
    flag = 0;
    if (fontSize_.has_value() && fontSizeFloat_ && !NearEqual(fontSize_.value().Value(), fontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
    }
    if (adaptMinFontSize_.has_value() && adaptMinFontSizeFloat_ &&
        !NearEqual(adaptMinFontSize_.value().Value(), adaptMinFontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
    }
    if (adaptMaxFontSize_.has_value() && adaptMaxFontSizeFloat_ &&
        !NearEqual(adaptMaxFontSize_.value().Value(), adaptMaxFontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
    }
    if (fontWeight_.has_value() && fontWeightFloat_ &&
        !NearEqual(static_cast<float>(fontWeight_.value()), fontWeightFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
    }
    if (textColor_.has_value() && animatableTextColor_ &&
        textColor_->GetValue() != animatableTextColor_->Get().GetValue()) {
        flag |= PROPERTY_UPDATE_MEASURE_SELF;
    }
    UpdateTextDecorationMeasureFlag(flag);
    flag &= (PROPERTY_UPDATE_MEASURE | PROPERTY_UPDATE_MEASURE_SELF | PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    return flag;
}

void TextFieldContentModifier::ProcessErrorParagraph(DrawingContext& context, float errorMargin)
{
    auto textFieldPattern = DynamicCast<TextFieldPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textFieldPattern);
    auto offset = contentOffset_->Get();
    auto textFrameRect = textFieldPattern->GetFrameRect();
    auto errorParagraph = textFieldPattern->GetErrorParagraph();
    auto errorValue = textFieldPattern->GetErrorTextString();
    auto frameNode = textFieldPattern->GetHost();
    auto& canvas = context.canvas;
    if (showErrorState_->Get() && errorParagraph && !textFieldPattern->IsDisabled() && !errorValue.empty()) {
        auto property = frameNode->GetLayoutProperty();
        float padding = 0.0f;
        if (property && property->GetPaddingProperty()) {
            const auto& paddingProperty = property->GetPaddingProperty();
            padding = paddingProperty->left.value_or(CalcLength(0.0)).GetDimension().ConvertToPx() +
                paddingProperty->right.value_or(CalcLength(0.0)).GetDimension().ConvertToPx();
        }
        float layoutWidth = textFrameRect.Width() - padding;
        // subtract border width
        float borderWidth = textFieldPattern->GetBorderLeft() + textFieldPattern->GetBorderRight();
        borderWidth = std::max(borderWidth, 0.0f);
        layoutWidth -= borderWidth;
        if (textFieldPattern->IsShowCount()) {
            // subtract counter length
            float counterWidth = CalCounterWidth(textFieldPattern);
            layoutWidth -= counterWidth;
        }
        if (layoutWidth <= 0) {
            return; // no enough space
        }
        errorParagraph->Layout(layoutWidth);
        if (errorParagraph->GetLongestLine() > layoutWidth) {
            return; // no enough space
        }
        auto isRTL = property->GetNonAutoLayoutDirection() == TextDirection::RTL;
        auto offSetX = offset.GetX();
        if (isRTL) {
            if (textFieldPattern->GetResponseArea()) {
                offSetX -= textFieldPattern->GetResponseArea()->GetAreaRect().Width();
            }
            if (textFieldPattern->GetCleanNodeResponseArea()) {
                offSetX -= textFieldPattern->GetCleanNodeResponseArea()->GetAreaRect().Width();
            }
        }
        errorParagraph->Paint(canvas, offSetX, textFrameRect.Bottom() - textFrameRect.Top() + errorMargin);
    }
}

void TextFieldContentModifier::SetTextDecoration(const TextDecoration& value, const Color& color,
    const TextDecorationStyle& style)
{
    auto oldTextDecoration = textDecoration_.value_or(TextDecoration::NONE);
    auto oldTextDecorationColor = textDecorationColor_.value_or(Color::BLACK);
    auto oldTextDecorationStyle = textDecorationStyle_.value_or(TextDecorationStyle::SOLID);
    if ((oldTextDecoration == value) && (oldTextDecorationColor == color) && (oldTextDecorationStyle == style)) {
        return;
    }

    textDecorationAnimatable_ = (oldTextDecoration == TextDecoration::NONE && value == TextDecoration::UNDERLINE) ||
                                (oldTextDecoration == TextDecoration::UNDERLINE && value == TextDecoration::NONE);

    textDecoration_ = value;
    textDecorationColor_ = color;
    textDecorationStyle_ = style;
    CHECK_NULL_VOID(textDecorationColorAlpha_);

    oldColorAlpha_ = textDecorationColorAlpha_->Get();
    if (textDecoration_ == TextDecoration::NONE) {
        textDecorationColorAlpha_->Set(0.0f);
    } else {
        textDecorationColorAlpha_->Set(static_cast<float>(textDecorationColor_.value().GetAlpha()));
    }
}

void TextFieldContentModifier::ModifyDecorationInTextStyle(TextStyle& textStyle)
{
    if (textDecoration_.has_value() && textDecorationColor_.has_value() && textDecorationColorAlpha_) {
        if (textDecorationAnimatable_) {
            uint8_t alpha = static_cast<uint8_t>(std::floor(textDecorationColorAlpha_->Get() + ROUND_VALUE));
            if (alpha == 0) {
                textStyle.SetTextDecoration(TextDecoration::NONE);
                textStyle.SetTextDecorationColor(textDecorationColor_.value());
            } else {
                textStyle.SetTextDecoration(TextDecoration::UNDERLINE);
                textStyle.SetTextDecorationColor(Color(textDecorationColor_.value()).ChangeAlpha(alpha));
            }
        } else {
            textStyle.SetTextDecoration(textDecoration_.value());
            textStyle.SetTextDecorationColor(textDecorationColor_.value());
        }
    }
    if (textDecorationStyle_.has_value()) {
        textStyle.SetTextDecorationStyle(textDecorationStyle_.value());
    }
}

void TextFieldContentModifier::UpdateTextDecorationMeasureFlag(PropertyChangeFlag& flag)
{
    if (textDecoration_.has_value() && textDecorationColor_.has_value() && textDecorationColorAlpha_) {
        uint8_t alpha = static_cast<uint8_t>(std::floor(textDecorationColorAlpha_->Get() + ROUND_VALUE));
        if (textDecoration_.value() == TextDecoration::UNDERLINE && alpha != textDecorationColor_.value().GetAlpha()) {
            flag |= PROPERTY_UPDATE_MEASURE;
        } else if (textDecoration_.value() == TextDecoration::NONE && alpha != 0.0) {
            flag |= PROPERTY_UPDATE_MEASURE;
        }
    }
}

} // namespace OHOS::Ace::NG
