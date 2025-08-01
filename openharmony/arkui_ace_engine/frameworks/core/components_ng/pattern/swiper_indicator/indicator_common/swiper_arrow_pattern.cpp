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

#include "core/components_ng/pattern/swiper_indicator/indicator_common/swiper_arrow_pattern.h"

#include "base/utils/utils.h"
#include "core/components/theme/icon_theme.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/gesture_event_hub.h"
#include "core/components_ng/pattern/swiper/swiper_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {
void SwiperArrowPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    if (isFirstCreate_) {
        InitNavigationArrow();
        auto swiperNode = GetSwiperNode();
        CHECK_NULL_VOID(swiperNode);
        auto swiperEventHub = swiperNode->GetOrCreateEventHub<SwiperEventHub>();
        CHECK_NULL_VOID(swiperEventHub);
        auto layoutProperty = GetSwiperArrowLayoutProperty();
        CHECK_NULL_VOID(layoutProperty);
        InitSwiperChangeEvent(swiperEventHub);
        index_ = layoutProperty->GetIndex().value_or(0);
        isFirstCreate_ = false;
        InitEvent();
        InitOnKeyEvent();
    } else {
        UpdateArrowContent();
    }
    UpdateButtonNode(index_);
    InitAccessibilityText();
}

void SwiperArrowPattern::InitOnKeyEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto focusHub = host->GetFocusHub();
    CHECK_NULL_VOID(focusHub);
    auto onKeyEvent = [wp = WeakClaim(this)](const KeyEvent& event) -> bool {
        auto pattern = wp.Upgrade();
        if (pattern) {
            return pattern->OnKeyEvent(event);
        }
        return false;
    };
    focusHub->SetOnKeyEventInternal(std::move(onKeyEvent));
}

bool SwiperArrowPattern::OnKeyEvent(const KeyEvent& event)
{
    if (event.action != KeyAction::DOWN) {
        return false;
    }

    if (event.code == KeyCode::KEY_ENTER || event.code == KeyCode::KEY_SPACE) {
        OnClick();
        return true;
    }
    return false;
}

void SwiperArrowPattern::OnClick() const
{
    auto swiperNode = GetSwiperNode();
    CHECK_NULL_VOID(swiperNode);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    auto swiperController = swiperPattern->GetSwiperController();
    CHECK_NULL_VOID(swiperController);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (host->GetTag() == V2::SWIPER_LEFT_ARROW_ETS_TAG) {
        if (swiperPattern->IsHorizontalAndRightToLeft()) {
            swiperController->ShowNext();
        } else {
            swiperController->ShowPrevious();
        }
        return;
    }
    if (host->GetTag() == V2::SWIPER_RIGHT_ARROW_ETS_TAG) {
        if (swiperPattern->IsHorizontalAndRightToLeft()) {
            swiperController->ShowPrevious();
        } else {
            swiperController->ShowNext();
        }
    }
}

void SwiperArrowPattern::InitSwiperChangeEvent(const RefPtr<SwiperEventHub>& swiperEventHub)
{
    ChangeEvent changeEvent = [weak = WeakClaim(this)](int32_t index) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->UpdateButtonNode(index);
    };
    if (swiperChangeEvent_) {
        (*swiperChangeEvent_).swap(changeEvent);
    } else {
        swiperChangeEvent_ = std::make_shared<ChangeEvent>(std::move(changeEvent));
        swiperEventHub->AddOnChangeEvent(swiperChangeEvent_);
    }
}

void SwiperArrowPattern::UpdateButtonNode(int32_t index)
{
    index_ = index;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto buttonNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(buttonNode);
    auto symbolNode = DynamicCast<FrameNode>(buttonNode->GetFirstChild());
    CHECK_NULL_VOID(symbolNode);
    SetButtonVisible(isVisible_);
    symbolNode->MarkModifyDone();
}

