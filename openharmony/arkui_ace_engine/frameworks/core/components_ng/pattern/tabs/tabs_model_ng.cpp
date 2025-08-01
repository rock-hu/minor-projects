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

#include "core/components_ng/pattern/tabs/tabs_model_ng.h"

#include <type_traits>

#include "base/log/ace_trace.h"
#include "base/memory/ace_type.h"
#include "base/memory/referenced.h"
#include "base/utils/utils.h"
#include "core/animation/animation_pub.h"
#include "core/components/common/layout/constants.h"
#include "core/components/common/properties/decoration.h"
#include "core/components/swiper/swiper_controller.h"
#include "core/components_ng/base/group_node.h"
#include "core/components_ng/base/view_stack_processor.h"
#include "core/components_ng/pattern/divider/divider_layout_property.h"
#include "core/components_ng/pattern/divider/divider_pattern.h"
#include "core/components_ng/pattern/divider/divider_render_property.h"
#include "core/components_ng/pattern/image/image_pattern.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/stack/stack_pattern.h"
#include "core/components_ng/pattern/swiper/swiper_layout_property.h"
#include "core/components_ng/pattern/swiper/swiper_paint_property.h"
#include "core/components_ng/pattern/swiper/swiper_pattern.h"
#include "core/components_ng/pattern/tabs/tab_bar_paint_property.h"
#include "core/components_ng/pattern/tabs/tab_bar_pattern.h"
#include "core/components_ng/pattern/tabs/tabs_controller.h"
#include "core/components_ng/pattern/tabs/tabs_node.h"
#include "core/components_ng/pattern/tabs/tabs_pattern.h"
#include "core/components_ng/pattern/text/text_pattern.h"
#include "core/components_ng/property/measure_utils.h"
#include "core/common/resource/resource_parse_utils.h"

namespace OHOS::Ace::NG {
namespace {
constexpr uint16_t PIXEL_ROUND = static_cast<uint16_t>(PixelRoundPolicy::NO_FORCE_ROUND_START) |
                                static_cast<uint16_t>(PixelRoundPolicy::NO_FORCE_ROUND_TOP) |
                                static_cast<uint16_t>(PixelRoundPolicy::NO_FORCE_ROUND_END) |
                                static_cast<uint16_t>(PixelRoundPolicy::NO_FORCE_ROUND_BOTTOM);

constexpr int32_t SWIPER_Z_INDEX = 0;
constexpr int32_t DIVIDER_Z_INDEX = 2;
constexpr int32_t TAB_BAR_Z_INDEX = 3;
constexpr int32_t EFFECT_Z_INDEX = 1;
} // namespace

void TabsModelNG::Create(BarPosition barPosition, int32_t index, const RefPtr<TabController>& /*tabController*/,
    const RefPtr<SwiperController>& swiperController)
{
    auto* stack = ViewStackProcessor::GetInstance();
    auto nodeId = stack->ClaimNodeId();
    ACE_LAYOUT_SCOPED_TRACE("Create[%s][self:%d][index:%d]", V2::TABS_ETS_TAG, nodeId, index);
    auto tabsNode = GetOrCreateTabsNode(V2::TABS_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TabsPattern>(); });
    InitTabsNode(tabsNode, swiperController);
    ViewStackProcessor::GetInstance()->Push(tabsNode);

    SetTabBarPosition(barPosition);
    auto tabsLayoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(tabsLayoutProperty);
    if (tabsLayoutProperty->GetIndex().has_value()) {
        auto preIndex = tabsLayoutProperty->GetIndex().value();
        if (preIndex == index || index < 0) {
            return;
        }
    }
    tabsLayoutProperty->UpdateIndexSetByUser(index);
    if (SystemProperties::ConfigChangePerform()) {
        tabsLayoutProperty->ResetDividerColorSetByUser();
        tabsLayoutProperty->ResetBarBackgroundColorSetByUser();
    }
}

RefPtr<SwiperController> TabsModelNG::GetSwiperController(const RefPtr<FrameNode>& swiperNode,
    const RefPtr<SwiperController>& swiperController)
{
    auto swiperPaintProperty = swiperNode->GetPaintProperty<SwiperPaintProperty>();
    swiperPaintProperty->UpdateEdgeEffect(EdgeEffect::SPRING);
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_RETURN(pipelineContext, nullptr);
    auto tabTheme = pipelineContext->GetTheme<TabTheme>();
    CHECK_NULL_RETURN(tabTheme, nullptr);
    swiperPaintProperty->UpdateDuration(tabTheme->GetTabContentAnimationDuration());
    swiperPaintProperty->UpdateCurve(TabBarPhysicalCurve);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    swiperLayoutProperty->UpdateLoop(false);
    swiperLayoutProperty->UpdateCachedCount(0);
    swiperLayoutProperty->UpdateShowIndicator(false);
    swiperLayoutProperty->UpdateSafeAreaExpandOpts(
        { .type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_TOP + SAFE_AREA_EDGE_BOTTOM });
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_RETURN(swiperPattern, nullptr);
    RefPtr<SwiperController> controller;
    if (swiperController) {
        controller = swiperController;
    } else {
        controller = AceType::MakeRefPtr<TabsControllerNG>();
    }
    swiperPattern->SetSwiperController(controller);
    swiperPattern->SetFinishCallbackType(FinishCallbackType::LOGICALLY);
    swiperPattern->SetHasTabsAncestor(true);
    return controller;
}

void TabsModelNG::InitSelectedMaskNode(const RefPtr<FrameNode>& selectedMaskNode)
{
    auto selectedImageNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ImagePattern>(); });
    selectedImageNode->MountToParent(selectedMaskNode);
    auto selectedMaskRenderContext = selectedMaskNode->GetRenderContext();
    auto selectedMaskProperty = selectedMaskNode->GetLayoutProperty<LinearLayoutProperty>();
    selectedMaskProperty->UpdateCrossAxisAlign(FlexAlign::FLEX_START);
    selectedMaskRenderContext->SetClipToBounds(true);
}

void TabsModelNG::InitUnselectedMaskNode(const RefPtr<FrameNode>& unselectedMaskNode)
{
    auto unselectedImageNode = FrameNode::GetOrCreateFrameNode(V2::IMAGE_ETS_TAG,
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<ImagePattern>(); });
    unselectedImageNode->MountToParent(unselectedMaskNode);
    auto unselectedMaskRenderContext = unselectedMaskNode->GetRenderContext();
    auto unselectedMaskProperty = unselectedMaskNode->GetLayoutProperty<LinearLayoutProperty>();
    unselectedMaskProperty->UpdateCrossAxisAlign(FlexAlign::FLEX_START);
    unselectedMaskRenderContext->SetClipToBounds(true);
}

