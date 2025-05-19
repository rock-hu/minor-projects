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

#include "core/components_ng/pattern/navigation/nav_bar_layout_algorithm.h"

#include "core/components/common/layout/grid_system_manager.h"
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_layout_util.h"
#include "core/components_ng/pattern/navigation/navigation_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_title_util.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navigation/tool_bar_node.h"
#include "core/components_ng/pattern/navigation/tool_bar_pattern.h"
#include "core/components_ng/property/measure_utils.h"

namespace OHOS::Ace::NG {
namespace {
float MeasureTitleBar(LayoutWrapper* layoutWrapper, const RefPtr<NavBarNode>& hostNode,
    const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty, const SizeF& navigationSize)
{
    CHECK_NULL_RETURN(hostNode, 0.0f);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(hostNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, 0.0f);
    auto index = hostNode->GetChildIndexById(titleBarNode->GetId());
    auto titleBarWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(titleBarWrapper, 0.0f);
    auto constraint = navBarLayoutProperty->CreateChildConstraint();

    /**
     * In the follow scenarios, we need to set the titleBar size to zero.
     * 1. TitleBar has no child.
     * 2. Titlebar is hidden and no titleBar animation is running.
     */
    auto translateState = navBarLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
    if ((translateState == BarTranslateState::NONE && navBarLayoutProperty->GetHideTitleBarValue(false)) ||
        titleBarNode->GetChildren().empty()) {
        constraint.selfIdealSize = OptionalSizeF(0.0f, 0.0f);
        titleBarWrapper->Measure(constraint);
        return 0.0f;
    }
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_RETURN(titleBarLayoutProperty, 0.0f);
    if (titleBarLayoutProperty->HasTitleHeight()) {
        auto titleHeight =
            titleBarLayoutProperty->GetTitleHeightValue().ConvertToPxWithSize(constraint.percentReference.Height());
        constraint.selfIdealSize.SetHeight(static_cast<float>(titleHeight));
        titleBarWrapper->Measure(constraint);
        return static_cast<float>(titleHeight);
    }

    auto singleLineTitleHeight = static_cast<float>(SINGLE_LINE_TITLEBAR_HEIGHT.ConvertToPx());
    auto doubleLineTitleBarHeight = static_cast<float>(DOUBLE_LINE_TITLEBAR_HEIGHT.ConvertToPx());
    if (Container::GreatOrEqualAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        doubleLineTitleBarHeight = static_cast<float>(SINGLE_LINE_TITLEBAR_HEIGHT.ConvertToPx());
    }
    // MINI mode
    if (navBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) == NavigationTitleMode::MINI) {
        if (titleBarNode->GetSubtitle()) {
            constraint.selfIdealSize =
                OptionalSizeF(navigationSize.Width(), doubleLineTitleBarHeight);
            titleBarWrapper->Measure(constraint);
            return doubleLineTitleBarHeight;
        }

        constraint.selfIdealSize =
            OptionalSizeF(navigationSize.Width(), singleLineTitleHeight);
        titleBarWrapper->Measure(constraint);
        return singleLineTitleHeight;
    }

    float titleBarHeight = 0.0f;
    if (navBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) == NavigationTitleMode::FREE) {
        auto titlePattern = titleBarNode->GetPattern<TitleBarPattern>();
        CHECK_NULL_RETURN(titlePattern, 0.0f);
        titleBarHeight = titlePattern->GetTempTitleBarHeight();
    }

    // FREE 和 FULL 模式，有subtitle
    if (titleBarNode->GetSubtitle()) {
        if (NearZero(titleBarHeight)) {
            titleBarHeight = static_cast<float>(FULL_DOUBLE_LINE_TITLEBAR_HEIGHT.ConvertToPx());
        }
        constraint.selfIdealSize = OptionalSizeF(navigationSize.Width(), titleBarHeight);
        titleBarWrapper->Measure(constraint);
        return titleBarHeight;
    }

    // no subtitle
    if (NearZero(titleBarHeight)) {
        titleBarHeight = static_cast<float>(FULL_SINGLE_LINE_TITLEBAR_HEIGHT.ConvertToPx());
    }
    constraint.selfIdealSize = OptionalSizeF(navigationSize.Width(), titleBarHeight);
    titleBarWrapper->Measure(constraint);
    return titleBarHeight;
}

