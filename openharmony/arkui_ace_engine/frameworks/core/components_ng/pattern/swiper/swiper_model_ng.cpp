/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/swiper/swiper_model_ng.h"

#include <cstdint>
#include <functional>
#include <memory>

#include "base/error/error_code.h"
#include "base/geometry/axis.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/components/swiper/swiper_component.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/swiper/arc_swiper_pattern.h"
#include "core/components_ng/pattern/swiper/swiper_pattern.h"
#include "core/components_ng/pattern/swiper/swiper_node.h"
#include "core/components_ng/pattern/swiper_indicator/indicator_common/swiper_indicator_utils.h"

namespace OHOS::Ace::NG {
typedef enum {
    ARKUI_SWIPER_ARROW_HIDE = 0,
    ARKUI_SWIPER_ARROW_SHOW,
    ARKUI_SWIPER_ARROW_SHOW_ON_HOVER,
} SwiperArrow;

RefPtr<SwiperController> SwiperModelNG::Create(bool isCreateArc)
{
    auto* stack = ViewStackProcessor::GetInstance();
    CHECK_NULL_RETURN(stack, nullptr);
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d]", V2::SWIPER_ETS_TAG, nodeId);
    RefPtr<FrameNode> swiperNode = nullptr;
    if (isCreateArc) {
        swiperNode = FrameNode::GetOrCreateFrameNode(
            V2::SWIPER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<ArcSwiperPattern>(); });
    } else {
        swiperNode = FrameNode::GetOrCreateFrameNode(
            V2::SWIPER_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<SwiperPattern>(); });
    }

    stack->Push(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    return pattern->GetSwiperController();
}

RefPtr<FrameNode> SwiperModelNG::CreateFrameNode(int32_t nodeId)
{
    auto swiperNode = ElementRegister::GetInstance()->GetSpecificItemById<SwiperNode>(nodeId);
    if (swiperNode) {
        if (swiperNode->GetTag() == V2::SWIPER_ETS_TAG) {
            return swiperNode;
        }
        ElementRegister::GetInstance()->RemoveItemSilently(nodeId);
        auto parent = swiperNode->GetParent();
        if (parent) {
            parent->RemoveChild(swiperNode);
        }
    }
    // adapt for capi
    swiperNode = AceType::MakeRefPtr<SwiperNode>(V2::SWIPER_ETS_TAG, nodeId, AceType::MakeRefPtr<SwiperPattern>());
    swiperNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(swiperNode);
    return swiperNode;
}

void SwiperModelNG::SetDirection(Axis axis)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, Direction, axis);
}

void SwiperModelNG::SetIndex(uint32_t index)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, Index, index);
}

void SwiperModelNG::SetDisplayMode(SwiperDisplayMode displayMode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayMode, displayMode);
}

void SwiperModelNG::SetDisplayCount(int32_t displayCount)
{
    if (displayCount <= 0) {
        return;
    }

    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayCount, displayCount);
}

void SwiperModelNG::ResetDisplayCount()
{
    ACE_RESET_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayCount);
}

void SwiperModelNG::SetMinSize(const Dimension& minSize)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, MinSize, minSize);
}

void SwiperModelNG::SetShowIndicator(bool showIndicator)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, ShowIndicator, showIndicator);
}

void SwiperModelNG::SetIndicatorType(SwiperIndicatorType indicatorType)
{
    SwiperIndicatorUtils::SetSwiperIndicatorType(indicatorType);
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, IndicatorType, indicatorType);
}

void SwiperModelNG::SetItemSpace(const Dimension& itemSpace)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, ItemSpace, itemSpace);
}

void SwiperModelNG::SetCachedCount(int32_t cachedCount)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetCachedCount(cachedCount);

    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, CachedCount, cachedCount);
}

void SwiperModelNG::SetCachedIsShown(bool isShown)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, CachedIsShown, isShown);
}

