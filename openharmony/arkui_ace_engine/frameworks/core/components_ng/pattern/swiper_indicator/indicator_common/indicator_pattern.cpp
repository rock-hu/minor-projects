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

#include "core/components_ng/pattern/swiper_indicator/indicator_common/indicator_pattern.h"

namespace OHOS::Ace::NG {
namespace {
constexpr Dimension INDICATOR_BORDER_RADIUS = 16.0_vp;
constexpr float DEFAULT_COUNT = 2.0f;
} // namespace

IndicatorPattern::IndicatorPattern()
{
    indicatorController_ = MakeRefPtr<IndicatorController>();
    InitIndicatorController();
}

void IndicatorPattern::InitIndicatorController()
{
    indicatorController_->SetShowNextImpl([weak = WeakClaim(this)]() {
        auto indicator = weak.Upgrade();
        CHECK_NULL_VOID(indicator);
        indicator->ShowNext();
    });

    indicatorController_->SetShowPrevImpl([weak = WeakClaim(this)]() {
        auto indicator = weak.Upgrade();
        CHECK_NULL_VOID(indicator);
        indicator->ShowPrevious();
    });

    indicatorController_->SetChangeIndexImpl([weak = WeakClaim(this)](int32_t index, bool useAnimation) {
        auto indicator = weak.Upgrade();
        CHECK_NULL_VOID(indicator);
        indicator->ChangeIndex(index, useAnimation);
    });
}

void IndicatorPattern::SetSwiperDigitalParameters(const SwiperDigitalParameters& swiperDigitalParameters)
{
    swiperDigitalParameters_ = std::make_shared<SwiperDigitalParameters>(swiperDigitalParameters);
}

SwiperIndicatorType IndicatorPattern::GetIndicatorTypeFromProperty() const
{
    auto swiperIndicatorLayoutProperty = GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_RETURN(swiperIndicatorLayoutProperty, SwiperIndicatorType::DOT);
    return swiperIndicatorLayoutProperty->GetIndicatorTypeValue(SwiperIndicatorType::DOT);
}

int32_t IndicatorPattern::GetInitialIndexFromProperty() const
{
    auto swiperIndicatorLayoutProperty = GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_RETURN(swiperIndicatorLayoutProperty, 0);
    return swiperIndicatorLayoutProperty->GetInitialIndex().value_or(0);
}

Axis IndicatorPattern::GetDirectionFromProperty() const
{
    auto swiperIndicatorLayoutProperty = GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_RETURN(swiperIndicatorLayoutProperty, Axis::HORIZONTAL);
    return swiperIndicatorLayoutProperty->GetDirection().value_or(Axis::HORIZONTAL);
}

int32_t IndicatorPattern::GetCountFromProperty() const
{
    auto swiperIndicatorLayoutProperty = GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_RETURN(swiperIndicatorLayoutProperty, DEFAULT_COUNT);
    return swiperIndicatorLayoutProperty->GetCount().value_or(DEFAULT_COUNT);
}

bool IndicatorPattern::IsLoopFromProperty() const
{
    auto swiperIndicatorLayoutProperty = GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_RETURN(swiperIndicatorLayoutProperty, true);
    return swiperIndicatorLayoutProperty->GetLoop().value_or(true);
}

void IndicatorPattern::FireChangeEvent() const
{
    auto indicatorEventHub = GetEventHub<IndicatorEventHub>();
    CHECK_NULL_VOID(indicatorEventHub);
    indicatorEventHub->FireChangeEvent(GetLoopIndex(GetCurrentIndex()));
}

std::shared_ptr<SwiperParameters> IndicatorPattern::GetSwiperParameters()
{
    if (swiperParameters_ == nullptr) {
        swiperParameters_ = std::make_shared<SwiperParameters>();
        auto pipelineContext = GetContext();
        CHECK_NULL_RETURN(pipelineContext, swiperParameters_);
        auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
        CHECK_NULL_RETURN(swiperIndicatorTheme, swiperParameters_);
        swiperParameters_->itemWidth = swiperIndicatorTheme->GetSize();
        swiperParameters_->itemHeight = swiperIndicatorTheme->GetSize();
        swiperParameters_->selectedItemWidth = swiperIndicatorTheme->GetSize();
        swiperParameters_->selectedItemHeight = swiperIndicatorTheme->GetSize();
        swiperParameters_->maskValue = false;
        swiperParameters_->colorVal = swiperIndicatorTheme->GetColor();
        swiperParameters_->selectedColorVal = swiperIndicatorTheme->GetSelectedColor();
    }
    return swiperParameters_;
}

std::shared_ptr<SwiperDigitalParameters> IndicatorPattern::GetSwiperDigitalParameters()
{
    if (swiperDigitalParameters_ == nullptr) {
        swiperDigitalParameters_ = std::make_shared<SwiperDigitalParameters>();
        auto pipelineContext = GetContext();
        CHECK_NULL_RETURN(pipelineContext, swiperDigitalParameters_);
        auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
        swiperDigitalParameters_->fontColor = swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor();
        swiperDigitalParameters_->selectedFontColor =
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor();
        swiperDigitalParameters_->fontSize = swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize();
        swiperDigitalParameters_->selectedFontSize = swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize();
        swiperDigitalParameters_->fontWeight = swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontWeight();
        swiperDigitalParameters_->selectedFontWeight =
            swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontWeight();
    }
    return swiperDigitalParameters_;
}

void IndicatorPattern::SaveDigitIndicatorProperty()
{
    auto indicatorNode = GetHost();
    CHECK_NULL_VOID(indicatorNode);
    auto layoutProperty = indicatorNode->GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto pipelineContext = indicatorNode->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    auto swiperDigitalParameters = GetSwiperDigitalParameters();
    CHECK_NULL_VOID(swiperDigitalParameters);
    layoutProperty->ResetIndicatorLayoutStyle();
    if (swiperDigitalParameters->dimLeft.has_value()) {
        layoutProperty->UpdateLeft(swiperDigitalParameters->dimLeft.value());
    }
    if (swiperDigitalParameters->dimTop.has_value()) {
        layoutProperty->UpdateTop(swiperDigitalParameters->dimTop.value());
    }
    if (swiperDigitalParameters->dimRight.has_value()) {
        layoutProperty->UpdateRight(swiperDigitalParameters->dimRight.value());
    }
    if (swiperDigitalParameters->dimBottom.has_value()) {
        layoutProperty->UpdateBottom(swiperDigitalParameters->dimBottom.value());
    }
    layoutProperty->UpdateFontColor(swiperDigitalParameters->fontColor.value_or(
        swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor()));
    layoutProperty->UpdateSelectedFontColor(swiperDigitalParameters->selectedFontColor.value_or(
        swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetTextColor()));
    layoutProperty->UpdateFontSize(
        swiperDigitalParameters->fontSize.value_or(swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize()));
    layoutProperty->UpdateSelectedFontSize(swiperDigitalParameters->selectedFontSize.value_or(
        swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontSize()));
    layoutProperty->UpdateFontWeight(swiperDigitalParameters->fontWeight.value_or(
        swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontWeight()));
    layoutProperty->UpdateSelectedFontWeight(swiperDigitalParameters->selectedFontWeight.value_or(
        swiperIndicatorTheme->GetDigitalIndicatorTextStyle().GetFontWeight()));
}

void IndicatorPattern::SaveDotIndicatorProperty()
{
    auto indicatorNode = GetHost();
    CHECK_NULL_VOID(indicatorNode);
    auto layoutProperty = indicatorNode->GetLayoutProperty<SwiperIndicatorLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto swiperParameters = GetSwiperParameters();
    CHECK_NULL_VOID(swiperParameters);
    layoutProperty->ResetIndicatorLayoutStyle();
    if (swiperParameters->dimLeft.has_value()) {
        layoutProperty->UpdateLeft(swiperParameters->dimLeft.value());
    }
    if (swiperParameters->dimTop.has_value()) {
        layoutProperty->UpdateTop(swiperParameters->dimTop.value());
    }
    if (swiperParameters->dimRight.has_value()) {
        layoutProperty->UpdateRight(swiperParameters->dimRight.value());
    }
    if (swiperParameters->dimBottom.has_value()) {
        layoutProperty->UpdateBottom(swiperParameters->dimBottom.value());
    }

    bool isRtl = GetNonAutoLayoutDirection() == TextDirection::RTL;
    if (swiperParameters->dimStart.has_value()) {
        auto dimValue = swiperParameters->dimStart.value();
        isRtl ? layoutProperty->UpdateRight(dimValue) : layoutProperty->UpdateLeft(dimValue);
    }
    if (swiperParameters->dimEnd.has_value()) {
        auto dimValue = swiperParameters->dimEnd.value();
        isRtl ? layoutProperty->UpdateLeft(dimValue) : layoutProperty->UpdateRight(dimValue);
    }

    UpdatePaintProperty();
}

void IndicatorPattern::UpdatePaintProperty()
{
    auto indicatorNode = GetHost();
    CHECK_NULL_VOID(indicatorNode);
    auto paintProperty = indicatorNode->GetPaintProperty<DotIndicatorPaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    auto pipelineContext = indicatorNode->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    auto swiperIndicatorTheme = pipelineContext->GetTheme<SwiperIndicatorTheme>();
    CHECK_NULL_VOID(swiperIndicatorTheme);
    auto swiperParameters = GetSwiperParameters();
    CHECK_NULL_VOID(swiperParameters);
    paintProperty->UpdateItemWidth(swiperParameters->itemWidth.value_or(swiperIndicatorTheme->GetSize()));
    paintProperty->UpdateItemHeight(swiperParameters->itemHeight.value_or(swiperIndicatorTheme->GetSize()));
    paintProperty->UpdateSelectedItemWidth(
        swiperParameters->selectedItemWidth.value_or(swiperIndicatorTheme->GetSize()));
    paintProperty->UpdateSelectedItemHeight(
        swiperParameters->selectedItemHeight.value_or(swiperIndicatorTheme->GetSize()));
    paintProperty->UpdateIndicatorMask(swiperParameters->maskValue.value_or(false));
    paintProperty->UpdateColor(swiperParameters->colorVal.value_or(swiperIndicatorTheme->GetColor()));
    paintProperty->UpdateSelectedColor(
        swiperParameters->selectedColorVal.value_or(swiperIndicatorTheme->GetSelectedColor()));
    paintProperty->UpdateIsCustomSize(isCustomSize_);
}

void IndicatorPattern::OnModifyDone()
{
    if (!hasSetInitialIndex_) {
        hasSetInitialIndex_ = true;
        currentIndexInSingleMode_ = GetInitialIndexFromProperty();
    }

    auto indicatorNode = GetHost();
    CHECK_NULL_VOID(indicatorNode);
    if (GetIndicatorType() == SwiperIndicatorType::DOT) {
        SaveDotIndicatorProperty();
    } else {
        SaveDigitIndicatorProperty();
    }

    auto renderContext = indicatorNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    BorderRadiusProperty radius;
    auto commonRadiusValue = renderContext->GetBorderRadius();
    if (commonRadiusValue.has_value()) {
        radius.UpdateWithCheck(commonRadiusValue.value());
    } else {
        radius.SetRadius(INDICATOR_BORDER_RADIUS);
    }
    renderContext->UpdateBorderRadius(radius);
    SwiperIndicatorPattern::OnModifyDone();
}

bool IndicatorPattern::GetDotCurrentOffset(OffsetF& offset, float indicatorWidth, float indicatorHeight)
{
    return false;
}

bool IndicatorPattern::GetDigitFrameSize(SizeF& frameSize) const
{
    auto indicatorNode = GetHost();
    CHECK_NULL_RETURN(indicatorNode, false);
    frameSize = indicatorNode->GetGeometryNode()->GetFrameSize();
    return true;
}

void IndicatorPattern::OnIndexChangeInSingleMode(int32_t index)
{
    if (!IsLoop()) {
        if (index >= RealTotalCount()) {
            SetCurrentIndexInSingleMode(RealTotalCount() - 1);
            return;
        } else if (index < 0) {
            SetCurrentIndexInSingleMode(0);
            return;
        }
    }
    SetCurrentIndexInSingleMode(GetLoopIndex(index));
    FireChangeEvent();
    SwiperIndicatorPattern::IndicatorOnChange();
}

void IndicatorPattern::ShowPrevious()
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::ShowPrevious();
    }
    auto isRtl = GetNonAutoLayoutDirection() == TextDirection::RTL;
    if (isRtl) {
        singleGestureState_ = GestureState::GESTURE_STATE_RELEASE_RIGHT;
    } else {
        singleGestureState_ = GestureState::GESTURE_STATE_RELEASE_LEFT;
    }

    OnIndexChangeInSingleMode(GetCurrentIndex() - 1);
}