void SwiperArrowPattern::InitEvent()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto arrowGestureHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(arrowGestureHub);
    auto arrowClickCallback = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->ButtonClickEvent();
    };
    if (arrowClickListener_) {
        arrowGestureHub->RemoveClickEvent(arrowClickListener_);
    }
    arrowClickListener_ = MakeRefPtr<ClickEvent>(std::move(arrowClickCallback));
    arrowGestureHub->AddClickEvent(arrowClickListener_);

    auto buttonNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(buttonNode);

    auto buttonGestureHub = buttonNode->GetOrCreateGestureEventHub();
    auto touchCallback = [weak = WeakClaim(this), weakButton = WeakClaim(RawPtr(buttonNode))](
                             const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto buttonNode = weakButton.Upgrade();
        CHECK_NULL_VOID(buttonNode);
        pattern->ButtonTouchEvent(buttonNode, info.GetTouches().front().GetTouchType());
    };
    buttonTouchListener_ = MakeRefPtr<TouchEventImpl>(std::move(touchCallback));
    buttonGestureHub->AddTouchEvent(buttonTouchListener_);

    auto hoverCallback = [weak = WeakClaim(this), weakButton = WeakClaim(RawPtr(buttonNode))](bool isHovered) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto buttonNode = weakButton.Upgrade();
        CHECK_NULL_VOID(buttonNode);
        pattern->ButtonOnHover(buttonNode, isHovered);
    };
    buttonOnHoverListener_ = MakeRefPtr<InputEvent>(std::move(hoverCallback));
    auto buttonInputHub = buttonNode->GetOrCreateInputEventHub();
    buttonInputHub->AddOnHoverEvent(buttonOnHoverListener_);
}

void SwiperArrowPattern::ButtonClickEvent()
{
    auto swiperArrowLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperArrowLayoutProperty);
    if (!hoverOnClickFlag_ && swiperArrowLayoutProperty->GetHoverShowValue(false)) {
        return;
    }

    OnClick();
}

void SwiperArrowPattern::InitAccessibilityText()
{
    auto swiperNode = GetSwiperNode();
    CHECK_NULL_VOID(swiperNode);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto accessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    auto pipelineContext = host->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    auto preAccessibilityText = swiperIndicatorTheme->GetPreAccessibilityText();
    auto nextAccessibilityText = swiperIndicatorTheme->GetNextAccessibilityText();
    if (host->GetTag() == V2::SWIPER_LEFT_ARROW_ETS_TAG) {
        std::string accessibilityLeftText = "";
        if (swiperPattern->IsHorizontalAndRightToLeft()) {
            accessibilityLeftText = nextAccessibilityText;
        } else {
            accessibilityLeftText = preAccessibilityText;
        }
        accessibilityProperty->SetAccessibilityText(accessibilityLeftText);
        accessibilityProperty->SetAccessibilityCustomRole("button");
    }
    if (host->GetTag() == V2::SWIPER_RIGHT_ARROW_ETS_TAG) {
        std::string accessibilityRightText = "";
        if (swiperPattern->IsHorizontalAndRightToLeft()) {
            accessibilityRightText = preAccessibilityText;
        } else {
            accessibilityRightText = nextAccessibilityText;
        }
        accessibilityProperty->SetAccessibilityText(accessibilityRightText);
        accessibilityProperty->SetAccessibilityCustomRole("button");
    }
}

void SwiperArrowPattern::InitNavigationArrow()
{
    auto buttonNode = FrameNode::GetOrCreateFrameNode(V2::BUTTON_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ButtonPattern>(); });
    auto buttonAccessibilityProperty = buttonNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(buttonAccessibilityProperty);
    buttonAccessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::NO_STR);
    auto buttonNodeFocusHub = buttonNode->GetFocusHub();
    CHECK_NULL_VOID(buttonNodeFocusHub);
    buttonNodeFocusHub->SetParentFocusable(false);
    auto swiperArrowLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperArrowLayoutProperty);
    auto imageNode = FrameNode::CreateFrameNode(
        V2::IMAGE_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), AceType::MakeRefPtr<ImagePattern>());
    if (SystemProperties::IsNeedSymbol()) {
        imageNode = FrameNode::GetOrCreateFrameNode(V2::SYMBOL_ETS_TAG,
            ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<TextPattern>(); });
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto renderContext = host->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    BorderRadiusProperty radius;
    auto pipelineContext = PipelineBase::GetCurrentContextSafely();
    CHECK_NULL_VOID(pipelineContext);
    auto theme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(theme);
    radius.SetRadius(swiperArrowLayoutProperty->GetBackgroundSizeValue(theme->GetSmallArrowBackgroundSize()));
    renderContext->UpdateBorderRadius(radius);
    host->AddChild(buttonNode);
    buttonNode->AddChild(imageNode);
    UpdateArrowContent();
    auto pattern = buttonNode->GetPattern<ButtonPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->HandleBackgroundColor();
    auto buttonLayoutProperty = buttonNode->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_VOID(buttonLayoutProperty);
    buttonLayoutProperty->UpdateType(ButtonType::CIRCLE);
}