MarginPropertyF GetNavigationMargin(const RefPtr<NavBarNode>& hostNode)
{
    CHECK_NULL_RETURN(hostNode, MarginPropertyF());
    auto parent = AceType::DynamicCast<NavigationGroupNode>(hostNode->GetParent());
    CHECK_NULL_RETURN(parent, MarginPropertyF());
    auto layoutProperty = parent->GetLayoutProperty();
    CHECK_NULL_RETURN(layoutProperty,  MarginPropertyF());
    return layoutProperty->CreateMargin();
}

bool CheckTopEdgeOverlap(const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty,
    const RefPtr<NavBarNode>& hostNode, const SafeAreaExpandOpts& opts)
{
    if (!navBarLayoutProperty || !hostNode) {
        return false;
    }
    const auto& margin = GetNavigationMargin(hostNode);
    float topMargin = margin.top.value_or(0.0f);
    const auto& padding = navBarLayoutProperty->CreatePaddingAndBorder();
    float topPadding = padding.top.value_or(0.0f);
    if (!NearEqual(topPadding, 0.0f) || !NearEqual(topMargin, 0.0f)) {
        return false;
    }

    auto parentGlobalOffset = hostNode->GetParentGlobalOffsetDuringLayout();
    auto NavBarGeometryNode = hostNode->GetGeometryNode();
    CHECK_NULL_RETURN(NavBarGeometryNode, false);
    auto frame = NavBarGeometryNode->GetFrameRect() + parentGlobalOffset;
    // only handle top-edge and system-type safeArea in current function
    if (!(opts.edges & SAFE_AREA_EDGE_TOP) || !(opts.type & SAFE_AREA_TYPE_SYSTEM)) {
        return false;
    }
    SafeAreaExpandOpts topSystemSafeAreaOpts = {.type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_TOP};
    auto pipeline = hostNode->GetContextWithCheck();
    CHECK_NULL_RETURN(pipeline, false);
    auto safeAreaManager = pipeline->GetSafeAreaManager();
    CHECK_NULL_RETURN(safeAreaManager, false);
    auto safeAreaPos = safeAreaManager->GetCombinedSafeArea(topSystemSafeAreaOpts);
    auto navBarPattern = hostNode->GetPattern<NavBarPattern>();
    CHECK_NULL_RETURN(navBarPattern, false);
    auto barStyle = navBarPattern->GetTitleBarStyle().value_or(BarStyle::STANDARD);
    if (!safeAreaPos.top_.IsOverlapped(frame.Top())) {
        return false;
    }
    if (navBarLayoutProperty->GetHideTitleBar().value_or(false) || barStyle == BarStyle::STACK ||
        (barStyle == BarStyle::SAFE_AREA_PADDING && !NearZero(navBarPattern->GetTitleBarOffsetY()))) {
        return true;
    }
    return false;
}

bool CheckBottomEdgeOverlap(const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty,
    const RefPtr<NavBarNode>& hostNode, SafeAreaExpandOpts opts)
{
    if (!navBarLayoutProperty || !hostNode) {
        return false;
    }
    const auto& margin = GetNavigationMargin(hostNode);
    float bottomMargin = margin.bottom.value_or(0.0f);
    const auto& padding = navBarLayoutProperty->CreatePaddingAndBorder();
    float bottomPadding = padding.bottom.value_or(0.0f);
    if (!NearEqual(bottomPadding, 0.0f) || !NearEqual(bottomMargin, 0.0f)) {
        return false;
    }

    auto parentGlobalOffset = hostNode->GetParentGlobalOffsetDuringLayout();
    auto NavBarGeometryNode = hostNode->GetGeometryNode();
    CHECK_NULL_RETURN(NavBarGeometryNode, false);
    auto frame = NavBarGeometryNode->GetFrameRect() + parentGlobalOffset;
    bool isToolBarVisible = hostNode->IsToolBarVisible();

    if ((opts.edges & SAFE_AREA_EDGE_BOTTOM) && (opts.type & SAFE_AREA_TYPE_SYSTEM)) {
        SafeAreaExpandOpts expandOpts = {.type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_BOTTOM};
        auto pipeline = hostNode->GetContextWithCheck();
        CHECK_NULL_RETURN(pipeline, false);
        auto safeAreaManager = pipeline->GetSafeAreaManager();
        CHECK_NULL_RETURN(safeAreaManager, false);
        auto safeAreaPos = safeAreaManager->GetCombinedSafeArea(expandOpts);
        if (safeAreaPos.bottom_.IsOverlapped(frame.Bottom()) && !isToolBarVisible) {
            return true;
        }
    }
    return false;
}