RefPtr<OHOS::Ace::NG::FrameNode> InitEffectNode(RefPtr<TabsNode> tabsNode)
{
    auto effectNode = FrameNode::GetOrCreateFrameNode(
        V2::STACK_ETS_TAG, tabsNode->GetEffectId(), []() { return AceType::MakeRefPtr<StackPattern>(); });

    auto accessibilityProperty = effectNode->GetAccessibilityProperty<NG::AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, effectNode);
    accessibilityProperty->SetAccessibilityLevel(AccessibilityProperty::Level::NO_STR);

    auto effectNodeLayoutProperty = effectNode->GetLayoutProperty();
    CHECK_NULL_RETURN(effectNodeLayoutProperty, effectNode);
    if (!effectNodeLayoutProperty->GetSafeAreaExpandOpts()) {
        effectNodeLayoutProperty->UpdateSafeAreaExpandOpts(
            { .type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_BOTTOM });
    }
    auto effectNodeContext = effectNode->GetRenderContext();
    CHECK_NULL_RETURN(effectNodeContext, effectNode);
    effectNodeContext->UpdateClipEdge(false);
    return effectNode;
}

void TabsModelNG::InitTabsNode(RefPtr<TabsNode> tabsNode, const RefPtr<SwiperController>& swiperController)
{
    bool hasSwiperNode = tabsNode->HasSwiperNode();
    bool hasTabBarNode = tabsNode->HasTabBarNode();
    bool hasDividerNode = tabsNode->HasDividerNode();
    bool hasSelectedMaskNode = tabsNode->HasSelectedMaskNode();
    bool hasUnselectedMaskNode = tabsNode->HasUnselectedMaskNode();

    // Create Swiper node to contain TabContent.
    auto swiperNode = FrameNode::GetOrCreateFrameNode(
        V2::SWIPER_ETS_TAG, tabsNode->GetSwiperId(), []() { return AceType::MakeRefPtr<SwiperPattern>(); });
    auto dividerNode = FrameNode::GetOrCreateFrameNode(
        V2::DIVIDER_ETS_TAG, tabsNode->GetDividerId(), []() { return AceType::MakeRefPtr<DividerPattern>(); });

    // Create TabBar to contain TabBar of TabContent.
    auto tabBarNode = FrameNode::GetOrCreateFrameNode(
        V2::TAB_BAR_ETS_TAG, tabsNode->GetTabBarId(), []() { return AceType::MakeRefPtr<TabBarPattern>(); });
    if (auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>(); tabBarPattern) {
        tabBarPattern->SetController(GetSwiperController(swiperNode, swiperController));
    }

    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (tabBarLayoutProperty->GetPixelRound() == static_cast<uint16_t>(PixelRoundPolicy::ALL_FORCE_ROUND)) {
        tabBarLayoutProperty->UpdatePixelRound(PIXEL_ROUND);
    }

    auto selectedMaskNode = FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, tabsNode->GetSelectedMaskId(),
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });

    auto unselectedMaskNode = FrameNode::GetOrCreateFrameNode(V2::COLUMN_ETS_TAG, tabsNode->GetUnselectedMaskId(),
        []() { return AceType::MakeRefPtr<LinearLayoutPattern>(true); });

    if (!hasSwiperNode) {
        swiperNode->MountToParent(tabsNode);
    }
    if (!hasDividerNode) {
        dividerNode->MountToParent(tabsNode);
    }
    if (!hasTabBarNode) {
        tabBarNode->MountToParent(tabsNode);
    }
    if (!hasSelectedMaskNode) {
        selectedMaskNode->MountToParent(tabBarNode);
        InitSelectedMaskNode(selectedMaskNode);
    }
    if (!hasUnselectedMaskNode) {
        unselectedMaskNode->MountToParent(tabBarNode);
        InitUnselectedMaskNode(unselectedMaskNode);
    }
}

RefPtr<FrameNode> TabsModelNG::CreateFrameNode(int32_t nodeId)
{
    auto tabsNode = GetOrCreateTabsNode(V2::TABS_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TabsPattern>(); });
    InitTabsNode(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    tabBarNode->MarkModifyDone();
    return tabsNode;
}

void TabsModelNG::SetTabBarPosition(BarPosition tabBarPosition)
{
    SetTabBarPosition(ViewStackProcessor::GetInstance()->GetMainFrameNode(), tabBarPosition);
}

void TabsModelNG::SetBarBackgroundBlurStyle(const BlurStyleOption& styleOption)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    SetBarBackgroundBlurStyle(frameNode, styleOption);
}

void TabsModelNG::SetTabBarMode(TabBarMode tabBarMode)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, TabBarMode, tabBarMode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateTabBarMode(tabBarMode);
}

void TabsModelNG::SetTabBarWidth(const Dimension& tabBarWidth)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarWidth, tabBarWidth);
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    auto scaleProperty = ScaleProperty::CreateScaleProperty();
    auto tabBarWidthToPx =
        ConvertToPx(tabBarWidth, scaleProperty, tabBarLayoutProperty->GetLayoutConstraint()->percentReference.Width());
    if (LessNotEqual(tabBarWidthToPx.value_or(0.0), 0.0)) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(true, false);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(NG::CalcLength(tabBarWidth), std::nullopt));
    }
    tabBarLayoutProperty->UpdateTabBarWidth(tabBarWidth);
}

void TabsModelNG::SetTabBarHeight(const Dimension& tabBarHeight)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarHeight, tabBarHeight);
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    auto scaleProperty = ScaleProperty::CreateScaleProperty();
    auto tabBarHeightToPx = ConvertToPx(
        tabBarHeight, scaleProperty, tabBarLayoutProperty->GetLayoutConstraint()->percentReference.Height());
    if (LessNotEqual(tabBarHeightToPx.value_or(0.0), 0.0)) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(false, true);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, NG::CalcLength(tabBarHeight)));
    }
    tabBarLayoutProperty->UpdateTabBarHeight(tabBarHeight);
}

void TabsModelNG::SetBarModifier(std::function<void(WeakPtr<NG::FrameNode>)>&& onApply)
{
    CHECK_NULL_VOID(onApply);
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    onApply(tabBarNode);
}

void TabsModelNG::SetWidthAuto(bool isAuto)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, WidthAuto, isAuto);
}

void TabsModelNG::SetWidthAuto(FrameNode* frameNode, bool isAuto)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, WidthAuto, isAuto, frameNode);
}

void TabsModelNG::SetHeightAuto(bool isAuto)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, HeightAuto, isAuto);
}

void TabsModelNG::SetHeightAuto(FrameNode* frameNode, bool isAuto)
{
    CHECK_NULL_VOID(frameNode);
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, HeightAuto, isAuto, frameNode);
}

void TabsModelNG::SetBarAdaptiveHeight(bool barAdaptiveHeight)
{
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateBarAdaptiveHeight(barAdaptiveHeight);
}

void TabsModelNG::SetNoMinHeightLimit(bool noMinHeightLimit)
{
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateNoMinHeightLimit(noMinHeightLimit);
}