void SwiperModelNG::SetCachedIsShown(FrameNode* frameNode, bool isShown)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, CachedIsShown, isShown, frameNode);
}

void SwiperModelNG::SetIsIndicatorCustomSize(bool isCustomSize)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIsIndicatorCustomSize(isCustomSize);
}

void SwiperModelNG::SetIndicatorInteractive(bool interactive)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIndicatorInteractive(interactive);
}

void SwiperModelNG::SetAutoPlay(bool autoPlay)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, AutoPlay, autoPlay);
}

void SwiperModelNG::SetAutoPlayInterval(uint32_t interval)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, AutoPlayInterval, interval);
}

void SwiperModelNG::SetDuration(uint32_t duration)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, Duration, duration);
}

void SwiperModelNG::SetLoop(bool loop)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, Loop, loop);
}

void SwiperModelNG::SetEnabled(bool enabled)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, Enabled, enabled);
}

void SwiperModelNG::SetDisableSwipe(bool disableSwipe)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisableSwipe, disableSwipe);
}

void SwiperModelNG::SetEdgeEffect(EdgeEffect edgeEffect)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, EdgeEffect, edgeEffect);
}

void SwiperModelNG::SetCurve(const RefPtr<Curve>& curve)
{
    ACE_UPDATE_PAINT_PROPERTY(SwiperPaintProperty, Curve, curve);
}

void SwiperModelNG::SetOnChange(std::function<void(const BaseEventInfo* info)>&& onChange)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);

    pattern->UpdateChangeEvent([event = std::move(onChange)](int32_t index) {
        CHECK_NULL_VOID(event);
        SwiperChangeEvent eventInfo(index);
        event(&eventInfo);
    });
}

void SwiperModelNG::SetOnAnimationStart(AnimationStartEvent&& onAnimationStart)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);

    pattern->UpdateAnimationStartEvent(
        [event = std::move(onAnimationStart)](int32_t index, int32_t targetIndex, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, targetIndex, info);
        }
    );
}

void SwiperModelNG::SetOnAnimationEnd(AnimationEndEvent&& onAnimationEnd)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);

    pattern->UpdateAnimationEndEvent(
        [event = std::move(onAnimationEnd)](int32_t index, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, info);
        }
    );
}

void SwiperModelNG::SetOnGestureSwipe(GestureSwipeEvent&& onGestureSwipe)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto eventHub = swiperNode->GetEventHub<SwiperEventHub>();
    CHECK_NULL_VOID(eventHub);

    eventHub->SetGestureSwipeEvent(
        [event = std::move(onGestureSwipe)](int32_t index, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, info);
        }
    );
}

void SwiperModelNG::SetNestedScroll(FrameNode* frameNode, const int32_t nestedOpt)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    NestedScrollOptions option;
    option.forward = static_cast<NestedScrollMode>(nestedOpt);
    option.backward = static_cast<NestedScrollMode>(nestedOpt);
    pattern->SetNestedScroll(option);
}

void SwiperModelNG::SetSwipeByGroup(FrameNode* frameNode, bool swipeByGroup)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, SwipeByGroup, swipeByGroup, frameNode);
}

void SwiperModelNG::SetRemoteMessageEventId(RemoteCallback&& remoteCallback) {}

void SwiperModelNG::SetIndicatorStyle(const SwiperParameters& swiperParameters)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperParameters(swiperParameters);
};

void SwiperModelNG::SetArcDotIndicatorStyle(const SwiperArcDotParameters& swiperArcDotParameters)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperArcDotParameters(swiperArcDotParameters);
}

void SwiperModelNG::SetDotIndicatorStyle(const SwiperParameters& swiperParameters)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperParameters(swiperParameters);
};

void SwiperModelNG::SetArcDotIndicatorStyle(FrameNode* frameNode, const SwiperArcDotParameters& swiperArcDotParameters)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperArcDotParameters(swiperArcDotParameters);
}

