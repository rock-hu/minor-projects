/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/navrouter/navdestination_layout_algorithm.h"

#include "core/components_ng/pattern/navigation/navigation_layout_algorithm.h"
#include "core/components_ng/pattern/navigation/navigation_layout_util.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_title_util.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"

namespace OHOS::Ace::NG {
namespace {
const std::unordered_set<std::string> EMBEDDED_NODE_TAG = { V2::SHEET_WRAPPER_TAG, V2::ALERT_DIALOG_ETS_TAG,
    V2::ACTION_SHEET_DIALOG_ETS_TAG, V2::DIALOG_ETS_TAG };
bool CheckTopEdgeOverlap(const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty,
    const RefPtr<NavDestinationGroupNode>& hostNode, SafeAreaExpandOpts opts)
{
    if (!navDestinationLayoutProperty || !hostNode) {
        return false;
    }
    auto layoutProperty = hostNode->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty, false);
    auto margin = layoutProperty->CreateMargin();
    float topMargin = margin.top.value_or(0.0f);
    const auto& padding = navDestinationLayoutProperty->CreatePaddingAndBorder();
    float topPadding = padding.top.value_or(0.0f);
    if (!NearEqual(topPadding, 0.0f) || !NearEqual(topMargin, 0.0f)) {
        return false;
    }

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto safeAreaManager = pipeline->GetSafeAreaManager();

    auto parentGlobalOffset = hostNode->GetParentGlobalOffsetDuringLayout();
    auto NavDesGeometryNode = hostNode->GetGeometryNode();
    CHECK_NULL_RETURN(NavDesGeometryNode, false);
    auto frame = NavDesGeometryNode->GetFrameRect() + parentGlobalOffset;
    // only handle top-edge and system-type safeArea in current function
    if (!(opts.edges & SAFE_AREA_EDGE_TOP) || !(opts.type & SAFE_AREA_TYPE_SYSTEM)) {
        return false;
    }
    SafeAreaExpandOpts topSystemSafeAreaOpts = {.type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_TOP};
    auto safeAreaPos = safeAreaManager->GetCombinedSafeArea(topSystemSafeAreaOpts);
    auto navDestinationPattern = hostNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(navDestinationPattern, false);
    auto barStyle = navDestinationPattern->GetTitleBarStyle().value_or(BarStyle::STANDARD);
    if (!safeAreaPos.top_.IsOverlapped(frame.Top())) {
        return false;
    }
    if (navDestinationLayoutProperty->GetHideTitleBar().value_or(false) || barStyle == BarStyle::STACK ||
        (barStyle == BarStyle::SAFE_AREA_PADDING && !NearZero(navDestinationPattern->GetTitleBarOffsetY()))) {
        return true;
    }
    return false;
}

bool CheckBottomEdgeOverlap(const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty,
    const RefPtr<NavDestinationGroupNode>& hostNode, SafeAreaExpandOpts opts)
{
    if (!navDestinationLayoutProperty || !hostNode) {
        return false;
    }
    auto layoutProperty = hostNode->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty, false);
    auto margin = layoutProperty->CreateMargin();
    float bottomMargin = margin.bottom.value_or(0.0f);
    const auto& padding = navDestinationLayoutProperty->CreatePaddingAndBorder();
    float bottomPadding = padding.bottom.value_or(0.0f);
    if (!NearEqual(bottomPadding, 0.0f) || !NearEqual(bottomMargin, 0.0f)) {
        return false;
    }

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto safeAreaManager = pipeline->GetSafeAreaManager();

    auto parentGlobalOffset = hostNode->GetParentGlobalOffsetDuringLayout();
    auto NavBarGeometryNode = hostNode->GetGeometryNode();
    CHECK_NULL_RETURN(NavBarGeometryNode, false);
    auto frame = NavBarGeometryNode->GetFrameRect() + parentGlobalOffset;
    bool isToolBarVisible = hostNode->IsToolBarVisible();

