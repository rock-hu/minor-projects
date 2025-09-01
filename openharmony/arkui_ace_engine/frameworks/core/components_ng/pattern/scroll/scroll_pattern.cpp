/*
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/scroll/scroll_pattern.h"

#include "base/log/dump_log.h"
#include "core/components_ng/pattern/scrollable/scrollable_animation_consts.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {

namespace {
constexpr float SCROLL_BY_SPEED = 250.0f; // move 250 pixels per second
constexpr float UNIT_CONVERT = 1000.0f;   // 1s convert to 1000ms
constexpr Dimension SELECT_SCROLL_MIN_WIDTH = 64.0_vp;
constexpr int32_t COLUMN_NUM = 2;
constexpr float SCROLL_PAGING_SPEED_THRESHOLD = 1200.0f;
constexpr int32_t SCROLL_LAYOUT_INFO_COUNT = 30;
constexpr int32_t SCROLL_MEASURE_INFO_COUNT = 30;
constexpr double SCROLL_SNAP_INTERVAL_SIZE_MIN_VALUE = 1.0;
#ifdef SUPPORT_DIGITAL_CROWN
constexpr int32_t CROWN_EVENT_NUN_THRESH_MIN = 5;
constexpr int64_t CROWN_VIBRATOR_INTERVAL_TIME = 30 * 1000 * 1000;
constexpr char CROWN_VIBRATOR_WEAK[] = "watchhaptic.feedback.crown.strength2";
#endif
} // namespace

void ScrollPattern::OnModifyDone()
{
    Pattern::OnModifyDone();
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty<ScrollLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto paintProperty = host->GetPaintProperty<ScrollablePaintProperty>();
    CHECK_NULL_VOID(paintProperty);
    const auto axis = layoutProperty->GetAxis().value_or(Axis::VERTICAL);
    const bool axisChanged = axis != GetAxis();
    if (axisChanged) {
        SetAxis(axis);
        ResetPosition();
    }
    UpdatePinchGesture();
    if (!GetScrollableEvent()) {
        AddScrollEvent();
#ifdef SUPPORT_DIGITAL_CROWN
        SetDigitalCrownEvent();
#endif
    }
    SetEdgeEffect();
    if (axisChanged) {
        // need to init after scrollableEvent
        if (axis == Axis::FREE) {
            freeScroll_ = MakeRefPtr<FreeScrollController>(*this);
            SetScrollEnabled(true); // always enable scrollEvent
            scrollBar2d_ = MakeRefPtr<ScrollBar2D>(*this);
            SetScrollBar(DisplayMode::OFF); // turn off single-axis scrollBar
            auto* ctx = GetRenderContext();
            CHECK_NULL_VOID(ctx);
            ctx->RemoveOverlayModifier(GetScrollBarOverlayModifier());
        } else {
            freeScroll_.Reset();
            scrollBar2d_.Reset();
        }
    }
    if (scrollBar2d_) {
        scrollBar2d_->Update(paintProperty->GetScrollBarProperty());
    } else {
        SetScrollBar(paintProperty->GetScrollBarProperty());
    }
    SetAccessibilityAction();
    if (scrollSnapUpdate_) {
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    }
    Register2DragDropManager();
    auto overlayNode = host->GetOverlayNode();
    if (!overlayNode && paintProperty->GetFadingEdge().value_or(false)) {
        CreateAnalyzerOverlay(host);
    }
}

RefPtr<PaintProperty> ScrollPattern::CreatePaintProperty()
{
    auto defaultDisplayMode = GetDefaultScrollBarDisplayMode();
    auto property = MakeRefPtr<ScrollPaintProperty>();
    property->UpdateScrollBarMode(defaultDisplayMode);
    return property;
}

RefPtr<NodePaintMethod> ScrollPattern::CreateNodePaintMethod()
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto layoutProperty = host->GetLayoutProperty<ScrollLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, nullptr);
    auto layoutDirection = layoutProperty->GetNonAutoLayoutDirection();
    auto drawDirection = (layoutDirection == TextDirection::RTL);
    auto paint = MakeRefPtr<ScrollPaintMethod>(GetAxis() == Axis::HORIZONTAL, drawDirection);
    if (scrollBar2d_) {
        paint->Set2DPainter(scrollBar2d_);
    } else {
        paint->SetScrollBar(GetScrollBar());
        paint->SetScrollBarOverlayModifier(GetScrollBarOverlayModifier());
    }
    auto scrollEffect = GetScrollEdgeEffect();
    if (scrollEffect && scrollEffect->IsFadeEffect()) {
        paint->SetEdgeEffect(scrollEffect);
    }
    if (!scrollContentModifier_) {
        scrollContentModifier_ = AceType::MakeRefPtr<ScrollContentModifier>();
    }
    paint->SetContentModifier(scrollContentModifier_);
    UpdateFadingEdge(paint);
    return paint;
}

bool ScrollPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipMeasure && config.skipLayout) {
        return false;
    }
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    if (!SetScrollProperties(dirty, host)) {
        return false;
    }
    UpdateScrollBarOffset();
    if (config.frameSizeChange && isInitialized_) {
        if (GetScrollBar() != nullptr) {
            GetScrollBar()->ScheduleDisappearDelayTask();
        }
    }
    auto eventHub = host->GetEventHub<ScrollEventHub>();
    CHECK_NULL_RETURN(eventHub, false);
    PrintOffsetLog(AceLogTag::ACE_SCROLL, host->GetId(), prevOffset_ - currentOffset_);
    FireOnDidScroll(prevOffset_ - currentOffset_);
    auto onReachStart = eventHub->GetOnReachStart();
    auto onJSFrameNodeReachStart = eventHub->GetJSFrameNodeOnReachStart();
    FireOnReachStart(onReachStart, onJSFrameNodeReachStart);
    auto onReachEnd = eventHub->GetOnReachEnd();
    auto onJSFrameNodeReachEnd = eventHub->GetJSFrameNodeOnReachEnd();
    FireOnReachEnd(onReachEnd, onJSFrameNodeReachEnd);
    auto onScrollStop = eventHub->GetOnScrollStop();
    auto onJSFrameNodeScrollStop = eventHub->GetJSFrameNodeOnScrollStop();
    OnScrollStop(onScrollStop, onJSFrameNodeScrollStop);
    ScrollSnapTrigger();
    CheckScrollable();
    prevOffset_ = currentOffset_;
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_RETURN(geometryNode, false);
    auto offsetRelativeToWindow = host->GetOffsetRelativeToWindow();
    auto globalViewPort = RectF(offsetRelativeToWindow, geometryNode->GetFrameRect().GetSize());
    host->SetViewPort(globalViewPort);
    isInitialized_ = true;
    ChangeAnimateOverScroll();
    SetScrollSource(SCROLL_FROM_NONE);
    auto paintProperty = GetPaintProperty<ScrollablePaintProperty>();
    CHECK_NULL_RETURN(paintProperty, false);
    if (scrollEdgeType_ != ScrollEdgeType::SCROLL_NONE && AnimateStoped()) {
        scrollEdgeType_ = ScrollEdgeType::SCROLL_NONE;
    }
    ChangeCanStayOverScroll();
    return paintProperty->GetFadingEdge().value_or(false) ||
            ((config.frameSizeChange || config.contentSizeChange) && paintProperty->GetContentClip().has_value());
}

bool ScrollPattern::SetScrollProperties(const RefPtr<LayoutWrapper>& dirty, const RefPtr<FrameNode>& host)
{
    auto layoutAlgorithmWrapper = DynamicCast<LayoutAlgorithmWrapper>(dirty->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithmWrapper, false);
    auto layoutAlgorithm = DynamicCast<ScrollLayoutAlgorithm>(layoutAlgorithmWrapper->GetLayoutAlgorithm());
    CHECK_NULL_RETURN(layoutAlgorithm, false);
    currentOffset_ = layoutAlgorithm->GetCurrentOffset();
    if (freeScroll_ && scrollBar2d_) {
        freeScroll_->OnLayoutFinished(layoutAlgorithm->GetFreeOffset(), layoutAlgorithm->GetScrollableArea());
        scrollBar2d_->SyncLayout(
            layoutAlgorithm->GetFreeOffset(), layoutAlgorithm->GetViewSize(), layoutAlgorithm->GetViewPortExtent());
    }
    auto oldScrollableDistance = scrollableDistance_;
    scrollableDistance_ = layoutAlgorithm->GetScrollableDistance();
    if (!NearEqual(oldScrollableDistance, scrollableDistance_)) {
        CheckScrollToEdge();
        AddScrollLayoutInfo();
    }

    if (LessNotEqual(scrollableDistance_, oldScrollableDistance) && !GetCanStayOverScroll()) {
        CheckRestartSpring(true);
    }
    auto axis = GetAxis();
    auto oldMainSize = GetMainAxisSize(viewPort_, axis);
    auto newMainSize = GetMainAxisSize(layoutAlgorithm->GetViewPort(), axis);
    auto oldExtentMainSize = GetMainAxisSize(viewPortExtent_, axis);
    auto newExtentMainSize = GetMainAxisSize(layoutAlgorithm->GetViewPortExtent(), axis);
    viewPortLength_ = layoutAlgorithm->GetViewPortLength();
    viewPort_ = layoutAlgorithm->GetViewPort();
    viewSize_ = layoutAlgorithm->GetViewSize();
    viewPortExtent_ = layoutAlgorithm->GetViewPortExtent();
    if (IsEnablePagingValid()) {
        SetIntervalSize(Dimension(static_cast<double>(viewPortLength_)));
    }
    if (scrollSnapUpdate_ || !NearEqual(oldMainSize, newMainSize) || !NearEqual(oldExtentMainSize, newExtentMainSize)) {
        CaleSnapOffsets(host);
        scrollSnapUpdate_ = false;
    }
    ProcessZoomScale();
    return true;
}

bool ScrollPattern::ScrollSnapTrigger()
{
    if (ScrollableIdle() && !AnimateRunning()) {
        SnapAnimationOptions snapAnimationOptions;
        if (StartSnapAnimation(snapAnimationOptions)) {
            if (!IsScrolling()) {
                FireOnScrollStart();
            }
            return true;
        }
    }
    return false;
}

void ScrollPattern::CheckScrollable()
{
    if (freeScroll_) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty<ScrollLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    if (GreatNotEqual(scrollableDistance_, 0.0f)) {
        SetScrollEnabled(layoutProperty->GetScrollEnabled().value_or(true));
    } else {
        SetScrollEnabled(layoutProperty->GetScrollEnabled().value_or(true) && GetAlwaysEnabled());
    }
}

bool ScrollPattern::OnScrollCallback(float offset, int32_t source)
{
    if (source != SCROLL_FROM_START) {
        if (GetAxis() == Axis::NONE) {
            return false;
        }
        if (!AnimateStoped()) {
            return false;
        }
        auto adjustOffset = static_cast<float>(offset);
        AdjustOffset(adjustOffset, source);
        return UpdateCurrentOffset(adjustOffset, source);
    } else {
        if (GetSnapType() == SnapType::SCROLL_SNAP) {
            SetScrollableCurrentPos(currentOffset_);
        }
        FireOnScrollStart();
    }
    return true;
}

void ScrollPattern::OnScrollEndCallback()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto eventHub = host->GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto scrollEndEvent = eventHub->GetScrollEndEvent();
    if (scrollEndEvent) {
        scrollEndEvent();
    }
    if (AnimateStoped()) {
        scrollStop_ = true;
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    }
#ifdef SUPPORT_DIGITAL_CROWN
    crownEventNum_ = 0;
#endif
}

void ScrollPattern::ResetPosition()
{
    currentOffset_ = 0.0f;
    lastOffset_ = 0.0f;
}

bool ScrollPattern::IsAtTop() const
{
    return GreatOrEqual(currentOffset_, 0.0);
}

bool ScrollPattern::IsAtBottom(bool considerRepeat) const
{
    if (LessNotEqual(scrollableDistance_, 0.0f)) {
        return LessOrEqual(currentOffset_, 0.0f);
    }
    return LessOrEqual(currentOffset_, -scrollableDistance_);
}

OverScrollOffset ScrollPattern::GetOverScrollOffset(double delta) const
{
    OverScrollOffset offset = { 0, 0 };
    auto startPos = currentOffset_;
    auto newStartPos = startPos + delta;
    if (startPos > 0 && newStartPos > 0) {
        offset.start = delta;
    }
    if (startPos > 0 && newStartPos <= 0) {
        offset.start = -startPos;
    }
    if (startPos <= 0 && newStartPos > 0) {
        offset.start = newStartPos;
    }

    auto endPos = currentOffset_;
    auto newEndPos = endPos + delta;
    auto endRefences = GreatOrEqual(scrollableDistance_, 0.0f) ? -scrollableDistance_ : 0;
    if (endPos < endRefences && newEndPos < endRefences) {
        offset.end = delta;
    }
    if (endPos < endRefences && newEndPos >= endRefences) {
        offset.end = endRefences - endPos;
    }
    if (endPos >= endRefences && newEndPos < endRefences) {
        offset.end = newEndPos - endRefences;
    }
    return offset;
}

bool ScrollPattern::IsOutOfBoundary(bool useCurrentDelta)
{
    if (Positive(scrollableDistance_)) {
        return Positive(currentOffset_) || LessNotEqual(currentOffset_, -scrollableDistance_);
    } else {
        return !NearZero(currentOffset_);
    }
}

bool ScrollPattern::ScrollPageCheck(float delta, int32_t source)
{
    return true;
}

void ScrollPattern::AdjustOffset(float& delta, int32_t source)
{
    if (NearZero(delta) || NearZero(viewPortLength_) || source == SCROLL_FROM_ANIMATION ||
        source == SCROLL_FROM_ANIMATION_SPRING || source == SCROLL_FROM_FOCUS_JUMP) {
        return;
    }
    // the distance above the top, if lower than top, it is zero
    float overScrollPastStart = 0.0f;
    // the distance below the bottom, if higher than bottom, it is zero
    float overScrollPastEnd = 0.0f;
    float overScrollPast = 0.0f;
    // not consider rowReverse or colReverse
    overScrollPastStart = std::max(currentOffset_, 0.0);
    if (Positive(scrollableDistance_)) {
        overScrollPastEnd = std::max(-scrollableDistance_ - currentOffset_, 0.0);
    } else {
        overScrollPastEnd = std::abs(std::min(currentOffset_, 0.0));
    }
    overScrollPast = std::max(overScrollPastStart, overScrollPastEnd);
    if (overScrollPast == 0.0f) {
        return;
    }
    float friction = CalculateFriction((overScrollPast - std::abs(delta)) / viewPortLength_);
    delta = delta * friction;
}

double ScrollPattern::ValidateOffset(int32_t source, double willScrollOffset)
{
    if (LessOrEqual(scrollableDistance_, 0.0f) || source == SCROLL_FROM_JUMP) {
        return willScrollOffset;
    }

    // restrict position between top and bottom
    if (IsRestrictBoundary() || source == SCROLL_FROM_BAR || source == SCROLL_FROM_BAR_FLING ||
        source == SCROLL_FROM_ROTATE || source == SCROLL_FROM_AXIS) {
        if (GetAxis() == Axis::HORIZONTAL) {
            if (IsRowReverse()) {
                willScrollOffset = std::clamp(willScrollOffset, 0.0, scrollableDistance_);
            } else {
                willScrollOffset = std::clamp(willScrollOffset, -scrollableDistance_, 0.0);
            }
        } else {
            willScrollOffset = std::clamp(willScrollOffset, -scrollableDistance_, 0.0);
        }
    }
    return willScrollOffset;
}

void ScrollPattern::ValidateOffset(int32_t source)
{
    if (LessOrEqual(scrollableDistance_, 0.0f) || source == SCROLL_FROM_JUMP) {
        return;
    }

    // restrict position between top and bottom
    if (IsRestrictBoundary() || source == SCROLL_FROM_BAR || source == SCROLL_FROM_BAR_FLING ||
        source == SCROLL_FROM_ROTATE || source == SCROLL_FROM_AXIS) {
        if (GetAxis() == Axis::HORIZONTAL) {
            if (IsRowReverse()) {
                currentOffset_ = std::clamp(currentOffset_, 0.0, scrollableDistance_);
            } else {
                currentOffset_ = std::clamp(currentOffset_, -scrollableDistance_, 0.0);
            }
        } else {
            currentOffset_ = std::clamp(currentOffset_, -scrollableDistance_, 0.0);
        }
    }
}

void ScrollPattern::HandleScrollPosition(float scroll)
{
    auto eventHub = GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onScroll = eventHub->GetOnScrollEvent();
    CHECK_NULL_VOID(onScroll);
    // not consider async call
    Dimension scrollX(0, DimensionUnit::VP);
    Dimension scrollY(0, DimensionUnit::VP);
    Dimension scrollPx(scroll, DimensionUnit::PX);
    auto scrollVpValue = scrollPx.ConvertToVp();
    if (GetAxis() == Axis::HORIZONTAL) {
        scrollX.SetValue(scrollVpValue);
    } else {
        scrollY.SetValue(scrollVpValue);
    }
    onScroll(scrollX, scrollY);
}

float ScrollPattern::FireTwoDimensionOnWillScroll(float scroll)
{
    auto eventHub = GetEventHub<ScrollEventHub>();
    CHECK_NULL_RETURN(eventHub, scroll);
    auto onScroll = eventHub->GetOnWillScrollEvent();
    auto onJsFrameNodeScroll = eventHub->GetJSFrameNodeOnScrollWillScroll();
    auto observer = positionController_ ? positionController_->GetObserverManager() : nullptr;
    CHECK_NULL_RETURN(onScroll || onJsFrameNodeScroll || observer, scroll);
    Dimension scrollX(0, DimensionUnit::VP);
    Dimension scrollY(0, DimensionUnit::VP);
    Dimension scrollPx(scroll, DimensionUnit::PX);
    auto scrollVpValue = scrollPx.ConvertToVp();
    if (GetAxis() == Axis::HORIZONTAL) {
        scrollX.SetValue(scrollVpValue);
    } else {
        scrollY.SetValue(scrollVpValue);
    }
    TwoDimensionScrollResult scrollRes { .xOffset = scrollX, .yOffset = scrollY };
    if (onScroll) {
        scrollRes = onScroll(scrollRes.xOffset, scrollRes.yOffset, GetScrollState(),
            ScrollablePattern::ConvertScrollSource(GetScrollSource()));
    }
    if (onJsFrameNodeScroll) {
        scrollRes = onJsFrameNodeScroll(scrollRes.xOffset, scrollRes.yOffset, GetScrollState(),
            ScrollablePattern::ConvertScrollSource(GetScrollSource()));
    }
    if (observer) {
        scrollRes = FireObserverTwoDimensionOnWillScroll(scrollRes.xOffset, scrollRes.yOffset, GetScrollState(),
            ScrollablePattern::ConvertScrollSource(GetScrollSource()));
    }
    auto context = GetContext();
    CHECK_NULL_RETURN(context, scroll);
    if (GetAxis() == Axis::HORIZONTAL) {
        return context->NormalizeToPx(scrollRes.xOffset);
    } else {
        return context->NormalizeToPx(scrollRes.yOffset);
    }
}

void ScrollPattern::FireOnDidScroll(float scroll)
{
    if (freeScroll_) {
        return; // using FreeModeFireOnDidScroll
    }
    FireObserverOnDidScroll(scroll);
    FireObserverOnScrollerAreaChange(scroll);
    auto eventHub = GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto onScroll = eventHub->GetOnDidScrollEvent();
    auto onJSFrameNodeDidScroll = eventHub->GetJSFrameNodeOnScrollDidScroll();
    CHECK_NULL_VOID(onScroll || onJSFrameNodeDidScroll);
    Dimension scrollX(0, DimensionUnit::VP);
    Dimension scrollY(0, DimensionUnit::VP);
    Dimension scrollPx(scroll, DimensionUnit::PX);
    auto scrollVpValue = scrollPx.ConvertToVp();
    if (GetAxis() == Axis::HORIZONTAL) {
        scrollX.SetValue(scrollVpValue);
    } else {
        scrollY.SetValue(scrollVpValue);
    }
    auto scrollState = GetScrollState();
    bool isTriggered = false;
    if (!NearZero(scroll)) {
        if (onScroll) {
            onScroll(scrollX, scrollY, scrollState);
        }
        if (onJSFrameNodeDidScroll) {
            onJSFrameNodeDidScroll(scrollX, scrollY, scrollState);
        }
        isTriggered = true;
    }
    if (scrollStop_ && !GetScrollAbort()) {
        if (scrollState != ScrollState::IDLE || !isTriggered) {
            if (onScroll) {
                onScroll(0.0_vp, 0.0_vp, ScrollState::IDLE);
            }
            if (onJSFrameNodeDidScroll) {
                onJSFrameNodeDidScroll(0.0_vp, 0.0_vp, ScrollState::IDLE);
            }
        }
    }
}

void ScrollPattern::FireOnReachStart(const OnReachEvent& onReachStart, const OnReachEvent& onJSFrameNodeReachStart)
{
    if (freeScroll_) {
        return; // not supported in FreeScroll mode
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (ReachStart(!isInitialized_)) {
        FireObserverOnReachStart();
        CHECK_NULL_VOID(onReachStart || onJSFrameNodeReachStart);
        ACE_SCOPED_TRACE("OnReachStart, id:%d, tag:Scroll", static_cast<int32_t>(host->GetAccessibilityId()));
        if (onReachStart) {
            onReachStart();
        }
        if (onJSFrameNodeReachStart) {
            onJSFrameNodeReachStart();
        }
        AddEventsFiredInfo(ScrollableEventType::ON_REACH_START);
    }
}

void ScrollPattern::FireOnReachEnd(const OnReachEvent& onReachEnd, const OnReachEvent& onJSFrameNodeReachEnd)
{
    if (freeScroll_) {
        return; // not supported in FreeScroll mode
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (ReachEnd(false)) {
        FireObserverOnReachEnd();
        CHECK_NULL_VOID(onReachEnd || onJSFrameNodeReachEnd);
        ACE_SCOPED_TRACE("OnReachEnd, id:%d, tag:Scroll", static_cast<int32_t>(host->GetAccessibilityId()));
        if (onReachEnd) {
            onReachEnd();
        }
        if (onJSFrameNodeReachEnd) {
            onJSFrameNodeReachEnd();
        }
        AddEventsFiredInfo(ScrollableEventType::ON_REACH_END);
    } else if (!isInitialized_ && ReachEnd(true)) {
        FireObserverOnReachEnd();
    }
}

bool ScrollPattern::IsCrashTop() const
{
    bool scrollUpToReachTop = LessNotEqual(lastOffset_, 0.0) && GreatOrEqual(currentOffset_, 0.0);
    bool scrollDownToReachTop = GreatNotEqual(lastOffset_, 0.0) && LessOrEqual(currentOffset_, 0.0);
    return scrollUpToReachTop || scrollDownToReachTop;
}

bool ScrollPattern::IsCrashBottom() const
{
    float minExtent = -scrollableDistance_;
    bool scrollDownToReachEnd = GreatNotEqual(lastOffset_, minExtent) && LessOrEqual(currentOffset_, minExtent);
    bool scrollUpToReachEnd = LessNotEqual(lastOffset_, minExtent) && GreatOrEqual(currentOffset_, minExtent);
    return (scrollUpToReachEnd || scrollDownToReachEnd);
}

bool ScrollPattern::ReachStart(bool firstLayout) const
{
    bool scrollUpToReachTop = (LessNotEqual(prevOffset_, 0.0) || firstLayout) && GreatOrEqual(currentOffset_, 0.0);
    bool scrollDownToReachTop = GreatNotEqual(prevOffset_, 0.0) && LessOrEqual(currentOffset_, 0.0);
    return scrollUpToReachTop || scrollDownToReachTop;
}

bool ScrollPattern::ReachEnd(bool firstLayout) const
{
    float minExtent = -scrollableDistance_;
    bool scrollDownToReachEnd =
        (GreatNotEqual(prevOffset_, minExtent) || firstLayout) && LessOrEqual(currentOffset_, minExtent);
    bool scrollUpToReachEnd = LessNotEqual(prevOffset_, minExtent) && GreatOrEqual(currentOffset_, minExtent);
    return (scrollUpToReachEnd || scrollDownToReachEnd);
}

void ScrollPattern::HandleCrashTop()
{
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(eventHub);
    const auto& onScrollEdge = eventHub->GetScrollEdgeEvent();
    CHECK_NULL_VOID(onScrollEdge);
    // not consider async call
    if (GetAxis() == Axis::HORIZONTAL) {
        onScrollEdge(ScrollEdge::LEFT);
        AddEventsFiredInfo(ScrollableEventType::ON_SCROLL_EDGE);
        return;
    }
    onScrollEdge(ScrollEdge::TOP);
    AddEventsFiredInfo(ScrollableEventType::ON_SCROLL_EDGE);
}

void ScrollPattern::HandleCrashBottom()
{
    auto frameNode = GetHost();
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(eventHub);
    const auto& onScrollEdge = eventHub->GetScrollEdgeEvent();
    CHECK_NULL_VOID(onScrollEdge);
    if (GetAxis() == Axis::HORIZONTAL) {
        onScrollEdge(ScrollEdge::RIGHT);
        AddEventsFiredInfo(ScrollableEventType::ON_SCROLL_EDGE);
        return;
    }
    onScrollEdge(ScrollEdge::BOTTOM);
    AddEventsFiredInfo(ScrollableEventType::ON_SCROLL_EDGE);
}

#ifdef SUPPORT_DIGITAL_CROWN
void ScrollPattern::StartVibrateFeedback()
{
    if (!GetCrownEventDragging()) {
        return;
    }
    if (crownEventNum_ < CROWN_EVENT_NUN_THRESH_MIN) {
        crownEventNum_++;
    }
    crownEventNum_ = (reachBoundary_ ? 0 : (crownEventNum_ + 1));
    TAG_LOGI(AceLogTag::ACE_SCROLLABLE, "StartVibrateFeedback crownEventNum_:%{public}d [reachBoundary_:%{public}s]",
        crownEventNum_, (reachBoundary_ ? "true" : "false"));
    auto currentTime = GetSysTimestamp();
    if (!reachBoundary_ &&
        (crownEventNum_ > CROWN_EVENT_NUN_THRESH_MIN && currentTime - lastTime_ > CROWN_VIBRATOR_INTERVAL_TIME)) {
        VibratorUtils::StartVibraFeedback(CROWN_VIBRATOR_WEAK);
        lastTime_ = GetSysTimestamp();
        TAG_LOGI(AceLogTag::ACE_SCROLLABLE, "StartVibrateFeedback StartVibrateFeedback %{public}s",
            CROWN_VIBRATOR_WEAK);
    }
}
#endif

bool ScrollPattern::UpdateCurrentOffset(float delta, int32_t source)
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    if (source != SCROLL_FROM_JUMP && !HandleEdgeEffect(delta, source, viewSize_)) {
        if (IsOutOfBoundary()) {
            host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
        }
        return false;
    }
    SetScrollSource(source);
    FireAndCleanScrollingListener();
    lastOffset_ = currentOffset_;
    auto willScrollPosition = currentOffset_ + delta;
    willScrollPosition = ValidateOffset(source, willScrollPosition);
    auto userOffset = FireTwoDimensionOnWillScroll(currentOffset_ - willScrollPosition);
    currentOffset_ -= userOffset;
    ValidateOffset(source);
    HandleScrollPosition(userOffset);
    host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    MarkScrollBarProxyDirty();
#ifdef SUPPORT_DIGITAL_CROWN
    if (source == SCROLL_FROM_CROWN && !ReachStart(true) && !ReachEnd(true)) {
        StartVibrateFeedback();
    }
#endif
    if (IsCrashTop()) {
        TAG_LOGI(AceLogTag::ACE_SCROLLABLE, "UpdateCurrentOffset==>[HandleCrashTop();]");
#ifdef SUPPORT_DIGITAL_CROWN
        SetReachBoundary(true);
#endif
        HandleCrashTop();
    } else if (IsCrashBottom()) {
        TAG_LOGI(AceLogTag::ACE_SCROLLABLE, "UpdateCurrentOffset==>[HandleCrashBottom();]");
#ifdef SUPPORT_DIGITAL_CROWN
        SetReachBoundary(true);
#endif
        HandleCrashBottom();
    }
#ifdef SUPPORT_DIGITAL_CROWN
    if (!IsCrashBottom() && !IsCrashTop()) {
        SetReachBoundary(false);
    }
#endif
    return true;
}

void ScrollPattern::OnAnimateStop()
{
    if (!GetIsDragging() || GetScrollAbort()) {
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
        scrollStop_ = true;
    }
}

void ScrollPattern::ScrollToEdge(ScrollEdgeType scrollEdgeType, bool smooth)
{
    if (scrollEdgeType == ScrollEdgeType::SCROLL_NONE) {
        return;
    }
    if (LessOrEqual(scrollableDistance_, 0.0)) {
        return;
    }
    float distance =
        scrollEdgeType == ScrollEdgeType::SCROLL_TOP ? -currentOffset_ : (-scrollableDistance_ - currentOffset_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    ACE_SCOPED_TRACE("Scroll ScrollToEdge scrollEdgeType:%zu, offset:%f, id:%d", scrollEdgeType, distance,
        static_cast<int32_t>(host->GetAccessibilityId()));
    ScrollBy(distance, distance, smooth);
    scrollEdgeType_ = scrollEdgeType;
}

void ScrollPattern::CheckScrollToEdge()
{
    if (scrollEdgeType_ != ScrollEdgeType::SCROLL_NONE) {
        ScrollToEdge(scrollEdgeType_, true);
    }
}

void ScrollPattern::ScrollBy(float pixelX, float pixelY, bool smooth, const std::function<void()>& onFinish)
{
    float distance = (GetAxis() == Axis::VERTICAL) ? pixelY : pixelX;
    if (NearZero(distance)) {
        return;
    }
    float position = currentOffset_ + distance;
    SetIsOverScroll(false);
    if (smooth) {
        AnimateTo(-position, fabs(distance) * UNIT_CONVERT / SCROLL_BY_SPEED, Curves::EASE_OUT, true, false, false);
        return;
    }
    JumpToPosition(position);
}

void ScrollPattern::ScrollPage(bool reverse, bool smooth, AccessibilityScrollType scrollType)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    float distance = reverse ? viewPortLength_ : -viewPortLength_;
    if (scrollType == AccessibilityScrollType::SCROLL_HALF) {
        distance = distance / 2.f;
    }
    ACE_SCOPED_TRACE(
        "Scroll ScrollPage distance:%f, id:%d", distance, static_cast<int32_t>(host->GetAccessibilityId()));
    ScrollBy(distance, distance, smooth);
}

void ScrollPattern::JumpToPosition(float position, int32_t source)
{
    // If an animation is playing, stop it.
    auto lastAnimateRunning = AnimateRunning();
    StopAnimate();
    DoJump(position, source);
    // AccessibilityEventType::SCROLL_END
    if (lastAnimateRunning) {
        SetScrollAbort(false);
    }
}

void ScrollPattern::ScrollTo(float position)
{
    SetAnimateCanOverScroll(GetCanStayOverScroll());
    JumpToPosition(-position, SCROLL_FROM_JUMP);
    SetIsOverScroll(GetCanStayOverScroll());
}

void ScrollPattern::DoJump(float position, int32_t source)
{
    float setPosition = (GetAxis() == Axis::HORIZONTAL && IsRowReverse()) ? -position : position;
    if ((!NearEqual(currentOffset_, setPosition) && GreatOrEqual(scrollableDistance_, 0.0f)) ||
        GetCanStayOverScroll()) {
        UpdateCurrentOffset(setPosition - currentOffset_, source);
    }
}

void ScrollPattern::SetEdgeEffectCallback(const RefPtr<ScrollEdgeEffect>& scrollEffect)
{
    scrollEffect->SetCurrentPositionCallback([weakScroll = AceType::WeakClaim(this)]() -> double {
        auto scroll = weakScroll.Upgrade();
        CHECK_NULL_RETURN(scroll, 0.0);
        return scroll->GetCurrentPosition();
    });
    scrollEffect->SetLeadingCallback([weakScroll = AceType::WeakClaim(this)]() -> double {
        auto scroll = weakScroll.Upgrade();
        if (scroll && !scroll->IsRowReverse() && !scroll->IsColReverse() && scroll->GetScrollableDistance() > 0) {
            return -scroll->GetScrollableDistance();
        }
        return 0.0;
    });
    scrollEffect->SetTrailingCallback([weakScroll = AceType::WeakClaim(this)]() -> double {
        auto scroll = weakScroll.Upgrade();
        if (scroll && (scroll->IsRowReverse() || scroll->IsColReverse())) {
            return scroll->GetScrollableDistance();
        }
        return 0.0;
    });
    scrollEffect->SetInitLeadingCallback([weakScroll = AceType::WeakClaim(this)]() -> double {
        auto scroll = weakScroll.Upgrade();
        if (scroll && !scroll->IsRowReverse() && !scroll->IsColReverse() && scroll->GetScrollableDistance() > 0) {
            return -scroll->GetScrollableDistance();
        }
        return 0.0;
    });
    scrollEffect->SetInitTrailingCallback([weakScroll = AceType::WeakClaim(this)]() -> double {
        auto scroll = weakScroll.Upgrade();
        if (scroll && (scroll->IsRowReverse() || scroll->IsColReverse())) {
            return scroll->GetScrollableDistance();
        }
        return 0.0;
    });
}

void ScrollPattern::UpdateScrollBarOffset()
{
    if (freeScroll_) {
        return;
    }
    CheckScrollBarOff();
    if (!GetScrollBar() && !GetScrollBarProxy()) {
        return;
    }

    float scrollBarOutBoundaryExtent = 0.0f;
    if (currentOffset_ > 0) {
        scrollBarOutBoundaryExtent = currentOffset_;
    } else if ((-currentOffset_) >= (GetMainSize(viewPortExtent_) - GetMainSize(viewPort_))) {
        scrollBarOutBoundaryExtent = -currentOffset_ - (GetMainSize(viewPortExtent_) - GetMainSize(viewPort_));
    }
    HandleScrollBarOutBoundary(scrollBarOutBoundaryExtent);

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty<ScrollLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto padding = layoutProperty->CreatePaddingAndBorder();
    auto contentEndOffset = layoutProperty->GetScrollContentEndOffsetValue(.0f);
    Size size(viewSize_.Width(), viewSize_.Height() - contentEndOffset);
    auto viewPortExtent = viewPortExtent_;
    AddPaddingToSize(padding, viewPortExtent);
    auto estimatedHeight = (GetAxis() == Axis::HORIZONTAL) ? viewPortExtent.Width() : viewPortExtent.Height();
    UpdateScrollBarRegion(-currentOffset_, estimatedHeight, size, Offset(0.0f, 0.0f));
}

void ScrollPattern::SetAccessibilityAction()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto accessibilityProperty = host->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetActionScrollForward([weakPtr = WeakClaim(this)](AccessibilityScrollType scrollType) {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto host = pattern->GetHost();
        CHECK_NULL_VOID(host);
        ACE_SCOPED_TRACE("accessibility action, scroll forward, isScrollable:%u, IsPositiveScrollableDistance:%u, "
                         "scrollType:%d, id:%d, tag:Scroll",
            pattern->IsScrollable(), pattern->IsPositiveScrollableDistance(), scrollType,
            static_cast<int32_t>(host->GetAccessibilityId()));
        if (pattern->IsScrollable() && pattern->IsPositiveScrollableDistance()) {
            pattern->ScrollPage(false, true, scrollType);
        }
    });

    accessibilityProperty->SetActionScrollBackward([weakPtr = WeakClaim(this)](AccessibilityScrollType scrollType) {
        const auto& pattern = weakPtr.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto host = pattern->GetHost();
        CHECK_NULL_VOID(host);
        ACE_SCOPED_TRACE("accessibility action, scroll backward, isScrollable:%u, IsPositiveScrollableDistance:%u, "
                         "scrollType:%d, id:%d, tag:Scroll",
            pattern->IsScrollable(), pattern->IsPositiveScrollableDistance(), scrollType,
            static_cast<int32_t>(host->GetAccessibilityId()));
        if (pattern->IsScrollable() && pattern->IsPositiveScrollableDistance()) {
            pattern->ScrollPage(true, true, scrollType);
        }
    });
}

OffsetF ScrollPattern::GetOffsetToScroll(const RefPtr<FrameNode>& childFrame) const
{
    auto frameNode = GetHost();
    CHECK_NULL_RETURN(frameNode, OffsetF());
    CHECK_NULL_RETURN(childFrame, OffsetF());
    auto childGeometryNode = childFrame->GetGeometryNode();
    CHECK_NULL_RETURN(childGeometryNode, OffsetF());
    OffsetF result = childGeometryNode->GetFrameOffset();
    auto parent = childFrame->GetParent();
    while (parent) {
        auto parentFrame = AceType::DynamicCast<FrameNode>(parent);
        if (!parentFrame) {
            parent = parent->GetParent();
            continue;
        }
        if (parentFrame == frameNode) {
            return result;
        }
        auto parentGeometryNode = parentFrame->GetGeometryNode();
        if (!parentGeometryNode) {
            parent = parent->GetParent();
            continue;
        }
        result += parentGeometryNode->GetFrameOffset();
        parent = parent->GetParent();
    }
    return OffsetF(0.0, 0.0);
}

bool ScrollPattern::ScrollToNode(const RefPtr<FrameNode>& focusFrameNode)
{
    CHECK_NULL_RETURN(focusFrameNode, false);
    auto focusGeometryNode = focusFrameNode->GetGeometryNode();
    CHECK_NULL_RETURN(focusGeometryNode, false);
    auto focusNodeSize = focusGeometryNode->GetFrameSize();
    auto focusNodeOffsetToScrolll = GetOffsetToScroll(focusFrameNode);
    auto scrollFrame = GetHost();
    CHECK_NULL_RETURN(scrollFrame, false);
    auto scrollGeometry = scrollFrame->GetGeometryNode();
    CHECK_NULL_RETURN(scrollGeometry, false);
    auto scrollFrameSize = scrollGeometry->GetFrameSize();
    float focusNodeDiffToScroll =
        GetAxis() == Axis::VERTICAL ? focusNodeOffsetToScrolll.GetY() : focusNodeOffsetToScrolll.GetX();
    if (NearZero(focusNodeDiffToScroll)) {
        return false;
    }
    float focusNodeLength = GetAxis() == Axis::VERTICAL ? focusNodeSize.Height() : focusNodeSize.Width();
    float scrollFrameLength = GetAxis() == Axis::VERTICAL ? scrollFrameSize.Height() : scrollFrameSize.Width();
    float moveOffset = 0.0;
    if (LessNotEqual(focusNodeDiffToScroll, 0)) {
        moveOffset = -focusNodeDiffToScroll;
    } else if (GreatNotEqual(focusNodeDiffToScroll + focusNodeLength, scrollFrameLength)) {
        moveOffset = scrollFrameLength - focusNodeDiffToScroll - focusNodeLength;
    }
    if (!NearZero(moveOffset)) {
        return OnScrollCallback(moveOffset, SCROLL_FROM_FOCUS_JUMP);
    }
    return false;
}

ScrollOffsetAbility ScrollPattern::GetScrollOffsetAbility()
{
    return { [wp = WeakClaim(this)](float moveOffset) -> bool {
                auto pattern = wp.Upgrade();
                CHECK_NULL_RETURN(pattern, false);
                return pattern->OnScrollCallback(moveOffset, SCROLL_FROM_FOCUS_JUMP);
            },
        GetAxis() };
}

std::optional<float> ScrollPattern::CalcPredictSnapOffset(
    float delta, float dragDistance, float velocity, SnapDirection snapDirection)
{
    std::optional<float> predictSnapOffset;
    CHECK_NULL_RETURN(IsScrollSnap(), predictSnapOffset);
    if (snapDirection != SnapDirection::NONE) {
        return CalcPredictNextSnapOffset(delta, snapDirection);
    }
    float finalPosition = currentOffset_ + delta;
    if (IsEnablePagingValid()) {
        finalPosition = GetPagingOffset(delta, dragDistance, velocity);
    }
    if (!IsSnapToInterval()) {
        if (!enableSnapToSide_.first) {
            CHECK_NULL_RETURN(!(GreatNotEqual(finalPosition, *(snapOffsets_.begin() + 1)) ||
                                  GreatNotEqual(currentOffset_, *(snapOffsets_.begin() + 1))),
                predictSnapOffset);
        }
        if (!enableSnapToSide_.second) {
            CHECK_NULL_RETURN(!(LessNotEqual(finalPosition, *(snapOffsets_.rbegin() + 1)) ||
                                  LessNotEqual(currentOffset_, *(snapOffsets_.rbegin() + 1))),
                predictSnapOffset);
        }
    }
    float head = 0.0f;
    float tail = -scrollableDistance_;
    if (GreatOrEqual(finalPosition, head) || LessOrEqual(finalPosition, tail)) {
        predictSnapOffset = finalPosition;
    } else if (LessNotEqual(finalPosition, head) && GreatOrEqual(finalPosition, *(snapOffsets_.begin()))) {
        predictSnapOffset = *(snapOffsets_.begin());
    } else if (GreatNotEqual(finalPosition, tail) && LessOrEqual(finalPosition, *(snapOffsets_.rbegin()))) {
        predictSnapOffset = *(snapOffsets_.rbegin());
    } else {
        auto iter = snapOffsets_.begin() + 1;
        float start = *(iter - 1);
        float end = *(iter);
        for (; iter != snapOffsets_.end(); ++iter) {
            if (GreatOrEqual(finalPosition, *iter)) {
                start = *(iter - 1);
                end = *(iter);
                predictSnapOffset = (LessNotEqual(start - finalPosition, finalPosition - end) ? start : end);
                break;
            }
        }
    }
    if (predictSnapOffset.has_value()) {
        predictSnapOffset = predictSnapOffset.value() - currentOffset_;
    }
    return predictSnapOffset;
}

std::optional<float> ScrollPattern::CalcPredictNextSnapOffset(float delta, SnapDirection snapDirection)
{
    std::optional<float> predictSnapOffset;
    int32_t start = 0;
    int32_t end = static_cast<int32_t>(snapOffsets_.size()) - 1;
    int32_t mid = 0;
    auto targetOffset = currentOffset_ + delta;
    if (LessOrEqual(targetOffset, snapOffsets_[end]) && snapDirection == SnapDirection::BACKWARD) {
        predictSnapOffset = -scrollableDistance_ - currentOffset_;
        return predictSnapOffset;
    } else if (GreatOrEqual(targetOffset, snapOffsets_[start]) && snapDirection == SnapDirection::FORWARD) {
        predictSnapOffset = -currentOffset_;
        return predictSnapOffset;
    }
    while (start < end) {
        mid = (start + end) / 2;
        if (LessNotEqual(snapOffsets_[mid], targetOffset)) {
            end = mid;
        } else if (GreatNotEqual(snapOffsets_[mid], targetOffset)) {
            start = mid + 1;
        } else {
            if (snapDirection == SnapDirection::FORWARD && mid > 0) {
                predictSnapOffset = snapOffsets_[mid - 1] - currentOffset_;
            } else if (snapDirection == SnapDirection::BACKWARD &&
                       (mid + 1) < static_cast<int32_t>(snapOffsets_.size())) {
                predictSnapOffset = snapOffsets_[mid + 1] - currentOffset_;
            }
            return predictSnapOffset;
        }
    }
    if (snapDirection == SnapDirection::FORWARD) {
        predictSnapOffset = snapOffsets_[std::max(start - 1, 0)] - currentOffset_;
    } else if (snapDirection == SnapDirection::BACKWARD) {
        predictSnapOffset = snapOffsets_[start] - currentOffset_;
    }
    return predictSnapOffset;
}

void ScrollPattern::CaleSnapOffsets(const RefPtr<FrameNode>& host)
{
    auto scrollSnapAlign = GetScrollSnapAlign(host);
    std::vector<float>().swap(snapOffsets_);
    if (scrollSnapAlign == ScrollSnapAlign::NONE) {
        CHECK_NULL_VOID(enablePagingStatus_ == ScrollPagingStatus::VALID);
        scrollSnapAlign = ScrollSnapAlign::START;
    }
    if (IsSnapToInterval()) {
        CaleSnapOffsetsByInterval(scrollSnapAlign, host);
    } else {
        CaleSnapOffsetsByPaginations(scrollSnapAlign);
    }
}

void ScrollPattern::CaleSnapOffsetsByInterval(ScrollSnapAlign scrollSnapAlign, const RefPtr<FrameNode>& host)
{
    auto mainSize = GetMainAxisSize(viewPort_, GetAxis());
    auto pipeline = host->GetContext();
    auto intervalSize = intervalSize_.Unit() == DimensionUnit::PERCENT
                            ? intervalSize_.Value() * mainSize
                            : (pipeline ? pipeline->NormalizeToPx(intervalSize_) : intervalSize_.ConvertToPx());
    CHECK_NULL_VOID(GreatOrEqual(intervalSize, SCROLL_SNAP_INTERVAL_SIZE_MIN_VALUE));
    auto extentMainSize = GetMainAxisSize(viewPortExtent_, GetAxis());
    auto start = 0.0f;
    auto end = -scrollableDistance_;
    auto snapOffset = 0.0f;
    auto sizeDelta = 0.0f;
    float temp = static_cast<int32_t>(extentMainSize / intervalSize) * intervalSize;
    switch (scrollSnapAlign) {
        case ScrollSnapAlign::START:
            end = -temp;
            break;
        case ScrollSnapAlign::CENTER:
            sizeDelta = (mainSize - intervalSize) / 2;
            start = Positive(sizeDelta) ? sizeDelta - static_cast<int32_t>(sizeDelta / intervalSize) * intervalSize
                                        : sizeDelta;
            end = -temp + (mainSize - extentMainSize + temp) / 2;
            break;
        case ScrollSnapAlign::END:
            sizeDelta = mainSize - intervalSize;
            start = Positive(sizeDelta) ? mainSize - static_cast<int32_t>(mainSize / intervalSize) * intervalSize
                                        : sizeDelta;
            end = -scrollableDistance_;
            break;
        default:
            break;
    }
    if (!Positive(start)) {
        snapOffsets_.emplace_back(start);
    }
    snapOffset = start - intervalSize;
    while (GreatOrEqual(snapOffset, -scrollableDistance_) && GreatOrEqual(snapOffset, end)) {
        snapOffsets_.emplace_back(snapOffset);
        snapOffset -= intervalSize;
    }
    if (GreatNotEqual(end, -scrollableDistance_)) {
        snapOffsets_.emplace_back(end);
    }
    if (IsEnablePagingValid()) {
        if (NearEqual(snapOffset + intervalSize, -scrollableDistance_)) {
            lastPageLength_ = 0.f;
            return;
        }
        lastPageLength_ = scrollableDistance_ + snapOffset + intervalSize;
        snapOffsets_.emplace_back(-scrollableDistance_);
    }
}

void ScrollPattern::CaleSnapOffsetsByPaginations(ScrollSnapAlign scrollSnapAlign)
{
    auto mainSize = GetMainAxisSize(viewPort_, GetAxis());
    auto extentMainSize = GetMainAxisSize(viewPortExtent_, GetAxis());
    auto start = 0.0f;
    auto end = -scrollableDistance_;
    auto snapOffset = 0.0f;
    snapOffsets_.emplace_back(start);
    int32_t length = 0;
    auto snapPaginations = snapPaginations_;
    snapPaginations.emplace(snapPaginations.begin(), Dimension(0.f));
    auto current = 0.0f;
    auto next = 0.0f;
    auto size = static_cast<int32_t>(snapPaginations.size());
    auto element = snapPaginations[length];
    auto nextElement = snapPaginations[length + 1];
    for (; length < size; length++) {
        element = snapPaginations[length];
        current = element.Unit() == DimensionUnit::PERCENT ? element.Value() * mainSize : element.ConvertToPx();
        if (length == size - 1) {
            next = extentMainSize;
        } else {
            nextElement = snapPaginations[length + 1];
            next = nextElement.Unit() == DimensionUnit::PERCENT ? nextElement.Value() * mainSize
                                                                : nextElement.ConvertToPx();
        }
        switch (scrollSnapAlign) {
            case ScrollSnapAlign::START:
                snapOffset = -current;
                break;
            case ScrollSnapAlign::CENTER:
                snapOffset = (mainSize - (current + next)) / 2.0f;
                break;
            case ScrollSnapAlign::END:
                snapOffset = mainSize - next;
                break;
            default:
                break;
        }
        if (!Negative(snapOffset)) {
            continue;
        }
        if (GreatNotEqual(snapOffset, -scrollableDistance_)) {
            snapOffsets_.emplace_back(snapOffset);
        } else {
            break;
        }
    }
    snapOffsets_.emplace_back(end);
}

bool ScrollPattern::NeedScrollSnapToSide(float delta)
{
    CHECK_NULL_RETURN(GetScrollSnapAlign() != ScrollSnapAlign::NONE, false);
    CHECK_NULL_RETURN(!IsSnapToInterval(), false);
    CHECK_NULL_RETURN(static_cast<int32_t>(snapOffsets_.size()) > 2, false);
    auto finalPosition = currentOffset_ + delta;
    if (!enableSnapToSide_.first) {
        if (GreatOrEqual(currentOffset_, *(snapOffsets_.begin() + 1)) &&
            LessOrEqual(finalPosition, *(snapOffsets_.begin() + 1))) {
            return true;
        }
    }
    if (!enableSnapToSide_.second) {
        if (LessOrEqual(currentOffset_, *(snapOffsets_.rbegin() + 1)) &&
            GreatOrEqual(finalPosition, *(snapOffsets_.rbegin() + 1))) {
            return true;
        }
    }
    return false;
}

ScrollSnapAlign ScrollPattern::GetScrollSnapAlign(const RefPtr<FrameNode>& host) const
{
    CHECK_NULL_RETURN(host, ScrollSnapAlign::NONE);
    auto scrollLayoutProperty = host->GetLayoutProperty<ScrollLayoutProperty>();
    CHECK_NULL_RETURN(scrollLayoutProperty, ScrollSnapAlign::NONE);
    return scrollLayoutProperty->GetScrollSnapAlign().value_or(ScrollSnapAlign::NONE);
}

std::string ScrollPattern::ProvideRestoreInfo()
{
    Dimension dimension(currentOffset_);
    return StringUtils::DoubleToString(dimension.ConvertToVp());
}

void ScrollPattern::OnRestoreInfo(const std::string& restoreInfo)
{
    Dimension dimension = StringUtils::StringToDimension(restoreInfo, true);
    currentOffset_ = dimension.ConvertToPx();
}

Rect ScrollPattern::GetItemRect(int32_t index) const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, Rect());
    if (index != 0 || host->TotalChildCount() != 1) {
        return Rect();
    }
    auto item = host->GetChildByIndex(index);
    CHECK_NULL_RETURN(item, Rect());
    auto itemGeometry = item->GetGeometryNode();
    CHECK_NULL_RETURN(itemGeometry, Rect());
    float scale = GetZoomScale();
    if (scale == 1.0f) {
        return Rect(itemGeometry->GetFrameRect().GetX(), itemGeometry->GetFrameRect().GetY(),
            itemGeometry->GetFrameRect().Width(), itemGeometry->GetFrameRect().Height());
    } else {
        auto rect = itemGeometry->GetFrameRect();
        auto cx = rect.Left() + rect.Width() / 2;
        auto cy = rect.Top() + rect.Height() / 2;
        auto left = cx - (cx - rect.Left()) * scale;
        auto top = cy - (cy - rect.Top()) * scale;
        auto size = itemGeometry->GetFrameSize() * scale;
        return Rect(left, top, size.Width(), size.Height());
    }
}

float ScrollPattern::GetSelectScrollWidth()
{
    RefPtr<GridColumnInfo> columnInfo = GridSystemManager::GetInstance().GetInfoByType(GridColumnType::MENU);
    auto parent = columnInfo->GetParent();
    CHECK_NULL_RETURN(parent, SELECT_SCROLL_MIN_WIDTH.ConvertToPx());
    parent->BuildColumnWidth();
    auto defaultWidth = static_cast<float>(columnInfo->GetWidth(COLUMN_NUM));
    auto scrollNode = GetHost();
    CHECK_NULL_RETURN(scrollNode, SELECT_SCROLL_MIN_WIDTH.ConvertToPx());
    float finalWidth = SELECT_SCROLL_MIN_WIDTH.ConvertToPx();

    if (IsWidthModifiedBySelect()) {
        auto scrollLayoutProperty = scrollNode->GetLayoutProperty<ScrollLayoutProperty>();
        CHECK_NULL_RETURN(scrollLayoutProperty, SELECT_SCROLL_MIN_WIDTH.ConvertToPx());
        auto selectModifiedWidth = scrollLayoutProperty->GetScrollWidth();
        finalWidth = selectModifiedWidth.value();
    } else {
        finalWidth = defaultWidth;
    }

    if (finalWidth < SELECT_SCROLL_MIN_WIDTH.ConvertToPx()) {
        finalWidth = defaultWidth;
    }

    return finalWidth;
}

float ScrollPattern::GetPagingOffset(float delta, float dragDistance, float velocity) const
{
    // handle last page
    if (GreatNotEqual(lastPageLength_, 0.f) && LessNotEqual(currentOffset_, -scrollableDistance_ + lastPageLength_)) {
        auto offset = fmod(currentOffset_, viewPortLength_);
        return currentOffset_ - offset + GetPagingDelta(offset, velocity, lastPageLength_);
    }
    // handle other pages
    float head = 0.0f;
    float tail = -scrollableDistance_;
    auto offset = fmod(currentOffset_, viewPortLength_);
    auto pagingPosition = currentOffset_ - offset + GetPagingDelta(offset, velocity, viewPortLength_);
    auto finalPosition = currentOffset_ + delta;
    auto useFinalPosition = (GreatOrEqual(pagingPosition, head) && !GreatOrEqual(finalPosition, head)) ||
                            (LessOrEqual(pagingPosition, tail) && !LessOrEqual(finalPosition, tail));
    return useFinalPosition ? finalPosition : pagingPosition;
}

float ScrollPattern::GetPagingDelta(float dragDistance, float velocity, float pageLength) const
{
    auto dragDistanceThreshold = pageLength * 0.5f;
    // dragDistance and velocity have not reached the threshold
    if (LessNotEqual(std::abs(dragDistance), dragDistanceThreshold) &&
        LessNotEqual(std::abs(velocity), SCROLL_PAGING_SPEED_THRESHOLD)) {
        return 0.f;
    }
    // The direction of dragDistance is the same as the direction of velocity
    if (GreatOrEqual(dragDistance * velocity, 0.f)) {
        auto direction = NearZero(dragDistance) ? velocity : dragDistance;
        return GreatNotEqual(direction, 0.f) ? pageLength : -pageLength;
    }
    // The direction of dragDistance is opposite to the direction of velocity
    if (GreatOrEqual(std::abs(dragDistance), dragDistanceThreshold) &&
        LessNotEqual(std::abs(velocity), SCROLL_PAGING_SPEED_THRESHOLD)) {
        return GreatNotEqual(dragDistance, 0.f) ? pageLength : -pageLength;
    }
    return 0.f;
}

void ScrollPattern::TriggerModifyDone()
{
    OnModifyDone();
}

void ScrollPattern::AddScrollMeasureInfo(const std::optional<LayoutConstraintF>& parentConstraint,
    const std::optional<LayoutConstraintF>& childConstraint, const SizeF& selfSize, const SizeF& childSize)
{
    if (scrollMeasureInfos_.size() >= SCROLL_MEASURE_INFO_COUNT) {
        scrollMeasureInfos_.pop_front();
    }
    scrollMeasureInfos_.push_back(ScrollMeasureInfo({
        .changedTime_ = GetSysTimestamp(),
        .parentConstraint_ = parentConstraint,
        .childConstraint_ = childConstraint,
        .selfSize_ = selfSize,
        .childSize_ = childSize,
    }));
}

void ScrollPattern::AddScrollLayoutInfo()
{
    if (scrollLayoutInfos_.size() >= SCROLL_LAYOUT_INFO_COUNT) {
        scrollLayoutInfos_.pop_front();
    }
    scrollLayoutInfos_.push_back(ScrollLayoutInfo({
        .changedTime_ = GetSysTimestamp(),
        .scrollableDistance_ = scrollableDistance_,
        .scrollSize_ = viewSize_,
        .viewPort_ = viewPort_,
        .childSize_ = viewPortExtent_,
    }));
}

void ScrollPattern::GetScrollSnapAlignDumpInfo()
{
    switch (GetScrollSnapAlign()) {
        case ScrollSnapAlign::NONE: {
            DumpLog::GetInstance().AddDesc("snapAlign: ScrollSnapAlign::NONE");
            break;
        }
        case ScrollSnapAlign::START: {
            DumpLog::GetInstance().AddDesc("snapAlign: ScrollSnapAlign::START");
            break;
        }
        case ScrollSnapAlign::CENTER: {
            DumpLog::GetInstance().AddDesc("snapAlign: ScrollSnapAlign::CENTER");
            break;
        }
        case ScrollSnapAlign::END: {
            DumpLog::GetInstance().AddDesc("snapAlign: ScrollSnapAlign::END");
            break;
        }
        default: {
            break;
        }
    }
}

void ScrollPattern::GetScrollPagingStatusDumpInfo()
{
    switch (enablePagingStatus_) {
        case ScrollPagingStatus::NONE: {
            DumpLog::GetInstance().AddDesc("enablePaging: ScrollPagingStatus::NONE");
            break;
        }
        case ScrollPagingStatus::INVALID: {
            DumpLog::GetInstance().AddDesc("enablePaging: ScrollPagingStatus::INVALID");
            break;
        }
        case ScrollPagingStatus::VALID: {
            DumpLog::GetInstance().AddDesc("enablePaging: ScrollPagingStatus::VALID");
            break;
        }
        default: {
            break;
        }
    }
}

void ScrollPattern::DumpAdvanceInfo()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hub = host->GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(hub);
    ScrollablePattern::DumpAdvanceInfo();
    DumpLog::GetInstance().AddDesc(std::string("currentOffset: ").append(std::to_string(currentOffset_)));
    GetScrollSnapAlignDumpInfo();
    auto snapPaginationStr = std::string("snapPagination: ");
    DumpLog::GetInstance().AddDesc(snapPaginationStr.append(GetScrollSnapPagination()));
    enableSnapToSide_.first ? DumpLog::GetInstance().AddDesc("enableSnapToStart: true")
                            : DumpLog::GetInstance().AddDesc("enableSnapToStart: false");
    enableSnapToSide_.second ? DumpLog::GetInstance().AddDesc("enableSnapToEnd: true")
                             : DumpLog::GetInstance().AddDesc("enableSnapToEnd: false");
    GetScrollPagingStatusDumpInfo();
    auto snapOffsetsStr = std::string("snapOffsets: [");
    for (const auto& iter : snapPaginations_) {
        snapOffsetsStr = snapOffsetsStr.append(iter.ToString()).append(" ");
    }
    DumpLog::GetInstance().AddDesc(snapOffsetsStr.append("]"));
    initialOffset_.has_value()
        ? DumpLog::GetInstance().AddDesc(
              std::string("initialOffset: ").append(initialOffset_->GetMainOffset(GetAxis()).ToString()))
        : DumpLog::GetInstance().AddDesc("initialOffset: None");
    auto onScrollEdge = hub->GetScrollEdgeEvent();
    onScrollEdge ? DumpLog::GetInstance().AddDesc("hasOnScrollEdge: true")
                 : DumpLog::GetInstance().AddDesc("hasOnScrollEdge: false");
    DumpLog::GetInstance().AddDesc("==========================scrollLayoutInfos==========================");
    for (const auto& info : scrollLayoutInfos_) {
        DumpLog::GetInstance().AddDesc(info.ToString());
    }
    DumpLog::GetInstance().AddDesc("==========================scrollLayoutInfos==========================");
    DumpLog::GetInstance().AddDesc("==========================scrollMeasureInfos==========================");
    for (const auto& info : scrollMeasureInfos_) {
        DumpLog::GetInstance().AddDesc(info.ToString());
    }
    DumpLog::GetInstance().AddDesc("==========================scrollMeasureInfos==========================");
}

void ScrollPattern::ToJsonValue(std::unique_ptr<JsonValue>& json, const InspectorFilter& filter) const
{
    ScrollablePattern::ToJsonValue(json, filter);
    /* no fixed attr below, just return */
    if (filter.IsFastFilter()) {
        return;
    }
    auto initialOffset = JsonUtil::Create(true);
    initialOffset->Put("xOffset", GetInitialOffset().GetX().ToString().c_str());
    initialOffset->Put("yOffset", GetInitialOffset().GetY().ToString().c_str());
    json->PutExtAttr("initialOffset", initialOffset, filter);
    if (enablePagingStatus_ != ScrollPagingStatus::NONE) {
        json->PutExtAttr("enablePaging", enablePagingStatus_ == ScrollPagingStatus::VALID, filter);
    }

    auto scrollSnapOptions = JsonUtil::Create(true);
    if (IsSnapToInterval()) {
        scrollSnapOptions->Put("snapPagination", intervalSize_.ToString().c_str());
    } else {
        auto snapPaginationArr = JsonUtil::CreateArray(true);
        auto iter = snapPaginations_.begin();
        for (auto i = 0; iter != snapPaginations_.end(); ++iter, ++i) {
            snapPaginationArr->Put(std::to_string(i).c_str(), (*iter).ToString().c_str());
        }
        scrollSnapOptions->Put("snapPagination", snapPaginationArr);
    }
    scrollSnapOptions->Put("enableSnapToStart", enableSnapToSide_.first);
    scrollSnapOptions->Put("enableSnapToEnd", enableSnapToSide_.second);
    json->PutExtAttr("scrollSnap", scrollSnapOptions, filter);
    json->PutExtAttr("maxZoomScale", maxZoomScale_, filter);
    json->PutExtAttr("minZoomScale", minZoomScale_, filter);
    json->PutExtAttr("zoomScale", zoomScale_.value_or(1.0f), filter);
    json->PutExtAttr("enableBouncesZoom", enableBouncesZoom_, filter);
}