void SwiperModelNG::SetDigitIndicatorStyle(const SwiperDigitalParameters& swiperDigitalParameters)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperDigitalParameters(swiperDigitalParameters);
};

void SwiperModelNG::SetOnClick(
    std::function<void(const BaseEventInfo* info, const RefPtr<V2::InspectorFunctionImpl>& impl)>&& value)
{}

void SwiperModelNG::SetMainSwiperSizeWidth() {}

void SwiperModelNG::SetMainSwiperSizeHeight() {}

void SwiperModelNG::SetPreviousMargin(const Dimension& prevMargin, bool ignoreBlank)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, PrevMargin, prevMargin);
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, PrevMarginIgnoreBlank, ignoreBlank);
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetPrevMarginIgnoreBlank(ignoreBlank);
}

void SwiperModelNG::SetNextMargin(const Dimension& nextMargin, bool ignoreBlank)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, NextMargin, nextMargin);
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, NextMarginIgnoreBlank, ignoreBlank);
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetNextMarginIgnoreBlank(ignoreBlank);
}

void SwiperModelNG::SetIndicatorInteractive(FrameNode* frameNode, bool interactive)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIndicatorInteractive(interactive);
}

void SwiperModelNG::SetOnChangeEvent(std::function<void(const BaseEventInfo* info)>&& onChangeEvent)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);

    pattern->UpdateOnChangeEvent([event = std::move(onChangeEvent)](int32_t index) {
        CHECK_NULL_VOID(event);
        SwiperChangeEvent eventInfo(index);
        event(&eventInfo);
    });
}

void SwiperModelNG::SetIndicatorIsBoolean(bool isBoolean)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIndicatorIsBoolean(isBoolean);
}

void SwiperModelNG::SetAutoPlayOptions(const SwiperAutoPlayOptions& swiperAutoPlayOptions)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetStopWhenTouched(swiperAutoPlayOptions.stopWhenTouched);
}

SwiperAutoPlayOptions SwiperModelNG::GetAutoPlayOptions(FrameNode* frameNode)
{
    SwiperAutoPlayOptions swiperAutoPlayOptions;
    CHECK_NULL_RETURN(frameNode, swiperAutoPlayOptions);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, swiperAutoPlayOptions);
    swiperAutoPlayOptions.stopWhenTouched = pattern->IsStopWhenTouched();
    return swiperAutoPlayOptions;
}

void SwiperModelNG::SetArrowStyle(const SwiperArrowParameters& swiperArrowParameters)
{
    if (swiperArrowParameters.isShowBackground.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, IsShowBackground, swiperArrowParameters.isShowBackground.value());
    }
    if (swiperArrowParameters.backgroundSize.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, BackgroundSize, swiperArrowParameters.backgroundSize.value());
    }
    if (swiperArrowParameters.backgroundColor.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, BackgroundColor, swiperArrowParameters.backgroundColor.value());
    }
    if (swiperArrowParameters.arrowSize.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, ArrowSize, swiperArrowParameters.arrowSize.value());
    }
    if (swiperArrowParameters.arrowColor.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, ArrowColor, swiperArrowParameters.arrowColor.value());
    }
    if (swiperArrowParameters.isSidebarMiddle.has_value()) {
        ACE_UPDATE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, IsSidebarMiddle, swiperArrowParameters.isSidebarMiddle.value());
    }
}

void SwiperModelNG::SetDisplayArrow(bool displayArrow)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayArrow, displayArrow);
}

void SwiperModelNG::SetHoverShow(bool hoverShow)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, HoverShow, hoverShow);
}

void SwiperModelNG::SetNestedScroll(const NestedScrollOptions& nestedOpt)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetNestedScroll(nestedOpt);
}

void SwiperModelNG::SetSwipeByGroup(bool swipeByGroup)
{
    ACE_UPDATE_LAYOUT_PROPERTY(SwiperLayoutProperty, SwipeByGroup, swipeByGroup);
}

