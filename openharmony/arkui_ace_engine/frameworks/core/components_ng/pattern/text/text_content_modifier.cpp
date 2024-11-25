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

#include "core/components_ng/pattern/text/text_content_modifier.h"
#include <cstdint>
#include <optional>

#include "base/log/ace_trace.h"
#include "base/utils/utils.h"
#include "core/components/common/layout/constants.h"
#include "core/components_ng/pattern/text/text_layout_adapter.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/render/animation_utils.h"
#include "core/components_ng/render/drawing.h"
#include "core/components_ng/render/drawing_prop_convertor.h"
#include "core/components_ng/render/image_painter.h"
#include "core/components_v2/inspector/utils.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "frameworks/core/components_ng/render/adapter/animated_image.h"
#include "frameworks/core/components_ng/render/adapter/pixelmap_image.h"

namespace OHOS::Ace::NG {
namespace {
constexpr float RACE_MOVE_PERCENT_MIN = 0.0f;
constexpr float RACE_MOVE_PERCENT_MAX = 100.0f;
constexpr int32_t RACE_DURATION = 2000;
constexpr float RACE_SPACE_WIDTH = 48.0f;
constexpr Dimension DEFAULT_MARQUEE_SCROLL_AMOUNT = 6.0_vp;
constexpr double DEFAULT_MARQUEE_SCROLL_DELAY = 85.0; // Delay time between each jump.
constexpr float ROUND_VALUE = 0.5f;
constexpr uint32_t POINT_COUNT = 4;
constexpr float OBSCURED_ALPHA = 0.2f;
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
    FontWeight::W700,       // FontWeight::BOLD
    FontWeight::W400,       // FontWeight::NORMAL
    FontWeight::W900,       // FontWeight::BOLDER,
    FontWeight::W100,       // FontWeight::LIGHTER
    FontWeight::W500,       // FontWeight::MEDIUM
    FontWeight::W400,       // FontWeight::REGULAR
};

inline FontWeight ConvertFontWeight(FontWeight fontWeight)
{
    return FONT_WEIGHT_CONVERT_MAP[static_cast<int>(fontWeight)];
}
} // namespace

TextContentModifier::TextContentModifier(const std::optional<TextStyle>& textStyle, const WeakPtr<Pattern>& pattern)
    : pattern_(pattern)
{
    auto patternUpgrade = pattern_.Upgrade();
    CHECK_NULL_VOID(patternUpgrade);
    auto textPattern = DynamicCast<TextPattern>(patternUpgrade);
    CHECK_NULL_VOID(textPattern);
    auto host = textPattern->GetHost();
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);

    contentChange_ = MakeRefPtr<PropertyInt>(0);
    AttachProperty(contentChange_);

    auto contentRect = geometryNode->GetContentRect();
    contentOffset_ = MakeRefPtr<PropertyOffsetF>(contentRect.GetOffset());
    contentSize_ = MakeRefPtr<PropertySizeF>(contentRect.GetSize());
    AttachProperty(contentOffset_);
    AttachProperty(contentSize_);
    dragStatus_ = MakeRefPtr<PropertyBool>(false);
    AttachProperty(dragStatus_);
    if (textStyle.has_value()) {
        SetDefaultAnimatablePropertyValue(textStyle.value());
    }

    textRaceSpaceWidth_ = RACE_SPACE_WIDTH;
    auto pipeline = host->GetContext();
    if (pipeline) {
        textRaceSpaceWidth_ *= pipeline->GetDipScale();
    }

    racePercentFloat_ = MakeRefPtr<AnimatablePropertyFloat>(0.0f);
    AttachProperty(racePercentFloat_);
    ResetTextRacePercent();

    if (Container::LessThanAPITargetVersion(PlatformVersion::VERSION_TWELVE)) {
        clip_ = MakeRefPtr<PropertyBool>(true);
    } else {
        clip_ = MakeRefPtr<PropertyBool>(false);
    }
    AttachProperty(clip_);
    fontReady_ = MakeRefPtr<PropertyBool>(false);
    AttachProperty(fontReady_);

    auto baselineOffset = textPattern->GetBaselineOffset();
    paintOffset_ = contentRect.GetOffset() - OffsetF(0.0, std::min(baselineOffset, 0.0f));
}

void TextContentModifier::ChangeDragStatus()
{
    CHECK_NULL_VOID(dragStatus_);
    dragStatus_->Set(!dragStatus_->Get());
}

void TextContentModifier::SetDefaultAnimatablePropertyValue(const TextStyle& textStyle)
{
    SetDefaultFontSize(textStyle);
    SetDefaultAdaptMinFontSize(textStyle);
    SetDefaultAdaptMaxFontSize(textStyle);
    SetDefaultFontWeight(textStyle);
    SetDefaultTextColor(textStyle);
    SetDefaultTextShadow(textStyle);
    SetDefaultTextDecoration(textStyle);
    SetDefaultBaselineOffset(textStyle);
    SetDefaultLineHeight(textStyle);
}