void TabsModelNG::SetIsVertical(bool isVertical)
{
    auto axis = isVertical ? Axis::VERTICAL : Axis::HORIZONTAL;
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, Axis, axis);

    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (tabBarLayoutProperty->GetAxis().value_or(Axis::HORIZONTAL) != axis) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
        CHECK_NULL_VOID(tabBarPattern);
        tabBarPattern->UpdateCurrentOffset(0.0f);
    }
    tabBarLayoutProperty->UpdateAxis(axis);
    auto swiperLayoutProperty = GetSwiperLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateDirection(axis);
}

void TabsModelNG::SetIndex(int32_t index)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateIndex(index);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    if (index < 0) {
        index = 0;
    }
    tabBarPattern->UpdateIndicator(index);
    tabBarPattern->UpdateTextColorAndFontWeight(index);
    swiperLayoutProperty->UpdateIndex(index);
    auto tabsFrameNode = AceType::DynamicCast<FrameNode>(tabsNode);
    CHECK_NULL_VOID(tabsFrameNode);
    auto tabsLayoutProperty = tabsFrameNode->GetLayoutProperty<TabsLayoutProperty>();
    tabsLayoutProperty->UpdateIndex(index);
    swiperNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void TabsModelNG::SetScrollable(bool scrollable)
{
    auto props = GetSwiperLayoutProperty();
    CHECK_NULL_VOID(props);
    props->UpdateDisableSwipe(!scrollable);
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetIsDisableSwipe(!scrollable);
}

void TabsModelNG::SetAnimationCurve(const RefPtr<Curve>& curve)
{
    SetAnimationCurve(ViewStackProcessor::GetInstance()->GetMainFrameNode(), curve);
}

void TabsModelNG::SetAnimationDuration(float duration)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    tabBarPattern->SetAnimationDuration(static_cast<int32_t>(duration));
    if (static_cast<int32_t>(duration) < 0) {
        return;
    }
    auto swiperPaintProperty = GetSwiperPaintProperty();
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateDuration(static_cast<int32_t>(duration));
}

void TabsModelNG::SetFadingEdge(bool fadingEdge)
{
    auto tabBarPaintProperty = GetTabBarPaintProperty();
    CHECK_NULL_VOID(tabBarPaintProperty);
    tabBarPaintProperty->UpdateFadingEdge(fadingEdge);
}

void TabsModelNG::SetBarOverlap(bool barOverlap)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarOverlap, barOverlap);

    BlurStyleOption option;
    if (barOverlap) {
        option.blurStyle = BlurStyle::COMPONENT_THICK;
    }
    SetBarBackgroundBlurStyle(option);
}

void TabsModelNG::SetOnChange(std::function<void(const BaseEventInfo*)>&& onChange)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnChangeEvent(std::move(onChange));
}

void TabsModelNG::SetOnTabBarClick(std::function<void(const BaseEventInfo*)>&& onTabBarClick)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnTabBarClickEvent(std::move(onTabBarClick));
}

void TabsModelNG::SetOnUnselected(std::function<void(const BaseEventInfo* info)>&& onUnselected)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnUnselectedEvent(std::move(onUnselected));
}

void TabsModelNG::SetOnAnimationStart(AnimationStartEvent&& onAnimationStart)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimationStartEvent(std::move(onAnimationStart));
}

void TabsModelNG::SetOnAnimationEnd(AnimationEndEvent&& onAnimationEnd)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimationEndEvent(std::move(onAnimationEnd));
}

void TabsModelNG::SetOnGestureSwipe(GestureSwipeEvent&& onGestureSwipe)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto eventHub = swiperNode->GetOrCreateEventHub<SwiperEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetGestureSwipeEvent(std::move(onGestureSwipe));
}

void TabsModelNG::SetOnSelected(std::function<void(const BaseEventInfo* info)>&& onSelected)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnSelectedEvent(std::move(onSelected));
}

void TabsModelNG::SetOnSelected(FrameNode* frameNode, std::function<void(const BaseEventInfo* info)>&& onSelected)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetOnSelectedEvent(std::move(onSelected));
}

void TabsModelNG::SetDivider(const TabsItemDivider& divider)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
    CHECK_NULL_VOID(dividerNode);
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_VOID(dividerRenderContext);
    if (divider.isNull) {
        dividerRenderContext->UpdateOpacity(0.0f);
        auto tabsLayoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
        CHECK_NULL_VOID(tabsLayoutProperty);
        auto currentDivider = tabsLayoutProperty->GetDivider().value_or(TabsItemDivider());
        currentDivider.strokeWidth = Dimension(1.0f);
        currentDivider.isNull = true;
        ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, currentDivider);
    } else {
        dividerRenderContext->UpdateOpacity(1.0f);
        ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider);
    }
}

void TabsModelNG::SetDividerColorByUser(bool isByUser)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, DividerColorSetByUser, isByUser);
}

void TabsModelNG::SetDividerColorByUser(FrameNode* frameNode, bool isByUser)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, DividerColorSetByUser, isByUser, frameNode);
}

void TabsModelNG::SetBarBackgroundColor(const Color& backgroundColor)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarRenderContext = tabBarNode->GetRenderContext();
    CHECK_NULL_VOID(tabBarRenderContext);
    tabBarRenderContext->UpdateBackgroundColor(backgroundColor);
}

void TabsModelNG::SetBarBackgroundColorByUser(bool isByUser)
{
    ACE_UPDATE_LAYOUT_PROPERTY(TabsLayoutProperty, BarBackgroundColorSetByUser, isByUser);
}

RefPtr<TabBarLayoutProperty> TabsModelNG::GetTabBarLayoutProperty()
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_RETURN(tabBarLayoutProperty, nullptr);
    return tabBarLayoutProperty;
}

RefPtr<TabBarPaintProperty> TabsModelNG::GetTabBarPaintProperty()
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarPaintProperty = tabBarNode->GetPaintProperty<TabBarPaintProperty>();
    CHECK_NULL_RETURN(tabBarPaintProperty, nullptr);
    return tabBarPaintProperty;
}

RefPtr<SwiperLayoutProperty> TabsModelNG::GetSwiperLayoutProperty()
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_RETURN(swiperLayoutProperty, nullptr);
    return swiperLayoutProperty;
}

RefPtr<SwiperPaintProperty> TabsModelNG::GetSwiperPaintProperty()
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperPaintProperty = swiperNode->GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, nullptr);
    return swiperPaintProperty;
}