NavSafeArea CheckIgnoreLayoutSafeArea(const RefPtr<NavBarNode>& hostNode,
    const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty)
{
    NavSafeArea safeArea;
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_RETURN(pipeline, safeArea);
    auto inset = pipeline->GetSafeArea();
    auto opts = navBarLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});
    bool edgeTopOverLayCondition = CheckTopEdgeOverlap(navBarLayoutProperty, hostNode, opts);
    bool edgeBottomOverLayCondition = CheckBottomEdgeOverlap(navBarLayoutProperty, hostNode, opts);

    if (edgeTopOverLayCondition) {
        auto navBarContentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
        SafeAreaExpandOpts topParam = {.type = SAFE_AREA_TYPE_SYSTEM, .edges = SAFE_AREA_EDGE_NONE};
        navBarContentNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(topParam);

        if (navBarLayoutProperty->GetHideToolBar().value_or(false)) {
            auto navBarContentRenderContext = navBarContentNode->GetRenderContext();
            CHECK_NULL_RETURN(navBarContentRenderContext, safeArea);
            navBarContentRenderContext->UpdateClipEdge(false);
        }
        safeArea.top = static_cast<float>(inset.top_.Length());
    }

    if (edgeBottomOverLayCondition) {
        safeArea.bottom = static_cast<float>(inset.bottom_.Length());
    }
    return safeArea;
}

float MeasureContentChild(LayoutWrapper* layoutWrapper, const RefPtr<NavBarNode>& hostNode,
    const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty, const SizeF& navigationSize, float titleBarHeight,
    float toolBarHeight, float toolBarDividerHeight)
{
    auto contentNode = hostNode->GetContentNode();
    CHECK_NULL_RETURN(contentNode, 0.0f);
    auto index = hostNode->GetChildIndexById(contentNode->GetId());
    auto contentWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(contentWrapper, 0.0f);
    auto constraint = navBarLayoutProperty->CreateChildConstraint();
    float contentHeight = navigationSize.Height() - titleBarHeight - toolBarHeight - toolBarDividerHeight;
    if (NavigationLayoutAlgorithm::IsAutoHeight(navBarLayoutProperty)) {
        constraint.selfIdealSize.SetWidth(navigationSize.Width());
        contentWrapper->Measure(constraint);
        return static_cast<float>(contentWrapper->GetGeometryNode()->GetFrameSize().Height());
    }
    constraint.selfIdealSize = OptionalSizeF(navigationSize.Width(), contentHeight);
    auto safeArea = CheckIgnoreLayoutSafeArea(hostNode, navBarLayoutProperty);
    auto currentHeight = static_cast<float>(constraint.selfIdealSize.Height().value());
    constraint.selfIdealSize.SetHeight(currentHeight + safeArea.top + safeArea.bottom);
    contentWrapper->Measure(constraint);
    return currentHeight;
}

float LayoutTitleBar(LayoutWrapper* layoutWrapper, const RefPtr<NavBarNode>& hostNode,
    const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty, float decorBarHeight = 0.0f)
{
    /**
     * When all the following conditions are met, we consider the titleBar height to be 0:
     * 1. TitleBar should hide.
     * 2. No titleBar animation is running or titleBar was translate out of navigation area.
     */
    auto translateState = navBarLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
    if (navBarLayoutProperty->GetHideTitleBarValue(false) && translateState != BarTranslateState::TRANSLATE_ZERO) {
        return 0.0f;
    }
    auto titleBarNode = hostNode->GetTitleBarNode();
    CHECK_NULL_RETURN(titleBarNode, 0.0f);
    auto index = hostNode->GetChildIndexById(titleBarNode->GetId());
    auto titleBarWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_RETURN(titleBarWrapper, 0.0f);
    auto geometryNode = titleBarWrapper->GetGeometryNode();
    float offsetY = 0.0f;
    offsetY = NavigationTitleUtil::CalculateTitlebarOffset(titleBarNode);
    offsetY += decorBarHeight;
    auto titleBarOffset = OffsetF(0.0f, offsetY);
    auto navBarPattern = hostNode->GetPattern<NavBarPattern>();
    CHECK_NULL_RETURN(navBarPattern, 0.0f);
    navBarPattern->SetTitleBarOffsetY(offsetY);
    geometryNode->SetMarginFrameOffset(titleBarOffset);
    titleBarWrapper->Layout();
    return geometryNode->GetFrameSize().Height();
}