void IndicatorPattern::ShowNext()
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::ShowNext();
    }
    singleGestureState_ = GestureState::GESTURE_STATE_RELEASE_RIGHT;
    OnIndexChangeInSingleMode(GetCurrentIndex() + 1);
}

void IndicatorPattern::ChangeIndex(int32_t index, bool useAnimation)
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::ChangeIndex(index, useAnimation);
    }
    if (useAnimation) {
        if (GetLoopIndex(GetCurrentIndex()) > GetLoopIndex(index)) {
            singleGestureState_ = GestureState::GESTURE_STATE_RELEASE_LEFT;
        } else if (GetLoopIndex(GetCurrentIndex()) < GetLoopIndex(index)) {
            singleGestureState_ = GestureState::GESTURE_STATE_RELEASE_RIGHT;
        }
    }
    OnIndexChangeInSingleMode(index);
}

int32_t IndicatorPattern::GetCurrentIndex() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::GetCurrentIndex();
    }
    return currentIndexInSingleMode_;
}

int32_t IndicatorPattern::GetCurrentShownIndex() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::GetCurrentShownIndex();
    }
    return currentIndexInSingleMode_;
}

int32_t IndicatorPattern::DisplayIndicatorTotalCount() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::DisplayIndicatorTotalCount();
    }
    return GetCountFromProperty();
}