std::string ScrollPattern::GetScrollSnapPagination() const
{
    auto snapPaginationStr = std::string("");
    if (IsSnapToInterval()) {
        snapPaginationStr = intervalSize_.ToString();
    } else {
        snapPaginationStr.append("[");
        auto iter = snapPaginations_.begin();
        for (; iter != snapPaginations_.end(); ++iter) {
            snapPaginationStr = snapPaginationStr.append((*iter).ToString()).append(" ");
        }
        snapPaginationStr.append("]");
    }
    return snapPaginationStr;
}

void ScrollPattern::OnColorModeChange(uint32_t colorMode)
{
    Pattern::OnColorModeChange(colorMode);
    if (!SystemProperties::ConfigChangePerform()) {
        return;
    }
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    if (scrollSnapUpdate_) {
        CaleSnapOffsets(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
    }
    host->MarkDirtyNode(PROPERTY_UPDATE_RENDER);
}

bool ScrollPattern::StartSnapAnimation(SnapAnimationOptions snapAnimationOptions)
{
    auto scrollBar = GetScrollBar();
    auto scrollBarProxy = GetScrollBarProxy();
    auto fromScrollBar = snapAnimationOptions.fromScrollBar;
    if (!fromScrollBar && scrollBar && scrollBar->IsDriving()) {
        return false;
    }
    if (!fromScrollBar && scrollBarProxy && scrollBarProxy->IsScrollSnapTrigger()) {
        return false;
    }
    auto predictSnapOffset = CalcPredictSnapOffset(snapAnimationOptions.snapDelta, snapAnimationOptions.dragDistance,
        snapAnimationOptions.animationVelocity, snapAnimationOptions.snapDirection);
    if (predictSnapOffset.has_value() && !NearZero(predictSnapOffset.value(), SPRING_ACCURACY)) {
        StartScrollSnapAnimation(predictSnapOffset.value(), snapAnimationOptions.animationVelocity, fromScrollBar);
        return true;
    }
    return false;
}

void ScrollPattern::StartScrollSnapAnimation(float scrollSnapDelta, float scrollSnapVelocity, bool fromScrollBar)
{
    auto scrollableEvent = GetScrollableEvent();
    CHECK_NULL_VOID(scrollableEvent);
    auto scrollable = scrollableEvent->GetScrollable();
    CHECK_NULL_VOID(scrollable);
    if (scrollable->IsSnapAnimationRunning()) {
        scrollable->UpdateScrollSnapEndWithOffset(
            -(scrollSnapDelta + scrollable->GetCurrentPos() - scrollable->GetSnapFinalPosition()));
    } else {
        scrollable->StartScrollSnapAnimation(scrollSnapDelta, scrollSnapVelocity, fromScrollBar);
        if (!IsScrolling()) {
            FireOnScrollStart();
        }
    }
}

void ScrollPattern::GetScrollPagingStatusDumpInfo(std::unique_ptr<JsonValue>& json)
{
    switch (enablePagingStatus_) {
        case ScrollPagingStatus::NONE: {
            json->Put("enablePaging", "ScrollPagingStatus::NONE");
            break;
        }
        case ScrollPagingStatus::INVALID: {
            json->Put("enablePaging", "ScrollPagingStatus::INVALID");
            break;
        }
        case ScrollPagingStatus::VALID: {
            json->Put("enablePaging", "ScrollPagingStatus::VALID");
            break;
        }
        default: {
            break;
        }
    }
}

void ScrollPattern::GetScrollSnapAlignDumpInfo(std::unique_ptr<JsonValue>& json)
{
    switch (GetScrollSnapAlign()) {
        case ScrollSnapAlign::NONE: {
            json->Put("snapAlign", "ScrollSnapAlign::NONE");
            break;
        }
        case ScrollSnapAlign::START: {
            json->Put("snapAlign", "ScrollSnapAlign::START");
            break;
        }
        case ScrollSnapAlign::CENTER: {
            json->Put("snapAlign", "ScrollSnapAlign::CENTER");
            break;
        }
        case ScrollSnapAlign::END: {
            json->Put("snapAlign", "ScrollSnapAlign::END");
            break;
        }
        default: {
            break;
        }
    }
}

void ScrollPattern::DumpAdvanceInfo(std::unique_ptr<JsonValue>& json)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto hub = host->GetEventHub<ScrollEventHub>();
    CHECK_NULL_VOID(hub);
    ScrollablePattern::DumpAdvanceInfo(json);
    json->Put("currentOffset", std::to_string(currentOffset_).c_str());

    GetScrollSnapAlignDumpInfo(json);
    auto snapPaginationStr = std::string("snapPagination: ");
    json->Put("snapPagination", GetScrollSnapPagination().c_str());
    json->Put("enableSnapToStart", enableSnapToSide_.first ? "true" : "false");
    json->Put("enableSnapToEnd", enableSnapToSide_.second ? "true" : "false");

    GetScrollPagingStatusDumpInfo(json);
    std::string snapOffsetsStr = "";
    for (const auto& iter : snapPaginations_) {
        snapOffsetsStr = snapOffsetsStr.append(iter.ToString()).append(" ");
    }
    json->Put("snapOffsets", snapOffsetsStr.c_str());
    json->Put("initialOffset",
        initialOffset_.has_value() ? initialOffset_->GetMainOffset(GetAxis()).ToString().c_str() : "None");
    auto onScrollEdge = hub->GetScrollEdgeEvent();
    json->Put("hasOnScrollEdge", onScrollEdge ? "true" : "false");

    std::unique_ptr<JsonValue> children = JsonUtil::CreateArray(true);
    for (const auto& info : scrollLayoutInfos_) {
        std::unique_ptr<JsonValue> child = JsonUtil::Create(true);
        info.ToJson(child);
        children->Put(child);
    }
    json->Put("scrollLayoutInfos", children);
    std::unique_ptr<JsonValue> infochildren = JsonUtil::CreateArray(true);
    for (const auto& info : scrollMeasureInfos_) {
        std::unique_ptr<JsonValue> child = JsonUtil::Create(true);
        info.ToJson(child);
        infochildren->Put(child);
    }
    json->Put("scrollMeasureInfos", infochildren);
}