void LayoutContent(LayoutWrapper* layoutWrapper, const RefPtr<NavBarNode>& hostNode,
    const RefPtr<NavBarLayoutProperty>& navBarLayoutProperty, float titlebarHeight)
{
    auto contentNode = hostNode->GetContentNode();
    CHECK_NULL_VOID(contentNode);
    auto index = hostNode->GetChildIndexById(hostNode->GetContentNode()->GetId());
    auto contentWrapper = layoutWrapper->GetOrCreateChildByIndex(index);
    CHECK_NULL_VOID(contentWrapper);
    auto geometryNode = contentWrapper->GetGeometryNode();
    auto pattern = hostNode->GetPattern<NavBarPattern>();
    float avoidKeyboardOffset = pattern ? pattern->GetAvoidKeyboardOffset() : 0.0f;
    auto contentOffset = OffsetF(0.0f, avoidKeyboardOffset);
    contentOffset += OffsetF(geometryNode->GetFrameOffset().GetX(), titlebarHeight);
    auto safeArea = CheckIgnoreLayoutSafeArea(hostNode, navBarLayoutProperty);
    auto offsetY = contentOffset.GetY();
    auto opts = navBarLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});
    if ((opts.edges & SAFE_AREA_EDGE_TOP) && NearEqual(offsetY, 0.0f)) {
        offsetY -= safeArea.top;
        contentOffset.SetY(offsetY);
    }
    geometryNode->SetMarginFrameOffset(contentOffset);
    contentWrapper->Layout();
}


float TransferBarHeight(const RefPtr<NavBarNode>& hostNode, float originBarHeight, bool isTitleBar)
{
    auto navBarPattern = hostNode->GetPattern<NavBarPattern>();
    CHECK_NULL_RETURN(navBarPattern, originBarHeight);
    auto navBarLayoutProperty = hostNode->GetLayoutProperty<NavBarLayoutProperty>();
    CHECK_NULL_RETURN(navBarLayoutProperty, originBarHeight);
    auto barStyle = isTitleBar ? navBarPattern->GetTitleBarStyle() : navBarPattern->GetToolBarStyle();
    if (isTitleBar) {
        /**
         * In the follow scenarios, we need to convert titleBar's height to zero.
         * 1. TitleBar has translate out of the navigation area.
         * 2. Titlebar is hidden and no titleBar animation is running.
         */
        auto translateState = navBarLayoutProperty->GetTitleBarTranslateStateValue(BarTranslateState::NONE);
        if (translateState == BarTranslateState::TRANSLATE_HEIGHT ||
            (translateState == BarTranslateState::NONE && navBarLayoutProperty->GetHideTitleBarValue(false))) {
            return 0.0f;
        }
    } else {
        /**
         * In the follow scenarios, we need to convert toolBar's height to zero.
         * 1. ToolBar has translate out of the navigation area.
         * 2. Toolbar is hidden and no toolBar animation is running.
         */
        auto translateState = navBarLayoutProperty->GetToolBarTranslateStateValue(BarTranslateState::NONE);
        if (translateState == BarTranslateState::TRANSLATE_HEIGHT ||
            (translateState == BarTranslateState::NONE && navBarLayoutProperty->GetHideToolBarValue(false))) {
            return 0.0f;
        }
    }
    return barStyle.value_or(BarStyle::STANDARD) == BarStyle::STANDARD ? originBarHeight : 0.0f;
}
} // namespace