void TabsModelNG::Pop()
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto tabsLayoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(tabsLayoutProperty);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);

    tabBarNode->MarkModifyDone();
    tabBarNode->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);
    auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
    CHECK_NULL_VOID(dividerNode);
    auto layoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);

    auto axis = layoutProperty->GetAxis().value_or((Axis::HORIZONTAL));
    TabsItemDivider defaultDivider;
    auto divider = layoutProperty->GetDivider().value_or(defaultDivider);
    auto dividerColor = divider.color;
    auto dividerStrokeWidth = divider.strokeWidth;

    auto dividerHub = dividerNode->GetOrCreateEventHub<EventHub>();
    CHECK_NULL_VOID(dividerHub);

    auto dividerRenderProperty = dividerNode->GetPaintProperty<DividerRenderProperty>();
    CHECK_NULL_VOID(dividerRenderProperty);
    dividerRenderProperty->UpdateDividerColor(dividerColor);
    dividerRenderProperty->UpdateLineCap(LineCap::BUTT);

    auto dividerLayoutProperty = dividerNode->GetLayoutProperty<DividerLayoutProperty>();
    CHECK_NULL_VOID(dividerLayoutProperty);
    dividerLayoutProperty->UpdateVertical(axis == Axis::VERTICAL);
    dividerLayoutProperty->UpdateStrokeWidth(dividerStrokeWidth);
    dividerLayoutProperty->UpdateStrokeWidthLimitation(false);
    CHECK_NULL_VOID(dividerNode);
    dividerNode->MarkModifyDone();

    swiperNode->MarkModifyDone();
    swiperNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);

    ViewStackProcessor::GetInstance()->PopContainer();
}

RefPtr<TabsNode> TabsModelNG::GetOrCreateTabsNode(
    const std::string& tag, int32_t nodeId, const std::function<RefPtr<Pattern>(void)>& patternCreator)
{
    auto tabsNode = ElementRegister::GetInstance()->GetSpecificItemById<TabsNode>(nodeId);
    if (tabsNode) {
        if (tabsNode->GetTag() == tag) {
            return tabsNode;
        }
        ElementRegister::GetInstance()->RemoveItemSilently(nodeId);
        auto parent = tabsNode->GetParent();
        if (parent) {
            parent->RemoveChild(tabsNode);
        }
    }

    auto pattern = patternCreator ? patternCreator() : AceType::MakeRefPtr<Pattern>();
    tabsNode = AceType::MakeRefPtr<TabsNode>(tag, nodeId, pattern, false);
    tabsNode->InitializePatternAndContext();
    ElementRegister::GetInstance()->AddUINode(tabsNode);
    return tabsNode;
}

void TabsModelNG::SetOnChangeEvent(std::function<void(const BaseEventInfo*)>&& onChangeEvent)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnIndexChangeEvent(std::move(onChangeEvent));
}

void TabsModelNG::SetClipEdge(bool clipEdge)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    ViewAbstract::SetClipEdge(clipEdge);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperRenderContext = swiperNode->GetRenderContext();
    CHECK_NULL_VOID(swiperRenderContext);
    swiperRenderContext->UpdateClipEdge(clipEdge);
    auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
    CHECK_NULL_VOID(dividerNode);
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_VOID(dividerRenderContext);
    dividerRenderContext->UpdateClipEdge(clipEdge);
}

void TabsModelNG::SetScrollableBarModeOptions(const ScrollableBarModeOptions& option)
{
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateScrollableBarModeOptions(option);
}

void TabsModelNG::ResetScrollableBarModeOptions()
{
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->ResetScrollableBarModeOptions();
}

void TabsModelNG::SetBarGridAlign(const BarGridColumnOptions& BarGridColumnOptions)
{
    auto tabBarLayoutProperty = GetTabBarLayoutProperty();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateBarGridAlign(BarGridColumnOptions);
}

RefPtr<TabBarLayoutProperty> TabsModelNG::GetTabBarLayoutProperty(FrameNode* frameNode)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_RETURN(tabBarLayoutProperty, nullptr);
    return tabBarLayoutProperty;
}

RefPtr<TabBarPaintProperty> TabsModelNG::GetTabBarPaintProperty(FrameNode* frameNode)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_RETURN(tabBarNode, nullptr);
    auto tabBarPaintProperty = tabBarNode->GetPaintProperty<TabBarPaintProperty>();
    CHECK_NULL_RETURN(tabBarPaintProperty, nullptr);
    return tabBarPaintProperty;
}

RefPtr<SwiperLayoutProperty> TabsModelNG::GetSwiperLayoutProperty(FrameNode* frameNode)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperLayoutProperty = swiperNode->GetLayoutProperty<SwiperLayoutProperty>();
    CHECK_NULL_RETURN(swiperLayoutProperty, nullptr);
    return swiperLayoutProperty;
}

RefPtr<SwiperPaintProperty> TabsModelNG::GetSwiperPaintProperty(FrameNode* frameNode)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_RETURN(tabsNode, nullptr);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_RETURN(swiperNode, nullptr);
    auto swiperPaintProperty = swiperNode->GetPaintProperty<SwiperPaintProperty>();
    CHECK_NULL_RETURN(swiperPaintProperty, nullptr);
    return swiperPaintProperty;
}

void TabsModelNG::SetTabBarMode(FrameNode* frameNode, TabBarMode tabBarMode)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, TabBarMode, tabBarMode, frameNode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateTabBarMode(tabBarMode);
}

void TabsModelNG::SetBarGridAlign(FrameNode* frameNode, const BarGridColumnOptions& BarGridColumnOptions)
{
    CHECK_NULL_VOID(frameNode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateBarGridAlign(BarGridColumnOptions);
}

void TabsModelNG::SetOnUnselected(FrameNode* frameNode, std::function<void(const BaseEventInfo* info)>&& onUnselected)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    pattern->SetOnUnselectedEvent(std::move(onUnselected));
}

void TabsModelNG::SetDivider(FrameNode* frameNode, const TabsItemDivider& divider)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
    CHECK_NULL_VOID(dividerNode);
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_VOID(dividerRenderContext);
    if (divider.isNull) {
        dividerRenderContext->UpdateOpacity(0.0f);
        auto tabsLayoutProperty = frameNode->GetLayoutProperty<TabsLayoutProperty>();
        CHECK_NULL_VOID(tabsLayoutProperty);
        auto currentDivider = tabsLayoutProperty->GetDivider().value_or(TabsItemDivider());
        currentDivider.strokeWidth = Dimension(1.0f);
        currentDivider.isNull = true;
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, currentDivider, frameNode);
    } else {
        dividerRenderContext->UpdateOpacity(1.0f);
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider, frameNode);
    }
}

void TabsModelNG::SetEffectNodeOption(FrameNode* frameNode, const TabsEffectNodeOption& option)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(AceType::Claim(frameNode));
    CHECK_NULL_VOID(tabsNode);
    auto effectNode = AceType::DynamicCast<FrameNode>(tabsNode->GetEffectNode());
    if (option.isNull) {
        if (effectNode) {
            tabsNode->RemoveChild(effectNode);
        }
    } else {
        if (!effectNode) {
            effectNode = InitEffectNode(tabsNode);
            effectNode->MountToParent(tabsNode);
            ViewAbstract::SetZIndex(AceType::RawPtr(effectNode), EFFECT_Z_INDEX);

            auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
            CHECK_NULL_VOID(swiperNode);
            ViewAbstract::SetZIndex(AceType::RawPtr(swiperNode), SWIPER_Z_INDEX);

            auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
            CHECK_NULL_VOID(dividerNode);
            ViewAbstract::SetZIndex(AceType::RawPtr(dividerNode), DIVIDER_Z_INDEX);

            auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
            CHECK_NULL_VOID(tabBarNode);
            ViewAbstract::SetZIndex(AceType::RawPtr(tabBarNode), TAB_BAR_Z_INDEX);
        }
    }
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, EffectNodeOption, option, frameNode);
}