void ScrollPattern::ProcessZoomScale()
{
    if (childScale_ != zoomScale_) {
        if (childScale_.value_or(1.0f) != zoomScale_.value_or(1.0f)) {
            auto hub = GetEventHub<ScrollEventHub>();
            if (hub) {
                hub->FireOnDidZoom(zoomScale_.value_or(1.0f));
            }
        }
        childScale_ = zoomScale_;
        SetChildScale(childScale_);
    }
}

void ScrollPattern::SetMaxZoomScale(float scale)
{
    if (scale > 0) {
        maxZoomScale_ = scale;
    } else {
        maxZoomScale_ = 1.0f;
    }
}

float ScrollPattern::GetMaxZoomScale() const
{
    return maxZoomScale_;
}

void ScrollPattern::SetMinZoomScale(float scale)
{
    if (scale > 0) {
        minZoomScale_ = scale;
    } else {
        minZoomScale_ = 1.0f;
    }
}

float ScrollPattern::GetMinZoomScale() const
{
    return minZoomScale_;
}

void ScrollPattern::SetZoomScale(std::optional<float> scale)
{
    if (scale.has_value() && scale.value() <= 0.0f) {
        scale = 1.0f;
    }
    if (scale != zoomScale_) {
        zoomScale_ = scale;
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        auto prop = host->GetLayoutProperty();
        CHECK_NULL_VOID(prop);
        prop->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE_SELF);
    }
}