int32_t SwiperArrowPattern::TotalCount() const
{
    auto swiperNode = GetSwiperNode();
    CHECK_NULL_RETURN(swiperNode, 0);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(swiperPattern, 0);
    return swiperPattern->RealTotalCount() - 1;
}

void SwiperArrowPattern::ButtonTouchEvent(RefPtr<FrameNode> buttonNode, TouchType touchType)
{
    auto swiperArrowLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperArrowLayoutProperty);
    const auto& renderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto pipelineContext = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    Color backgroundColor;
    if (touchType == TouchType::UP || touchType == TouchType::CANCEL) {
        isTouch_ = false;
        if (isHover_) {
            backgroundColor = swiperIndicatorTheme->GetHoverArrowBackgroundColor();
            renderContext->ResetBlendBgColor();
            renderContext->BlendBgColor(backgroundColor);
        } else {
            renderContext->ResetBlendBgColor();
        }
    }
    if (!hoverOnClickFlag_ && swiperArrowLayoutProperty->GetHoverShowValue(false)) {
        return;
    }
    if (touchType == TouchType::DOWN) {
        isTouch_ = true;
        if (isHover_) {
            backgroundColor = swiperIndicatorTheme->GetHoverArrowBackgroundColor().BlendColor(
                swiperIndicatorTheme->GetClickArrowBackgroundColor());
        } else {
            backgroundColor = swiperIndicatorTheme->GetClickArrowBackgroundColor();
        }
        renderContext->ResetBlendBgColor();
        renderContext->BlendBgColor(backgroundColor);
        if (SystemProperties::IsNeedSymbol()) {
            RefPtr<FrameNode> symbolNode = DynamicCast<FrameNode>(buttonNode->GetFirstChild());
            CHECK_NULL_VOID(symbolNode);
            auto symbolLayoutProperty = symbolNode->GetLayoutProperty<TextLayoutProperty>();
            CHECK_NULL_VOID(symbolLayoutProperty);
            auto symbolEffectOptions = symbolLayoutProperty->GetSymbolEffectOptionsValue(SymbolEffectOptions());
            symbolEffectOptions.SetEffectType(SymbolEffectType::BOUNCE);
            symbolEffectOptions.SetIsTxtActive(true);
            symbolEffectOptions.SetIsTxtActiveSource(1);
            symbolLayoutProperty->UpdateSymbolEffectOptions(symbolEffectOptions);
            symbolNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
        }
    }
}

void SwiperArrowPattern::ButtonOnHover(RefPtr<FrameNode> buttonNode, bool isHovered)
{
    CHECK_NULL_VOID(buttonNode);
    hoverOnClickFlag_ = isHovered;
    isHover_ = isHovered;
    const auto& renderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto pipelineContext = PipelineBase::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    Color backgroundColor;

    auto swiperNode = GetSwiperNode();
    CHECK_NULL_VOID(swiperNode);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    auto swiperLayoutProperty = swiperPattern->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_VOID(swiperLayoutProperty);
    if (swiperLayoutProperty->GetHoverShowValue(false)) {
        swiperPattern->ArrowHover(isHover_, HOVER_ARROW);
    }
    if (isHovered) {
        if (isTouch_) {
            backgroundColor = swiperIndicatorTheme->GetHoverArrowBackgroundColor().BlendColor(
                swiperIndicatorTheme->GetClickArrowBackgroundColor());
        } else {
            backgroundColor = swiperIndicatorTheme->GetHoverArrowBackgroundColor();
        }
        renderContext->ResetBlendBgColor();
        renderContext->BlendBgColor(backgroundColor);
    } else {
        if (isTouch_) {
            backgroundColor = swiperIndicatorTheme->GetClickArrowBackgroundColor();
            renderContext->ResetBlendBgColor();
            renderContext->BlendBgColor(backgroundColor);
        } else {
            renderContext->ResetBlendBgColor();
        }
    }
}

void SwiperArrowPattern::SetLayoutDisplayCount(int32_t displayCount)
{
    if (displayCount_ != displayCount) {
        displayCount_ = displayCount;
        SetButtonVisible(isVisible_);
    }
}