void TabsModelNG::SetFadingEdge(FrameNode* frameNode, bool fadingEdge)
{
    CHECK_NULL_VOID(frameNode);
    auto tabBarPaintProperty = GetTabBarPaintProperty(frameNode);
    CHECK_NULL_VOID(tabBarPaintProperty);
    tabBarPaintProperty->UpdateFadingEdge(fadingEdge);
}

void TabsModelNG::SetBarBackgroundColor(FrameNode* frameNode, const Color& backgroundColor)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarRenderContext = tabBarNode->GetRenderContext();
    CHECK_NULL_VOID(tabBarRenderContext);
    tabBarRenderContext->UpdateBackgroundColor(backgroundColor);
}

void TabsModelNG::SetBarBackgroundColorByUser(FrameNode* frameNode, bool isByUser)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, BarBackgroundColorSetByUser, isByUser, frameNode);
}

void TabsModelNG::SetBarBackgroundBlurStyle(FrameNode* frameNode, const BlurStyleOption& styleOption)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto pipeline = tabBarNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (styleOption.policy == BlurStyleActivePolicy::FOLLOWS_WINDOW_ACTIVE_STATE) {
        pipeline->AddWindowFocusChangedCallback(tabBarNode->GetId());
    } else {
        pipeline->RemoveWindowFocusChangedCallback(tabBarNode->GetId());
    }
    auto target = tabBarNode->GetRenderContext();
    if (target) {
        if (target->GetBackgroundEffect().has_value()) {
            target->UpdateBackgroundEffect(std::nullopt);
        }
        target->UpdateBackBlurStyle(styleOption);
        if (target->GetBackBlurRadius().has_value()) {
            target->UpdateBackBlurRadius(Dimension());
        }
    }
}

void TabsModelNG::SetBarOverlap(FrameNode* frameNode, bool barOverlap)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, BarOverlap, barOverlap, frameNode);

    BlurStyleOption option;
    if (barOverlap) {
        option.blurStyle = BlurStyle::COMPONENT_THICK;
    }
    SetBarBackgroundBlurStyle(frameNode, option);
}

void TabsModelNG::SetIsVertical(FrameNode* frameNode, bool isVertical)
{
    auto axis = isVertical ? Axis::VERTICAL : Axis::HORIZONTAL;
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Axis, axis, frameNode);

    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    if (tabBarLayoutProperty->GetAxis().value_or(Axis::HORIZONTAL) != axis) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
        CHECK_NULL_VOID(tabBarPattern);
        tabBarPattern->UpdateCurrentOffset(0.0f);
    }
    tabBarLayoutProperty->UpdateAxis(axis);
    auto swiperLayoutProperty = GetSwiperLayoutProperty(frameNode);
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateDirection(axis);
}

void TabsModelNG::SetTabBarPosition(FrameNode* frameNode, BarPosition tabBarPosition)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsLayoutProperty = frameNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(tabsLayoutProperty);
    auto oldTabBarPosition = tabsLayoutProperty->GetTabBarPosition();
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, TabBarPosition, tabBarPosition, frameNode);

    if ((!oldTabBarPosition.has_value() && tabBarPosition == BarPosition::END) ||
        (oldTabBarPosition.has_value() && oldTabBarPosition.value() == tabBarPosition)) {
        return;
    }

    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabsFocusNode = tabsNode->GetFocusHub();
    CHECK_NULL_VOID(tabsFocusNode);
    if (!tabsFocusNode->IsCurrentFocus()) {
        auto tabBarPosition = tabsLayoutProperty->GetTabBarPosition().value_or(BarPosition::START);
        if (tabBarPosition == BarPosition::START) {
            auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
            CHECK_NULL_VOID(tabBarNode);
            auto tabBarFocusNode = tabBarNode->GetFocusHub();
            CHECK_NULL_VOID(tabBarFocusNode);
            tabsFocusNode->SetLastWeakFocusNode(AceType::WeakClaim(AceType::RawPtr(tabBarFocusNode)));
        } else {
            auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
            CHECK_NULL_VOID(swiperNode);
            auto swiperFocusNode = swiperNode->GetFocusHub();
            CHECK_NULL_VOID(swiperFocusNode);
            tabsFocusNode->SetLastWeakFocusNode(AceType::WeakClaim(AceType::RawPtr(swiperFocusNode)));
        }
    }
}

void TabsModelNG::SetScrollable(FrameNode* frameNode, bool scrollable)
{
    CHECK_NULL_VOID(frameNode);
    auto props = GetSwiperLayoutProperty(frameNode);
    CHECK_NULL_VOID(props);
    props->UpdateDisableSwipe(!scrollable);
    auto tabPattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetIsDisableSwipe(!scrollable);
}

void TabsModelNG::SetTabBarWidth(FrameNode* frameNode, const Dimension& tabBarWidth)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, BarWidth, tabBarWidth, frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    auto scaleProperty = ScaleProperty::CreateScaleProperty();
    auto tabBarWidthToPx =
        ConvertToPx(tabBarWidth, scaleProperty, tabBarLayoutProperty->GetLayoutConstraint()->percentReference.Width());
    if (LessNotEqual(tabBarWidthToPx.value_or(0.0), 0.0)) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(true, false);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(NG::CalcLength(tabBarWidth), std::nullopt));
    }
    tabBarLayoutProperty->UpdateTabBarWidth(tabBarWidth);
}

void TabsModelNG::SetTabBarHeight(FrameNode* frameNode, const Dimension& tabBarHeight)
{
    ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, BarHeight, tabBarHeight, frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarLayoutProperty = tabBarNode->GetLayoutProperty<TabBarLayoutProperty>();
    CHECK_NULL_VOID(tabBarLayoutProperty);
    auto scaleProperty = ScaleProperty::CreateScaleProperty();
    auto tabBarHeightToPx = ConvertToPx(
        tabBarHeight, scaleProperty, tabBarLayoutProperty->GetLayoutConstraint()->percentReference.Height());
    if (LessNotEqual(tabBarHeightToPx.value_or(0.0), 0.0)) {
        tabBarLayoutProperty->ClearUserDefinedIdealSize(false, true);
    } else {
        tabBarLayoutProperty->UpdateUserDefinedIdealSize(CalcSize(std::nullopt, NG::CalcLength(tabBarHeight)));
    }
    tabBarLayoutProperty->UpdateTabBarHeight(tabBarHeight);
}

void TabsModelNG::SetAnimationCurve(FrameNode* frameNode, const RefPtr<Curve>& curve)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    tabBarPattern->SetAnimationCurve(curve);
    auto swiperPaintProperty = GetSwiperPaintProperty(frameNode);
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateCurve(tabBarPattern->GetAnimationCurve(TabBarPhysicalCurve));
}