void SwiperModelNG::SetCustomContentTransition(SwiperContentAnimatedTransition& transition)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperCustomContentTransition(transition);
}

void SwiperModelNG::SetCustomContentTransition(FrameNode* frameNode, SwiperContentAnimatedTransition& transition)
{
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperCustomContentTransition(transition);
}

void SwiperModelNG::SetOnContentDidScroll(ContentDidScrollEvent&& onContentDidScroll)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetOnContentDidScroll(std::move(onContentDidScroll));
}

void SwiperModelNG::SetOnContentDidScroll(FrameNode* frameNode, ContentDidScrollEvent&& onContentDidScroll)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetOnContentDidScroll(std::move(onContentDidScroll));
}

void SwiperModelNG::SetPageFlipMode(int32_t pageFlipMode)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetPageFlipMode(pageFlipMode);
}

void SwiperModelNG::SetPageFlipMode(FrameNode* frameNode, int32_t options)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetPageFlipMode(options);
}

int32_t SwiperModelNG::GetPageFlipMode(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, 0);
    return pattern->GetPageFlipMode();
}

void SwiperModelNG::SetDisableTransitionAnimation(bool isDisable)
{
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetDisableTransitionAnimation(isDisable);
}

void SwiperModelNG::SetDigitalCrownSensitivity(int32_t sensitivity)
{
#ifdef SUPPORT_DIGITAL_CROWN
    auto swiperNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(swiperNode);
    auto pattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetDigitalCrownSensitivity(static_cast<CrownSensitivity>(sensitivity));
#endif
}

void SwiperModelNG::SetNextMargin(FrameNode* frameNode, const Dimension& nextMargin, bool ignoreBlank)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, NextMargin, nextMargin, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, NextMarginIgnoreBlank, ignoreBlank, frameNode);
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetNextMarginIgnoreBlank(ignoreBlank);
}

void SwiperModelNG::SetPreviousMargin(FrameNode* frameNode, const Dimension& prevMargin, bool ignoreBlank)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, PrevMargin, prevMargin, frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, PrevMarginIgnoreBlank, ignoreBlank, frameNode);
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetPrevMarginIgnoreBlank(ignoreBlank);
}

void SwiperModelNG::SetIndex(FrameNode* frameNode, uint32_t index)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, Index, index, frameNode);
}

void SwiperModelNG::SetAutoPlayInterval(FrameNode* frameNode, uint32_t interval)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, AutoPlayInterval, interval, frameNode);
}

void SwiperModelNG::SetDuration(FrameNode* frameNode, uint32_t duration)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, Duration, duration, frameNode);
}

void SwiperModelNG::SetCachedCount(FrameNode* frameNode, int32_t cachedCount)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetCachedCount(cachedCount);

    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, CachedCount, cachedCount, frameNode);
}

int32_t SwiperModelNG::GetCachedCount(FrameNode* frameNode)
{
    int32_t cachedCount = 1;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, CachedCount, cachedCount, frameNode, 1);
    return cachedCount;
}

bool SwiperModelNG::GetCachedIsShown(FrameNode* frameNode)
{
    bool isShown = false;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, CachedIsShown, isShown, frameNode, false);
    return isShown;
}

void SwiperModelNG::SetAutoPlay(FrameNode* frameNode, bool autoPlay)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, AutoPlay, autoPlay, frameNode);
}

void SwiperModelNG::SetLoop(FrameNode* frameNode, bool loop)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, Loop, loop, frameNode);
}

void SwiperModelNG::SetDirection(FrameNode* frameNode, Axis axis)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, Direction, axis, frameNode);
}

void SwiperModelNG::SetDisableSwipe(FrameNode* frameNode, bool disableSwipe)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisableSwipe, disableSwipe, frameNode);
}

void SwiperModelNG::SetItemSpace(FrameNode* frameNode, const Dimension& itemSpace)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, ItemSpace, itemSpace, frameNode);
}