    if ((opts.edges & SAFE_AREA_EDGE_BOTTOM) && (opts.type & SAFE_AREA_TYPE_SYSTEM)) {
        SafeAreaExpandOpts expandOpts = { .type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_BOTTOM };
        auto safeAreaPos = safeAreaManager->GetCombinedSafeArea(expandOpts);
        if (safeAreaPos.bottom_.IsOverlapped(frame.Bottom()) && !isToolBarVisible) {
            return true;
        }
    }
    return false;
}

NavSafeArea CheckIgnoreLayoutSafeArea(LayoutWrapper* layoutWrapper,
    const RefPtr<NavDestinationGroupNode>& hostNode,
    const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty)
{
    NavSafeArea safeArea;
    auto opts = navDestinationLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, safeArea);
    auto inset = pipeline->GetSafeArea();
    auto safeAreaManager = pipeline->GetSafeAreaManager();

    bool edgeTopOverLayCondition = CheckTopEdgeOverlap(navDestinationLayoutProperty, hostNode, opts);
    bool edgeBottomOverLayCondition = CheckBottomEdgeOverlap(navDestinationLayoutProperty, hostNode, opts);

    if (edgeTopOverLayCondition) {
        auto contentNode =  AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
        CHECK_NULL_RETURN(contentNode, safeArea);
        SafeAreaExpandOpts opts = {.type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_NONE};
        contentNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(opts);
        safeArea.top = static_cast<float>(inset.top_.Length());
    }

    if (edgeBottomOverLayCondition) {
        safeArea.bottom = static_cast<float>(inset.bottom_.Length());
    }
    return safeArea;
}

float MeasureTitleBar(LayoutWrapper* layoutWrapper, const RefPtr<NavDestinationGroupNode>& hostNode,
    const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty, const SizeF& size,
    const std::optional<float>& containerModalTitleHeight)
{
    auto navDestinationPattern = hostNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(navDestinationPattern, 0.0f);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(hostNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, 0.0f);
    auto index = hostNode->GetChildIndexById(titleBarNode->GetId());
    auto titleBarWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(titleBarWrapper, 0.0f);
    auto constraint = navDestinationLayoutProperty->CreateChildConstraint();
    auto translateState = navDestinationLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
    /**
     * In the follow scenarios, we need to set the titleBar size to zero.
     * 1. TitleBar has no mainTitle&subTitle and hide backBotton.
     * 2. Titlebar is hidden and no titleBar animation is running.
     */
    if ((translateState == BarTranslateState::NONE &&
            navDestinationLayoutProperty->GetHideTitleBar().value_or(false)) ||
        (!titleBarNode->GetSubtitle() && !titleBarNode->GetTitle() && !navDestinationPattern->GetBackButtonState())) {
        constraint.selfIdealSize = OptionalSizeF(0.0f, 0.0f);
        titleBarWrapper->Measure(constraint);
        return 0.0f;
    }

    if (containerModalTitleHeight.has_value()) {
        constraint.selfIdealSize.SetHeight(containerModalTitleHeight.value());
        titleBarWrapper->Measure(constraint);
        return containerModalTitleHeight.value();
    }

    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_RETURN(titleBarLayoutProperty, 0.0f);
    if (titleBarLayoutProperty->HasTitleHeight()) {
        auto titleHeight = static_cast<float>(
            titleBarLayoutProperty->GetTitleHeightValue().ConvertToPxWithSize(constraint.percentReference.Height()));
        constraint.selfIdealSize.SetHeight((titleHeight));
        titleBarWrapper->Measure(constraint);
        return titleHeight;
    }
    auto singleLineTitleHeight = SINGLE_LINE_TITLEBAR_HEIGHT;
    auto doubleLineTitleBarHeight = DOUBLE_LINE_TITLEBAR_HEIGHT;
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        doubleLineTitleBarHeight = SINGLE_LINE_TITLEBAR_HEIGHT;
    }
    auto titleHeight = titleBarLayoutProperty->GetTitleHeightValue(
        titleBarNode->GetSubtitle() ? doubleLineTitleBarHeight : singleLineTitleHeight);
    constraint.selfIdealSize = OptionalSizeF(
        size.Width(), static_cast<float>(titleHeight.ConvertToPxWithSize(constraint.percentReference.Height())));
    titleBarWrapper->Measure(constraint);
    return static_cast<float>(titleHeight.ConvertToPxWithSize(constraint.percentReference.Height()));
}