void TabsModelNG::SetAnimationDuration(FrameNode* frameNode, float duration)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto tabBarPattern = tabBarNode->GetPattern<TabBarPattern>();
    CHECK_NULL_VOID(tabBarPattern);
    tabBarPattern->SetAnimationDuration(static_cast<int32_t>(duration));
    if (static_cast<int32_t>(duration) < 0) {
        return;
    }
    auto swiperPaintProperty = GetSwiperPaintProperty(frameNode);
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateDuration(static_cast<int32_t>(duration));
}

void TabsModelNG::SetScrollableBarModeOptions(FrameNode* frameNode, const ScrollableBarModeOptions& option)
{
    CHECK_NULL_VOID(frameNode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateScrollableBarModeOptions(option);
}

void TabsModelNG::SetBarAdaptiveHeight(FrameNode* frameNode, bool barAdaptiveHeight)
{
    CHECK_NULL_VOID(frameNode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateBarAdaptiveHeight(barAdaptiveHeight);
}

void TabsModelNG::SetNoMinHeightLimit(FrameNode* frameNode, bool noMinHeightLimit)
{
    CHECK_NULL_VOID(frameNode);
    auto tabBarLayoutProperty = GetTabBarLayoutProperty(frameNode);
    CHECK_NULL_VOID(tabBarLayoutProperty);
    tabBarLayoutProperty->UpdateNoMinHeightLimit(noMinHeightLimit);
}

void TabsModelNG::SetIsCustomAnimation(bool isCustom)
{
    auto swiperLayoutProperty = GetSwiperLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateIsCustomAnimation(isCustom);
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetIsCustomAnimation(isCustom);
}

void TabsModelNG::SetOnCustomAnimation(TabsCustomAnimationEvent&& onCustomAnimation)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    swiperPattern->SetTabsCustomContentTransition(std::move(onCustomAnimation));
}

void TabsModelNG::SetClipEdge(FrameNode* frameNode, bool clipEdge)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    ViewAbstract::SetClipEdge(tabsNode, clipEdge);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperRenderContext = swiperNode->GetRenderContext();
    CHECK_NULL_VOID(swiperRenderContext);
    swiperRenderContext->UpdateClipEdge(clipEdge);
    auto dividerNode = AceType::DynamicCast<FrameNode>(tabsNode->GetDivider());
    CHECK_NULL_VOID(dividerNode);
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_VOID(dividerRenderContext);
    dividerRenderContext->UpdateClipEdge(clipEdge);
}

void TabsModelNG::SetOnContentWillChange(std::function<bool(int32_t, int32_t)>&& callback)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetInterceptStatus(true);
    tabPattern->SetOnContentWillChange(std::move(callback));
}

void TabsModelNG::SetAnimateMode(TabAnimateMode mode)
{
    auto tabsNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimateMode(mode);
}

void TabsModelNG::SetAnimateMode(FrameNode* frameNode, TabAnimateMode mode)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimateMode(mode);
}

void TabsModelNG::SetEdgeEffect(EdgeEffect edgeEffect)
{
    auto swiperPaintProperty = GetSwiperPaintProperty();
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateEdgeEffect(edgeEffect);
}

void TabsModelNG::SetEdgeEffect(FrameNode* frameNode, int32_t edgeEffect)
{
    auto swiperPaintProperty = GetSwiperPaintProperty(frameNode);
    CHECK_NULL_VOID(swiperPaintProperty);
    swiperPaintProperty->UpdateEdgeEffect(static_cast<EdgeEffect>(edgeEffect));
}

void TabsModelNG::SetTabBarIndex(FrameNode* frameNode, int32_t index)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabsLayoutProperty = tabsNode->GetLayoutProperty<TabsLayoutProperty>();
    CHECK_NULL_VOID(tabsLayoutProperty);
    if (tabsLayoutProperty->GetIndex().has_value()) {
        auto preIndex = tabsLayoutProperty->GetIndex().value();
        if (preIndex == index || index < 0) {
            return;
        }
    }
    tabsLayoutProperty->UpdateIndexSetByUser(index);
}

void TabsModelNG::SetTabsController(FrameNode* frameNode, const RefPtr<SwiperController>& tabsController)
{
    CHECK_NULL_VOID(frameNode);
    auto nodeId = frameNode->GetId();
    auto tabsNode = GetOrCreateTabsNode(V2::TABS_ETS_TAG, nodeId, []() { return AceType::MakeRefPtr<TabsPattern>(); });
    CHECK_NULL_VOID(tabsNode);
    InitTabsNode(tabsNode, tabsController);
}

void TabsModelNG::SetBarModifier(FrameNode* frameNode, std::function<void(WeakPtr<NG::FrameNode>)>&& onApply)
{
    CHECK_NULL_VOID(onApply);
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    onApply(tabBarNode);
}

void TabsModelNG::SetBarBackgroundEffect(const EffectOption& effectOption)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    SetBarBackgroundEffect(frameNode, effectOption);
}

void TabsModelNG::SetBarBackgroundEffect(FrameNode* frameNode, const EffectOption& effectOption)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
    CHECK_NULL_VOID(tabBarNode);
    auto pipeline = tabBarNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (effectOption.policy == BlurStyleActivePolicy::FOLLOWS_WINDOW_ACTIVE_STATE) {
        pipeline->AddWindowFocusChangedCallback(tabBarNode->GetId());
    } else {
        pipeline->RemoveWindowFocusChangedCallback(tabBarNode->GetId());
    }
    auto target = tabBarNode->GetRenderContext();
    if (target) {
        if (target->GetBackBlurRadius().has_value()) {
            target->UpdateBackBlurRadius(Dimension());
        }
        if (target->GetBackBlurStyle().has_value()) {
            target->UpdateBackBlurStyle(std::nullopt);
        }
        target->UpdateBackgroundEffect(effectOption);
    }
}

void TabsModelNG::SetPageFlipMode(int32_t pageFlipMode)
{
    auto tabsNode = AceType::DynamicCast<TabsNode>(ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    swiperPattern->SetPageFlipMode(pageFlipMode);
}

void TabsModelNG::SetPageFlipMode(FrameNode* frameNode, int32_t options)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto swiperPattern = swiperNode->GetPattern<SwiperPattern>();
    CHECK_NULL_VOID(swiperPattern);
    swiperPattern->SetPageFlipMode(options);
}

void TabsModelNG::SetCachedMaxCount(std::optional<int32_t> cachedMaxCount, TabsCacheMode cacheMode)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    SetCachedMaxCount(frameNode, cachedMaxCount, cacheMode);
}

void TabsModelNG::SetCachedMaxCount(
    FrameNode* frameNode, std::optional<int32_t> cachedMaxCount, TabsCacheMode cacheMode)
{
    CHECK_NULL_VOID(frameNode);
    if (cachedMaxCount.has_value()) {
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, CachedMaxCount, cachedMaxCount.value(), frameNode);
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, CacheMode, cacheMode, frameNode);
    } else {
        ACE_RESET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, CachedMaxCount, frameNode);
        ACE_RESET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, CacheMode, frameNode);
    }
}