void SwiperModelNG::SetDisplayMode(FrameNode* frameNode, SwiperDisplayMode displayMode)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayMode, displayMode, frameNode);
}

void SwiperModelNG::SetEdgeEffect(FrameNode* frameNode, EdgeEffect edgeEffect)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, EdgeEffect, edgeEffect, frameNode);
}

void SwiperModelNG::SetMinSize(FrameNode* frameNode, const Dimension& minSize)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, MinSize, minSize, frameNode);
}

void SwiperModelNG::SetDisplayCount(FrameNode* frameNode, int32_t displayCount)
{
    if (displayCount <= 0) {
        return;
    }

    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayCount, displayCount, frameNode);
}

void SwiperModelNG::ResetDisplayCount(FrameNode* frameNode)
{
    ACE_RESET_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayCount, frameNode);
}

void SwiperModelNG::SetCurve(FrameNode* frameNode, const RefPtr<Curve>& curve)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, Curve, curve, frameNode);
}

void SwiperModelNG::SetAutoPlayOptions(FrameNode* frameNode, const SwiperAutoPlayOptions& swiperAutoPlayOptions)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetStopWhenTouched(swiperAutoPlayOptions.stopWhenTouched);
}

void SwiperModelNG::SetArrowStyle(FrameNode* frameNode, const SwiperArrowParameters& swiperArrowParameters)
{
    if (swiperArrowParameters.isShowBackground.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, IsShowBackground, swiperArrowParameters.isShowBackground.value(), frameNode);
    }
    if (swiperArrowParameters.backgroundSize.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, BackgroundSize, swiperArrowParameters.backgroundSize.value(), frameNode);
    }
    if (swiperArrowParameters.backgroundColor.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, BackgroundColor, swiperArrowParameters.backgroundColor.value(), frameNode);
    }
    if (swiperArrowParameters.arrowSize.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, ArrowSize, swiperArrowParameters.arrowSize.value(), frameNode);
    }
    if (swiperArrowParameters.arrowColor.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, ArrowColor, swiperArrowParameters.arrowColor.value(), frameNode);
    }
    if (swiperArrowParameters.isSidebarMiddle.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(
            SwiperLayoutProperty, IsSidebarMiddle, swiperArrowParameters.isSidebarMiddle.value(), frameNode);
    }
}

void SwiperModelNG::SetDisplayArrow(FrameNode* frameNode, bool displayArrow)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, DisplayArrow, displayArrow, frameNode);
}

void SwiperModelNG::SetHoverShow(FrameNode* frameNode, bool hoverShow)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, HoverShow, hoverShow, frameNode);
}

void SwiperModelNG::SetShowIndicator(FrameNode* frameNode, bool showIndicator)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, ShowIndicator, showIndicator, frameNode);
}

void SwiperModelNG::SetIndicatorIsBoolean(FrameNode* frameNode, bool isBoolean)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIndicatorIsBoolean(isBoolean);
}

void SwiperModelNG::SetDigitIndicatorStyle(FrameNode* frameNode, const SwiperDigitalParameters& swiperDigitalParameters)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperDigitalParameters(swiperDigitalParameters);
}

void SwiperModelNG::SetIndicatorType(FrameNode* frameNode, SwiperIndicatorType indicatorType)
{
    SwiperIndicatorUtils::SetSwiperIndicatorType(indicatorType);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(SwiperLayoutProperty, IndicatorType, indicatorType, frameNode);
}

void SwiperModelNG::SetIsIndicatorCustomSize(FrameNode* frameNode, bool isCustomSize)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetIsIndicatorCustomSize(isCustomSize);
}

void SwiperModelNG::SetDotIndicatorStyle(FrameNode* frameNode, const SwiperParameters& swiperParameters)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetSwiperParameters(swiperParameters);
}

void SwiperModelNG::SetEnabled(FrameNode* frameNode, bool enabled)
{
    ACE_UPDATE_NODE_PAINT_PROPERTY(SwiperPaintProperty, Enabled, enabled, frameNode);
}