float ScrollPattern::GetZoomScale() const
{
    return zoomScale_.value_or(1.0f);
}

void ScrollPattern::UpdateZoomScale(float scale)
{
    if (scale <= 0.f) {
        scale = 1.f;
    }
    if (!zoomScale_.has_value() || scale != zoomScale_.value()) {
        zoomScale_ = scale;
        auto host = GetHost();
        CHECK_NULL_VOID(host);
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
        auto eventHub = host->GetEventHub<ScrollEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FireOnZoomScaleChange(scale);
    }
}

void ScrollPattern::SetEnableBouncesZoom(bool enable)
{
    enableBouncesZoom_ = enable;
}

bool ScrollPattern::GetEnableBouncesZoom() const
{
    return enableBouncesZoom_;
}

void ScrollPattern::SetChildScale(std::optional<float> scale)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto child = AceType::DynamicCast<FrameNode>(host->GetChildByIndex(0));
    CHECK_NULL_VOID(child);
    auto renderContext = child->GetRenderContext();
    if (scale.has_value()) {
        renderContext->SetScrollScale(scale.value());
    } else {
        renderContext->ResetScrollScale();
    }
}

void ScrollPattern::UpdatePinchGesture()
{
    if (!zoomCtrl_ && (maxZoomScale_ != 1.0f || minZoomScale_ != 1.0f)) {
        zoomCtrl_ = MakeRefPtr<ZoomController>(*this);
    } else if (zoomCtrl_ && maxZoomScale_ == 1.0f && minZoomScale_ == 1.0f) {
        zoomCtrl_.Reset();
    }
}