void TextContentModifier::SetDefaultFontSize(const TextStyle& textStyle)
{
    float fontSizeValue = textStyle.GetFontSize().ConvertToPxDistribute(
        textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    fontSizeFloat_ = MakeRefPtr<AnimatablePropertyFloat>(fontSizeValue);
    AttachProperty(fontSizeFloat_);
}

void TextContentModifier::SetDefaultAdaptMinFontSize(const TextStyle& textStyle)
{
    float fontSizeValue = textStyle.GetFontSize().Value();
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    if (pipelineContext) {
        fontSizeValue = textStyle.GetAdaptMinFontSize().ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    }

    adaptMinFontSizeFloat_ = MakeRefPtr<AnimatablePropertyFloat>(fontSizeValue);
    AttachProperty(adaptMinFontSizeFloat_);
}

void TextContentModifier::SetDefaultAdaptMaxFontSize(const TextStyle& textStyle)
{
    float fontSizeValue = textStyle.GetFontSize().Value();
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    if (pipelineContext) {
        fontSizeValue = textStyle.GetAdaptMaxFontSize().ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    }

    adaptMaxFontSizeFloat_ = MakeRefPtr<AnimatablePropertyFloat>(fontSizeValue);
    AttachProperty(adaptMaxFontSizeFloat_);
}

void TextContentModifier::SetDefaultFontWeight(const TextStyle& textStyle)
{
    fontWeightFloat_ =
        MakeRefPtr<AnimatablePropertyFloat>(static_cast<float>(ConvertFontWeight(textStyle.GetFontWeight())));
    AttachProperty(fontWeightFloat_);
}

void TextContentModifier::SetDefaultTextColor(const TextStyle& textStyle)
{
    animatableTextColor_ = MakeRefPtr<AnimatablePropertyColor>(LinearColor(textStyle.GetTextColor()));
    AttachProperty(animatableTextColor_);
}

void TextContentModifier::SetDefaultTextShadow(const TextStyle& textStyle)
{
    auto&& textShadows = textStyle.GetTextShadows();
    if (textShadows.empty()) {
        AddDefaultShadow();
        return;
    }
    shadows_.clear();
    shadows_.reserve(textShadows.size());
    for (auto&& textShadow : textShadows) {
        AddShadow(textShadow);
    }
}

void TextContentModifier::AddShadow(const Shadow& shadow)
{
    auto shadowBlurRadiusFloat = MakeRefPtr<AnimatablePropertyFloat>(shadow.GetBlurRadius());
    auto shadowOffsetXFloat = MakeRefPtr<AnimatablePropertyFloat>(shadow.GetOffset().GetX());
    auto shadowOffsetYFloat = MakeRefPtr<AnimatablePropertyFloat>(shadow.GetOffset().GetY());
    auto shadowColor = MakeRefPtr<AnimatablePropertyColor>(LinearColor(shadow.GetColor()));
    Shadow textShadow;
    textShadow.SetBlurRadius(shadow.GetBlurRadius());
    textShadow.SetOffset(shadow.GetOffset());
    textShadow.SetColor(shadow.GetColor());
    shadows_.emplace_back(ShadowProp {
        .blurRadius = shadowBlurRadiusFloat,
        .offsetX = shadowOffsetXFloat,
        .offsetY = shadowOffsetYFloat,
        .color = shadowColor });
    AttachProperty(shadowBlurRadiusFloat);
    AttachProperty(shadowOffsetXFloat);
    AttachProperty(shadowOffsetYFloat);
    AttachProperty(shadowColor);
}

void TextContentModifier::SetDefaultTextDecoration(const TextStyle& textStyle)
{
    textDecoration_ = textStyle.GetTextDecoration();
    textDecorationColor_ = textStyle.GetTextDecorationColor();
    textDecorationColorAlpha_ = MakeRefPtr<AnimatablePropertyFloat>(
        textDecoration_ == TextDecoration::NONE ? 0.0f : textDecorationColor_->GetAlpha());
    AttachProperty(textDecorationColorAlpha_);
}
void TextContentModifier::SetDefaultBaselineOffset(const TextStyle& textStyle)
{
    float baselineOffset = textStyle.GetBaselineOffset().Value();
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    if (pipelineContext) {
        baselineOffset = textStyle.GetBaselineOffset().ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    }

    baselineOffsetFloat_ = MakeRefPtr<AnimatablePropertyFloat>(baselineOffset);
    AttachProperty(baselineOffsetFloat_);
}

void TextContentModifier::SetDefaultLineHeight(const TextStyle& textStyle)
{
    float lineHeight = textStyle.GetLineHeight().Value();
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    if (pipelineContext) {
        lineHeight = textStyle.GetLineHeight().ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    }

    lineHeightFloat_ = MakeRefPtr<AnimatablePropertyFloat>(lineHeight);
    AttachProperty(lineHeightFloat_);
}

void TextContentModifier::SetClip(bool clip)
{
    if (clip_) {
        clip_->Set(clip);
    }
}

void TextContentModifier::SetFontReady(bool value)
{
    if (fontReady_) {
        fontReady_->Set(value);
    }
}

void TextContentModifier::UpdateImageNodeVisible(const VisibleType visible)
{
    for (const auto& imageWeak : imageNodeList_) {
        auto imageNode = imageWeak.Upgrade();
        if (!imageNode) {
            continue;
        }
        auto layoutProperty = imageNode->GetLayoutProperty();
        if (!layoutProperty) {
            continue;
        }
        layoutProperty->UpdateVisibility(visible, true);
    }
}

void TextContentModifier::PaintImage(RSCanvas& canvas, float x, float y)
{
    if (!AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        return;
    }
    auto pattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    size_t index = 0;
    auto rectsForPlaceholders = pattern->GetRectsForPlaceholders();
    auto placeHolderIndexVector = pattern->GetPlaceHolderIndex();
    auto placeholdersSize = rectsForPlaceholders.size();
    auto placeHolderIndexSize = static_cast<int32_t>(placeHolderIndexVector.size());
    for (const auto& imageWeak : imageNodeList_) {
        auto imageChild = imageWeak.Upgrade();
        if (!imageChild) {
            continue;
        }
        if (index >= placeholdersSize) {
            return;
        }
        auto tmp = static_cast<int32_t>(placeHolderIndexVector.at(index));
        if (tmp >= placeHolderIndexSize || tmp < 0) {
            return;
        }
        auto rect = rectsForPlaceholders.at(tmp);
        if (!DrawImage(imageChild, canvas, x, y, rect)) {
            continue;
        }
        ++index;
    }
}

bool TextContentModifier::DrawImage(const RefPtr<FrameNode>& imageNode, RSCanvas& canvas, float x, float y,
    const RectF& rect)
{
    CHECK_NULL_RETURN(imageNode, false);
    auto imagePattern = DynamicCast<ImagePattern>(imageNode->GetPattern());
    if (!imagePattern) {
        return false;
    }
    auto layoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    if (!layoutProperty) {
        return false;
    }
    const auto& marginProperty = layoutProperty->GetMarginProperty();
    float marginTop = 0.0f;
    float marginLeft = 0.0f;
    if (marginProperty) {
        marginLeft = marginProperty->left.has_value() ? marginProperty->left->GetDimension().ConvertToPx() : 0.0f;
        marginTop = marginProperty->top.has_value() ? marginProperty->top->GetDimension().ConvertToPx() : 0.0f;
    }
    auto canvasImage = imagePattern->GetCanvasImage();
    if (!canvasImage) {
        canvasImage = imagePattern->GetAltCanvasImage();
    }
    if (AceType::InstanceOf<AnimatedImage>(canvasImage)) {
        auto animatedImage = DynamicCast<AnimatedImage>(canvasImage);
        if (!animatedImage->GetIsAnimating()) {
            animatedImage->ControlAnimation(true);
        }
    }
    auto geometryNode = imageNode->GetGeometryNode();
    if (!canvasImage || !geometryNode) {
        return false;
    }
    RectF imageRect(rect.Left() + x + marginLeft, rect.Top() + y + marginTop,
        geometryNode->GetFrameSize().Width(), geometryNode->GetFrameSize().Height());
    const auto config = canvasImage->GetPaintConfig();
    SizeF imageSize(canvasImage->GetWidth(), canvasImage->GetHeight());
    SizeF contentSize(geometryNode->GetFrameSize().Width(), geometryNode->GetFrameSize().Height());
    RectF srcRect;
    RectF dstRect;
    ImagePainter::ApplyImageFit(config.imageFit_, imageSize, contentSize, srcRect, dstRect);
    canvasImage->DrawRect(canvas, ToRSRect(srcRect), ToRSRect(imageRect));
    return true;
}

void TextContentModifier::PaintCustomSpan(DrawingContext& drawingContext)
{
    auto pattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto pManager = pattern->GetParagraphManager();
    CHECK_NULL_VOID(pManager);
    auto rectsForPlaceholders = pattern->GetRectsForPlaceholders();
    auto customSpanPlaceholderInfo = pattern->GetCustomSpanPlaceholderInfo();
    auto x = paintOffset_.GetX();
    auto y = paintOffset_.GetY();
    auto rectsForPlaceholderSize = rectsForPlaceholders.size();
    for (const auto& customSpanPlaceholder : customSpanPlaceholderInfo) {
        if (!customSpanPlaceholder.onDraw) {
            continue;
        }
        auto index = customSpanPlaceholder.customSpanIndex;
        if (index >= static_cast<int32_t>(rectsForPlaceholderSize) || index < 0) {
            return;
        }
        auto rect = rectsForPlaceholders.at(index);
        auto lineMetrics = pManager->GetLineMetricsByRectF(rect, customSpanPlaceholder.paragraphIndex);
        CustomSpanOptions customSpanOptions;
        customSpanOptions.x = static_cast<float>(rect.Left()) + x;
        customSpanOptions.lineTop = lineMetrics.y + y;
        customSpanOptions.lineBottom = lineMetrics.y + lineMetrics.height + y;
        customSpanOptions.baseline = lineMetrics.y + lineMetrics.ascender + y;
        customSpanPlaceholder.onDraw(drawingContext, customSpanOptions);
    }
}

void TextContentModifier::onDraw(DrawingContext& drawingContext)
{
    auto textPattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textPattern);
    auto pManager = textPattern->GetParagraphManager();
    CHECK_NULL_VOID(pManager);
    auto host = textPattern->GetHost();
    CHECK_NULL_VOID(host);
    if (pManager->GetParagraphs().empty()) {
        textPattern->DumpRecord("onDraw GetParagraphs empty:" + std::to_string(host->GetId()));
        return;
    }
    ACE_SCOPED_TRACE("[Text][id:%d] paint[offset:%f,%f]", host->GetId(), paintOffset_.GetX(), paintOffset_.GetY());

    PropertyChangeFlag flag = 0;
    if (NeedMeasureUpdate(flag)) {
        host->MarkDirtyNode(flag);
        auto layoutProperty = host->GetLayoutProperty<TextLayoutProperty>();
        CHECK_NULL_VOID(layoutProperty);
        layoutProperty->OnPropertyChangeMeasure();
    }

    if (!ifPaintObscuration_) {
        auto& canvas = drawingContext.canvas;
        CHECK_NULL_VOID(contentSize_);
        CHECK_NULL_VOID(contentOffset_);
        auto contentSize = contentSize_->Get();
        auto contentOffset = contentOffset_->Get();
        canvas.Save();
        if (clip_ && clip_->Get() &&
            (!fontSize_.has_value() || !fontSizeFloat_ ||
                NearEqual(fontSize_.value().Value(), fontSizeFloat_->Get()))) {
            RSRect clipInnerRect = RSRect(contentOffset.GetX(), contentOffset.GetY(),
                contentSize.Width() + contentOffset.GetX(), contentSize.Height() + contentOffset.GetY());
            canvas.ClipRect(clipInnerRect, RSClipOp::INTERSECT);
        }
        if (!CheckMarqueeState(MarqueeState::RUNNING)) {
            auto paintOffsetY = paintOffset_.GetY();
            textPattern->DumpRecord(std::to_string(host->GetId()) + " ,paintOffset:" + paintOffset_.ToString().c_str());
            auto paragraphs = pManager->GetParagraphs();
            for (auto&& info : paragraphs) {
                auto paragraph = info.paragraph;
                CHECK_NULL_VOID(paragraph);
                ChangeParagraphColor(paragraph);
                paragraph->Paint(canvas, paintOffset_.GetX(), paintOffsetY);
                paintOffsetY += paragraph->GetHeight();
            }
        } else {
            // Racing
            DrawTextRacing(drawingContext);
        }
        canvas.Restore();
    } else {
        DrawObscuration(drawingContext);
    }
    PaintCustomSpan(drawingContext);
}