void SwiperModelNG::SetOnChange(FrameNode* frameNode, std::function<void(const BaseEventInfo* info)>&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->UpdateChangeEvent([event = std::move(onChange)](int32_t index) {
        CHECK_NULL_VOID(event);
        SwiperChangeEvent eventInfo(index);
        event(&eventInfo);
    });
}

void SwiperModelNG::SetOnAnimationStart(FrameNode* frameNode, AnimationStartEvent&& onAnimationStart)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->UpdateAnimationStartEvent(
        [event = std::move(onAnimationStart)](int32_t index, int32_t targetIndex, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, targetIndex, info);
        }
    );
}

void SwiperModelNG::SetOnAnimationEnd(FrameNode* frameNode, AnimationEndEvent&& onAnimationEnd)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->UpdateAnimationEndEvent(
        [event = std::move(onAnimationEnd)](int32_t index, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, info);
        }
    );
}

void SwiperModelNG::SetOnGestureSwipe(FrameNode* frameNode, GestureSwipeEvent&& onGestureSwipe)
{
    CHECK_NULL_VOID(frameNode);
    auto eventHub = frameNode->GetEventHub<SwiperEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetGestureSwipeEvent(
        [event = std::move(onGestureSwipe)](int32_t index, const AnimationCallbackInfo& info) {
            CHECK_NULL_VOID(event);
            event(index, info);
        }
    );
}

bool SwiperModelNG::GetLoop(FrameNode* frameNode)
{
    bool value = true;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, Loop, value, frameNode, value);
    return value;
}

bool SwiperModelNG::GetAutoPlay(FrameNode* frameNode)
{
    bool value = false;
    ACE_GET_NODE_PAINT_PROPERTY_WITH_DEFAULT_VALUE(SwiperPaintProperty, AutoPlay, value, frameNode, value);
    return value;
}

int SwiperModelNG::GetIndex(FrameNode* frameNode)
{
    int value = 0;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, Index, value, frameNode, value);
    return value;
}

Axis SwiperModelNG::GetDirection(FrameNode* frameNode)
{
    Axis value = Axis::HORIZONTAL;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, Direction, value, frameNode, value);
    return value;
}

uint32_t SwiperModelNG::GetDuration(FrameNode* frameNode)
{
    uint32_t value = SwiperAnimationStyle::DEFAULT_DURATION;
    ACE_GET_NODE_PAINT_PROPERTY_WITH_DEFAULT_VALUE(SwiperPaintProperty, Duration, value, frameNode, value);
    return value;
}

int SwiperModelNG::GetDisplayCount(FrameNode* frameNode)
{
    int value = 1;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, DisplayCount, value, frameNode, value);
    return value;
}

int SwiperModelNG::GetAutoPlayInterval(FrameNode* frameNode)
{
    int value = SwiperAnimationStyle::DEFAULT_INTERVAL;
    ACE_GET_NODE_PAINT_PROPERTY_WITH_DEFAULT_VALUE(SwiperPaintProperty, AutoPlayInterval, value, frameNode, value);
    return value;
}

RefPtr<Curve> SwiperModelNG::GetCurve(FrameNode* frameNode)
{
    RefPtr<Curve> value = OHOS::Ace::Curves::LINEAR;
    ACE_GET_NODE_PAINT_PROPERTY_WITH_DEFAULT_VALUE(SwiperPaintProperty, Curve, value, frameNode, value);
    return value;
}

bool SwiperModelNG::GetDisableSwipe(FrameNode* frameNode)
{
    bool value = false;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, DisableSwipe, value, frameNode, value);
    return value;
}

float SwiperModelNG::GetItemSpace(FrameNode* frameNode)
{
    Dimension value(0, DimensionUnit::VP);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, ItemSpace, value, frameNode, value);
    return value.Value();
}