SizeF ScrollPattern::GetChildrenExpandedSize()
{
    auto axis = GetAxis();
    if (axis == Axis::VERTICAL) {
        return SizeF(viewPort_.Width(), viewPortExtent_.Height());
    } else if (axis == Axis::HORIZONTAL) {
        return SizeF(viewPortExtent_.Width(), viewPort_.Height());
    } else if (axis == Axis::FREE) {
        return SizeF(viewPortExtent_.Width(), viewPortExtent_.Height());
    }
    return SizeF();
}

void ScrollPattern::TriggerScrollBarDisplay()
{
    auto scrollBar = GetScrollBar();
    CHECK_NULL_VOID(scrollBar);
    scrollBar->PlayScrollBarAppearAnimation();
    scrollBar->ScheduleDisappearDelayTask();
}

Offset ScrollPattern::GetFreeScrollOffset() const
{
    if (freeScroll_) {
        auto&& res = freeScroll_->GetLayoutOffset();
        return { Dimension(-res.GetX()).ConvertToVp(), Dimension(-res.GetY()).ConvertToVp() };
    }
    return {};
}

RefPtr<NGGestureRecognizer> ScrollPattern::GetOverrideRecognizer()
{
    if (!freeScroll_) {
        return nullptr;
    }
    if (!zoomCtrl_) {
        return freeScroll_->GetFreePanGesture();
    }
    auto pan = freeScroll_->GetFreePanGesture();
    auto pinch = zoomCtrl_->GetPinchGesture();
    if (!gestureGroup_) {
        std::vector<RefPtr<NGGestureRecognizer>> recognizers = { pan, pinch };
        gestureGroup_ = MakeRefPtr<ParallelRecognizer>(recognizers);
    } else if (gestureGroup_->GetGroupRecognizer().empty()) {
        std::list<RefPtr<NGGestureRecognizer>> recognizers = { pan, pinch };
        gestureGroup_->AddChildren(recognizers);
    }
    return gestureGroup_;
}