float MeasureContentChild(LayoutWrapper* layoutWrapper, const RefPtr<NavDestinationGroupNode>& hostNode,
    const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty,
    const SizeF& size, float titleBarAndToolBarHeight)
{
    auto contentNode = hostNode->GetContentNode();
    CHECK_NULL_RETURN(contentNode, 0.0f);
    auto index = hostNode->GetChildIndexById(contentNode->GetId());
    auto contentWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(contentWrapper, 0.0f);
    auto constraint = navDestinationLayoutProperty->CreateChildConstraint();
    float contentHeight = size.Height() - titleBarAndToolBarHeight;
    if (NavigationLayoutAlgorithm::IsAutoHeight(navDestinationLayoutProperty)) {
        constraint.selfIdealSize.SetWidth(size.Width());
        contentWrapper->Measure(constraint);
        return static_cast<float>(contentWrapper->GetGeometryNode()->GetFrameSize().Height());
    } else {
        constraint.selfIdealSize = OptionalSizeF(size.Width(), contentHeight);
    }
    auto currentHeight = constraint.selfIdealSize.Height().value() ;
    auto safeArea = CheckIgnoreLayoutSafeArea(layoutWrapper, hostNode, navDestinationLayoutProperty);
    constraint.selfIdealSize.SetHeight(currentHeight + safeArea.top + safeArea.bottom);
    contentWrapper->Measure(constraint);
    return currentHeight;
}

float LayoutTitleBar(LayoutWrapper* layoutWrapper, const RefPtr<NavDestinationGroupNode>& hostNode,
    const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty)
{
    /**
     * When all the following conditions are met, we consider the titleBar height to be 0:
     * 1. TitleBar should hide.
     * 2. No titleBar animation is running or titleBar was translate out of navigation area.
     */
    auto translateState = navDestinationLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
    if (translateState != BarTranslateState::TRANSLATE_ZERO &&
        navDestinationLayoutProperty->GetHideTitleBar().value_or(false)) {
        return 0.0f;
    }
    auto titleBarNode = hostNode->GetTitleBarNode();
    CHECK_NULL_RETURN(titleBarNode, 0.0f);
    auto index = hostNode->GetChildIndexById(titleBarNode->GetId());
    auto titleBarWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(titleBarWrapper, 0.0f);
    auto geometryNode = titleBarWrapper->GetGeometryNode();
    auto offsetY = NavigationTitleUtil::CalculateTitlebarOffset(titleBarNode);
    auto navDestinationPattern = hostNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(navDestinationPattern, 0.0f);
    navDestinationPattern->SetTitleBarOffsetY(offsetY);
    auto titleBarOffset = OffsetT<float>(0.0f, offsetY);
    const auto& padding = navDestinationLayoutProperty->CreatePaddingAndBorder();
    titleBarOffset.AddX(padding.left.value_or(0.0f));
    titleBarOffset.AddY(padding.top.value_or(0.0f));
    geometryNode->SetMarginFrameOffset(titleBarOffset);
    titleBarWrapper->Layout();
    return geometryNode->GetFrameSize().Height();
}