void TabsModelNG::SetOnChange(FrameNode* frameNode, std::function<void(const BaseEventInfo*)>&& onChange)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnChangeEvent(std::move(onChange));
}
void TabsModelNG::SetOnTabBarClick(FrameNode* frameNode, std::function<void(const BaseEventInfo*)>&& onTabBarClick)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetOnTabBarClickEvent(std::move(onTabBarClick));
}
void TabsModelNG::SetOnAnimationStart(FrameNode* frameNode, AnimationStartEvent&& onAnimationStart)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimationStartEvent(std::move(onAnimationStart));
}
void TabsModelNG::SetOnAnimationEnd(FrameNode* frameNode, AnimationEndEvent&& onAnimationEnd)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetAnimationEndEvent(std::move(onAnimationEnd));
}
void TabsModelNG::SetOnGestureSwipe(FrameNode* frameNode, GestureSwipeEvent&& gestureSwipe)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto swiperNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabs());
    CHECK_NULL_VOID(swiperNode);
    auto eventHub = swiperNode->GetOrCreateEventHub<SwiperEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->SetGestureSwipeEvent(std::move(gestureSwipe));
}
void TabsModelNG::SetIsCustomAnimation(FrameNode* frameNode, bool isCustom)
{
    CHECK_NULL_VOID(frameNode);
    auto swiperLayoutProperty = GetSwiperLayoutProperty();
    CHECK_NULL_VOID(swiperLayoutProperty);
    swiperLayoutProperty->UpdateIsCustomAnimation(isCustom);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetIsCustomAnimation(isCustom);
}
void TabsModelNG::SetOnContentWillChange(
    FrameNode* frameNode, std::function<bool(int32_t, int32_t)>&& callback)
{
    CHECK_NULL_VOID(frameNode);
    auto tabsNode = AceType::DynamicCast<TabsNode>(frameNode);
    CHECK_NULL_VOID(tabsNode);
    auto tabPattern = tabsNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(tabPattern);
    tabPattern->SetInterceptStatus(true);
    tabPattern->SetOnContentWillChange(std::move(callback));
}
void TabsModelNG::CreateWithResourceObj(TabJsResType jsResourceType, const RefPtr<ResourceObject>& resObj)
{
    CHECK_NULL_VOID(SystemProperties::ConfigChangePerform());
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    if (SystemProperties::ConfigChangePerform()) {
        auto pattern = frameNode->GetPattern<TabsPattern>();
        CHECK_NULL_VOID(pattern);
        std::string key = "tabs." + std::to_string(static_cast<int32_t>(jsResourceType));
        pattern->RemoveResObj(key);
    }
    switch (jsResourceType) {
        case TabJsResType::BAR_BACKGROUND_COLOR:
            HandleBarBackgroundColor(frameNode, resObj);
            break;
        case TabJsResType::BAR_WIDTH:
            HandleBarWidth(frameNode, resObj);
            break;
        case TabJsResType::BAR_HEIGHT:
            HandleBarHeight(frameNode, resObj);
            break;
        case TabJsResType::BAR_GRID_GUTTER:
            HandleBarGridGutter(frameNode, resObj);
            break;
        case TabJsResType::BAR_GRID_MARGIN:
            HandleBarGridMargin(frameNode, resObj);
            break;
        case TabJsResType::DIVIDER_STROKE_WIDTH:
            HandleDividerStrokeWidth(frameNode, resObj);
            break;
        case TabJsResType::DIVIDER_COLOR:
            HandleDividerColor(frameNode, resObj);
            break;
        case TabJsResType::DIVIDER_START_MARGIN:
            HandleDividerStartMargin(frameNode, resObj);
            break;
        case TabJsResType::DIVIDER_END_MARGIN:
            HandleDividerEndMargin(frameNode, resObj);
            break;
        case TabJsResType::SCROLLABLE_BAR_MARGIN:
            HandleScrollableBarMargin(frameNode, resObj);
            break;
        case TabJsResType::COLOR:
            HandleBackgroundEffectColor(frameNode, resObj);
            break;
        case TabJsResType::INACTIVE_COLOR:
            HandleBackgroundEffectInactiveColor(frameNode, resObj);
            break;
        case TabJsResType::BlurStyle_INACTIVE_COLOR:
            HandleBackgroundBlurStyleInactiveColor(frameNode, resObj);
            break;
        default:
            break;
    }
}