void NavBarLayoutAlgorithm::Measure(LayoutWrapper* layoutWrapper)
{
    auto hostNode = AceType::DynamicCast<NavBarNode>(layoutWrapper->GetHostNode());
    CHECK_NULL_VOID(hostNode);
    auto navBarPattern = hostNode->GetPattern<NavBarPattern>();
    CHECK_NULL_VOID(navBarPattern);
    auto navBarLayoutProperty = AceType::DynamicCast<NavBarLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(navBarLayoutProperty);
    const auto& constraint = navBarLayoutProperty->GetLayoutConstraint();
    CHECK_NULL_VOID(constraint);
    auto geometryNode = layoutWrapper->GetGeometryNode();
    auto size = CreateIdealSize(constraint.value(), Axis::HORIZONTAL, MeasureType::MATCH_PARENT, true);
    const auto& padding = layoutWrapper->GetLayoutProperty()->CreatePaddingAndBorder();
    MinusPaddingToSize(padding, size);
    NavigationLayoutUtil::UpdateTitleBarMenuNode(hostNode, size);
    float titleBarHeight = MeasureTitleBar(layoutWrapper, hostNode, navBarLayoutProperty, size);
    navBarPattern->MarkSafeAreaPaddingChangedWithCheckTitleBar(titleBarHeight);
    navBarPattern->SetTitleBarHeight(titleBarHeight);
    auto transferedTitleBarHeight = TransferBarHeight(hostNode, titleBarHeight, true);
    float toolBarHeight = NavigationLayoutUtil::MeasureToolBar(layoutWrapper, hostNode, navBarLayoutProperty, size);
    navBarPattern->SetToolBarHeight(toolBarHeight);
    // after the visibility of title/tool bar determined, update safeAreaPadding of content node if needed.
    NavigationLayoutUtil::UpdateContentSafeAreaPadding(hostNode, titleBarHeight);
    auto transferedToolBarHeight = TransferBarHeight(hostNode, toolBarHeight, false);
    float toolBarDividerHeight = NavigationLayoutUtil::MeasureToolBarDivider(
        layoutWrapper, hostNode, navBarLayoutProperty, size, transferedToolBarHeight);
    navBarPattern->SetToolBarDividerHeight(toolBarDividerHeight);
    auto transferedToolBarDividerHeight = TransferBarHeight(hostNode, toolBarDividerHeight, false);
    float contentChildHeight = MeasureContentChild(layoutWrapper, hostNode, navBarLayoutProperty, size,
        transferedTitleBarHeight, transferedToolBarHeight, transferedToolBarDividerHeight);
    size.SetHeight(
        transferedTitleBarHeight + transferedToolBarHeight + transferedToolBarDividerHeight + contentChildHeight);
    layoutWrapper->GetGeometryNode()->SetFrameSize(size);
}

void NavBarLayoutAlgorithm::Layout(LayoutWrapper* layoutWrapper)
{
    auto hostNode = AceType::DynamicCast<NavBarNode>(layoutWrapper->GetHostNode());
    CHECK_NULL_VOID(hostNode);
    auto navBarLayoutProperty = AceType::DynamicCast<NavBarLayoutProperty>(layoutWrapper->GetLayoutProperty());
    CHECK_NULL_VOID(navBarLayoutProperty);

    auto opts = navBarLayoutProperty->GetIgnoreLayoutSafeAreaValue({.type = SAFE_AREA_TYPE_NONE,
        .edges = SAFE_AREA_TYPE_NONE});
    bool edgeTopOverLayCondition = CheckTopEdgeOverlap(navBarLayoutProperty, hostNode, opts);
    bool edgeBottomOverLayCondition = CheckBottomEdgeOverlap(navBarLayoutProperty, hostNode, opts);
    if (edgeTopOverLayCondition || edgeBottomOverLayCondition) {
        Measure(layoutWrapper);
    }
    float decorBarHeight = 0.0f;
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(hostNode->GetNavigationNode());
    if (navigationNode) {
        auto navigationPattern = navigationNode->GetPattern<NavigationPattern>();
        auto navBarPattern = hostNode->GetPattern<NavBarPattern>();
        if (navigationPattern && navBarPattern) {
            auto toolbarManager = navigationPattern->GetToolBarManager();
            auto barStyle = navBarPattern->GetTitleBarStyle().value_or(BarStyle::STANDARD);
            if (toolbarManager && toolbarManager->GetIsMoveUp() && barStyle == BarStyle::STACK) {
                decorBarHeight = static_cast<float>(toolbarManager->GetTitleHeight().ConvertToPx());
            }
        }
    }
    float titlebarHeight = LayoutTitleBar(layoutWrapper, hostNode, navBarLayoutProperty, decorBarHeight);
    auto transferedTitleBarHeight = TransferBarHeight(hostNode, titlebarHeight, true);
    LayoutContent(layoutWrapper, hostNode, navBarLayoutProperty, transferedTitleBarHeight);
    float toolbarHeight = NavigationLayoutUtil::LayoutToolBar(
        layoutWrapper, hostNode, navBarLayoutProperty, false);
    NavigationLayoutUtil::LayoutToolBarDivider(
        layoutWrapper, hostNode, navBarLayoutProperty, toolbarHeight, false);
}
} // namespace OHOS::Ace::NG