void TextContentModifier::DrawTextRacing(DrawingContext& drawingContext)
{
    auto pattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto pManager = pattern->GetParagraphManager();
    CHECK_NULL_VOID(pManager);
    auto paragraph = pManager->GetParagraphs().front().paragraph;
    CHECK_NULL_VOID(paragraph);
    ChangeParagraphColor(paragraph);
    float textRacePercent = GetTextRaceDirection() == TextDirection::LTR ?
        GetTextRacePercent() : RACE_MOVE_PERCENT_MAX - GetTextRacePercent();
    float paragraph1Offset =
        (paragraph->GetTextWidth() + textRaceSpaceWidth_) * textRacePercent / RACE_MOVE_PERCENT_MAX * -1;
    if ((paintOffset_.GetX() + paragraph1Offset + paragraph->GetTextWidth()) > 0) {
        paragraph->Paint(drawingContext.canvas, paintOffset_.GetX() + paragraph1Offset, paintOffset_.GetY());
        PaintImage(drawingContext.canvas, paintOffset_.GetX() + paragraph1Offset, paintOffset_.GetY());
    }
    float paragraph2Offset = paragraph1Offset + paragraph->GetTextWidth() + textRaceSpaceWidth_;
    if ((paintOffset_.GetX() + paragraph2Offset) < drawingContext.width) {
        paragraph->Paint(drawingContext.canvas, paintOffset_.GetX() + paragraph2Offset, paintOffset_.GetY());
        PaintImage(drawingContext.canvas, paintOffset_.GetX() + paragraph2Offset, paintOffset_.GetY());
    }
}