void LayoutContent(LayoutWrapper* layoutWrapper, const RefPtr<NavDestinationGroupNode>& hostNode,
    const RefPtr<NavDestinationLayoutProperty>& navDestinationLayoutProperty, float titlebarHeight)
{
    auto contentNode = hostNode->GetContentNode();
    CHECK_NULL_VOID(contentNode);
    auto index = hostNode->GetChildIndexById(hostNode->GetContentNode()->GetId());
    auto contentWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_VOID(contentWrapper);
    auto geometryNode = contentWrapper->GetGeometryNode();
    auto pattern = hostNode->GetPattern<NavDestinationPattern>();
    float avoidKeyboardOffset = 0.0f;
    if (pattern) {
        avoidKeyboardOffset = pattern->GetAvoidKeyboardOffset();
    }
    auto contentOffset = OffsetF(0.0f, avoidKeyboardOffset);
    contentOffset += OffsetF(0.0f, titlebarHeight);
    const auto& padding = navDestinationLayoutProperty->CreatePaddingAndBorder();
    contentOffset.AddX(padding.left.value_or(0.0f));
    contentOffset.AddY(padding.top.value_or(0.0f));

    auto safeArea = CheckIgnoreLayoutSafeArea(layoutWrapper, hostNode, navDestinationLayoutProperty);
    auto offsetY = contentOffset.GetY();
    auto opts = navDestinationLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});
    if ((opts.edges & SAFE_AREA_EDGE_TOP) && NearEqual(offsetY, 0.0f)) {
        offsetY -= safeArea.top;
        contentOffset.SetY(offsetY);
    }

    geometryNode->SetMarginFrameOffset(contentOffset);
    contentWrapper->Layout();
}

void MeasureOverlay(const RefPtr<NavDestinationGroupNode>& hostNode,
    const LayoutConstraintF& constraint)
{
    for (const auto& children : hostNode->GetAllChildrenWithBuild()) {
        if (children && EMBEDDED_NODE_TAG.find(children->GetHostNode()->GetTag()) != EMBEDDED_NODE_TAG.end()) {
            children->Measure(constraint);
        }
    }
}

void LayoutOverlay(const RefPtr<NavDestinationGroupNode>& hostNode)
{
    auto navdestinationLayoutProperty = hostNode->GetLayoutProperty<NavDestinationLayoutProperty>();
    CHECK_NULL_VOID(navdestinationLayoutProperty);
    for (const auto& children : hostNode->GetAllChildrenWithBuild()) {
        if (children && EMBEDDED_NODE_TAG.find(children->GetHostNode()->GetTag()) != EMBEDDED_NODE_TAG.end()) {
            auto geometryNode = children->GetGeometryNode();
            auto paddingOffset = OffsetT<float>(0.0f, 0.0f);
            const auto& padding = navdestinationLayoutProperty->CreatePaddingAndBorder();
            paddingOffset.AddX(padding.left.value_or(0.0f));
            paddingOffset.AddY(padding.top.value_or(0.0f));
            geometryNode->SetMarginFrameOffset(paddingOffset);
            children->Layout();
        }
    }
}

float TransferBarHeight(const RefPtr<NavDestinationGroupNode>& hostNode, float defaultBarHeight, bool isTitleBar)
{
    CHECK_NULL_RETURN(hostNode, 0.0f);
    auto navDestinationPattern = hostNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(navDestinationPattern, defaultBarHeight);
    auto navDestinationLayoutProperty = hostNode->GetLayoutProperty<NavDestinationLayoutProperty>();
    CHECK_NULL_RETURN(navDestinationLayoutProperty, defaultBarHeight);
    if (isTitleBar) {
        /**
         * In the follow scenarios, we need to convert titleBar's height to zero.
         * 1. TitleBar has translate out of the navigation area.
         * 2. Titlebar is hidden and no titleBar animation is running.
         */
        auto translateState = navDestinationLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
        if (translateState == BarTranslateState::TRANSLATE_HEIGHT ||
            (translateState == BarTranslateState::NONE && navDestinationLayoutProperty->GetHideTitleBarValue(false))) {
            return 0.0f;
        }
    } else {
        /**
         * In the follow scenarios, we need to convert toolBar's height to zero.
         * 1. ToolBar has translate out of the navigation area.
         * 2. Toolbar is hidden and no toolBar animation is running.
         */
        auto translateState = navDestinationLayoutProperty->GetToolBarTranslateStateValue(BarTranslateState::NONE);
        if (translateState == BarTranslateState::TRANSLATE_HEIGHT ||
            (translateState == BarTranslateState::NONE && navDestinationLayoutProperty->GetHideToolBarValue(false))) {
            return 0.0f;
        }
    }
    return navDestinationPattern->GetTitleBarStyle().value_or(BarStyle::STANDARD) == BarStyle::STANDARD ?
        defaultBarHeight : 0.0f;
}