std::tuple<bool, bool, bool> SwiperArrowPattern::CheckHoverStatus()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, std::make_tuple(false, false, true));
    auto swiperArrowLayoutProperty = host->GetLayoutProperty<SwiperArrowLayoutProperty>();
    CHECK_NULL_RETURN(swiperArrowLayoutProperty, std::make_tuple(false, false, true));

    auto swiperNode = DynamicCast<FrameNode>(host->GetParent());
    auto swiperPattern = swiperNode ? swiperNode->GetPattern<SwiperPattern>() : nullptr;
    CHECK_NULL_RETURN(swiperPattern, std::make_tuple(false, false, true));

    displayCount_ = swiperPattern->GetDisplayCount();
    bool leftArrowIsHidden = (index_ == 0);
    bool rightArrowIsHidden = (index_ == swiperPattern->TotalCount() - displayCount_);
    if (!swiperPattern->IsAutoLinear() && swiperPattern->IsSwipeByGroup()) {
        leftArrowIsHidden = (index_ < displayCount_);
        rightArrowIsHidden = (index_ >= swiperPattern->TotalCount() - displayCount_);
    }
    if (swiperPattern->IsHorizontalAndRightToLeft()) {
        std::swap(leftArrowIsHidden, rightArrowIsHidden);
    }
    auto isLeftArrow = host->GetTag() == V2::SWIPER_LEFT_ARROW_ETS_TAG;
    auto isRightArrow = host->GetTag() == V2::SWIPER_RIGHT_ARROW_ETS_TAG;
    auto isLoop = swiperArrowLayoutProperty->GetLoopValue(true);
    auto needHideArrow = (((isLeftArrow && leftArrowIsHidden) || (isRightArrow && rightArrowIsHidden)) && !isLoop)
        || (swiperPattern->RealTotalCount() <= displayCount_);
    auto isHoverShow = swiperArrowLayoutProperty->GetHoverShowValue(false);
    auto isHoverNone = swiperPattern->IsHoverNone();
    return std::make_tuple(needHideArrow, isHoverShow, isHoverNone);
}

void SwiperArrowPattern::SetButtonVisible(bool visible)
{
    isVisible_ = visible;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto buttonNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(buttonNode);
    auto buttonNodeGestureHub = buttonNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(buttonNodeGestureHub);
    const auto& renderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto hostFocusHub = host->GetFocusHub();
    CHECK_NULL_VOID(hostFocusHub);
    auto arrowGestureHub = host->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(arrowGestureHub);

    auto [needHideArrow, isHoverShow, isHoverNone] = CheckHoverStatus();
    if (needHideArrow || isHoverShow) {
        hostFocusHub->SetParentFocusable(false);
        hostFocusHub->LostSelfFocus();
        if (needHideArrow || isHoverNone) {
            visible = false;
        }
    } else {
        hostFocusHub->SetParentFocusable(true);
        visible = true;
    }
    renderContext->SetVisible(visible);
    // Set hit test mode BLOCK to make sure button respond to the touch events when visible.
    if (AceApplicationInfo::GetInstance().IsAccessibilityEnabled()) {
        arrowGestureHub->SetHitTestMode(visible ? HitTestMode::HTMBLOCK : HitTestMode::HTMTRANSPARENT);
    } else {
        arrowGestureHub->SetHitTestMode(visible ? HitTestMode::HTMDEFAULT : HitTestMode::HTMTRANSPARENT);
    }
    if (arrowClickListener_) {
        arrowGestureHub->RemoveClickEvent(arrowClickListener_);
        if (visible) {
            arrowGestureHub->AddClickEvent(arrowClickListener_);
        }
    }
    auto accessibilityProperty = buttonNode->GetAccessibilityProperty<AccessibilityProperty>();
    auto arrowAccessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    auto swiperNode = GetSwiperNode();
    CHECK_NULL_VOID(swiperNode);
    CHECK_NULL_VOID(accessibilityProperty);
    if (visible) {
        accessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::AUTO);
        arrowAccessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::AUTO);
    } else {
        accessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::NO_STR);
        arrowAccessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::NO_STR);
        auto isArrowFocus = arrowAccessibilityProperty->GetAccessibilityFocusState();
        if (isArrowFocus) {
            swiperNode->OnAccessibilityEvent(AccessibilityEventType::CHANGE);
        }
    }
}