void TextContentModifier::ChangeParagraphColor(const RefPtr<Paragraph>& paragraph)
{
    CHECK_NULL_VOID(paragraph);
    if (onlyTextColorAnimation_ && animatableTextColor_) {
        auto length = paragraph->GetParagraphText().length();
        paragraph->UpdateColor(0, length, Color(animatableTextColor_->Get().GetValue()));
    }
}

void TextContentModifier::DrawObscuration(DrawingContext& drawingContext)
{
    RSCanvas& canvas = drawingContext.canvas;
    RSBrush brush;
    std::vector<RSPoint> radiusXY(POINT_COUNT);
    Dimension borderRadius = Dimension(2.0, DimensionUnit::VP);
    for (auto& radius : radiusXY) {
        radius.SetX(static_cast<float>(borderRadius.ConvertToPx()));
        radius.SetY(static_cast<float>(borderRadius.ConvertToPx()));
    }
    CHECK_NULL_VOID(animatableTextColor_);
    Color fillColor = Color(animatableTextColor_->Get().GetValue());
    RSColor rrSColor(fillColor.GetRed(), fillColor.GetGreen(), fillColor.GetBlue(),
        (uint32_t)(fillColor.GetAlpha() * OBSCURED_ALPHA));
    brush.SetColor(rrSColor);
    brush.SetAntiAlias(true);
    canvas.AttachBrush(brush);
    CHECK_NULL_VOID(fontSizeFloat_);
    float fontSize = fontSizeFloat_->Get();
    std::vector<float> textLineWidth;
    float currentLineWidth = 0.0f;
    int32_t maxLineCount = 0;
    CHECK_NULL_VOID(contentSize_);
    CHECK_NULL_VOID(contentOffset_);
    for (auto i = 0U; i < drawObscuredRects_.size(); i++) {
        if (!NearEqual(drawObscuredRects_[i].Width(), 0.0f) && !NearEqual(drawObscuredRects_[i].Height(), 0.0f)) {
            currentLineWidth += drawObscuredRects_[i].Width();
            if (i == (!drawObscuredRects_.empty() ? drawObscuredRects_.size() - 1 : 0)) {
                textLineWidth.push_back(currentLineWidth);
                maxLineCount += LessOrEqual(drawObscuredRects_[i].Bottom(), contentSize_->Get().Height()) ? 1 : 0;
            } else if (!NearEqual(drawObscuredRects_[i].Bottom(), drawObscuredRects_[i + 1].Bottom())) {
                textLineWidth.push_back(currentLineWidth);
                maxLineCount += LessOrEqual(drawObscuredRects_[i].Bottom(), contentSize_->Get().Height()) ? 1 : 0;
                currentLineWidth = 0;
            } else {
                /** nothing to do **/
            }
        }
    }
    CHECK_NULL_VOID(baselineOffsetFloat_);
    auto baselineOffset = baselineOffsetFloat_->Get();
    int32_t obscuredLineCount = std::min(maxLineCount, static_cast<int32_t>(textLineWidth.size()));
    float offsetY = (contentSize_->Get().Height() - std::fabs(baselineOffset) - (obscuredLineCount * fontSize)) /
                    (obscuredLineCount + 1);
    for (auto i = 0; i < obscuredLineCount; i++) {
        RSRoundRect rSRoundRect(
            RSRect(contentOffset_->Get().GetX(),
                contentOffset_->Get().GetY() - std::min(baselineOffset, 0.0f) +
                    std::max(offsetY + ((offsetY + fontSize) * i), 0.0f),
                contentOffset_->Get().GetX() + std::min(textLineWidth[i], contentSize_->Get().Width()),
                contentOffset_->Get().GetY() - std::min(baselineOffset, 0.0f) +
                    std::min(offsetY + ((offsetY + fontSize) * i) + fontSize, contentSize_->Get().Height())),
            radiusXY);
        canvas.DrawRoundRect(rSRoundRect);
    }
    canvas.DetachBrush();
}