int32_t IndicatorPattern::RealTotalCount() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::RealTotalCount();
    }
    return GetCountFromProperty();
}

Axis IndicatorPattern::GetDirection() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::GetDirection();
    }
    return GetDirectionFromProperty();
}

bool IndicatorPattern::IsHorizontalAndRightToLeft() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::IsHorizontalAndRightToLeft();
    }
    return GetDirection() == Axis::HORIZONTAL && GetNonAutoLayoutDirection() == TextDirection::RTL;
}

TextDirection IndicatorPattern::GetNonAutoLayoutDirection() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::GetNonAutoLayoutDirection();
    }
    auto host = GetHost();
    CHECK_NULL_RETURN(host, TextDirection::LTR);
    CHECK_NULL_RETURN(host->GetLayoutProperty(), TextDirection::LTR);
    return host->GetLayoutProperty()->GetNonAutoLayoutDirection();
}

bool IndicatorPattern::IsLoop() const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::IsLoop();
    }
    return IsLoopFromProperty();
}

void IndicatorPattern::GetTextContentSub(std::string& firstContent, std::string& lastContent) const
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::GetTextContentSub(firstContent, lastContent);
    }
    // If the mode is set to single mode, the initial information is taken from the Indicator component
    // interface.
    auto currentIndex = GetLoopIndex(GetCurrentIndex()) + 1;
    bool isRtl = GetNonAutoLayoutDirection() == TextDirection::RTL;
    firstContent = isRtl ? std::to_string(GetCountFromProperty()) : std::to_string(currentIndex);
    lastContent = isRtl ? std::to_string(currentIndex) + "\\" : "/" + std::to_string(GetCountFromProperty());
}

void IndicatorPattern::SwipeTo(std::optional<int32_t> mouseClickIndex)
{
    if (GetBindSwiperNode()) {
        return SwiperIndicatorPattern::SwipeTo(mouseClickIndex);
    }
    if (mouseClickIndex) {
        OnIndexChangeInSingleMode(mouseClickIndex.value());
    }
}
} // namespace OHOS::Ace::NG