void SwiperArrowPattern::UpdateArrowContentBySymbol(RefPtr<FrameNode>& buttonNode,
    RefPtr<SwiperArrowLayoutProperty>& swiperArrowLayoutProperty)
{
    RefPtr<FrameNode> symbolNode = DynamicCast<FrameNode>(buttonNode->GetFirstChild());
    CHECK_NULL_VOID(symbolNode);
    auto symbolLayoutProperty = symbolNode->GetLayoutProperty<TextLayoutProperty>();
    CHECK_NULL_VOID(symbolLayoutProperty);
    auto swiperLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    auto pipelineContext = PipelineBase::GetCurrentContextSafely();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    bool isRtl = swiperLayoutProperty->GetNonAutoLayoutDirection() == TextDirection::RTL;
    if (V2::SWIPER_LEFT_ARROW_ETS_TAG == GetHost()->GetTag()) {
        if (swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL) == Axis::HORIZONTAL) {
            symbolLayoutProperty->UpdateSymbolSourceInfo(SymbolSourceInfo(
                isRtl ? swiperIndicatorTheme->GetRightSymbolId() : swiperIndicatorTheme->GetLeftSymbolId()));
        } else {
            symbolLayoutProperty->UpdateSymbolSourceInfo(SymbolSourceInfo(swiperIndicatorTheme->GetUpSymbolId()));
        }
    } else if (V2::SWIPER_RIGHT_ARROW_ETS_TAG == GetHost()->GetTag()) {
        if (swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL) == Axis::HORIZONTAL) {
            symbolLayoutProperty->UpdateSymbolSourceInfo(SymbolSourceInfo(
                isRtl ? swiperIndicatorTheme->GetLeftSymbolId() : swiperIndicatorTheme->GetRightSymbolId()));
        } else {
            symbolLayoutProperty->UpdateSymbolSourceInfo(SymbolSourceInfo(swiperIndicatorTheme->GetDownSymbolId()));
        }
    }
    symbolLayoutProperty->UpdateFontSize(swiperArrowLayoutProperty->GetArrowSizeValue(
        swiperIndicatorTheme->GetSmallArrowSize()));
    symbolLayoutProperty->UpdateSymbolColorList({ swiperArrowLayoutProperty->GetArrowColorValue(
        swiperIndicatorTheme->GetSmallArrowColor()) });
    if (!swiperArrowLayoutProperty->GetEnabledValue(true)) {
        buttonNode->GetRenderContext()->UpdateBackgroundColor(
            backgroundColor_.BlendOpacity(swiperIndicatorTheme->GetArrowDisabledAlpha()));
        symbolLayoutProperty->UpdateSymbolColorList({ swiperArrowLayoutProperty->GetArrowColorValue(
            swiperIndicatorTheme->GetSmallArrowColor()).BlendOpacity(
            swiperIndicatorTheme->GetArrowDisabledAlpha()) });
    }
    symbolNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    symbolNode->MarkModifyDone();
}