bool ScrollPattern::FreeScrollBy(const OffsetF& delta)
{
    CHECK_NULL_RETURN(freeScroll_, false);
    freeScroll_->UpdateOffset(delta);
    return true;
}
bool ScrollPattern::FreeScrollPage(bool reverse, bool smooth)
{
    CHECK_NULL_RETURN(freeScroll_, false);
    const float dy = reverse ? viewSize_.Height() : -viewSize_.Height();
    if (smooth) {
        freeScroll_->ScrollTo(freeScroll_->GetOffset() + OffsetF { 0, dy }, std::nullopt);
    } else {
        freeScroll_->UpdateOffset({ 0, dy });
    }
    return true;
}
bool ScrollPattern::FreeScrollToEdge(ScrollEdgeType type, bool smooth, std::optional<float> velocity)
{
    CHECK_NULL_RETURN(freeScroll_, false);
    auto pos = freeScroll_->GetOffset();
    switch (type) {
        case ScrollEdgeType::SCROLL_LEFT:
            pos.SetX(0.0f);
            break;
        case ScrollEdgeType::SCROLL_RIGHT:
            pos.SetX(-FLT_MAX);
            break;
        case ScrollEdgeType::SCROLL_TOP:
            pos.SetY(0.0f);
            break;
        case ScrollEdgeType::SCROLL_BOTTOM:
            pos.SetY(-FLT_MAX);
            break;
        default:
            break;
    }
    if (smooth) {
        if (velocity) {
            constexpr float VELOCITY_TO_SPRING_RATIO = 100.0f;
            *velocity /= VELOCITY_TO_SPRING_RATIO; // Adjust velocity for smooth scrolling
        }
        freeScroll_->ScrollTo(pos, velocity);
    } else {
        freeScroll_->SetOffset(pos);
    }
    return true;
}
void ScrollPattern::FreeScrollTo(const ScrollControllerBase::ScrollToParam& param)
{
    CHECK_NULL_VOID(freeScroll_);
    if (param.xOffset.Unit() == DimensionUnit::PERCENT || param.yOffset.Unit() == DimensionUnit::PERCENT) {
        TAG_LOGE(AceLogTag::ACE_SCROLL, "FreeScrollTo does not support percent offset.");
        return;
    }
    OffsetF pos { -static_cast<float>(param.xOffset.ConvertToPx()), -static_cast<float>(param.yOffset.ConvertToPx()) };
    if (param.smooth) {
        freeScroll_->ScrollTo(pos, std::nullopt, param.duration, param.curve, param.canOverScroll);
    } else {
        freeScroll_->SetOffset(pos, param.canOverScroll);
    }
}

TwoDimensionScrollResult ScrollPattern::FireObserverTwoDimensionOnWillScroll(Dimension xOffset, Dimension yOffset,
    ScrollState state, ScrollSource source)
{
    TwoDimensionScrollResult result = { .xOffset = xOffset, .yOffset = yOffset };
    CHECK_NULL_RETURN(positionController_, result);
    auto obsMgr = positionController_->GetObserverManager();
    CHECK_NULL_RETURN(obsMgr, result);
    ScrollFrameResult xResult = { .offset = xOffset };
    ScrollFrameResult yResult = { .offset = yOffset };
    obsMgr->HandleTwoDimensionOnWillScrollEvent(xResult, yResult, state, source);
    result.xOffset = xResult.offset;
    result.yOffset = yResult.offset;
    return result;
}
} // namespace OHOS::Ace::NG