void TabsModelNG::HandleBarBackgroundColor(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBarBackgroundColor";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto node = weak.Upgrade();
        CHECK_NULL_VOID(node);
        Color result = Color::BLACK.BlendOpacity(0.0f);
        ResourceParseUtils::ParseResColor(resObj, result);
        TabsModelNG::SetBarBackgroundColor(AceType::RawPtr(node), result);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBarWidth(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBarWidth";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        CalcDimension result = Dimension(-1.0, DimensionUnit::VP);
        ResourceParseUtils::ParseResDimensionVpNG(resObj, result);
        TabsModelNG::SetTabBarWidth(AceType::RawPtr(tabsNode), result);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBarHeight(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBarHeight";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        CalcDimension result = Dimension(-1.0, DimensionUnit::VP);
        ResourceParseUtils::ParseResDimensionVpNG(resObj, result);
        TabsModelNG::SetTabBarHeight(AceType::RawPtr(tabsNode), result);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBarGridGutter(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBarGridGutter";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        CalcDimension result;
        BarGridColumnOptions tempUsed;
        if (ResourceParseUtils::ParseResDimensionVp(resObj, result) && NonNegative(result.Value()) &&
            result.Unit() != DimensionUnit::PERCENT) {
            tempUsed.gutter = result;
        }
        BarGridColumnOptions columnOption;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, BarGridAlign, columnOption, tabBarNode);
        columnOption.gutter = tempUsed.gutter;
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, BarGridAlign, columnOption, tabBarNode);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBarGridMargin(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBarGridMargin";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        CalcDimension result;
        BarGridColumnOptions tempUsed;
        if (ResourceParseUtils::ParseResDimensionVp(resObj, result) && NonNegative(result.Value()) &&
            result.Unit() != DimensionUnit::PERCENT) {
            tempUsed.margin = result;
        }
        BarGridColumnOptions columnOption;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, BarGridAlign, columnOption, tabBarNode);
        columnOption.margin = tempUsed.margin;
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, BarGridAlign, columnOption, tabBarNode);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleDividerStrokeWidth(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsDividerStrokeWidth";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto &&updateFunc = [weak = AceType::WeakClaim(frameNode),
                            weakPattern = AceType::WeakClaim(AceType::RawPtr(pattern))](
                            const RefPtr<ResourceObject> &resObj) {
        auto node = weak.Upgrade();
        CHECK_NULL_VOID(node);
        auto tabsPattern = weakPattern.Upgrade();
        CHECK_NULL_VOID(tabsPattern);
        CalcDimension result;
        TabsItemDivider divider;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider, node);
        ResourceParseUtils::ParseResDimensionVp(resObj, result);
        if (result.Value() < 0.0f || result.Unit() == DimensionUnit::PERCENT) {
            divider.strokeWidth.Reset();
        } else {
            divider.strokeWidth = result;
        }
        TabsModelNG::SetDivider(AceType::RawPtr(node), divider);
        tabsPattern->UpdateDividerStrokeWidth();
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleDividerColor(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsDividerColor";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto &&updateFunc = [weak = AceType::WeakClaim(frameNode),
                            weakPattern = AceType::WeakClaim(AceType::RawPtr(pattern))](
                            const RefPtr<ResourceObject> &resObj) {
        auto node = weak.Upgrade();
        CHECK_NULL_VOID(node);
        auto tabsPattern = weakPattern.Upgrade();
        CHECK_NULL_VOID(tabsPattern);
        Color result;
        TabsItemDivider divider;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider, node);
        if (ResourceParseUtils::ParseResColor(resObj, result)) {
            divider.color = result;
            TabsModelNG::SetDividerColorByUser(AceType::RawPtr(node), true);
        } else {
            auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
            CHECK_NULL_VOID(pipelineContext);
            auto tabTheme = pipelineContext->GetTheme<TabTheme>();
            CHECK_NULL_VOID(tabTheme);
            divider.color = tabTheme->GetDividerColor();
            TabsModelNG::SetDividerColorByUser(AceType::RawPtr(node), false);
        }
        TabsModelNG::SetDivider(AceType::RawPtr(node), divider);
        tabsPattern->UpdateDividerColor();
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleDividerStartMargin(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsDividerStartMargin";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto node = weak.Upgrade();
        CHECK_NULL_VOID(node);
        CalcDimension result;
        TabsItemDivider divider;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider, node);
        ResourceParseUtils::ParseResDimensionVp(resObj, result);
        if (result.Value() < 0.0f || result.Unit() == DimensionUnit::PERCENT) {
            divider.startMargin.Reset();
        } else {
            divider.startMargin = result;
        }
        TabsModelNG::SetDivider(AceType::RawPtr(node), divider);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleDividerEndMargin(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsDividerEndMargin";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto node = weak.Upgrade();
        CHECK_NULL_VOID(node);
        CalcDimension result;
        TabsItemDivider divider;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabsLayoutProperty, Divider, divider, node);
        ResourceParseUtils::ParseResDimensionVp(resObj, result);
        if (result.Value() < 0.0f || result.Unit() == DimensionUnit::PERCENT) {
            divider.endMargin.Reset();
        } else {
            divider.endMargin = result;
        }
        TabsModelNG::SetDivider(AceType::RawPtr(node), divider);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleScrollableBarMargin(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsScrollableBarMargin";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode)](const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        CalcDimension result;
        ScrollableBarModeOptions option;
        ACE_GET_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, ScrollableBarModeOptions, option, tabBarNode);
        if (!ResourceParseUtils::ParseResDimensionVp(resObj, result) || Negative(result.Value()) ||
            result.Unit() == DimensionUnit::PERCENT) {
            option.margin = 0.0_vp;
        } else {
            option.margin = result;
        }
        ACE_UPDATE_NODE_LAYOUT_PROPERTY(TabBarLayoutProperty, ScrollableBarModeOptions, option, tabBarNode);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBackgroundEffectColor(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBackGroundEffectColor";
    pattern->RemoveResObj(key);
    CHECK_NULL_VOID(resObj);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode),
                                        weakPattern = AceType::WeakClaim(AceType::RawPtr(pattern))](
                                        const RefPtr<ResourceObject>& resObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        auto target = tabBarNode->GetRenderContext();
        CHECK_NULL_VOID(target);
        EffectOption option = target->GetBackgroundEffect().value_or(EffectOption{});
        option.isWindowFocused = true; // set to default value
        Color result = Color::TRANSPARENT;
        ResourceParseUtils::ParseResColor(resObj, result);
        option.color = result;
        TabsModelNG::SetBarBackgroundEffect(AceType::RawPtr(tabsNode), option);
    };
    pattern->AddResObj(key, resObj, std::move(updateFunc));
}

void TabsModelNG::HandleBackgroundEffectInactiveColor(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBackGroundEffectInactiveColor";
    pattern->RemoveResObj(key);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode), resObj](const RefPtr<ResourceObject>& dummyResObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        auto target = tabBarNode->GetRenderContext();
        CHECK_NULL_VOID(target);
        EffectOption option = target->GetBackgroundEffect().value_or(EffectOption{});
        option.isWindowFocused = true; // set to default value
        if (!resObj) {
            TabsModelNG::SetBarBackgroundEffect(AceType::RawPtr(tabsNode), option);
            return;
        }
        Color result = Color::TRANSPARENT;
        option.isValidColor = ResourceParseUtils::ParseResColor(resObj, result);
        option.inactiveColor = result;
        TabsModelNG::SetBarBackgroundEffect(AceType::RawPtr(tabsNode), option);
    };
    RefPtr<ResourceObject> dummyResObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    pattern->AddResObj(key, dummyResObj, std::move(updateFunc));
}

void TabsModelNG::HandleBackgroundBlurStyleInactiveColor(FrameNode* frameNode, const RefPtr<ResourceObject>& resObj,
    bool isModifier)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<TabsPattern>();
    CHECK_NULL_VOID(pattern);
    const std::string key = "tabsBackGroundBlurStyle";
    pattern->RemoveResObj(key);
    auto&& updateFunc = [weak = AceType::WeakClaim(frameNode), resObj](const RefPtr<ResourceObject>& dummyResObj) {
        auto tabsNode = AceType::DynamicCast<TabsNode>(weak.Upgrade());
        CHECK_NULL_VOID(tabsNode);
        auto tabBarNode = AceType::DynamicCast<FrameNode>(tabsNode->GetTabBar());
        CHECK_NULL_VOID(tabBarNode);
        auto target = tabBarNode->GetRenderContext();
        CHECK_NULL_VOID(target);
        BlurStyleOption styleOption = target->GetBackBlurStyle().value_or(BlurStyleOption{});
        styleOption.isWindowFocused = true; // set to default value
        if (!resObj) {
            TabsModelNG::SetBarBackgroundBlurStyle(AceType::RawPtr(tabsNode), styleOption);
            return;
        }
        Color result = Color::TRANSPARENT;
        styleOption.isValidColor = ResourceParseUtils::ParseResColor(resObj, result);
        styleOption.inactiveColor = result;
        TabsModelNG::SetBarBackgroundBlurStyle(AceType::RawPtr(tabsNode), styleOption);
    };
    RefPtr<ResourceObject> dummyResObj = AceType::MakeRefPtr<ResourceObject>("", "", -1);
    pattern->AddResObj(key, dummyResObj, std::move(updateFunc));
}
} // namespace OHOS::Ace::NG