std::optional<float> GetContainerModalTitleHeightIfNeeded(
    const RefPtr<NavDestinationPattern>& navDestPattern, const SizeF& navDestSize)
{
    std::optional<float> titleHeight;
    CHECK_NULL_RETURN(navDestPattern, titleHeight);
    auto navDestNode = AceType::DynamicCast<NavDestinationGroupNode>(navDestPattern->GetHost());
    CHECK_NULL_RETURN(navDestNode, titleHeight);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, titleHeight);
    /**
     * When all of the following conditions are met, the titleBar height of NavDestination
     * needs to be set to the window titleBar height:
     *  1. TitleBar of window is invisible.
     *  2. Size of Navigation match size of belonged page.
     *  3. Height of NavDestination match height of Navigation.
     *
     * When all of the following conditions are met, the titleBar of NavDestination
     * needs to avoid the Control Buttons of ContainerModal:
     *  1. TitleBar of window is invisible.
     *  2. Size of Navigation match size of belonged page.
     */
    titleBarNode->SetUseContainerModalTitleHeight(false);
    titleBarNode->SetNeedAvoidContainerModal(false);
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(navDestPattern->GetNavigationNode());
    CHECK_NULL_RETURN(navigationNode, titleHeight);
    auto pipeline = navigationNode->GetContext();
    CHECK_NULL_RETURN(pipeline, titleHeight);
    auto avoidInfoMgr = pipeline->GetAvoidInfoManager();
    CHECK_NULL_RETURN(avoidInfoMgr, titleHeight);
    if (!avoidInfoMgr->NeedAvoidContainerModal()) {
        return titleHeight;
    }
    auto navigationPattern = navigationNode->GetPattern<NavigationPattern>();
    CHECK_NULL_RETURN(navigationPattern, titleHeight);
    if (!navigationPattern->IsFullPageNavigation()) {
        return titleHeight;
    }
    titleBarNode->SetNeedAvoidContainerModal(true);
    auto navigationSize = navigationPattern->GetNavigationSize();
    if (!NearEqual(navigationSize.Height(), navDestSize.Height())) {
        return titleHeight;
    }
    auto height = avoidInfoMgr->GetContainerModalTitleHeight();
    if (height <= 0) {
        return titleHeight;
    }
    titleBarNode->SetUseContainerModalTitleHeight(true);
    titleHeight = height;
    return titleHeight;
}
} // namespace

void NavDestinationLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto hostNode = AceType::DynamicCast<NavDestinationGroupNode>(layoutWrapper->GetHostNode());
    CHECK_NULL_VOID(hostNode);
    auto navDestinationPattern = hostNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    auto navDestinationLayoutProperty =
        AceType::DynamicCast<NavDestinationLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(navDestinationLayoutProperty);
    const auto& constraint = navDestinationLayoutProperty->GetLayoutConstraint();
    CHECK_NULL_VOID(constraint);
    auto geometryNode = layoutWrapper->GetGeometryNode();
    auto size = CreateIdealSize(constraint.value(), Axis::HORIZONTAL, MeasureType::MATCH_PARENT, true);
    auto containerModalTitleHeight = GetContainerModalTitleHeightIfNeeded(navDestinationPattern, size);

    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    NavigationLayoutUtil::UpdateTitleBarMenuNode(hostNode, size);
    float titleBarHeight = MeasureTitleBar(
        layoutWrapper, hostNode, navDestinationLayoutProperty, size, containerModalTitleHeight);
    navDestinationPattern->MarkSafeAreaPaddingChangedWithCheckTitleBar(titleBarHeight);
    navDestinationPattern->SetTitleBarHeight(titleBarHeight);
    auto transferedTitleBarHeight = TransferBarHeight(hostNode, titleBarHeight, true);
    float toolBarHeight =
        NavigationLayoutUtil::MeasureToolBar(layoutWrapper, hostNode, navDestinationLayoutProperty, size);
    navDestinationPattern->SetToolBarHeight(toolBarHeight);
    auto transferedToolBarHeight = TransferBarHeight(hostNode, toolBarHeight, false);
    float toolBarDividerHeight = NavigationLayoutUtil::MeasureToolBarDivider(
        layoutWrapper, hostNode, navDestinationLayoutProperty, size, toolBarHeight);
    navDestinationPattern->SetToolBarDividerHeight(toolBarDividerHeight);
    // after the visibility of title/tool bar determined, update safeAreaPadding of content node if needed.
    NavigationLayoutUtil::UpdateContentSafeAreaPadding(hostNode, titleBarHeight);
    auto transferedToolBarDividerHeight = TransferBarHeight(hostNode, toolBarDividerHeight, false);
    float titleBarAndToolBarHeight =
        transferedTitleBarHeight + transferedToolBarHeight + transferedToolBarDividerHeight;
    float contentChildHeight =
        MeasureContentChild(layoutWrapper, hostNode, navDestinationLayoutProperty, size, titleBarAndToolBarHeight);
    size.SetHeight(
        transferedTitleBarHeight + transferedToolBarHeight + transferedToolBarDividerHeight + contentChildHeight);
    if (NearZero(size.Height())) {
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto height = pipeline->GetRootHeight();
        size.SetHeight(height);
    } else {
        size.AddWidth(padding.left.value_or(0.0f) + padding.right.value_or(0.0f));
        size.AddHeight(padding.top.value_or(0.0f) + padding.bottom.value_or(0.0f));
    }
    layoutWrapper->GetGeometryNode()->SetFrameSize(size);
    MeasureOverlay(hostNode, navDestinationLayoutProperty->CreateChildConstraint());
}

void NavDestinationLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    auto hostNode = AceType::DynamicCast<NavDestinationGroupNode>(layoutWrapper->GetHostNode());
    CHECK_NULL_VOID(hostNode);
    auto navDestinationLayoutProperty =
        AceType::DynamicCast<NavDestinationLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(navDestinationLayoutProperty);

    auto expandOpts = navDestinationLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});
    bool edgeTopOverLayCondition = CheckTopEdgeOverlap(navDestinationLayoutProperty, hostNode, expandOpts);
    bool edgeBottomOverLayCondition = CheckBottomEdgeOverlap(navDestinationLayoutProperty, hostNode, expandOpts);
    if (edgeTopOverLayCondition || edgeBottomOverLayCondition) {
        Measure(layoutWrapper);
    }

    float titlebarHeight = LayoutTitleBar(layoutWrapper, hostNode, navDestinationLayoutProperty);
    auto resetTitleBarHeight = TransferBarHeight(hostNode, titlebarHeight, true);
    LayoutContent(layoutWrapper, hostNode, navDestinationLayoutProperty, resetTitleBarHeight);
    float toolbarHeight = NavigationLayoutUtil::LayoutToolBar(
        layoutWrapper, hostNode, navDestinationLayoutProperty, true);
    NavigationLayoutUtil::LayoutToolBarDivider(
        layoutWrapper, hostNode, navDestinationLayoutProperty, toolbarHeight, true);
    auto&& opts = navDestinationLayoutProperty->GetSafeAreaExpandOpts();
    if (opts) {
        auto geometryNode = hostNode->GetGeometryNode();
        CHECK_NULL_VOID(geometryNode);
        TAG_LOGD(AceLogTag::ACE_NAVIGATION,
            "Navdestination id is %{public}d, frameRect is %{public}s",
            hostNode->GetId(), geometryNode->GetFrameRect().ToString().c_str());
    }
    LayoutOverlay(hostNode);
}

} // namespace OHOS::Ace::NG