void TextContentModifier::ModifyFontSizeInTextStyle(TextStyle& textStyle)
{
    if (fontSize_.has_value() && fontSizeFloat_) {
        lastFontSize_ = fontSizeFloat_->Get();
        textStyle.SetFontSize(Dimension(fontSizeFloat_->Get(), DimensionUnit::PX));
    }
}

void TextContentModifier::ModifyAdaptMinFontSizeInTextStyle(TextStyle& textStyle)
{
    if (adaptMinFontSize_.has_value() && adaptMinFontSizeFloat_) {
        lastMinFontSize_ = adaptMinFontSizeFloat_->Get();
        textStyle.SetAdaptMinFontSize(Dimension(adaptMinFontSizeFloat_->Get(), DimensionUnit::PX));
    }
}

void TextContentModifier::ModifyAdaptMaxFontSizeInTextStyle(TextStyle& textStyle)
{
    if (adaptMaxFontSize_.has_value() && adaptMaxFontSizeFloat_) {
        lastMaxFontSize_ = adaptMaxFontSizeFloat_->Get();
        textStyle.SetAdaptMaxFontSize(Dimension(adaptMaxFontSizeFloat_->Get(), DimensionUnit::PX));
    }
}

void TextContentModifier::ModifyFontWeightInTextStyle(TextStyle& textStyle)
{
    if (fontWeight_.has_value() && fontWeightFloat_) {
        lastFontWeight_ = fontWeightFloat_->Get();
        textStyle.SetFontWeight(static_cast<FontWeight>(std::floor(fontWeightFloat_->Get() + ROUND_VALUE)));
    }
}

void TextContentModifier::ModifyTextColorInTextStyle(TextStyle& textStyle)
{
    if (textColor_.has_value() && animatableTextColor_) {
        lastTextColor_.SetValue(animatableTextColor_->Get().GetValue());
        textStyle.SetTextColor(Color(animatableTextColor_->Get().GetValue()));
    }
}

void TextContentModifier::ModifyTextShadowsInTextStyle(TextStyle& textStyle)
{
    std::vector<Shadow> shadows;
    shadows.reserve(shadows_.size());
    for (auto&& shadow : shadows_) {
        auto blurRadius = shadow.blurRadius->Get();
        auto offsetX = shadow.offsetX->Get();
        auto offsetY = shadow.offsetY->Get();
        auto color = shadow.color->Get();
        auto shadowValue = Shadow(blurRadius, 0, Offset(offsetX, offsetY), Color(color.GetValue()));
        shadow.lastShadow = shadowValue;
        shadows.emplace_back(shadowValue);
    }
    textStyle.SetTextShadows(shadows);
}

void TextContentModifier::ModifyDecorationInTextStyle(TextStyle& textStyle)
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
            lastTextDecorationColorAlpha_ = textDecorationColorAlpha_->Get();
        } else {
            textStyle.SetTextDecoration(textDecoration_.value());
            textStyle.SetTextDecorationColor(textDecorationColor_.value());
        }
    }
}

void TextContentModifier::ModifyBaselineOffsetInTextStyle(TextStyle& textStyle)
{
    if (baselineOffset_.has_value() && baselineOffsetFloat_) {
        lastBaselineOffsetFloat_ = baselineOffsetFloat_->Get();
        textStyle.SetBaselineOffset(Dimension(baselineOffsetFloat_->Get(), DimensionUnit::PX));
    }
}

void TextContentModifier::ModifyLineHeightInTextStyle(TextStyle& textStyle)
{
    if (lineHeight_.has_value() && lineHeightFloat_) {
        lastLineHeight_ = lineHeightFloat_->Get();
        textStyle.SetLineHeight(Dimension(lineHeightFloat_->Get(), DimensionUnit::PX));
    }
}

void TextContentModifier::ModifyTextStyle(TextStyle& textStyle)
{
    ModifyFontSizeInTextStyle(textStyle);
    ModifyAdaptMinFontSizeInTextStyle(textStyle);
    ModifyAdaptMaxFontSizeInTextStyle(textStyle);
    ModifyFontWeightInTextStyle(textStyle);
    ModifyTextColorInTextStyle(textStyle);
    ModifyTextShadowsInTextStyle(textStyle);
    ModifyDecorationInTextStyle(textStyle);
    ModifyBaselineOffsetInTextStyle(textStyle);
    ModifyLineHeightInTextStyle(textStyle);
}