bool SwiperModelNG::GetShowIndicator(FrameNode* frameNode)
{
    bool value = true;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, ShowIndicator, value, frameNode, value);
    return value;
}

int SwiperModelNG::GetShowDisplayArrow(FrameNode* frameNode)
{
    bool show = false;
    bool hoverShow = false;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, DisplayArrow, show, frameNode, show);
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, HoverShow, hoverShow, frameNode, hoverShow);
    int ArrowType = 0;
    if (show && hoverShow) {
        return ArrowType = static_cast<int>(SwiperArrow::ARKUI_SWIPER_ARROW_SHOW_ON_HOVER);
    }
    if (show) {
        ArrowType = static_cast<int>(SwiperArrow::ARKUI_SWIPER_ARROW_SHOW);
    } else {
        ArrowType = static_cast<int>(SwiperArrow::ARKUI_SWIPER_ARROW_HIDE);
    }
    return ArrowType;
}

EdgeEffect SwiperModelNG::GetEffectMode(FrameNode* frameNode)
{
    EdgeEffect mode = EdgeEffect::SPRING;
    ACE_GET_NODE_PAINT_PROPERTY_WITH_DEFAULT_VALUE(SwiperPaintProperty, EdgeEffect, mode, frameNode, mode);
    return mode;
}

int32_t SwiperModelNG::GetNestedScroll(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, ERROR_CODE_PARAM_INVALID);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, ERROR_CODE_PARAM_INVALID);
    return static_cast<int>(pattern->GetNestedScroll().forward);
}

int32_t SwiperModelNG::RealTotalCount(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, 0);
    return pattern->RealTotalCount();
}

void SwiperModelNG::SetSwiperToIndex(FrameNode* frameNode, int32_t index, bool useAnimation)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->ChangeIndex(index, useAnimation);
}

void SwiperModelNG::GetPreviousMargin(FrameNode* frameNode, int32_t unit, SwiperMarginOptions* options)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    Dimension prevMargin(0.0f, static_cast<DimensionUnit>(unit));
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        SwiperLayoutProperty, PrevMargin, prevMargin, frameNode, prevMargin);
    options->margin = prevMargin.Value();
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    options->ignoreBlank = pattern->GetPrevMarginIgnoreBlank();
}

void SwiperModelNG::GetNextMargin(FrameNode* frameNode, int32_t unit, SwiperMarginOptions* options)
{
    CHECK_NULL_VOID(frameNode);
    CHECK_NULL_VOID(options);
    Dimension nextMargin(0.0f, static_cast<DimensionUnit>(unit));
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(
        SwiperLayoutProperty, NextMargin, nextMargin, frameNode, nextMargin);
    options->margin = nextMargin.Value();
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(pattern);
    options->ignoreBlank = pattern->GetNextMarginIgnoreBlank();
}

std::shared_ptr<SwiperParameters> SwiperModelNG::GetDotIndicator(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    return pattern->GetSwiperParameters();
}

int32_t SwiperModelNG::GetIndicatorType(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, 0);
    SwiperIndicatorType value = SwiperIndicatorType::DOT;
    ACE_GET_NODE_LAYOUT_PROPERTY_WITH_DEFAULT_VALUE(SwiperLayoutProperty, IndicatorType, value, frameNode, value);
    return static_cast<int32_t>(value);

}

RefPtr<SwiperController> SwiperModelNG::GetOrCreateSwiperController(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    if (!pattern->GetSwiperController()) {
        auto controller = AceType::MakeRefPtr<SwiperController>();
        pattern->SetSwiperController(controller);
    }
    return pattern->GetSwiperController();
}

RefPtr<SwiperController> SwiperModelNG::GetSwiperController(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, nullptr);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, nullptr);
    return pattern->GetSwiperController();
}

bool SwiperModelNG::GetIndicatorInteractive(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto pattern = frameNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(pattern, false);
    return pattern->IsIndicatorInteractive();
}
} // namespace OHOS::Ace::NG