void SwiperArrowPattern::UpdateArrowContentByImage(RefPtr<FrameNode>& buttonNode,
    RefPtr<SwiperArrowLayoutProperty>& swiperArrowLayoutProperty)
{
    RefPtr<FrameNode> imageNode = DynamicCast<FrameNode>(buttonNode->GetFirstChild());
    CHECK_NULL_VOID(imageNode);
    auto imageLayoutProperty = imageNode->GetLayoutProperty<ImageLayoutProperty>();
    CHECK_NULL_VOID(imageLayoutProperty);
    auto pipelineContext = PipelineBase::GetCurrentContextSafely();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    imageLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(
        swiperArrowLayoutProperty->GetArrowSizeValue(swiperIndicatorTheme->GetSmallArrowSize())),
        CalcLength(swiperArrowLayoutProperty->GetArrowSizeValue(swiperIndicatorTheme->GetSmallArrowSize()))));
    imageLayoutProperty->UpdateImageFit(ImageFit::FILL);
    ImageSourceInfo imageSourceInfo;
    auto swiperLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    if (V2::SWIPER_LEFT_ARROW_ETS_TAG == GetHost()->GetTag()) {
        if (swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL) == Axis::HORIZONTAL) {
            imageSourceInfo.SetResourceId(InternalResource::ResourceId::IC_PUBLIC_ARROW_LEFT_SVG);
        } else {
            imageSourceInfo.SetResourceId(InternalResource::ResourceId::IC_PUBLIC_ARROW_UP_SVG);
        }
    } else if (V2::SWIPER_RIGHT_ARROW_ETS_TAG == GetHost()->GetTag()) {
        if (swiperLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL) == Axis::HORIZONTAL) {
            imageSourceInfo.SetResourceId(InternalResource::ResourceId::IC_PUBLIC_ARROW_RIGHT_SVG);
        } else {
            imageSourceInfo.SetResourceId(InternalResource::ResourceId::IC_PUBLIC_ARROW_DOWN_SVG);
        }
    }
    imageSourceInfo.SetFillColor(swiperArrowLayoutProperty->GetArrowColorValue());
    if (!swiperArrowLayoutProperty->GetEnabledValue(true)) {
        buttonNode->GetRenderContext()->UpdateBackgroundColor(
            backgroundColor_.BlendOpacity(swiperIndicatorTheme->GetArrowDisabledAlpha()));
        imageSourceInfo.SetFillColor(swiperArrowLayoutProperty->GetArrowColorValue(
            swiperIndicatorTheme->GetSmallArrowColor()).BlendOpacity(
            swiperIndicatorTheme->GetArrowDisabledAlpha()));
    }
    imageLayoutProperty->UpdateImageSourceInfo(imageSourceInfo);
    imageNode->MarkModifyDone();
}

void SwiperArrowPattern::UpdateArrowContent()
{
    auto swiperArrowLayoutProperty = GetSwiperArrowLayoutProperty();
    CHECK_NULL_VOID(swiperArrowLayoutProperty);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto buttonNode = DynamicCast<FrameNode>(host->GetFirstChild());
    CHECK_NULL_VOID(buttonNode);
    auto buttonRenderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(buttonRenderContext);
    buttonRenderContext->UpdateBackgroundColor(
        swiperArrowLayoutProperty->GetIsShowBackgroundValue(false)
            ? swiperArrowLayoutProperty->GetBackgroundColorValue(backgroundColor_)
            : Color::TRANSPARENT);
    auto buttonLayoutProperty = buttonNode->GetLayoutProperty<ButtonLayoutProperty>();
    CHECK_NULL_VOID(buttonLayoutProperty);
    auto pipelineContext = PipelineBase::GetCurrentContextSafely();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    buttonLayoutProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(swiperArrowLayoutProperty->GetBackgroundSizeValue(
            swiperIndicatorTheme->GetSmallArrowBackgroundSize())),
            CalcLength(swiperArrowLayoutProperty->GetBackgroundSizeValue(
                swiperIndicatorTheme->GetSmallArrowBackgroundSize()))));
    backgroundColor_ = buttonRenderContext->GetBackgroundColorValue(Color::TRANSPARENT);
    if (SystemProperties::IsNeedSymbol()) {
        UpdateArrowContentBySymbol(buttonNode, swiperArrowLayoutProperty);
    } else {
        UpdateArrowContentByImage(buttonNode, swiperArrowLayoutProperty);
    }
}

void SwiperArrowPattern::DumpAdvanceInfo()
{
    DumpLog::GetInstance().AddDesc("index:" + std::to_string(index_));
    isTouch_ ? DumpLog::GetInstance().AddDesc("isTouch:true") : DumpLog::GetInstance().AddDesc("isTouch:false");
    isHover_ ? DumpLog::GetInstance().AddDesc("isHover:true") : DumpLog::GetInstance().AddDesc("isHover:false");
    hoverOnClickFlag_ ? DumpLog::GetInstance().AddDesc("hoverOnClickFlag:true")
                      : DumpLog::GetInstance().AddDesc("hoverOnClickFlag:false");
}

void SwiperArrowPattern::DumpAdvanceInfo(std::unique_ptr<JsonValue>& json)
{
    json->Put("index", index_);
    json->Put("isTouch", isTouch_);
    json->Put("isHover", isHover_);
    json->Put("hoverOnClickFlag", hoverOnClickFlag_);
}

RefPtr<SwiperPattern> SwiperArrowPattern::GetSwiperPattern() const
{
    auto swiperNode = GetSwiperNode();
    CHECK_NULL_RETURN(swiperNode, nullptr);
    return swiperNode->GetPattern<SwiperPattern>();
}
} // namespace OHOS::Ace::NG