void TextContentModifier::UpdateFontSizeMeasureFlag(PropertyChangeFlag& flag)
{
    if (fontSize_.has_value() && fontSizeFloat_ && !NearEqual(lastFontSize_, fontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastFontSize_ = fontSizeFloat_->Get();
    }
}

void TextContentModifier::UpdateAdaptMinFontSizeMeasureFlag(PropertyChangeFlag& flag)
{
    if (adaptMinFontSize_.has_value() && adaptMinFontSizeFloat_ &&
        !NearEqual(lastMinFontSize_, adaptMinFontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastMinFontSize_ = adaptMinFontSizeFloat_->Get();
    }
}

void TextContentModifier::UpdateAdaptMaxFontSizeMeasureFlag(PropertyChangeFlag& flag)
{
    if (adaptMaxFontSize_.has_value() && adaptMaxFontSizeFloat_ &&
        !NearEqual(lastMaxFontSize_, adaptMaxFontSizeFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastMaxFontSize_ = adaptMaxFontSizeFloat_->Get();
    }
}

void TextContentModifier::UpdateFontWeightMeasureFlag(PropertyChangeFlag& flag)
{
    if (fontWeight_.has_value() && fontWeightFloat_ && !NearEqual(lastFontWeight_, fontWeightFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastFontWeight_ = fontWeightFloat_->Get();
    }
}

void TextContentModifier::UpdateTextColorMeasureFlag(PropertyChangeFlag& flag)
{
    if (textColor_.has_value() && animatableTextColor_ &&
        lastTextColor_.GetValue() != animatableTextColor_->Get().GetValue()) {
        flag |= PROPERTY_UPDATE_MEASURE_SELF;
        lastTextColor_.SetValue(animatableTextColor_->Get().GetValue());
    }
}

void TextContentModifier::UpdateTextShadowMeasureFlag(PropertyChangeFlag& flag)
{
    for (auto&& shadow : shadows_) {
        auto blurRadius = shadow.blurRadius->Get();
        auto offsetX = shadow.offsetX->Get();
        auto offsetY = shadow.offsetY->Get();
        auto color = shadow.color->Get();
        auto compareShadow = Shadow(blurRadius, 0, Offset(offsetX, offsetY), Color(color.GetValue()));
        if (shadow.lastShadow != compareShadow) {
            flag |= PROPERTY_UPDATE_MEASURE;
            shadow.lastShadow = compareShadow;
            return;
        }
    }
}

void TextContentModifier::UpdateTextDecorationMeasureFlag(PropertyChangeFlag& flag)
{
    if (textDecoration_.has_value() && textDecorationColor_.has_value() && textDecorationColorAlpha_) {
        uint8_t alpha = static_cast<uint8_t>(std::floor(textDecorationColorAlpha_->Get() + ROUND_VALUE));
        if (textDecoration_.value() == TextDecoration::UNDERLINE &&
            !NearEqual(textDecorationColorAlpha_->Get(), lastTextDecorationColorAlpha_)) {
            flag |= PROPERTY_UPDATE_MEASURE;
        } else if (textDecoration_.value() == TextDecoration::NONE &&
                   (alpha != 0.0 || !NearZero(lastTextDecorationColorAlpha_))) {
            flag |= PROPERTY_UPDATE_MEASURE;
        }
        lastTextDecorationColorAlpha_ = textDecorationColorAlpha_->Get();
    }
}

void TextContentModifier::UpdateBaselineOffsetMeasureFlag(PropertyChangeFlag& flag)
{
    if (baselineOffset_.has_value() && baselineOffsetFloat_ &&
        !NearEqual(lastBaselineOffsetFloat_, baselineOffsetFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastBaselineOffsetFloat_ = baselineOffsetFloat_->Get();
    }
}

void TextContentModifier::UpdateLineHeightMeasureFlag(PropertyChangeFlag& flag)
{
    if (lineHeight_.has_value() && lineHeightFloat_ &&
        !NearEqual(lastLineHeight_, lineHeightFloat_->Get())) {
        flag |= PROPERTY_UPDATE_MEASURE;
        lastLineHeight_ = lineHeightFloat_->Get();
    }
}

bool TextContentModifier::NeedMeasureUpdate(PropertyChangeFlag& flag)
{
    flag = 0;
    UpdateFontSizeMeasureFlag(flag);
    UpdateAdaptMinFontSizeMeasureFlag(flag);
    UpdateAdaptMaxFontSizeMeasureFlag(flag);
    UpdateFontWeightMeasureFlag(flag);
    UpdateTextShadowMeasureFlag(flag);
    UpdateTextDecorationMeasureFlag(flag);
    UpdateBaselineOffsetMeasureFlag(flag);
    UpdateLineHeightMeasureFlag(flag);
    flag &= (PROPERTY_UPDATE_MEASURE | PROPERTY_UPDATE_MEASURE_SELF | PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    if (flag) {
        onlyTextColorAnimation_ = false;
    }
    if (!onlyTextColorAnimation_) {
        UpdateTextColorMeasureFlag(flag);
        flag &= (PROPERTY_UPDATE_MEASURE | PROPERTY_UPDATE_MEASURE_SELF | PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    }
    return flag;
}

void TextContentModifier::SetFontSize(const Dimension& value, const TextStyle& textStyle, bool isReset)
{
    auto fontSizeValue =
        value.ConvertToPxDistribute(textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    if (!isReset) {
        fontSize_ = Dimension(fontSizeValue);
    } else {
        fontSize_ = std::nullopt;
    }
    CHECK_NULL_VOID(fontSizeFloat_);
    fontSizeFloat_->Set(fontSizeValue);
}

void TextContentModifier::SetAdaptMinFontSize(const Dimension& value, const TextStyle& textStyle, bool isReset)
{
    auto fontSizeValue =
        value.ConvertToPxDistribute(textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    if (!isReset) {
        adaptMinFontSize_ = Dimension(fontSizeValue);
    } else {
        adaptMinFontSize_ = std::nullopt;
    }
    CHECK_NULL_VOID(adaptMinFontSizeFloat_);
    adaptMinFontSizeFloat_->Set(fontSizeValue);
}

void TextContentModifier::SetAdaptMaxFontSize(const Dimension& value, const TextStyle& textStyle, bool isReset)
{
    auto fontSizeValue =
        value.ConvertToPxDistribute(textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
    if (!isReset) {
        adaptMaxFontSize_ = Dimension(fontSizeValue);
    } else {
        adaptMaxFontSize_ = std::nullopt;
    }
    CHECK_NULL_VOID(adaptMaxFontSizeFloat_);
    adaptMaxFontSizeFloat_->Set(fontSizeValue);
}

void TextContentModifier::SetFontWeight(const FontWeight& value, bool isReset)
{
    if (!isReset) {
        fontWeight_ = ConvertFontWeight(value);
    } else {
        fontWeight_ = std::nullopt;
    }
    CHECK_NULL_VOID(fontWeightFloat_);
    fontWeightFloat_->Set(static_cast<int>(ConvertFontWeight(value)));
}

void TextContentModifier::SetTextColor(const Color& value, bool isReset)
{
    onlyTextColorAnimation_ = false;
    if (!isReset) {
        textColor_ = value;
    } else {
        textColor_ = std::nullopt;
    }
    CHECK_NULL_VOID(animatableTextColor_);
    animatableTextColor_->Set(LinearColor(value));
}

void TextContentModifier::TextColorModifier(const Color& value)
{
    SetTextColor(value);
    onlyTextColorAnimation_ = true;
}

void TextContentModifier::SetTextShadow(const std::vector<Shadow>& value)
{
    while (value.size() > shadows_.size()) {
        AddDefaultShadow();
    }
    // else
    while (value.size() < shadows_.size()) {
        shadows_.pop_back();
    }

    for (size_t i = 0; i < shadows_.size(); ++i) {
        auto&& newShadow = value[i];
        Shadow textShadow;
        textShadow.SetBlurRadius(newShadow.GetBlurRadius());
        textShadow.SetOffset(newShadow.GetOffset());
        textShadow.SetColor(newShadow.GetColor());
        shadows_[i].blurRadius->Set(newShadow.GetBlurRadius());
        shadows_[i].offsetX->Set(newShadow.GetOffset().GetX());
        shadows_[i].offsetY->Set(newShadow.GetOffset().GetY());
        shadows_[i].color->Set(LinearColor(newShadow.GetColor()));
    }
}

void TextContentModifier::SetTextDecoration(const TextDecoration& type, bool isReset)
{
    auto oldTextDecoration = textDecoration_.value_or(TextDecoration::NONE);
    if (oldTextDecoration == type) {
        return;
    }

    textDecorationAnimatable_ = (oldTextDecoration == TextDecoration::NONE && type == TextDecoration::UNDERLINE) ||
                                (oldTextDecoration == TextDecoration::UNDERLINE && type == TextDecoration::NONE);
    if (!isReset) {
        textDecoration_ = type;
    } else {
        textDecoration_ = std::nullopt;
    }
    CHECK_NULL_VOID(textDecorationColorAlpha_);

    if (textDecoration_.has_value() && textDecoration_.value() == TextDecoration::NONE) {
        textDecorationColorAlpha_->Set(0.0f);
    } else if (textDecorationColor_.has_value()) {
        textDecorationColorAlpha_->Set(static_cast<float>(textDecorationColor_.value().GetAlpha()));
    }
}

void TextContentModifier::SetTextDecorationColor(const Color& color, bool isReset)
{
    if (!isReset) {
        textDecorationColor_ = color;
    } else {
        textDecorationColor_ = std::nullopt;
    }
}

void TextContentModifier::SetBaselineOffset(const Dimension& value, const TextStyle& textStyle, bool isReset)
{
    float baselineOffsetValue = 0.0f;
    if (!isReset) {
        baselineOffsetValue = value.ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
        baselineOffset_ = Dimension(baselineOffsetValue);
    } else {
        baselineOffset_ = std::nullopt;
    }
    CHECK_NULL_VOID(baselineOffsetFloat_);
    baselineOffsetFloat_->Set(baselineOffsetValue);
}

void TextContentModifier::SetLineHeight(const Dimension& value, const TextStyle& textStyle, bool isReset)
{
    float lineHeightValue = 0.0f;
    if (!isReset) {
        lineHeightValue = value.ConvertToPxDistribute(
            textStyle.GetMinFontScale(), textStyle.GetMaxFontScale(), textStyle.IsAllowScale());
        lineHeight_ = Dimension(lineHeightValue);
    } else {
        lineHeight_ = std::nullopt;
    }
    CHECK_NULL_VOID(lineHeightFloat_);
    lineHeightFloat_->Set(lineHeightValue);
}

void TextContentModifier::SetContentOffset(OffsetF& value)
{
    CHECK_NULL_VOID(contentOffset_);
    contentOffset_->Set(value);
}

void TextContentModifier::SetContentSize(SizeF& value)
{
    CHECK_NULL_VOID(contentSize_);
    contentSize_->Set(value);
}

void TextContentModifier::StartTextRace()
{
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        UpdateImageNodeVisible(VisibleType::INVISIBLE);
    }
    if (!CheckMarqueeState(MarqueeState::IDLE) && !CheckMarqueeState(MarqueeState::STOPPED)) {
        return;
    }

    textRaceSpaceWidth_ = RACE_SPACE_WIDTH;
    auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
    if (pipeline) {
        textRaceSpaceWidth_ *= pipeline->GetDipScale();
    }

    AnimationOption option = AnimationOption();
    RefPtr<Curve> curve = MakeRefPtr<LinearCurve>();
    option.SetDuration(GetDuration());
    option.SetDelay(0);
    option.SetCurve(curve);
    option.SetIteration(-1);
    raceAnimation_ = AnimationUtils::StartAnimation(option, [weak = WeakClaim(this)]() {
        auto modifier = weak.Upgrade();
        CHECK_NULL_VOID(modifier);
        float startPercent = modifier->GetTextRacePercent();
        modifier->racePercentFloat_->Set(RACE_MOVE_PERCENT_MAX + startPercent);
    });
    SetMarqueeState(MarqueeState::RUNNING);
}

void TextContentModifier::StopTextRace()
{
    if (!CheckMarqueeState(MarqueeState::RUNNING) && !CheckMarqueeState(MarqueeState::PAUSED)) {
        return;
    }
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        UpdateImageNodeVisible(VisibleType::VISIBLE);
    }
    if (raceAnimation_) {
        AnimationUtils::StopAnimation(raceAnimation_);
    }

    SetMarqueeState(MarqueeState::STOPPED);
    ResetTextRacePercent();
}

void TextContentModifier::ResumeAnimation()
{
    CHECK_NULL_VOID(raceAnimation_);
    if (!CheckMarqueeState(MarqueeState::PAUSED)) {
        return;
    }
    AnimationUtils::ResumeAnimation(raceAnimation_);
    SetMarqueeState(MarqueeState::RUNNING);
}

void TextContentModifier::PauseAnimation()
{
    CHECK_NULL_VOID(raceAnimation_);
    if (!CheckMarqueeState(MarqueeState::RUNNING)) {
        return;
    }
    AnimationUtils::PauseAnimation(raceAnimation_);
    SetMarqueeState(MarqueeState::PAUSED);
}

float TextContentModifier::GetTextRacePercent()
{
    float percent = 0;
    if (racePercentFloat_) {
        percent = racePercentFloat_->Get();
    }
    return percent;
}

TextDirection TextContentModifier::GetTextRaceDirection() const
{
    auto textPattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_RETURN(textPattern, TextDirection::LTR);
    auto frameNode = textPattern->GetHost();
    CHECK_NULL_RETURN(frameNode, TextDirection::LTR);
    auto layoutProperty = frameNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, TextDirection::LTR);
    auto direction = layoutProperty->GetLayoutDirection();
    if (direction == TextDirection::AUTO) {
        direction = GetTextRaceDirectionByContent();
    }
    return direction;
}

TextDirection TextContentModifier::GetTextRaceDirectionByContent() const
{
    auto textPattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_RETURN(textPattern, TextDirection::LTR);
    auto pManager = textPattern->GetParagraphManager();
    CHECK_NULL_RETURN(pManager, TextDirection::LTR);
    if (pManager->GetParagraphs().size() == 0) {
        return TextDirection::LTR;
    }
    auto paragraph = pManager->GetParagraphs().front().paragraph;
    CHECK_NULL_RETURN(paragraph, TextDirection::LTR);
    auto paragraphText = StringUtils::Str16ToStr8(paragraph->GetParagraphText());
    auto content = StringUtils::ToWstring(paragraphText);
    for (const auto& charFromContent : content) {
        if (TextLayoutadapter::IsLeftToRight(charFromContent)) {
            return TextDirection::LTR;
        } else if (TextLayoutadapter::IsRightToLeft(charFromContent)) {
            return TextDirection::RTL;
        } else if (TextLayoutadapter::IsRightTOLeftArabic(charFromContent)) {
            return TextDirection::RTL;
        }
    }
    return AceApplicationInfo::GetInstance().IsRightToLeft() ?
        TextDirection::RTL : TextDirection::LTR;
}

void TextContentModifier::ResetTextRacePercent() const
{
    if (GetTextRaceDirection() == TextDirection::LTR) {
        // LTR start 0%
        racePercentFloat_->Set(RACE_MOVE_PERCENT_MIN);
        return;
    }
    // RTL
    auto textPattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(textPattern);
    auto pManager = textPattern->GetParagraphManager();
    CHECK_NULL_VOID(pManager);
    if (pManager->GetParagraphs().size() == 0) {
        return;
    }
    auto paragraph = pManager->GetParagraphs().front().paragraph;
    CHECK_NULL_VOID(paragraph);
    auto textRectWidth = textPattern->GetTextRect().Width();
    float textWidth = paragraph->GetTextWidth();
    racePercentFloat_->Set(
        (textRaceSpaceWidth_ + textRectWidth) / (textWidth + textRaceSpaceWidth_) *
        RACE_MOVE_PERCENT_MAX - RACE_MOVE_PERCENT_MAX);
}

void TextContentModifier::ContentChange()
{
    CHECK_NULL_VOID(contentChange_);
    contentChange_->Set(contentChange_->Get() + 1);
}

void TextContentModifier::AddDefaultShadow()
{
    Shadow emptyShadow;
    auto blurRadius = MakeRefPtr<AnimatablePropertyFloat>(emptyShadow.GetBlurRadius());
    auto offsetX = MakeRefPtr<AnimatablePropertyFloat>(emptyShadow.GetOffset().GetX());
    auto offsetY = MakeRefPtr<AnimatablePropertyFloat>(emptyShadow.GetOffset().GetY());
    auto color = MakeRefPtr<AnimatablePropertyColor>(LinearColor(emptyShadow.GetColor()));
    shadows_.emplace_back(ShadowProp {
        .blurRadius = blurRadius, .offsetX = offsetX, .offsetY = offsetY, .color = color });
    AttachProperty(blurRadius);
    AttachProperty(offsetX);
    AttachProperty(offsetY);
    AttachProperty(color);
}

void TextContentModifier::SetMarqueeState(MarqueeState state)
{
    auto prevState = marqueeState_;
    marqueeState_ = state;
    auto pattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_VOID(pattern);
    auto host = pattern->GetHost();
    CHECK_NULL_VOID(host);
    TAG_LOGI(AceLogTag::ACE_TEXT, "SetMarqueeState: id %{public}d, from state %{public}d to state %{public}d",
        host->GetId(), prevState, state);
}

int32_t TextContentModifier::GetDuration() const
{
    auto pattern = DynamicCast<TextPattern>(pattern_.Upgrade());
    CHECK_NULL_RETURN(pattern, RACE_DURATION);
    auto pManager = pattern->GetParagraphManager();
    CHECK_NULL_RETURN(pManager, RACE_DURATION);
    CHECK_NULL_RETURN(!pManager->GetParagraphs().empty(), RACE_DURATION);
    auto paragraph = pManager->GetParagraphs().front().paragraph;
    CHECK_NULL_RETURN(paragraph, RACE_DURATION);
    auto textRaceWidth = paragraph->GetTextWidth() + textRaceSpaceWidth_;
    if (LessOrEqual(DEFAULT_MARQUEE_SCROLL_AMOUNT.ConvertToPx(), 0.0)) {
        return RACE_DURATION;
    }
    return static_cast<int32_t>(
        textRaceWidth / DEFAULT_MARQUEE_SCROLL_AMOUNT.ConvertToPx() * DEFAULT_MARQUEE_SCROLL_DELAY);
}

void TextContentModifier::ContentModifierDump()
{
    auto& dumpLog = DumpLog::GetInstance();
    if (animatableTextColor_) {
        dumpLog.AddDesc(
            std::string("animatableTextColor: ").append(Color(animatableTextColor_->Get().GetValue()).ColorToString()));
    }
    dumpLog.AddDesc(std::string("onlyTextColorAnimation: ").append(std::to_string(onlyTextColorAnimation_)));
}
} // namespace OHOS::Ace::NG
