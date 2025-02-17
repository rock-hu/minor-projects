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

#include "core/components_ng/pattern/navigation/navigation_pattern.h"

#include "base/log/dump_log.h"
#include "base/log/event_report.h"
#include "base/perfmonitor/perf_constants.h"
#include "core/common/ime/input_method_manager.h"
#include "core/components_ng/pattern/navigation/nav_bar_node.h"
#include "core/components_ng/pattern/navigation/nav_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_drag_bar_pattern.h"
#include "core/components_ng/pattern/navigation/navigation_model_data.h"
#include "core/components_ng/pattern/navigation/navigation_title_util.h"
#include "core/components_ng/pattern/navigation/title_bar_pattern.h"
#include "core/components_ng/pattern/navigation/tool_bar_node.h"
#include "core/components_ng/pattern/navigation/tool_bar_pattern.h"

#ifdef WINDOW_SCENE_SUPPORTED
#include "core/components_ng/pattern/window_scene/helper/window_scene_helper.h"
#endif
namespace OHOS::Ace::NG {

constexpr int32_t NAVIMODE_CHANGE_ANIMATION_DURATION = 250;
constexpr int32_t OPACITY_ANIMATION_DURATION_APPEAR = 150;
constexpr int32_t OPACITY_ANIMATION_DURATION_DISAPPEAR = 250;
constexpr int32_t EMPTY_DESTINATION_CHILD_SIZE = 1;
constexpr Dimension DEFAULT_DRAG_REGION = 12.0_vp;
constexpr Dimension DEFAULT_DRAG_BAR_HOT_ZONE = 12.0_vp;
constexpr float DEFAULT_HALF = 2.0f;
const Color MASK_COLOR = Color::FromARGB(25, 0, 0, 0);
constexpr int32_t PAGE_NODES = 1000;
constexpr int32_t PAGE_DEPTH = 300;
constexpr int32_t HALF_POSITION = 50;
constexpr int32_t END_POSITION = 100;
constexpr Dimension DRAG_BAR_RADIUS = 6.0_vp;
constexpr Dimension DRAG_BAR_BLUR_RADIUS = 20.0_vp;
constexpr Dimension DRAG_BAR_ITEM_RADIUS = 1.0_vp;
constexpr int32_t SECOND_ZINDEX_VALUE = 2;
constexpr int32_t INVALID_ANIMATION_ID = -1;
namespace {
constexpr int32_t MODE_SWITCH_ANIMATION_DURATION = 500; // ms
const RefPtr<CubicCurve> MODE_SWITCH_CURVE = AceType::MakeRefPtr<CubicCurve>(0.2f, 0.2f, 0.1f, 1.0f);

GradientColor CreatePercentGradientColor(int32_t percent, Color color)
{
    NG::GradientColor gredient = GradientColor(color);
    gredient.SetDimension(CalcDimension(percent, DimensionUnit::PERCENT));
    return gredient;
}

void BuildNavDestinationInfoFromContext(const std::string& navigationId, NavDestinationState state,
    const RefPtr<NavDestinationContext>& context, bool isFrom, std::optional<NavDestinationInfo>& info)
{
    if (!context) {
        info.reset();
        return;
    }

    int32_t index = isFrom ? context->GetPreIndex() : context->GetIndex();
    std::string navDestinationId = std::to_string(context->GetNavDestinationId());
    std::string name;
    napi_value param = nullptr;
    auto pathInfo = context->GetNavPathInfo();
    if (pathInfo) {
        name = pathInfo->GetName();
        param = pathInfo->GetParamObj();
    }
    NavDestinationMode mode = context->GetMode();
    int32_t uniqueId = context->GetUniqueId();
    info = std::make_optional<NavDestinationInfo>(navigationId, name, state, index, param,
        navDestinationId, mode, uniqueId);
}

void LogCustomAnimationStart(const RefPtr<NavDestinationGroupNode>& preTopDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, NavigationOperation operation)
{
    RefPtr<NavDestinationPattern> prePattern =
        preTopDestination ? preTopDestination->GetPattern<NavDestinationPattern>() : nullptr;
    RefPtr<NavDestinationPattern> newPattern =
        newTopNavDestination ? newTopNavDestination->GetPattern<NavDestinationPattern>() : nullptr;
    TAG_LOGI(AceLogTag::ACE_NAVIGATION,
        "custom animation start: operation: %{public}d, pre name: %{public}s, id: %{public}s."
        "top name: %{public}s, id: %{public}s",
        operation, prePattern ? prePattern->GetName().c_str() : "null",
        prePattern ? std::to_string(prePattern->GetNavDestinationId()).c_str() : "null",
        newPattern ? newPattern->GetName().c_str() : "null",
        newPattern ? std::to_string(newPattern->GetNavDestinationId()).c_str() : "null");
}

int32_t TriggerNavDestinationTransition(const RefPtr<NavDestinationGroupNode>& navDestination,
    NavigationOperation operation, bool isEnter)
{
    CHECK_NULL_RETURN(navDestination, INVALID_ANIMATION_ID);
    return navDestination->DoTransition(operation, isEnter);
}
} // namespace

NavigationPattern::NavigationPattern()
{
    navigationController_ = std::make_shared<InnerNavigationController>(WeakClaim(this), Container::CurrentId());
}

RefPtr<RenderContext> NavigationPattern::GetTitleBarRenderContext()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, nullptr);
    auto layoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_RETURN(layoutProperty, nullptr);
    auto contentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
    CHECK_NULL_RETURN(contentNode, nullptr);
    if (contentNode->FindChildNodeOfClass<NavDestinationGroupNode>()) {
        auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
        CHECK_NULL_RETURN(navBarNode, nullptr);
        auto renderContext = navBarNode->GetRenderContext();
        return renderContext;
    } else {
        auto renderContext = contentNode->GetRenderContext();
        return renderContext;
    }
}

void NavigationPattern::DoAnimation(NavigationMode usrNavigationMode)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto layoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);

    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    layoutProperty->UpdateNavigationMode(navigationMode_);
    hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    AnimationOption option = AnimationOption();
    option.SetDuration(NAVIMODE_CHANGE_ANIMATION_DURATION);
    option.SetCurve(Curves::FRICTION);
    option.SetFillMode(FillMode::FORWARDS);
    AnimationOption optionAlpha = AnimationOption();
    optionAlpha.SetCurve(Curves::SHARP);
    optionAlpha.SetFillMode(FillMode::FORWARDS);
    auto renderContext = GetTitleBarRenderContext();
    CHECK_NULL_VOID(renderContext);

    std::function<void()> finishCallback = [optionAlpha, renderContext, hostNode]() {
        renderContext->OpacityAnimation(optionAlpha, 0, 1);
        hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    };

    context->OpenImplicitAnimation(option, option.GetCurve(), finishCallback);
    layoutProperty->UpdateNavigationMode(usrNavigationMode);
    hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    context->FlushUITasks();
    if (usrNavigationMode == NavigationMode::STACK || navigationMode_ == NavigationMode::SPLIT) {
        optionAlpha.SetDuration(OPACITY_ANIMATION_DURATION_DISAPPEAR);
        renderContext->OpacityAnimation(optionAlpha, 1, 0);
    } else if (usrNavigationMode == NavigationMode::SPLIT || navigationMode_ == NavigationMode::STACK) {
        optionAlpha.SetDuration(OPACITY_ANIMATION_DURATION_APPEAR);
        renderContext->OpacityAnimation(optionAlpha, 0, 1);
    }
    context->CloseImplicitAnimation();
    navigationMode_ = usrNavigationMode;
}

void NavigationPattern::OnAttachToFrameNode()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    pipelineContext->AddWindowStateChangedCallback(host->GetId());
    pipelineContext->AddWindowSizeChangeCallback(host->GetId());

    auto theme = NavigationGetTheme();
    if (theme && theme->GetNavBarUnfocusEffectEnable()) {
        pipelineContext->AddWindowFocusChangedCallback(host->GetId());
    }
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_ELEVEN)) {
        SafeAreaExpandOpts opts = { .type = SAFE_AREA_TYPE_ALL, .edges = SAFE_AREA_EDGE_ALL };
        host->GetLayoutProperty()->UpdateSafeAreaExpandOpts(opts);
    }
}

void NavigationPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
    auto id = frameNode->GetId();
    auto pipeline = AceType::DynamicCast<PipelineContext>(PipelineBase::GetCurrentContext());
    CHECK_NULL_VOID(pipeline);
    pipeline->RemoveWindowStateChangedCallback(id);
    pipeline->RemoveWindowSizeChangeCallback(id);
}


void NavigationPattern::DoNavbarHideAnimation(const RefPtr<NavigationGroupNode>& hostNode)
{
    AnimationOption option;
    option.SetCurve(MODE_SWITCH_CURVE);
    option.SetFillMode(FillMode::FORWARDS);
    option.SetDuration(MODE_SWITCH_ANIMATION_DURATION);
    AnimationUtils::Animate(option, [weakHost = WeakPtr<NavigationGroupNode>(hostNode)]() {
        auto hostNode = weakHost.Upgrade();
        CHECK_NULL_VOID(hostNode);
        auto layoutProperty = AceType::DynamicCast<NavigationLayoutProperty>(hostNode->GetLayoutProperty());
        CHECK_NULL_VOID(layoutProperty);
        bool hideNavBar = layoutProperty->GetHideNavBarValue(false);
        auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
        CHECK_NULL_VOID(navBarNode);
        auto navBarLayoutProperty = navBarNode->GetLayoutProperty();
        CHECK_NULL_VOID(navBarLayoutProperty);
        navBarLayoutProperty->UpdateVisibility(hideNavBar ? VisibleType::INVISIBLE : VisibleType::VISIBLE, true);
        hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        hostNode->GetContext()->FlushUITasks();
    });
}

void NavigationPattern::InitDragBarEvent()
{
    auto dragBarNode = AceType::DynamicCast<FrameNode>(GetDragBarNode());
    CHECK_NULL_VOID(dragBarNode);
    auto dragGestureHub = dragBarNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dragGestureHub);
    InitDragBarPanEvent(dragGestureHub);
    InitTouchEvent(dragGestureHub);

    // clear divider hover and pan event
    auto dividerNode = GetDividerNode();
    CHECK_NULL_VOID(dividerNode);
    auto dividerGestureHub = dividerNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dividerGestureHub);
    auto dividerInputHub = dividerNode->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(dividerInputHub);
    if (hoverEvent_) {
        dividerInputHub->RemoveOnHoverEvent(hoverEvent_);
        hoverEvent_.Reset();
    }
    if (panEvent_) {
        dividerGestureHub->RemovePanEvent(panEvent_);
        panEvent_.Reset();
    }
}

void NavigationPattern::ClearDragBarEvent()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto dragBarNode = AceType::DynamicCast<FrameNode>(GetDragBarNode());
    CHECK_NULL_VOID(dragBarNode);
    auto dragGestureHub = dragBarNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dragGestureHub);

    // clear drag bar touch and pan event
    if (touchEvent_) {
        dragGestureHub->RemoveTouchEvent(touchEvent_);
        touchEvent_.Reset();
    }
    if (dragBarPanEvent_) {
        dragGestureHub->RemovePanEvent(dragBarPanEvent_);
        dragBarPanEvent_.Reset();
    }

    hostNode->RemoveChild(dragBarNode);
    hostNode->SetDragBarNode(nullptr);
}

void NavigationPattern::BuildDragBar()
{
    if (!AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_TEN)) {
        return;
    }
    if (enableDragBar_) {
        if (GetDragBarNode()) {
            // if dragBar is already in navigation, do nothing
            return;
        }
        // create drag bar and init drag bar gesture event
        auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
        CHECK_NULL_VOID(hostNode);
        CreateDragBarNode(hostNode);
        InitDragBarEvent();
        return;
    }
    auto dividerNode = GetDividerNode();
    CHECK_NULL_VOID(dividerNode);
    auto dividerGestureHub = dividerNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dividerGestureHub);
    auto dividerInputHub = dividerNode->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(dividerInputHub);
    InitDividerPanEvent(dividerGestureHub);
    InitDividerMouseEvent(dividerInputHub);
    if (GetDragBarNode()) {
        // clear drag bar gesture event and remove dragBar
        ClearDragBarEvent();
    }
}

void NavigationPattern::OnModifyDone()
{
    // !!! Do not add operations about NavPathStack here, see @SyncWithJsStackIfNeeded
    Pattern::OnModifyDone();
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    navBarNode->MarkModifyDone();
    isRightToLeft_ = AceApplicationInfo::GetInstance().IsRightToLeft();

    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    BuildDragBar();

    auto layoutProperty = hostNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto curNavBarPosition = layoutProperty->GetNavBarPositionValue(NavBarPosition::START);
    if (preNavBarPosition_.has_value() && preNavBarPosition_.value() != curNavBarPosition) {
        MarkAllNavDestinationDirtyIfNeeded(hostNode);
    }
    preNavBarPosition_ = curNavBarPosition;

    auto&& opts = layoutProperty->GetSafeAreaExpandOpts();
    if (opts) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "Navigation SafArea expand as %{public}s", opts->ToString().c_str());
        uint8_t ignoreExpandKeyboard = 0x11;
        SafeAreaExpandOpts optsExceptKeyboard = { .type = opts->type & ignoreExpandKeyboard,
            .edges = opts->edges };
        navBarNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(optsExceptKeyboard);
        navBarNode->MarkModifyDone();

        auto navigationContentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
        CHECK_NULL_VOID(navigationContentNode);
        navigationContentNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(optsExceptKeyboard);
        navigationContentNode->MarkModifyDone();

        auto dividerNode = AceType::DynamicCast<FrameNode>(hostNode->GetDividerNode());
        CHECK_NULL_VOID(dividerNode);
        dividerNode->GetLayoutProperty()->UpdateSafeAreaExpandOpts(optsExceptKeyboard);
        dividerNode->MarkModifyDone();
    }

    bool enableModeChangeAnimation = layoutProperty->GetEnableModeChangeAnimation().value_or(true);
    if (enableModeChangeAnimation && GetNavigationMode() == NavigationMode::SPLIT && GetNavBarVisibilityChange()) {
        DoNavbarHideAnimation(hostNode);
    }

    // AddRecoverableNavigation function will check inside whether current navigation can be recovered
    pipeline->GetNavigationManager()->AddRecoverableNavigation(hostNode->GetCurId(), hostNode);
    RestoreJsStackIfNeeded();
}

void NavigationPattern::SetSystemBarStyle(const RefPtr<SystemBarStyle>& style)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_VOID(windowManager);
    if (!backupStyle_.has_value()) {
        backupStyle_ = windowManager->GetSystemBarStyle();
    }
    currStyle_ = style;

    // The systemBarStyle may only take effect when navigation fills the entire page.
    if (!isFullPageNavigation_) {
        return;
    }

    // When there is NavDestination in the stack, the systemBarStyle set for Navigation does not take effect.
    do {
        if (!navigationStack_) {
            break;
        }
        auto topPath = navigationStack_->GetTopNavPath();
        if (!topPath.has_value()) {
            break;
        }
        auto topNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(topPath->second));
        if (topNavDestination) {
            return;
        }
    } while (false);

    /**
     * When developers provide a valid style to systemBarStyle, we should set the style to window;
     * when 'undefined' was provided, we should restore the style.
     */
    if (currStyle_.value() != nullptr) {
        windowManager->SetSystemBarStyle(currStyle_.value());
    } else {
        TryRestoreSystemBarStyle(windowManager);
    }
}

void NavigationPattern::OnAttachToMainTree()
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    InitPageNode(host);
    InitFoldState();
    RegisterContainerModalButtonsRectChangeListener(host);
}

void NavigationPattern::InitFoldState()
{
    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    container->InitIsFoldable();
    if (container->IsFoldable()) {
        currentFoldStatus_ = container->GetCurrentFoldStatus();
    }
}

void NavigationPattern::OnDetachFromMainTree()
{
    isFullPageNavigation_ = false;
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    UnregisterContainerModalButtonsRectChangeListener(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_VOID(windowManager);
    TryRestoreSystemBarStyle(windowManager);
    backupStyle_.reset();
    currStyle_.reset();
    pageNode_ = nullptr;
}

bool NavigationPattern::IsTopNavDestination(const RefPtr<UINode>& node) const
{
    if (!navigationStack_) {
        return false;
    }
    auto topPath = navigationStack_->GetTopNavPath();
    if (!topPath.has_value()) {
        return false;
    }
    auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(
        NavigationGroupNode::GetNavDestinationNode(topPath->second));
    return navDestination == node;
}

bool NavigationPattern::JudgeFoldStateChangeAndUpdateState()
{
    auto container = Container::Current();
    CHECK_NULL_RETURN(container, false);
    auto foldStatus = container->GetCurrentFoldStatus();
    TAG_LOGI(AceLogTag::ACE_NAVIGATION, "newFoldStatus: %{public}d, currentFoldStatus: %{public}d.",
        static_cast<int32_t>(foldStatus), static_cast<int32_t>(currentFoldStatus_));
    if (foldStatus != currentFoldStatus_) {
        currentFoldStatus_ = foldStatus;
        return true;
    }
    return false;
}

void NavigationPattern::UpdateIsFullPageNavigation(const RefPtr<FrameNode>& host)
{
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto frame = geometryNode->GetFrameRect();
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_VOID(windowManager);

    bool isFullPage = false;
    auto pageNode = pageNode_.Upgrade();
    if (pageNode) {
        auto pageNodeGeometryNode = pageNode->GetGeometryNode();
        if (pageNodeGeometryNode) {
            auto pageFrame = pageNodeGeometryNode->GetFrameRect();
            isFullPage = pageFrame.GetSize() == frame.GetSize();
        }
    }
    pageNode = nullptr;

    if (isFullPage == isFullPageNavigation_) {
        return;
    }

    isFullPageNavigation_ = isFullPage;
    MarkAllNavDestinationDirtyIfNeeded(host);
    UpdateSystemBarStyleOnFullPageStateChange(windowManager);
    if (isFullPageNavigation_) {
        RegisterPageVisibilityChangeCallback();
    }
}

void NavigationPattern::UpdateSystemBarStyleOnFullPageStateChange(const RefPtr<WindowManager>& windowManager)
{
    // full page -> partial page
    if (!isFullPageNavigation_) {
        TryRestoreSystemBarStyle(windowManager);
        return;
    }

    // partial page -> full page
    auto topPath = navigationStack_->GetTopNavPath();
    UpdateSystemBarStyleWithTopNavPath(windowManager, topPath);
}

void NavigationPattern::UpdateSystemBarStyleOnTopNavPathChange(
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& newTopNavPath)
{
    if (!isFullPageNavigation_) {
        return;
    }

    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_VOID(windowManager);
    UpdateSystemBarStyleWithTopNavPath(windowManager, newTopNavPath);
}

void NavigationPattern::UpdateSystemBarStyleWithTopNavPath(const RefPtr<WindowManager>& windowManager,
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& topNavPath)
{
    if (ApplyTopNavPathSystemBarStyleOrRestore(windowManager, topNavPath)) {
        return;
    }

    if (currStyle_.has_value() && currStyle_.value() != nullptr) {
        windowManager->SetSystemBarStyle(currStyle_.value());
    } else {
        TryRestoreSystemBarStyle(windowManager);
    }
}

void NavigationPattern::TryRestoreSystemBarStyle(const RefPtr<WindowManager>& windowManager)
{
    if (backupStyle_.has_value()) {
        windowManager->SetSystemBarStyle(backupStyle_.value());
    }
}

void NavigationPattern::UpdateSystemBarStyleOnPageVisibilityChange(bool show)
{
    if (!isFullPageNavigation_) {
        return;
    }

    CHECK_NULL_VOID(navigationStack_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto windowManager = pipeline->GetWindowManager();
    CHECK_NULL_VOID(windowManager);
    if (show) {
        // page containing Navigation, hide -> show
        auto topPath = navigationStack_->GetTopNavPath();
        UpdateSystemBarStyleWithTopNavPath(windowManager, topPath);
    } else {
        // page containing Navigation, show -> hide
        TryRestoreSystemBarStyle(windowManager);
    }
}

void NavigationPattern::RegisterPageVisibilityChangeCallback()
{
    auto pageNode = pageNode_.Upgrade();
    CHECK_NULL_VOID(pageNode);
    RefPtr<PagePattern> pagePattern = pageNode->GetPattern<PagePattern>();
    CHECK_NULL_VOID(pagePattern);
    auto callback = [weak = WeakClaim(this)](bool show) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        // we need update the "systemBarStyle" at the beginning of the transition animation on the router page
        pattern->UpdateSystemBarStyleOnPageVisibilityChange(show);
    };
    pagePattern->SetPageVisibilityChangeCallback(std::move(callback));
}

bool NavigationPattern::ApplyTopNavPathSystemBarStyleOrRestore(
    const RefPtr<WindowManager>& windowManager,
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& topNavPath)
{
    if (!topNavPath.has_value()) {
        return false;
    }

    auto topNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
        NavigationGroupNode::GetNavDestinationNode(topNavPath->second));
    if (!topNavDestination) {
        return false;
    }

    auto navDestinationPattern = topNavDestination->GetPattern<NavDestinationPattern>();
    if (!navDestinationPattern) {
        return false;
    }
    /**
     * Backup is only performed when the developer sets the "systemBarStyle" attribute,
     * and the entire Navigation is only backed up once.
     * Therefore, when developer only set the "systemBarStyle" attribute to NavDestination, we need to
     * save the attribute to Navigation.
     */
    auto backupFromNavDestination = navDestinationPattern->GetBackupStyle();
    if (!backupStyle_.has_value() && backupFromNavDestination.has_value()) {
        backupStyle_ = backupFromNavDestination;
    }

    auto destCurrStyle = navDestinationPattern->GetCurrentStyle();
    if (destCurrStyle.has_value() && destCurrStyle.value() != nullptr) {
        windowManager->SetSystemBarStyle(destCurrStyle.value());
    } else {
        TryRestoreSystemBarStyle(windowManager);
    }
    return true;
}

void NavigationPattern::InitPageNode(const RefPtr<FrameNode>& host)
{
    CHECK_NULL_VOID(host);
    auto parent = host->GetParent();
    CHECK_NULL_VOID(parent);
    RefPtr<FrameNode> pageNode = nullptr;
    while (parent) {
        if (parent->GetTag() == V2::PAGE_ETS_TAG) {
            pageNode = AceType::DynamicCast<FrameNode>(parent);
            break;
        }
        parent = parent->GetParent();
    }
    if (!pageNode) {
        TAG_LOGE(AceLogTag::ACE_NAVIGATION, "Failed to find PageNode of Navigation");
    } else {
        pageNode_ = WeakPtr<FrameNode>(pageNode);
    }
}

void NavigationPattern::OnLanguageConfigurationUpdate()
{
    bool isRightToLeft = AceApplicationInfo::GetInstance().IsRightToLeft();
    if (isRightToLeft != isRightToLeft_) {
        auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
        CHECK_NULL_VOID(hostNode);
        hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        isRightToLeft_ = isRightToLeft;
    }
}

void NavigationPattern::SyncWithJsStackIfNeeded()
{
    if (!needSyncWithJsStack_) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION,
            "not need SyncWithJsStack, needSyncWithJsStack_ %{public}d", needSyncWithJsStack_);
        return;
    }
    CHECK_NULL_VOID(navigationStack_);
    needSyncWithJsStack_ = false;
    if (!isFinishInteractiveAnimation_) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION,
            "not need SyncWithJsStack, interactive animation: %{public}d", isFinishInteractiveAnimation_);
        return;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    TAG_LOGI(AceLogTag::ACE_NAVIGATION,
        "sync with js stack, id: %{public}s, UINodeId: %{public}d, preStackSize: %{public}d, newStackSize: %{public}d",
        hostNode->GetCurId().c_str(), hostNode->GetId(), navigationStack_->PreSize(),
        static_cast<int32_t>(navigationStack_->GetAllPathName().size()));
    preTopNavPath_ = navigationStack_->GetPreTopNavPath();
    preStackSize_ = navigationStack_->PreSize();
    preContext_ = nullptr;
    if (preTopNavPath_.has_value()) {
        auto preDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(preTopNavPath_->second));
        if (preDestination) {
            auto pattern = AceType::DynamicCast<NavDestinationPattern>(preDestination->GetPattern());
            preContext_ = pattern->GetNavDestinationContext();
            if (preContext_) {
                preContext_->SetPreIndex(preStackSize_ - 1);
            }
        }
    }
    if (isCustomAnimation_) {
        navigationStack_->UpdateRecoveryList();
    }
    navigationStack_->SavePreNavList();
    UpdateNavPathList();
    auto newTopNavPath = navigationStack_->GetTopNavPath();
    auto replaceValue = navigationStack_->GetReplaceValue();
    if (preTopNavPath_ != newTopNavPath || replaceValue == 1) {
        isReplace_ = replaceValue != 0;
        UpdateIsAnimation(preTopNavPath_);
        lastPreIndex_ = 0;
        if (preTopNavPath_.has_value()) {
            lastPreIndex_ = navigationStack_->FindIndex(preTopNavPath_->first, preTopNavPath_->second, true);
        }
        FireInterceptionEvent(true, newTopNavPath);
        if (needSyncWithJsStack_) {
            TAG_LOGI(AceLogTag::ACE_NAVIGATION, "sync with js stack in before interception");
            UpdateNavPathList();
            needSyncWithJsStack_ = false;
        }
    }
    RefreshNavDestination();
}

void NavigationPattern::UpdateNavPathList()
{
    CHECK_NULL_VOID(navigationStack_);
    auto pathNames = navigationStack_->GetAllPathName();
    auto indexes = navigationStack_->GetAllPathIndex();
    navigationStack_->InitNavPathIndex(pathNames);
    auto cacheNodes = navigationStack_->GetAllCacheNodes();
    NavPathList navPathList;
    int32_t pathListSize = static_cast<int32_t>(pathNames.size());
    isCurTopNewInstance_ = false;
    // lastRecoveredStandardIndex will be only used in recovery case
    int32_t lastRecoveredStandardIndex = 0;
    int32_t removeSize = 0; // push destination failed size
    for (int32_t index = 0; index < pathListSize; ++index) {
        auto pathName = pathNames[index];
        RefPtr<UINode> uiNode = nullptr;
        int32_t arrayIndex = index - removeSize;
        if (navigationStack_->IsFromRecovery(arrayIndex)) {
            if (navigationStack_->GetRecoveredDestinationMode(arrayIndex) ==
                static_cast<int32_t>(NavDestinationMode::STANDARD)) {
                lastRecoveredStandardIndex = arrayIndex;
            }
            navPathList.emplace_back(std::make_pair(pathName, uiNode));
            // only create recovery node when it is at top
            if (index == pathListSize - 1) {
                removeSize += GenerateUINodeFromRecovery(lastRecoveredStandardIndex, navPathList);
            }
            continue;
        }
        auto pathIndex = indexes[index];
        if (navigationStack_->NeedBuildNewInstance(arrayIndex)) {
            // if marked NEW_INSTANCE when push/replace in frontend, build a new instance anyway
            if (!GenerateUINodeByIndex(arrayIndex, uiNode)) {
                removeSize++;
                continue;
            }
            navPathList.emplace_back(std::make_pair(pathName, uiNode));
            navigationStack_->SetNeedBuildNewInstance(arrayIndex, false);
            if (index == pathListSize - 1) {
                isCurTopNewInstance_ = true;
            }
            continue;
        }
        if (index == pathListSize - 1 && addByNavRouter_) {
            addByNavRouter_ = false;
            uiNode = navigationStack_->Get();
        } else {
            uiNode = navigationStack_->Get(pathIndex);
        }
        if (uiNode) {
            TAG_LOGD(AceLogTag::ACE_NAVIGATION, "find in list, navigation stack reserve node, "
                "old index: %{public}d, index: %{public}d, removeSize: %{public}d, name: %{public}s.",
                pathIndex, index, removeSize, pathName.c_str());
            /**
             * If we call the function pushPath/pushDestination with singleton mode(
             * LaunchMode == MOVE_TO_TOP_SINGLETON/POP_TO_SINGLETON), and the top NavDestination of stack
             * is the NavDestination which we need to push(NavDestination's name == NavPathInfo's name),
             * then wee need to update the NavDestination's parameters.
             */
            navigationStack_->UpdatePathInfoIfNeeded(uiNode, arrayIndex);
            auto navDestinationGroupNode = AceType::DynamicCast<NavDestinationGroupNode>(
                NavigationGroupNode::GetNavDestinationNode(uiNode));
            if (navDestinationGroupNode && navDestinationGroupNode->GetCanReused()) {
                navPathList.emplace_back(std::make_pair(pathName, uiNode));
                continue;
            }
        }
        uiNode = navigationStack_->GetFromCacheNode(cacheNodes, pathName);
        if (uiNode) {
            TAG_LOGI(AceLogTag::ACE_NAVIGATION, "find in cached node, navigation stack reserve node, "
                "index: %{public}d, removeSize: %{public}d, name: %{public}s.", index, removeSize, pathName.c_str());
            navPathList.emplace_back(std::make_pair(pathName, uiNode));
            navigationStack_->RemoveCacheNode(cacheNodes, pathName, uiNode);
            auto navDestination =
                DynamicCast<NavDestinationGroupNode>(NavigationGroupNode::GetNavDestinationNode(uiNode));
            if (navDestination) {
                auto eventHub = navDestination->GetEventHub<EventHub>();
                CHECK_NULL_VOID(eventHub);
                eventHub->SetEnabledInternal(true);
            }
            continue;
        }
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "find in nowhere, navigation stack create new node, "
            "index: %{public}d, removeSize: %{public}d, name: %{public}s.", index, removeSize, pathName.c_str());
        if (!GenerateUINodeByIndex(arrayIndex, uiNode)) {
            std::string replacedName = "";
            int32_t replacedIndex = -1;
            if (navigationStack_->CheckIsReplacedDestination(arrayIndex, replacedName, replacedIndex)) {
                navigationStack_->SetRecoveryFromReplaceDestination(arrayIndex, false);
                pathNames[index] = replacedName;
                indexes[index] = replacedIndex;
                index--;
                continue;
            }
            removeSize++;
            continue;
        }
        navPathList.emplace_back(std::make_pair(pathName, uiNode));
    }
    navigationStack_->SetNavPathList(navPathList);
}

void NavigationPattern::RefreshNavDestination()
{
    isChanged_ = false;
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto preTopNavPath = std::move(preTopNavPath_);
    auto preLastStandardIndex = hostNode->GetLastStandardIndex();
    auto& navPathList = navigationStack_->GetAllNavDestinationNodes();
    hostNode->UpdateNavDestinationNodeWithoutMarkDirty(
        preTopNavPath.has_value() ? preTopNavPath->second : nullptr, navigationModeChange_);
    auto newTopNavPath = navigationStack_->GetTopNavPath();
#if defined(ENABLE_NAV_SPLIT_MODE)
    isBackPage_ = newTopNavPath.has_value() ?
        navigationStack_->isLastListContains(newTopNavPath->first, newTopNavPath->second) : false;
#endif
    std::string navDestinationName = "";
    CheckTopNavPathChange(preTopNavPath, newTopNavPath, preLastStandardIndex);

    // close keyboard
#if defined(ENABLE_STANDARD_INPUT)
    RefPtr<FrameNode> targetNode = newTopNavPath.has_value() ? AceType::DynamicCast<FrameNode>(
            NavigationGroupNode::GetNavDestinationNode(newTopNavPath->second)) :
            AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
    if (isChanged_ && GetIsFocusable(targetNode)) {
        InputMethodManager::GetInstance()->CloseKeyboard();
    }
#endif

#if defined(ENABLE_NAV_SPLIT_MODE)
    navigationStack_->SetLastNavPathList(navPathList);
#endif

    /* if first navDestination is removed, the new one will be refreshed */
    if (!navPathList.empty()) {
        auto firstNavDesNode = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(navPathList.front().second));
        CHECK_NULL_VOID(firstNavDesNode);
        firstNavDesNode->MarkModifyDone();
    }

    if (newTopNavPath.has_value()) {
        navDestinationName = newTopNavPath->first;
    }
    pipeline->AddPredictTask([weak = WeakClaim(this), weakNode = WeakPtr<FrameNode>(hostNode),
        navDestinationName](int64_t deadline, bool canUseLongPredictTask) {
            auto navigationPattern = weak.Upgrade();
            CHECK_NULL_VOID(navigationPattern);
            auto navigationNode = weakNode.Upgrade();
            CHECK_NULL_VOID(navigationNode);
            int32_t count = 0;
            int32_t depth = 0;
            navigationNode->GetPageNodeCountAndDepth(&count, &depth);
            navigationPattern->PerformanceEventReport(count, depth, navDestinationName);
        });
}

void NavigationPattern::CheckTopNavPathChange(
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& preTopNavPath,
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& newTopNavPath,
    int32_t preLastStandardIndex)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    if (preTopNavPath != newTopNavPath) {
        UpdateSystemBarStyleOnTopNavPathChange(newTopNavPath);
    }
    auto replaceValue = navigationStack_->GetReplaceValue();
    if (preTopNavPath == newTopNavPath) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "page is not change. don't transition");
        auto currentProxy = GetTopNavigationProxy();
        if (currentProxy) {
            currentProxy->SetIsSuccess(false);
        }
        hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_WILL_HIDE);
        NotifyDialogChange(NavDestinationLifecycle::ON_WILL_SHOW, true);
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        pipeline->AddAfterLayoutTask([weakPattern = WeakClaim(this)]() {
            auto pattern = weakPattern.Upgrade();
            CHECK_NULL_VOID(pattern);
            auto hostNode = AceType::DynamicCast<NavigationGroupNode>(pattern->GetHost());
            CHECK_NULL_VOID(hostNode);
            hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_HIDE);
            hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_WILL_DISAPPEAR);
            pattern->NotifyDialogChange(NavDestinationLifecycle::ON_SHOW, true);
            hostNode->RemoveDialogDestination();
        });
        navigationStack_->ClearRecoveryList();
        return;
    }

    isChanged_ = true;
    UpdateIsAnimation(preTopNavPath);
    isReplace_ = replaceValue != 0;
    if (replaceValue == 1) {
        const int32_t replaceAnimation = 2;
        navigationStack_->UpdateReplaceValue(replaceAnimation);
    }
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    // close the text selection menu before transition.
    auto selectOverlayManager = context->GetSelectOverlayManager();
    if (selectOverlayManager) {
        selectOverlayManager->ResetSelectionAndDestroySelectOverlay();
    }
    // fire onHidden and lostFocus event
    RefPtr<NavDestinationGroupNode> preTopNavDestination;
    int32_t lastPreIndex = -1;
    bool isPopPage = false;
    if (preTopNavPath.has_value()) {
        // pre page is not in the current stack
        lastPreIndex = navigationStack_->FindIndex(preTopNavPath->first, preTopNavPath->second, true);
        isPopPage |= lastPreIndex == -1;
        preTopNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(preTopNavPath->second));
    }
    if (isCurTopNewInstance_) {
        isPopPage = false;
    }
    RefPtr<NavDestinationGroupNode> newTopNavDestination;
    if (newTopNavPath.has_value()) {
        newTopNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(newTopNavPath->second));
        do {
            if (!newTopNavDestination) {
                break;
            }
            if (!GetIsFocusable(newTopNavDestination)) {
                break;
            }
            auto navDestinationPattern = newTopNavDestination->GetPattern<NavDestinationPattern>();
            auto navDestinationFocusView = AceType::DynamicCast<FocusView>(navDestinationPattern);
            CHECK_NULL_VOID(navDestinationFocusView);
            if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TWELVE)) {
                navDestinationFocusView->SetIsViewRootScopeFocused(false);
            }
            navDestinationFocusView->FocusViewShow();
        } while (0);
    } else {
        // back to navBar case
        auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
        CHECK_NULL_VOID(navBarNode);
        auto navigationLayoutProperty = AceType::DynamicCast<NavigationLayoutProperty>(hostNode->GetLayoutProperty());
        if (!navigationLayoutProperty->GetHideNavBarValue(false)) {
            navBarNode->GetLayoutProperty()->UpdateVisibility(VisibleType::VISIBLE);
            navBarNode->SetJSViewActive(true);
        }
        ProcessPageShowEvent();
        navBarNode->GetEventHub<EventHub>()->SetEnabledInternal(true);
        if (GetIsFocusable(navBarNode)) {
            auto navBarFocusView = navBarNode->GetPattern<FocusView>();
            CHECK_NULL_VOID(navBarFocusView);
            if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TWELVE)) {
                navBarFocusView->SetIsViewRootScopeFocused(false);
            }
            navBarFocusView->FocusViewShow();
        }
    }
    bool isShow = false;
    bool isDialog =
        (preTopNavDestination && preTopNavDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG) ||
        (newTopNavDestination && newTopNavDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG);
    if (preTopNavDestination && isDialog) {
        auto lastStandardIndex = hostNode->GetLastStandardIndex();
        isShow = (lastPreIndex != -1) && (lastPreIndex >= lastStandardIndex);
        hostNode->SetNeedSetInvisible(lastStandardIndex >= 0);
        if (lastStandardIndex < 0) {
            auto navBarNode = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
            auto layoutProperty = navBarNode->GetLayoutProperty();
            layoutProperty->UpdateVisibility(VisibleType::VISIBLE, true);
            navBarNode->SetJSViewActive(true);
        }
    }
    bool disableAllAnimation = navigationStack_->GetDisableAnimation();
    bool animated = navigationStack_->GetAnimatedValue();
    TAG_LOGI(AceLogTag::ACE_NAVIGATION,
        "transition start, disableAllAnimation: %{public}d, animated: %{public}d, isPopPage: %{public}d, isDialog: "
        "%{public}d, isReplace: %{public}d, isCustomAnimation: %{public}d",
        disableAllAnimation, animated, isPopPage, isDialog, isReplace_, isCustomAnimation_);
    if (disableAllAnimation || !animated) {
        // transition without animation need to run before layout for geometryTransition.
        StartTransition(preTopNavDestination, newTopNavDestination, false, isPopPage, isShow);
        navigationStack_->UpdateAnimatedValue(true);
        hostNode->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE);
        return;
    }
    if (isDialog && !isCustomAnimation_) {
        bool isNeedAnimation =
            AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_THIRTEEN) ?
            true : false;
        StartTransition(preTopNavDestination, newTopNavDestination, isNeedAnimation, isPopPage, isShow);
        hostNode->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE);
        return;
    }

    // before the animation of navDes replacing, update the zIndex of the previous navDes node
    UpdatePreNavDesZIndex(preTopNavDestination, newTopNavDestination, preLastStandardIndex);
    // transition with animation need to run after layout task
    StartTransition(preTopNavDestination, newTopNavDestination, true, isPopPage, isShow);
    hostNode->GetLayoutProperty()->UpdatePropertyChangeFlag(PROPERTY_UPDATE_MEASURE);
}

int32_t NavigationPattern::FireNavDestinationStateChange(NavDestinationLifecycle lifecycle)
{
    const auto& navDestinationNodes = navigationStack_->GetAllNavDestinationNodes();
    auto errIndex = static_cast<int32_t>(navDestinationNodes.size());
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, errIndex);
    NotifyDialogChange(lifecycle, true);
    return hostNode->GetLastStandardIndex();
}

void NavigationPattern::FireNavigationStateChange(const RefPtr<UINode>& node, bool isShow)
{
    if (isShow) {
        NavigationPattern::FireNavigationLifecycleChange(node, NavDestinationLifecycle::ON_SHOW);
        return;
    }
    NavigationPattern::FireNavigationLifecycleChange(node, NavDestinationLifecycle::ON_HIDE);
}

bool NavigationPattern::CheckParentDestinationIsOnhide(const RefPtr<NavDestinationGroupNode>& destinationNode)
{
    CHECK_NULL_RETURN(destinationNode, false);
    auto destinationNodePattern = destinationNode->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(destinationNodePattern, false);
    return !destinationNodePattern->GetIsOnShow();
}

bool NavigationPattern::CheckParentDestinationInactive()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, false);
    auto parentDestination = hostNode->GetParentDestinationNode().Upgrade();
    CHECK_NULL_RETURN(parentDestination, false);
    auto pattern = parentDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_RETURN(pattern, false);
    return !pattern->IsActive();
}

bool NavigationPattern::CheckDestinationIsPush(const RefPtr<NavDestinationGroupNode>& destinationNode)
{
    CHECK_NULL_RETURN(destinationNode, false);
    return destinationNode->GetIndex() != -1 || destinationNode->GetNavDestinationCustomNode();
}

void NavigationPattern::FireNavigationInner(const RefPtr<UINode>& node, bool isOnShow)
{
    CHECK_NULL_VOID(node);
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(node);
    if (!navigationNode) {
        NavigationPattern::FireNavigationChange(node, isOnShow, false);
        return;
    }
    auto navigationPattern = navigationNode->GetPattern<NavigationPattern>();
    CHECK_NULL_VOID(navigationPattern);
    CHECK_NULL_VOID(navigationPattern->navigationStack_);
    const auto& navDestinationNodes = navigationPattern->navigationStack_->GetAllNavDestinationNodes();
    auto lastStandardIndex = navigationNode->GetLastStandardIndex();
    int32_t standardIndex = lastStandardIndex >= 0 ? lastStandardIndex : 0;
    int32_t start = standardIndex;
    int32_t end = navigationPattern->navigationStack_->Size();
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto overlayManager = pipeline->GetOverlayManager();
    if (isOnShow) {
        if (overlayManager && overlayManager->HasModalPage()) {
            return;
        }
        for (int32_t index = start; index < end; index++) {
            const auto& curPath = navDestinationNodes[index];
            auto curDestination = AceType::DynamicCast<NavDestinationGroupNode>(
                navigationNode->GetNavDestinationNode(curPath.second));
            if (!curDestination || !curDestination->GetLayoutProperty()) {
                continue;
            }
            auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
            CHECK_NULL_VOID(navDestinationPattern);
            auto property = curDestination->GetLayoutProperty();
            if (property->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE ||
                !curDestination->IsActive() || navDestinationPattern->GetIsOnShow() == isOnShow) {
                continue;
            }
            auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
            CHECK_NULL_VOID(eventHub);
            auto param = Recorder::EventRecorder::Get().IsPageParamRecordEnable() ?
                navigationPattern->navigationStack_->GetRouteParam() : "";
            eventHub->FireOnShownEvent(navDestinationPattern->GetName(), param);
            navDestinationPattern->SetIsOnShow(true);
            NavigationPattern::FireNavigationChange(curDestination, true, false);
            NavigationPattern::NotifyPerfMonitorPageMsg(navDestinationPattern->GetName());
        }
        return;
    }
    for (int32_t index = end - 1; index >= 0 && index >= start; index--) {
        const auto& curPath = navDestinationNodes[index];
        auto curDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            navigationNode->GetNavDestinationNode(curPath.second));
        if (!curDestination || !curDestination->GetLayoutProperty()) {
            continue;
        }
        auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
        CHECK_NULL_VOID(navDestinationPattern);
        auto property = curDestination->GetLayoutProperty();
        if (property->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE ||
            !curDestination->IsActive() || navDestinationPattern->GetIsOnShow() == isOnShow) {
            continue;
        }
        auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
        CHECK_NULL_VOID(eventHub);
        eventHub->FireOnHiddenEvent(navDestinationPattern->GetName());
        navDestinationPattern->SetIsOnShow(false);
        NavigationPattern::FireNavigationChange(curDestination, false, false);
    }
}

void NavigationPattern::FireNavigationChange(const RefPtr<UINode>& node, bool isOnShow, bool isFirst)
{
    CHECK_NULL_VOID(node);
    if (isFirst) {
        FireNavigationInner(node, isOnShow);
        return;
    }
    const auto& children = node->GetChildren();
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        FireNavigationInner(child, isOnShow);
    }
}

void NavigationPattern::FireNavigationLifecycleChange(const RefPtr<UINode>& node, NavDestinationLifecycle lifecycle)
{
    CHECK_NULL_VOID(node);
    const auto& children = node->GetChildren(true);
    for (auto iter = children.rbegin(); iter != children.rend(); ++iter) {
        auto& child = *iter;
        auto navigation = AceType::DynamicCast<NavigationGroupNode>(child);
        if (navigation) {
            auto destinationNode = navigation->GetParentDestinationNode().Upgrade();
            if ((lifecycle == NavDestinationLifecycle::ON_SHOW) && CheckParentDestinationIsOnhide(destinationNode) &&
                CheckDestinationIsPush(destinationNode)) {
                TAG_LOGI(AceLogTag::ACE_NAVIGATION, "navigation parent is onhide");
                continue;
            }
            auto navigationPattern = AceType::DynamicCast<NavigationPattern>(navigation->GetPattern());
            CHECK_NULL_VOID(navigationPattern);
            navigationPattern->FireNavDestinationStateChange(lifecycle);
        } else {
            NavigationPattern::FireNavigationLifecycleChange(child, lifecycle);
        }
    }
}

void NavigationPattern::NotifyPageHide(const std::string& pageName)
{
    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    auto pageUrlChecker = container->GetPageUrlChecker();
    CHECK_NULL_VOID(pageUrlChecker);
    pageUrlChecker->NotifyPageHide(pageName);
}

void NavigationPattern::NotifyPageShow(const std::string& pageName)
{
    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    auto pageUrlChecker = container->GetPageUrlChecker();
    CHECK_NULL_VOID(pageUrlChecker);
    pageUrlChecker->NotifyPageShow(pageName);
    if (PerfMonitor::GetPerfMonitor() != nullptr) {
        PerfMonitor::GetPerfMonitor()->SetPageName(pageName);
    }
}

void NavigationPattern::ProcessPageShowEvent()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto context = hostNode->GetContext();
    CHECK_NULL_VOID(context);
    auto stageManager = context->GetStageManager();
    if (stageManager) {
        RefPtr<FrameNode> pageNode = stageManager->GetLastPage();
        CHECK_NULL_VOID(pageNode);
        auto pagePattern = pageNode->GetPattern<NG::PagePattern>();
        CHECK_NULL_VOID(pagePattern);
        auto pageInfo = pagePattern->GetPageInfo();
        CHECK_NULL_VOID(pageInfo);
        NotifyPageShow(pageInfo->GetPageUrl());
    }
}

void NavigationPattern::ReplaceAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    bool preUseCustomTransition = TriggerNavDestinationTransition(
        preTopNavDestination, NavigationOperation::REPLACE, false) != INVALID_ANIMATION_ID;
    TriggerNavDestinationTransition(newTopNavDestination, NavigationOperation::REPLACE, true);
    if (newTopNavDestination && preTopNavDestination && !preUseCustomTransition) {
        navigationNode->DealNavigationExit(preTopNavDestination, false, false);
    } else if (newTopNavDestination && navigationMode_ == NavigationMode::STACK) {
        navigationNode->DealNavigationExit(navBarNode, true, false);
    }
    navigationNode->RemoveDialogDestination();
    auto id = navigationNode->GetTopDestination() ? navigationNode->GetTopDestination()->GetAccessibilityId() : -1;
    navigationNode->OnAccessibilityEvent(
        AccessibilityEventType::PAGE_CHANGE, id, WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID);
    navigationStack_->UpdateReplaceValue(0);
}

void NavigationPattern::TransitionWithOutAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage, bool needVisible)
{
    navigationStack_->ClearRecoveryList();
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);

    // replace
    auto replaceVal = navigationStack_->GetReplaceValue();
    if (replaceVal != 0) {
        ReplaceAnimation(preTopNavDestination, newTopNavDestination);
        return;
    }

    // navDestination push/pop navDestination
    if (newTopNavDestination && preTopNavDestination) {
        if (isPopPage) {
            newTopNavDestination->SetTransitionType(PageTransitionType::ENTER_POP);
            preTopNavDestination->CleanContent();
            auto parent = preTopNavDestination->GetParent();
            CHECK_NULL_VOID(parent);
            parent->RemoveChild(preTopNavDestination, true);
            parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        } else {
            preTopNavDestination->GetRenderContext()->RemoveClipWithRRect();
            preTopNavDestination->SetTransitionType(PageTransitionType::EXIT_PUSH);
            newTopNavDestination->SetTransitionType(PageTransitionType::ENTER_PUSH);
            DealTransitionVisibility(preTopNavDestination, needVisible, false);
            if (preTopNavDestination->NeedRemoveInPush()) {
                preTopNavDestination->CleanContent();
                auto parent = preTopNavDestination->GetParent();
                CHECK_NULL_VOID(parent);
                parent->RemoveChild(preTopNavDestination, true);
                parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            }
        }
        navigationNode->RemoveDialogDestination();
        auto id = navigationNode->GetTopDestination() ? navigationNode->GetTopDestination()->GetAccessibilityId() : -1;
        navigationNode->OnAccessibilityEvent(
            AccessibilityEventType::PAGE_CHANGE, id, WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID);
        return;
    }

    // navBar push navDestination
    if (newTopNavDestination && newTopNavDestination->GetNavDestinationMode() == NavDestinationMode::STANDARD) {
        newTopNavDestination->SetTransitionType(PageTransitionType::ENTER_PUSH);
        auto navBar = AceType::DynamicCast<NavBarNode>(navBarNode);
        // current mode is stack, set navBar invisible
        if (navigationMode_ == NavigationMode::STACK && navBar) {
            navBar->SetTransitionType(PageTransitionType::EXIT_PUSH);
            DealTransitionVisibility(navBarNode, false, true);
        }
        // if current mode is auto, need set navBar need set invisible true
        navigationNode->SetNeedSetInvisible(true);
    }

    // navDestination pop to navBar
    if (preTopNavDestination) {
        preTopNavDestination->CleanContent();
        auto parent = preTopNavDestination->GetParent();
        CHECK_NULL_VOID(parent);
        parent->RemoveChild(preTopNavDestination, true);
        navigationNode->SetNeedSetInvisible(false);
        auto navBar = AceType::DynamicCast<NavBarNode>(navBarNode);
        if (navBar) {
            navBar->SetTransitionType(PageTransitionType::ENTER_POP);
        }
        parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
    }
    navigationNode->RemoveDialogDestination();
    auto id = navigationNode->GetTopDestination() ? navigationNode->GetTopDestination()->GetAccessibilityId() : -1;
    navigationNode->OnAccessibilityEvent(
        AccessibilityEventType::PAGE_CHANGE, id, WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID);
}

void NavigationPattern::TransitionWithAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage, bool isNeedVisible)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    auto layoutProperty = navigationNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    if (layoutProperty->GetHideNavBarValue(false) && (!newTopNavDestination || !preTopNavDestination)) {
        // hide navBarNode and need to do animation with navBarNode
        if (preTopNavDestination) {
            // remove preTopNavDestination node in pop
            auto parent = preTopNavDestination->GetParent();
            CHECK_NULL_VOID(parent);
            if (preTopNavDestination->GetContentNode()) {
                preTopNavDestination->GetContentNode()->Clean();
            }
            parent->RemoveChild(preTopNavDestination);
            parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
        }
        navigationNode->RemoveDialogDestination();
        navigationStack_->ClearRecoveryList();
        return;
    }
    if (isCustomAnimation_ && TriggerCustomAnimation(preTopNavDestination, newTopNavDestination, isPopPage)) {
        auto operation = NavigationOperation::REPLACE;
        if (navigationStack_->GetReplaceValue() == 0) {
            operation = isPopPage ? NavigationOperation::POP : NavigationOperation::PUSH;
        }
        TriggerNavDestinationTransition(preTopNavDestination, operation, false);
        TriggerNavDestinationTransition(newTopNavDestination, operation, true);
        return;
    }
    StartDefaultAnimation(preTopNavDestination, newTopNavDestination, isPopPage, isNeedVisible);
}

void NavigationPattern::DialogAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage, bool isNeedVisible)
{
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_THIRTEEN)) {
        TransitionWithDialogAnimation(preTopNavDestination, newTopNavDestination, isPopPage);
    } else {
        TransitionWithOutAnimation(preTopNavDestination, newTopNavDestination, isPopPage, isNeedVisible);
    }
}

void NavigationPattern::StartDefaultAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage, bool isNeedVisible)
{
    auto currentProxy = GetTopNavigationProxy();
    if (currentProxy) {
        currentProxy->SetIsFinished(true);
    }
    navigationStack_->ClearRecoveryList();
    bool isPreDialog = preTopNavDestination &&
        preTopNavDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG;
    bool isNewDialog = newTopNavDestination &&
        newTopNavDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG;
    if (isPreDialog || isNewDialog) {
        DialogAnimation(preTopNavDestination, newTopNavDestination, isPopPage, isNeedVisible);
        return;
    }
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    // replace
    auto replaceValue = navigationStack_->GetReplaceValue();
    if (replaceValue != 0) {
        if (newTopNavDestination && preTopNavDestination) {
            navigationNode->TransitionWithReplace(preTopNavDestination, newTopNavDestination, false);
        } else if (newTopNavDestination && navigationMode_ == NavigationMode::STACK) {
            navigationNode->TransitionWithReplace(navBarNode, newTopNavDestination, true);
        }
        navigationStack_->UpdateReplaceValue(0);
        return;
    }
    // navDestination push/pop navDestination
    if (newTopNavDestination && preTopNavDestination) {
        if (isPopPage) {
            navigationNode->TransitionWithPop(preTopNavDestination, newTopNavDestination);
        } else {
            navigationNode->TransitionWithPush(preTopNavDestination, newTopNavDestination);
        }
        return;
    }
    // navBar push navDestination
    if (newTopNavDestination && navigationMode_ == NavigationMode::STACK) {
        navigationNode->TransitionWithPush(navBarNode, newTopNavDestination, true);
        return;
    }
    // navDestination pop to navBar
    if (preTopNavDestination) {
        if (navigationMode_ == NavigationMode::SPLIT) {
            navigationNode->TransitionWithPop(preTopNavDestination, nullptr);
        }
        if (navigationMode_ == NavigationMode::STACK) {
            navigationNode->TransitionWithPop(preTopNavDestination, navBarNode, true);
        }
    }
}

void NavigationPattern::OnVisibleChange(bool isVisible)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto eventHub = hostNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireNavBarStateChangeEvent(isVisible);
}

void NavigationPattern::OnNavBarStateChange(bool modeChange)
{
    auto layoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto visibilityValue = layoutProperty->GetVisibilityValue(VisibleType::VISIBLE);
    if (visibilityValue != VisibleType::VISIBLE) {
        return;
    }

    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto eventHub = hostNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto currentNavigationMode = GetNavigationMode();

    auto lastStandardIndex = hostNode->GetLastStandardIndex();
    if (modeChange) {
        bool navbarIsHidden = (currentNavigationMode == NavigationMode::STACK && lastStandardIndex >= 0) ||
                              layoutProperty->GetHideNavBar().value_or(false);
        eventHub->FireNavBarStateChangeEvent(!navbarIsHidden);
        SetNavBarVisibilityChange(false);
        return;
    }

    if (GetNavBarVisibilityChange()) {
        if (!layoutProperty->GetHideNavBarValue(false)) {
            eventHub->FireNavBarStateChangeEvent(true);
        } else {
            eventHub->FireNavBarStateChangeEvent(false);
        }
        SetNavBarVisibilityChange(false);
        return;
    }

    if (currentNavigationMode == NavigationMode::STACK) {
        bool navbarIsHidden = (lastStandardIndex >= 0) || layoutProperty->GetHideNavBar().value_or(false);
        eventHub->FireNavBarStateChangeEvent(!navbarIsHidden);
    }
}

void NavigationPattern::OnNavigationModeChange(bool modeChange)
{
    if (!modeChange) {
        return;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto eventHub = hostNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireNavigationModeChangeEvent(navigationMode_);
    // fire navigation stack navigation mode change event
    navigationStack_->FireNavigationModeChange(navigationMode_);
}

bool NavigationPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipMeasure && config.skipLayout) {
        return false;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, false);
    UpdateIsFullPageNavigation(hostNode);
    if (navigationModeChange_) {
        if (NavigationMode::STACK == navigationMode_) {
            // Set focus on navDestination when mode changes to STACK
            RefreshFocusToDestination();
        }
        AbortAnimation(hostNode);
    }
    auto context = PipelineContext::GetCurrentContext();
    if (context) {
        context->GetTaskExecutor()->PostTask(
            [weak = WeakClaim(this), navigationStackWeak = WeakPtr<NavigationStack>(navigationStack_),
                navigationWeak = WeakPtr<NavigationGroupNode>(hostNode)] {
                auto pattern = weak.Upgrade();
                CHECK_NULL_VOID(pattern);
                auto navigationGroupNode = navigationWeak.Upgrade();
                CHECK_NULL_VOID(navigationGroupNode);
                auto navigationLayoutProperty =
                    AceType::DynamicCast<NavigationLayoutProperty>(navigationGroupNode->GetLayoutProperty());
                CHECK_NULL_VOID(navigationLayoutProperty);
                auto navigationStack = navigationStackWeak.Upgrade();
                CHECK_NULL_VOID(navigationStack);
                auto curTopNavPath = navigationStack->GetTopNavPath();
                if (curTopNavPath.has_value()) {
                    // considering backButton visibility
                    auto curTopNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
                        NavigationGroupNode::GetNavDestinationNode(curTopNavPath->second));
                    pattern->UpdateContextRect(curTopNavDestination, navigationGroupNode);
                }
                // considering navBar visibility
                auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationGroupNode->GetNavBarNode());
                CHECK_NULL_VOID(navBarNode);
                auto navBarLayoutProperty = navBarNode->GetLayoutProperty<NavBarLayoutProperty>();
                CHECK_NULL_VOID(navBarLayoutProperty);
                bool isSetInvisible =
                    (navigationGroupNode->GetNeedSetInvisible() && navigationStack->Size() != 0) ? true : false;
                if (navigationLayoutProperty->GetHideNavBar().value_or(false) ||
                    (pattern->GetNavigationMode() == NavigationMode::STACK && isSetInvisible)) {
                    navBarLayoutProperty->UpdateVisibility(VisibleType::INVISIBLE);
                    navBarNode->SetJSViewActive(false);
                } else {
                    navBarNode->GetRenderContext()->UpdateOpacity(1.0f);
                    navBarLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
                    navBarNode->SetJSViewActive(true);
                }
                auto navigationContentNode = AceType::DynamicCast<FrameNode>(navigationGroupNode->GetContentNode());
                CHECK_NULL_VOID(navigationContentNode);
                auto navDestinationNode =
                    AceType::DynamicCast<NavDestinationGroupNode>(navigationContentNode->GetLastChild());
                CHECK_NULL_VOID(navDestinationNode);
                auto navDestinationPattern = navDestinationNode->GetPattern<NavDestinationPattern>();
                auto navDestinationFocusHub = navDestinationNode->GetFocusHub();
                CHECK_NULL_VOID(navDestinationFocusHub);
                auto defaultFocusHub = navDestinationFocusHub->GetChildFocusNodeByType(FocusNodeType::DEFAULT);
                if (!defaultFocusHub && navDestinationNode->GetChildren(true).size() <= EMPTY_DESTINATION_CHILD_SIZE &&
                    navDestinationPattern->GetBackButtonState()) {
                    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestinationNode->GetTitleBarNode());
                    CHECK_NULL_VOID(titleBarNode);
                    auto backButtonNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetBackButton());
                    backButtonNode->GetOrCreateFocusHub()->SetIsDefaultFocus(true);
                    auto navigation = pattern->GetHost();
                    CHECK_NULL_VOID(navigation);
                    auto navigationFocusHub = navigation->GetFocusHub();
                    CHECK_NULL_VOID(navigationFocusHub);
                    auto navDestinationFocusView = navDestinationNode->GetPattern<FocusView>();
                    if (navigationFocusHub->IsCurrentFocus() && navDestinationFocusView) {
                        if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TWELVE)) {
                            navDestinationFocusView->SetIsViewRootScopeFocused(false);
                        }
                        navDestinationFocusView->FocusViewShow();
                    }
                }
            },
            TaskExecutor::TaskType::UI, "ArkUINavigationDirtyLayoutWrapperSwap",
            TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
    }
    auto navigationLayoutProperty = AceType::DynamicCast<NavigationLayoutProperty>(hostNode->GetLayoutProperty());
    CHECK_NULL_RETURN(navigationLayoutProperty, false);
    UpdateTitleModeChangeEventHub(hostNode);
    FireNavBarWidthChangeEvent(dirty);
    AddDragBarHotZoneRect();
    AddDividerHotZoneRect();
    ifNeedInit_ = false;
    return false;
}

void NavigationPattern::AbortAnimation(RefPtr<NavigationGroupNode>& hostNode)
{
    TAG_LOGD(AceLogTag::ACE_NAVIGATION, "Aborting navigation animations");
    if (!hostNode->GetPushAnimations().empty()) {
        auto pushAnimations = hostNode->GetPushAnimations();
        for (const auto& animation : pushAnimations) {
            if (animation) {
                AnimationUtils::StopAnimation(animation);
            }
        }
    }
    if (!hostNode->GetPopAnimations().empty()) {
        auto popAnimations = hostNode->GetPopAnimations();
        for (const auto& animation : popAnimations) {
            if (animation) {
                AnimationUtils::StopAnimation(animation);
            }
        }
    }
    hostNode->CleanPushAnimations();
    hostNode->CleanPopAnimations();
}

void NavigationPattern::UpdateContextRect(
    const RefPtr<NavDestinationGroupNode>& curDestination, const RefPtr<NavigationGroupNode>& hostNode)
{
    CHECK_NULL_VOID(curDestination);
    CHECK_NULL_VOID(hostNode);
    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navigationPattern = AceType::DynamicCast<NavigationPattern>(hostNode->GetPattern());
    CHECK_NULL_VOID(navigationPattern);

    if (navigationPattern->GetNavigationMode() == NavigationMode::STACK) {
        curDestination->GetRenderContext()->SetActualForegroundColor(Color::TRANSPARENT);
        return;
    }
    auto navigationLayoutProperty = hostNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(navigationLayoutProperty);
    auto navBarProperty = navBarNode->GetLayoutProperty();
    navBarProperty->UpdateVisibility(VisibleType::VISIBLE);
    navBarNode->SetJSViewActive(true);
    if (!curDestination->IsOnAnimation()) {
        curDestination->GetRenderContext()->UpdateTranslateInXY(OffsetF { 0.0f, 0.0f });
        curDestination->GetRenderContext()->SetActualForegroundColor(Color::TRANSPARENT);
        navBarNode->GetEventHub<EventHub>()->SetEnabledInternal(true);
        auto titleBarNode = DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
        CHECK_NULL_VOID(titleBarNode);
        auto titleNode = AceType::DynamicCast<FrameNode>(titleBarNode->GetTitle());
        CHECK_NULL_VOID(titleNode);
        titleNode->GetRenderContext()->UpdateTranslateInXY(OffsetF { 0.0f, 0.0f });
    }
}

bool NavigationPattern::UpdateTitleModeChangeEventHub(const RefPtr<NavigationGroupNode>& hostNode)
{
    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_RETURN(navBarNode, false);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    CHECK_NULL_RETURN(titleBarNode, false);
    auto titleBarLayoutProperty = titleBarNode->GetLayoutProperty<TitleBarLayoutProperty>();
    CHECK_NULL_RETURN(titleBarLayoutProperty, false);
    auto eventHub = hostNode->GetEventHub<NavigationEventHub>();
    CHECK_NULL_RETURN(eventHub, false);
    if (titleBarLayoutProperty->GetTitleModeValue(NavigationTitleMode::FREE) == NavigationTitleMode::FREE) {
        auto titleBarPattern = AceType::DynamicCast<TitleBarPattern>(titleBarNode->GetPattern());
        CHECK_NULL_RETURN(titleBarPattern, false);
        NavigationTitleMode titleMode = titleBarPattern->GetNavigationTitleMode();
        if (titleMode != NavigationTitleMode::FREE && titleMode_ != titleMode) {
            NavigationTitleModeChangeEvent navigationTitleModeChange(titleMode == NavigationTitleMode::MINI);
            eventHub->FireChangeEvent(&navigationTitleModeChange);
            titleMode_ = titleMode;
        }
    }
    return true;
}

void NavigationPattern::FireNavBarWidthChangeEvent(const RefPtr<LayoutWrapper>& layoutWrapper)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto frameSize = geometryNode->GetFrameSize();
    auto frameWidth = frameSize.Width();
    auto navigationLayoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(navigationLayoutProperty);
    auto userSetDimensionUnit = navigationLayoutProperty->GetNavBarWidthValue(DEFAULT_NAV_BAR_WIDTH).Unit();
    CHECK_NULL_VOID(layoutWrapper);
    auto layoutAlgorithm = layoutWrapper->GetLayoutAlgorithm();
    CHECK_NULL_VOID(layoutAlgorithm);
    auto navigationLayoutAlgorithm = AceType::DynamicCast<NavigationLayoutAlgorithm>(
        layoutAlgorithm->GetLayoutAlgorithm());
    CHECK_NULL_VOID(navigationLayoutAlgorithm);
    auto realBavBarWidth = navigationLayoutAlgorithm->GetRealNavBarWidth();
    auto realNavBarWidthDimension = Dimension(realBavBarWidth, DimensionUnit::PX);
    Dimension usrSetUnitWidth = Dimension(0.0, userSetDimensionUnit);
    if (!NearZero(frameWidth)) {
        usrSetUnitWidth = DimensionUnit::PERCENT == userSetDimensionUnit ?
            Dimension(realBavBarWidth / frameWidth, DimensionUnit::PERCENT) :
            Dimension(realNavBarWidthDimension.GetNativeValue(userSetDimensionUnit), userSetDimensionUnit);
    }
    auto eventHub = host->GetEventHub<NavigationEventHub>();
    CHECK_NULL_VOID(eventHub);
    eventHub->FireNavBarWidthChangeEvent(usrSetUnitWidth);
}

int32_t NavigationPattern::GenerateUINodeFromRecovery(int32_t lastStandardIndex, NavPathList& navPathList)
{
    /**
     * In case several pages at the top of stack are dialog pages.
     * We need to recovery node until a standard page created.
     * And the creation process should be bottom-up to satisfy the order of life-cycle.
     */
    int32_t jsStackSize = static_cast<int32_t>(navPathList.size());
    int32_t removeSize = 0;
    for (int32_t index = lastStandardIndex; index < jsStackSize; ++ index) {
        if (navPathList[index].second || !navigationStack_->IsFromRecovery(index)) {
            continue;
        }
        if (!GenerateUINodeByIndex(index - removeSize, navPathList[index].second)) {
            removeSize++;
            continue;
        }
        navigationStack_->SetFromRecovery(index, false);
        auto navdestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(navPathList[index].second));
        navdestination->SetNeedAppearFromRecovery(true);
    }
    return removeSize;
}

bool NavigationPattern::GenerateUINodeByIndex(int32_t index, RefPtr<UINode>& node)
{
    bool isCreate = navigationStack_->CreateNodeByIndex(index, parentNode_, node);
    if (node) {
        node->SetFreeze(true, true);
    }
    auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(
        NavigationGroupNode::GetNavDestinationNode(node));
    CHECK_NULL_RETURN(navDestinationNode, isCreate);
    // set navigation id
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    auto navDestinationPattern = AceType::DynamicCast<NavDestinationPattern>(navDestinationNode->GetPattern());
    if (navigationNode && navDestinationPattern) {
        navDestinationPattern->SetNavigationNode(navigationNode);
        navDestinationPattern->SetNavigationId(navigationNode->GetInspectorId().value_or(""));
        navigationStack_->SetDestinationIdToJsStack(
            index, std::to_string(navDestinationPattern->GetNavDestinationId()));
    }
    auto eventHub = navDestinationNode->GetEventHub<NavDestinationEventHub>();
    CHECK_NULL_RETURN(eventHub, isCreate);
    eventHub->FireOnWillAppear();
    return isCreate;
}

void NavigationPattern::InitDividerMouseEvent(const RefPtr<InputEventHub>& inputHub)
{
    CHECK_NULL_VOID(inputHub);
    CHECK_NULL_VOID(!hoverEvent_);

    auto hoverTask = [weak = WeakClaim(this)](bool isHover) {
        auto pattern = weak.Upgrade();
        if (pattern) {
            pattern->OnHover(isHover);
        }
    };
    hoverEvent_ = MakeRefPtr<InputEvent>(std::move(hoverTask));
    inputHub->AddOnHoverEvent(hoverEvent_);
}

void NavigationPattern::HandleDragStart()
{
    preNavBarWidth_ = realNavBarWidth_;
    if (!isDividerDraggable_) {
        return;
    }
    isInDividerDrag_ = true;
    if (!enableDragBar_) {
        SetMouseStyle(MouseFormat::RESIZE_LEFT_RIGHT);
    }
}

void NavigationPattern::HandleDragUpdate(float xOffset)
{
    auto navigationLayoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(navigationLayoutProperty);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto geometryNode = host->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto frameSize = geometryNode->GetFrameSize();
    auto frameWidth = frameSize.Width();
    auto constraint = navigationLayoutProperty->GetLayoutConstraint();
    auto parentSize = CreateIdealSize(constraint.value(), Axis::HORIZONTAL, MeasureType::MATCH_PARENT);

    float minNavBarWidthPx = minNavBarWidthValue_.ConvertToPxWithSize(parentSize.Width().value_or(0.0f));
    float maxNavBarWidthPx = maxNavBarWidthValue_.ConvertToPxWithSize(parentSize.Width().value_or(0.0f));
    float minContentWidthPx = minContentWidthValue_.ConvertToPxWithSize(parentSize.Width().value_or(0.0f));
    auto dividerWidth = static_cast<float>(DIVIDER_WIDTH.ConvertToPx());

    auto navigationPosition = navigationLayoutProperty->GetNavBarPosition().value_or(NavBarPosition::START);
    bool isNavBarStart = navigationPosition == NavBarPosition::START;
    auto navBarLine = isRightToLeft_ ? preNavBarWidth_ + (isNavBarStart ? -xOffset : xOffset)
                                     : preNavBarWidth_ + (isNavBarStart ? xOffset : -xOffset);
    float currentNavBarWidth = realNavBarWidth_;

    if (maxNavBarWidthPx + dividerWidth + minContentWidthPx > frameWidth) {
        maxNavBarWidthPx = frameWidth - minContentWidthPx - dividerWidth;
    }
    navBarLine = std::min(navBarLine, maxNavBarWidthPx);

    if (userSetMinContentFlag_ && !userSetNavBarRangeFlag_) {
        if (minContentWidthPx >= frameWidth) {
            realNavBarWidth_ = 0.0f;
        } else if (navBarLine + dividerWidth + minContentWidthPx <= frameWidth) {
            realNavBarWidth_ = navBarLine;
        } else {
            realNavBarWidth_ = frameWidth - minContentWidthPx - dividerWidth;
        }
    } else {
        realDividerWidth_ = dividerWidth;
        float remainingSpace = frameWidth - navBarLine - dividerWidth;
        if (remainingSpace >= minContentWidthPx) {
            realNavBarWidth_ = navBarLine;
        } else if (remainingSpace < minContentWidthPx && navBarLine > minNavBarWidthPx) {
            realNavBarWidth_ = frameWidth - minContentWidthPx - dividerWidth;
        } else {
            realNavBarWidth_ = minNavBarWidthPx;
        }
    }
    realNavBarWidth_ = std::min(realNavBarWidth_, frameWidth);
    realNavBarWidth_ = std::min(realNavBarWidth_, maxNavBarWidthPx);
    realNavBarWidth_ = std::max(realNavBarWidth_, minNavBarWidthPx);
    // MEASURE
    if (realNavBarWidth_ != currentNavBarWidth) {
        host->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
    }
}

void NavigationPattern::HandleDragEnd()
{
    preNavBarWidth_ = realNavBarWidth_;
    if (!isDividerDraggable_) {
        return;
    }
    isInDividerDrag_ = false;
    SetMouseStyle(MouseFormat::DEFAULT);
}

void NavigationPattern::InitDividerPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID(!panEvent_);
    auto actionStartTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragStart();
    };
    auto actionUpdateTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragUpdate(static_cast<float>(info.GetOffsetX()));
    };
    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragEnd();
    };
    auto actionCancelTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragEnd();
    };
    panEvent_ = MakeRefPtr<PanEvent>(
        std::move(actionStartTask), std::move(actionUpdateTask), std::move(actionEndTask), std::move(actionCancelTask));
    PanDirection panDirection = { .type = PanDirection::HORIZONTAL };
    gestureHub->AddPanEvent(panEvent_, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
}

void NavigationPattern::InitDragBarPanEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    CHECK_NULL_VOID(!dragBarPanEvent_);
    auto actionStartTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragStart();
    };
    auto actionUpdateTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragUpdate(static_cast<float>(info.GetOffsetX()));
    };
    auto actionEndTask = [weak = WeakClaim(this)](const GestureEvent& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragEnd();
    };
    auto actionCancelTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleDragEnd();
    };
    dragBarPanEvent_ = MakeRefPtr<PanEvent>(
        std::move(actionStartTask), std::move(actionUpdateTask), std::move(actionEndTask), std::move(actionCancelTask));
    PanDirection panDirection = { .type = PanDirection::HORIZONTAL };
    gestureHub->AddPanEvent(dragBarPanEvent_, panDirection, DEFAULT_PAN_FINGER, DEFAULT_PAN_DISTANCE);
}

void NavigationPattern::OnHover(bool isHover)
{
    if (isInDividerDrag_) {
        return;
    }
    auto layoutProperty = GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    auto userSetMinNavBarWidthValue = layoutProperty->GetMinNavBarWidthValue(Dimension(0.0));
    auto userSetMaxNavBarWidthValue = layoutProperty->GetMaxNavBarWidthValue(Dimension(0.0));
    bool navBarWidthRangeEqual = userSetMinNavBarWidthValue.Value() >= userSetMaxNavBarWidthValue.Value();
    if ((userSetNavBarWidthFlag_ && !userSetNavBarRangeFlag_) || (userSetNavBarRangeFlag_ && navBarWidthRangeEqual)) {
        isDividerDraggable_ = false;
        return;
    }
    isDividerDraggable_ = true;
    MouseFormat format = isHover ? MouseFormat::RESIZE_LEFT_RIGHT : MouseFormat::DEFAULT;
    SetMouseStyle(format);
}

RefPtr<FrameNode> NavigationPattern::GetDividerNode() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(host);
    CHECK_NULL_RETURN(navigationNode, nullptr);
    auto dividerFrameNode = AceType::DynamicCast<FrameNode>(navigationNode->GetDividerNode());
    CHECK_NULL_RETURN(dividerFrameNode, nullptr);
    return dividerFrameNode;
}

RefPtr<FrameNode> NavigationPattern::GetDragBarNode() const
{
    auto host = GetHost();
    CHECK_NULL_RETURN(host, nullptr);
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(host);
    CHECK_NULL_RETURN(navigationNode, nullptr);
    auto dragBarNode = AceType::DynamicCast<FrameNode>(navigationNode->GetDragBarNode());
    CHECK_NULL_RETURN(dragBarNode, nullptr);
    return dragBarNode;
}

void NavigationPattern::BeforeSyncGeometryProperties(const DirtySwapConfig& /* config */)
{
    AddDividerHotZoneRect();
}

void NavigationPattern::AddDividerHotZoneRect()
{
    if (NearZero(realDividerWidth_)) {
        return;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto navBarNode = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto geometryNode = navBarNode->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);

    OffsetF hotZoneOffset;
    hotZoneOffset.SetX(-DEFAULT_DIVIDER_HOT_ZONE_HORIZONTAL_PADDING.ConvertToPx());
    hotZoneOffset.SetY(DEFAULT_DIVIDER_START_MARGIN.ConvertToPx());
    SizeF hotZoneSize;
    hotZoneSize.SetWidth(realDividerWidth_ + DIVIDER_HOT_ZONE_HORIZONTAL_PADDING_NUM *
                                                 DEFAULT_DIVIDER_HOT_ZONE_HORIZONTAL_PADDING.ConvertToPx());
    hotZoneSize.SetHeight(geometryNode->GetFrameSize().Height());
    DimensionRect hotZoneRegion;
    auto paintHeight = GetPaintRectHeight(navBarNode);
    if (navigationMode_ == NavigationMode::STACK || enableDragBar_) {
        hotZoneRegion.SetSize(DimensionSize(Dimension(0.0f), Dimension(0.0f)));
    } else {
        hotZoneRegion.SetSize(DimensionSize(
            Dimension(hotZoneSize.Width()), Dimension(NearZero(paintHeight) ? hotZoneSize.Height() : paintHeight)));
    }
    hotZoneRegion.SetOffset(DimensionOffset(Dimension(hotZoneOffset.GetX()), Dimension(hotZoneOffset.GetY())));

    std::vector<DimensionRect> mouseRegion;
    mouseRegion.emplace_back(hotZoneRegion);

    auto dividerFrameNode = GetDividerNode();
    CHECK_NULL_VOID(dividerFrameNode);
    auto dividerGestureHub = dividerFrameNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dividerGestureHub);
    dividerGestureHub->SetMouseResponseRegion(mouseRegion);

    auto dragRectOffset = geometryNode->GetMarginFrameOffset();
    dragRectOffset.SetX(-DEFAULT_DRAG_REGION.ConvertToPx());
    dragRect_.SetOffset(dragRectOffset);
    if (navigationMode_ == NavigationMode::STACK || enableDragBar_) {
        dragRect_.SetSize(SizeF(0.0f, 0.0f));
    } else {
        dragRect_.SetSize(SizeF(DEFAULT_DRAG_REGION.ConvertToPx() * DEFAULT_HALF + realDividerWidth_,
            NearZero(paintHeight) ? geometryNode->GetFrameSize().Height() : paintHeight));
    }

    std::vector<DimensionRect> responseRegion;
    DimensionOffset responseOffset(dragRectOffset);
    DimensionRect responseRect(Dimension(dragRect_.Width(), DimensionUnit::PX),
        Dimension(dragRect_.Height(), DimensionUnit::PX), responseOffset);
    responseRegion.emplace_back(responseRect);
    dividerGestureHub->SetResponseRegion(responseRegion);
}

void NavigationPattern::AddDragBarHotZoneRect()
{
    if (NearZero(realDividerWidth_)) {
        return;
    }
    auto dargBarNode = GetDragBarNode();
    CHECK_NULL_VOID(dargBarNode);
    auto geometryNode = dargBarNode->GetGeometryNode();
    CHECK_NULL_VOID(geometryNode);
    auto dragBarGestureHub = dargBarNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(dragBarGestureHub);
    
    auto dragRectOffset = geometryNode->GetMarginFrameOffset();
    dragRectOffset.SetX(-DEFAULT_DRAG_BAR_HOT_ZONE.ConvertToPx());
    dragRectOffset.SetY(0.0f);
    dragBarRect_.SetOffset(dragRectOffset);
    if (navigationMode_ == NavigationMode::STACK) {
        dragBarRect_.SetSize(SizeF(0.0f, 0.0f));
    } else {
        dragBarRect_.SetSize(SizeF(DEFAULT_DRAG_BAR_HOT_ZONE.ConvertToPx() * DEFAULT_HALF +
            geometryNode->GetFrameSize().Width(), geometryNode->GetFrameSize().Height()));
    }
    std::vector<DimensionRect> responseRegion;
    DimensionOffset responseOffset(dragRectOffset);
    DimensionRect responseRect(Dimension(dragBarRect_.Width(), DimensionUnit::PX),
        Dimension(dragBarRect_.Height(), DimensionUnit::PX), responseOffset);
    responseRegion.emplace_back(responseRect);
    dragBarGestureHub->SetResponseRegion(responseRegion);
}

void NavigationPattern::NotifyDialogChange(NavDestinationLifecycle lifecycle, bool isFromStandardIndex)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    const auto& navDestinationNodes = navigationStack_->GetAllNavDestinationNodes();
    int32_t lastStandardIndex = hostNode->GetLastStandardIndex();
    int32_t standardIndex = lastStandardIndex >= 0 ? lastStandardIndex : 0;
    int32_t start = isFromStandardIndex ? standardIndex : 0;
    int32_t end = isFromStandardIndex ? navigationStack_->Size() : standardIndex;
    bool isShow = (lifecycle == NavDestinationLifecycle::ON_SHOW)
        || (lifecycle == NavDestinationLifecycle::ON_WILL_SHOW);
    if (isShow) {
        for (int32_t index = start; index < end; index++) {
            NotifyDestinationLifecycle(navDestinationNodes[index].second, lifecycle);
        }
    } else {
        for (int32_t index = end - 1; index >= 0 && index >= start; index--) {
            NotifyDestinationLifecycle(navDestinationNodes[index].second, lifecycle);
        }
    }
}

void NavigationPattern::DumpInfo()
{
    if (!navigationStack_) {
        return;
    }
    DumpLog::GetInstance().AddDesc(std::string("size").append(std::to_string(navigationStack_->Size())));
}

bool NavigationPattern::TriggerCustomAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage)
{
    TAG_LOGI(AceLogTag::ACE_NAVIGATION, "will trigger navigation custom animation");
    if ((!preTopNavDestination && !newTopNavDestination) || !onTransition_) {
        return false;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    hostNode->SetIsOnAnimation(true);
    if (!newTopNavDestination) {
        // pop animation with top navDestination, recover navBar visible tag
        hostNode->SetNeedSetInvisible(false);
    }
    auto proxy = AceType::MakeRefPtr<NavigationTransitionProxy>();
    proxy->SetPreDestination(preTopNavDestination);
    proxy->SetTopDestination(newTopNavDestination);
    auto proxyId = proxy->GetProxyId();
    proxyList_.emplace_back(proxy);
    auto navigationTransition = ExecuteTransition(preTopNavDestination, newTopNavDestination, isPopPage);
    if (!navigationTransition.isValid) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "custom transition value is invalid, do default animation");
        return false;
    }
    ExecuteAddAnimation(preTopNavDestination, newTopNavDestination, isPopPage, proxy, navigationTransition);
    ACE_SCOPED_TRACE_COMMERCIAL("Navigation page custom transition start");
    if (navigationTransition.interactive) {
        PerfMonitor::GetPerfMonitor()->Start(PerfConstants::ABILITY_OR_PAGE_SWITCH_INTERACTIVE,
            PerfActionType::FIRST_MOVE, "");
        auto finishCallback = [weakNavigation = WeakClaim(this),
                                  weakPreNavDestination = WeakPtr<NavDestinationGroupNode>(preTopNavDestination),
                                  weakNewNavDestination = WeakPtr<NavDestinationGroupNode>(newTopNavDestination),
                                  isPopPage, proxyId]() {
            auto pattern = weakNavigation.Upgrade();
            CHECK_NULL_VOID(pattern);
            auto proxy = pattern->GetProxyById(proxyId);
            if (proxy == nullptr) {
                return;
            }
            if (!proxy->GetInteractive()) {
                pattern->RemoveProxyById(proxyId);
                return;
            }
            TAG_LOGI(AceLogTag::ACE_NAVIGATION, "interactive animation is finish: %{public}d", proxy->GetIsSuccess());
            pattern->isFinishInteractiveAnimation_ = true;
            auto preDestination = weakPreNavDestination.Upgrade();
            auto topDestination = weakNewNavDestination.Upgrade();
            proxy->SetIsFinished(true);
            // this flag will be update in cancelTransition or finishTransition
            ACE_SCOPED_TRACE_COMMERCIAL("navigation page custom transition end");
            PerfMonitor::GetPerfMonitor()->End(PerfConstants::ABILITY_OR_PAGE_SWITCH_INTERACTIVE, true);
            if (proxy->GetIsSuccess()) {
                pattern->GetNavigationStack()->ClearRecoveryList();
                pattern->OnCustomAnimationFinish(preDestination, topDestination, isPopPage);
            } else {
                // fire page cancel transition
                TAG_LOGI(AceLogTag::ACE_NAVIGATION, "interactive animation canceled");
                pattern->RecoveryToLastStack(preDestination, topDestination);
                pattern->SyncWithJsStackIfNeeded();
            }
            proxy->FireEndCallback();
            pattern->RemoveProxyById(proxyId);
        };
        auto pipelineContext = hostNode->GetContext();
        CHECK_NULL_RETURN(pipelineContext, false);
        auto navigationManager = pipelineContext->GetNavigationManager();
        CHECK_NULL_RETURN(navigationManager, false);
        navigationManager->SetInteractive(hostNode->GetId());
        proxy->SetInteractiveAnimation(AnimationUtils::CreateInteractiveAnimation(
            nullptr, finishCallback), finishCallback);
        navigationTransition.transition(proxy);
        isFinishInteractiveAnimation_ = false;
        navigationManager->FinishInteractiveAnimation();
        proxy->StartAnimation();
    } else {
        PerfMonitor::GetPerfMonitor()->Start(PerfConstants::ABILITY_OR_PAGE_SWITCH, PerfActionType::LAST_UP, "");
        navigationStack_->ClearRecoveryList();
        navigationTransition.transition(proxy);
        // enable render group for text node during custom animation to reduce
        // unnecessary redrawing
        if (isPopPage && preTopNavDestination) {
            preTopNavDestination->UpdateTextNodeListAsRenderGroup(isPopPage, proxy);
        }
        if (!isPopPage && newTopNavDestination) {
            newTopNavDestination->UpdateTextNodeListAsRenderGroup(isPopPage, proxy);
        }
    }

    RefPtr<EventHub> eventHub;
    if (!preTopNavDestination && navigationMode_ == NavigationMode::STACK) {
        auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
        CHECK_NULL_RETURN(hostNode, true);
        auto navBarNode = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
        CHECK_NULL_RETURN(navBarNode, true);
        eventHub = navBarNode->GetEventHub<EventHub>();
    }
    if (preTopNavDestination) {
        eventHub = preTopNavDestination->GetEventHub<EventHub>();
    }
    CHECK_NULL_RETURN(eventHub, true);
    eventHub->SetEnabledInternal(false);
    return true;
}

void NavigationPattern::OnCustomAnimationFinish(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage)
{
    if (!preTopNavDestination && !newTopNavDestination) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "preDestination and topDestination is invalid");
        return;
    }
    ACE_SCOPED_TRACE_COMMERCIAL("Navigation page custom transition end");
    PerfMonitor::GetPerfMonitor()->End(PerfConstants::ABILITY_OR_PAGE_SWITCH, true);
    auto replaceValue = navigationStack_->GetReplaceValue();
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    hostNode->SetIsOnAnimation(false);
    if (preTopNavDestination) {
        preTopNavDestination->SetIsOnAnimation(false);
    }
    if (newTopNavDestination) {
        newTopNavDestination->SetIsOnAnimation(false);
    }
    auto id = hostNode->GetTopDestination() ? hostNode->GetTopDestination()->GetAccessibilityId() : -1;
    hostNode->OnAccessibilityEvent(
        AccessibilityEventType::PAGE_CHANGE, id, WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID);
    do {
        if (replaceValue != 0) {
            hostNode->DealNavigationExit(preTopNavDestination, preTopNavDestination == nullptr);
            navigationStack_->UpdateReplaceValue(0);
            break;
        }
        if ((newTopNavDestination && preTopNavDestination && isPopPage) ||
            (preTopNavDestination && !newTopNavDestination)) {
            PageTransitionType preNodeTransitionType = preTopNavDestination->GetTransitionType();
            if (preNodeTransitionType != PageTransitionType::EXIT_POP) {
                TAG_LOGI(AceLogTag::ACE_NAVIGATION, "previous destination node is executing another transition");
                return;
            }
            auto preDestinationPattern = preTopNavDestination->GetPattern<NavDestinationPattern>();
            CHECK_NULL_VOID(preDestinationPattern);
            auto shallowBuilder = preDestinationPattern->GetShallowBuilder();
            if (shallowBuilder) {
                shallowBuilder->MarkIsExecuteDeepRenderDone(false);
            }
            auto parent = preTopNavDestination->GetParent();
            CHECK_NULL_VOID(parent);
            parent->RemoveChild(preTopNavDestination);
            parent->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
            break;
        }
        if ((newTopNavDestination && preTopNavDestination && !isPopPage) ||
            (!preTopNavDestination && newTopNavDestination && navigationMode_ == NavigationMode::STACK)) {
            hostNode->SetNeedSetInvisible(true);
            RefPtr<FrameNode> node;
            PageTransitionType preNodeTransitionType;
            if (preTopNavDestination) {
                preNodeTransitionType = preTopNavDestination->GetTransitionType();
                node = preTopNavDestination;
            } else {
                // pre destination is nullptr, preNode is navBarNode
                auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
                CHECK_NULL_VOID(navBarNode);
                preNodeTransitionType = navBarNode->GetTransitionType();
                node = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
                CHECK_NULL_VOID(node);
            }
            if (preNodeTransitionType != PageTransitionType::EXIT_PUSH) {
                TAG_LOGI(AceLogTag::ACE_NAVIGATION, "previous destination node is executing another transition");
                return;
            }
            // recover event hub
            auto eventHub = node->GetEventHub<EventHub>();
            if (eventHub) {
                eventHub->SetEnabledInternal(true);
            }
            bool isDialog = newTopNavDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG;
            if (isDialog) {
                return;
            }
            auto property = node->GetLayoutProperty();
            property->UpdateVisibility(VisibleType::INVISIBLE);
            node->SetJSViewActive(false);
            if (!preTopNavDestination) {
                hostNode->NotifyPageHide();
            }
        }
    } while (0);
    hostNode->RemoveDialogDestination();
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    context->MarkNeedFlushMouseEvent();
}

NavigationTransition NavigationPattern::ExecuteTransition(const RefPtr<NavDestinationGroupNode>& preTopDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage)
{
    NavigationTransition navigationTransition;
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, navigationTransition);
    NavigationOperation operation;
    auto currentProxy = GetTopNavigationProxy();
    auto preInfo = currentProxy->GetPreDestinationContext();
    auto topInfo = currentProxy->GetTopDestinationContext();
    auto replaceValue = navigationStack_->GetReplaceValue();
    if (replaceValue != 0) {
        operation = NavigationOperation::REPLACE;
    } else if (!preTopDestination) {
        operation = NavigationOperation::PUSH;
        // if animated with navBarNode, recover navBar visibility
        hostNode->SetNeedSetInvisible(false);
    } else {
        operation = (!newTopNavDestination || isPopPage) ? NavigationOperation::POP : NavigationOperation::PUSH;
    }
    /* set transition animation flag fro navBarNode or navDestinationNode */
    if (operation == NavigationOperation::PUSH) {
        if (preTopDestination != nullptr) {
            preTopDestination->SetTransitionType(PageTransitionType::EXIT_PUSH);
        } else {
            // preTopDestination is nullptr, previous node is navBar node
            auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
            CHECK_NULL_RETURN(navBarNode, navigationTransition);
            navBarNode->SetTransitionType(PageTransitionType::EXIT_PUSH);
        }
        if (newTopNavDestination != nullptr) {
            newTopNavDestination->SetTransitionType(PageTransitionType::ENTER_PUSH);
        }
    }
    if (operation == NavigationOperation::POP) {
        if (preTopDestination != nullptr) {
            preTopDestination->SetTransitionType(PageTransitionType::EXIT_POP);
        }
        if (newTopNavDestination != nullptr) {
            newTopNavDestination->SetTransitionType(PageTransitionType::ENTER_POP);
        } else {
            // newTopNavDestination is nullptr, current node is navBar node
            auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
            CHECK_NULL_RETURN(navBarNode, navigationTransition);
            navBarNode->SetTransitionType(PageTransitionType::ENTER_POP);
        }
    }
    LogCustomAnimationStart(preTopDestination, newTopNavDestination, operation);
    return onTransition_(preInfo, topInfo, operation);
}

void NavigationPattern::OnColorConfigurationUpdate()
{
    auto dividerNode = GetDividerNode();
    CHECK_NULL_VOID(dividerNode);
    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    dividerNode->GetRenderContext()->UpdateBackgroundColor(theme->GetNavigationDividerColor());

    auto dragBarNode = GetDragBarNode();
    CHECK_NULL_VOID(dragBarNode);
    auto dragPattern = dragBarNode->GetPattern<NavigationDragBarPattern>();
    CHECK_NULL_VOID(dragPattern);
    dragPattern->UpdateDefaultColor();
}

void NavigationPattern::UpdatePreNavDesZIndex(const RefPtr<FrameNode> &preTopNavDestination,
    const RefPtr<FrameNode> &newTopNavDestination, int32_t preLastStandardIndex)
{
    auto replaceVal = navigationStack_->GetReplaceValue();
    if (replaceVal != 0 && preTopNavDestination && newTopNavDestination) {
        auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
        CHECK_NULL_VOID(hostNode);
        auto navigationContentNode = AceType::DynamicCast<FrameNode>(hostNode->GetContentNode());
        CHECK_NULL_VOID(navigationContentNode);
        auto newDesNodeContext = newTopNavDestination->GetRenderContext();
        CHECK_NULL_VOID(newDesNodeContext);
        std::optional<int32_t> newNodeZIndex = newDesNodeContext->GetZIndex();
        int32_t standardIndex = newNodeZIndex.value_or(0) - 1;
        auto hideNodes = hostNode->GetHideNodes();
        for (auto iter = hideNodes.begin(); iter != hideNodes.end(); ++iter) {
            // if navdestination nodes is not need removed, default zIndex is satisfied, don't need change
            if (!iter->second) {
                continue;
            }
            auto navDestination = iter->first;
            if (!navDestination) {
                continue;
            }
            auto navDestinationContext = navDestination->GetRenderContext();
            if (!navDestinationContext) {
                continue;
            }
            // get navDestination index in hideNodes, use navdestination index in pre navigation stack
            int32_t hideNodesIndex =
                static_cast<int32_t>(hideNodes.size()) - (navDestination->GetIndex() - preLastStandardIndex);
            navDestinationContext->UpdateZIndex(standardIndex - hideNodesIndex);
        }
        auto preDesNodeContext = preTopNavDestination->GetRenderContext();
        CHECK_NULL_VOID(preDesNodeContext);
        preDesNodeContext->UpdateZIndex(standardIndex);
        navigationContentNode->RebuildRenderContextTree();
        auto context = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(context);
        context->RequestFrame();
    }
}

void NavigationPattern::SetNavigationStack(const RefPtr<NavigationStack>& navigationStack)
{
    if (navigationStack_) {
        navigationStack_->SetOnStateChangedCallback(nullptr);
    }
    navigationStack_ = navigationStack;
    if (navigationStack_) {
        navigationStack_->SetNavigationNode(GetHost());
        WeakPtr<NavigationPattern> weakPattern = WeakClaim(this);
        auto id = Container::CurrentId();
        auto callback = [weakPattern, id]() {
            ContainerScope scope(id);
            auto pattern = weakPattern.Upgrade();
            CHECK_NULL_VOID(pattern);
            if (pattern->NeedSyncWithJsStackMarked()) {
                return;
            }
            pattern->MarkNeedSyncWithJsStack();
            auto context = PipelineContext::GetCurrentContext();
            CHECK_NULL_VOID(context);
            context->AddBuildFinishCallBack([weakPattern]() {
                auto pattern = weakPattern.Upgrade();
                CHECK_NULL_VOID(pattern);
                pattern->SyncWithJsStackIfNeeded();
                auto host = pattern->GetHost();
                CHECK_NULL_VOID(host);
                host->MarkDirtyNode();
            });
            context->RequestFrame();
        };
        navigationStack_->SetOnStateChangedCallback(callback);
    }
}

RefPtr<NavigationPattern> NavigationPattern::GetParentNavigationPattern()
{
    RefPtr<UINode> node = GetHost();
    CHECK_NULL_RETURN(node, nullptr);
    node = node->GetParent();
    while (node) {
        if (node->GetTag() == V2::NAVIGATION_VIEW_ETS_TAG) {
            break;
        }
        node = node->GetParent();
    }
    auto groupNode = AceType::DynamicCast<NavigationGroupNode>(node);
    CHECK_NULL_RETURN(groupNode, nullptr);
    return AceType::DynamicCast<NavigationPattern>(groupNode->GetPattern());
}

void NavigationPattern::AttachNavigationStackToParent()
{
    CHECK_NULL_VOID(navigationStack_);
    auto parentPattern = GetParentNavigationPattern();
    CHECK_NULL_VOID(parentPattern);
    auto parentStack = parentPattern->GetNavigationStack();
    if (parentStack) {
        navigationStack_->OnAttachToParent(parentStack);
    }
}

void NavigationPattern::DetachNavigationStackFromParent()
{
    if (navigationStack_) {
        navigationStack_->OnDetachFromParent();
    }
}

void NavigationPattern::DealTransitionVisibility(const RefPtr<FrameNode>& node, bool isVisible, bool isNavBar)
{
    auto renderContext = node->GetRenderContext();
    if (!renderContext->HasDisappearTransition()) {
        auto layoutProperty = node->GetLayoutProperty();
        layoutProperty->UpdateVisibility(isVisible ? VisibleType::VISIBLE : VisibleType::INVISIBLE);
        node->SetJSViewActive(isVisible);
        return;
    }
    auto layoutProperty = node->GetLayoutProperty();
    layoutProperty->UpdateVisibility(isVisible ? VisibleType::VISIBLE : VisibleType::INVISIBLE, true);
    renderContext->SetTransitionOutCallback([weakNode = WeakPtr<FrameNode>(node), isVisible, isNavBar] {
        auto curNode = weakNode.Upgrade();
        CHECK_NULL_VOID(curNode);
        if (isNavBar) {
            auto navBarNode = AceType::DynamicCast<NavBarNode>(curNode);
            if (navBarNode->GetTransitionType() != PageTransitionType::EXIT_PUSH) {
                return;
            }
        } else {
            auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(curNode);
            if (navDestinationNode->GetTransitionType() != PageTransitionType::EXIT_PUSH) {
                return;
            }
        }
        curNode->SetJSViewActive(isVisible);
    });
}

void NavigationPattern::AddToDumpManager()
{
    auto node = GetHost();
    auto context = PipelineContext::GetCurrentContext();
    if (!node || !context) {
        return;
    }
    auto mgr = context->GetNavigationManager();
    if (!mgr) {
        return;
    }
    auto callback = [weakPattern = WeakClaim(this)](int depth) {
        auto pattern = weakPattern.Upgrade();
        if (!pattern) {
            return;
        }
        const auto& stack = pattern->GetNavigationStack();
        if (!stack) {
            return;
        }
        auto infos = stack->DumpStackInfo();
    };
    mgr->AddNavigationDumpCallback(node->GetId(), node->GetDepth(), callback);
}

void NavigationPattern::RemoveFromDumpManager()
{
    auto node = GetHost();
    auto context = PipelineContext::GetCurrentContext();
    if (!node || !context) {
        return;
    }
    auto mgr = context->GetNavigationManager();
    if (mgr) {
        mgr->RemoveNavigationDumpCallback(node->GetId(), node->GetDepth());
    }
}

void NavigationPattern::FireInterceptionEvent(bool isBefore,
    const std::optional<std::pair<std::string, RefPtr<UINode>>>& newTopPath)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    RefPtr<NavDestinationContext> to;
    if (newTopPath.has_value()) {
        auto topDestination =
            AceType::DynamicCast<NavDestinationGroupNode>(hostNode->GetNavDestinationNode(newTopPath->second));
        if (topDestination) {
            auto pattern = AceType::DynamicCast<NavDestinationPattern>(topDestination->GetPattern());
            to = pattern->GetNavDestinationContext();
        }
    }
    NavigationOperation operation;
    if (isReplace_ != 0) {
        operation = NavigationOperation::REPLACE;
    } else {
        operation = lastPreIndex_ == -1 ? NavigationOperation::POP : NavigationOperation::PUSH;
    }
    auto layoutProperty = hostNode->GetLayoutProperty<NavigationLayoutProperty>();
    // mode is split and stack size is one,don't need to do animation.
    if ((layoutProperty->GetUsrNavigationModeValue(NavigationMode::AUTO) == NavigationMode::SPLIT
        || navigationMode_ == NavigationMode::SPLIT) && !preContext_) {
        isAnimated_ = false;
    }
    navigationStack_->FireNavigationInterception(isBefore, preContext_, to, operation,
        isAnimated_);

    if (!isBefore) {
        NotifyNavDestinationSwitch(preContext_, to, operation);
    }
}

void NavigationPattern::UpdateIsAnimation(const std::optional<std::pair<std::string, RefPtr<UINode>>>& preTopNavPath)
{
    auto disAbleAnimation = navigationStack_->GetDisableAnimation();
    auto animated = navigationStack_->GetAnimatedValue();
    // current animation flag is false
    if (disAbleAnimation || !animated) {
        isAnimated_ = false;
        return;
    }
    // check is dialog mode
    bool isDialog = false;
    if (preTopNavPath.has_value()) {
        auto preDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(preTopNavPath->second));
        if (preDestination) {
            isDialog = isDialog || (preDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG);
        }
    }
    auto topNode = navigationStack_->Get();
    if (topNode) {
        auto newTopDestination = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(topNode));
        if (newTopDestination) {
            isDialog = isDialog || (newTopDestination->GetNavDestinationMode() == NavDestinationMode::DIALOG);
        }
    }
    if (!isDialog) {
        isAnimated_ = true;
        return;
    }
    isAnimated_ = isCustomAnimation_;
}

void NavigationPattern::NotifyNavDestinationSwitch(const RefPtr<NavDestinationContext>& from,
    const RefPtr<NavDestinationContext>& to, NavigationOperation operation)
{
    auto host = GetHost();
    auto NavdestinationSwitchFunc =
        UIObserverHandler::GetInstance().GetHandleNavDestinationSwitchFunc();
    if (!host || !NavdestinationSwitchFunc) {
        return;
    }

    std::string navigationId = host->GetInspectorIdValue("");
    std::optional<NavDestinationInfo> fromInfo;
    std::optional<NavDestinationInfo> toInfo;
    BuildNavDestinationInfoFromContext(navigationId, NavDestinationState::ON_HIDDEN, from, true, fromInfo);
    BuildNavDestinationInfoFromContext(navigationId, NavDestinationState::ON_SHOWN, to, false, toInfo);
    UIObserverHandler::GetInstance().NotifyNavDestinationSwitch(
        std::move(fromInfo), std::move(toInfo), operation);
}

void NavigationPattern::StartTransition(const RefPtr<NavDestinationGroupNode>& preDestination,
    const RefPtr<NavDestinationGroupNode>& topDestination,
    bool isAnimated, bool isPopPage, bool isNeedVisible)
{
    std::string fromPathInfo;
    std::string toPathInfo;
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    bool isNotNeedAnimation = !isAnimated;
#if defined(ENABLE_NAV_SPLIT_MODE)
    isNotNeedAnimation = !isAnimated ||
        (navigationMode_ == NavigationMode::SPLIT && navigationStack_->Size() <= 1 &&
            !isBackPage_ && !isCustomAnimation_);
    TAG_LOGI(AceLogTag::ACE_NAVIGATION, "StartTransition navigationMode_:%{public}d isNotNeedAnimation:%{public}d",
        navigationMode_, isNotNeedAnimation);
#endif

    if (preDestination) {
        fromPathInfo = preDestination->GetNavDestinationPathInfo();
        auto preDestinationPattern = preDestination->GetPattern<NavDestinationPattern>();
        CHECK_NULL_VOID(preDestinationPattern);
        auto navDestinationName = preDestinationPattern->GetName();
        fromPathInfo += ", navDesitinationName: " + navDestinationName;
        if ((isPopPage || preDestination->NeedRemoveInPush()) && isNotNeedAnimation) {
            /**
             * when transition without animation, 'pop' and 'push with remove' need to post
             * afterLayoutTask to delay old top's onDisappear. So set this flag to 'false'
             */
            preDestination->SetIsAnimated(false);
        }
    } else {
        fromPathInfo = hostNode->GetNavigationPathInfo();
    }
    if (topDestination) {
        toPathInfo = topDestination->GetNavDestinationPathInfo();
        auto topDestinationPattern = topDestination->GetPattern<NavDestinationPattern>();
        CHECK_NULL_VOID(topDestinationPattern);
        auto navDestinationName = topDestinationPattern->GetName();
        toPathInfo += ", navDesitinationName: " + navDestinationName;
    } else {
        toPathInfo = hostNode->GetNavigationPathInfo();
    }
    ACE_SCOPED_TRACE_COMMERCIAL("NavDestination Page from %s to %s", fromPathInfo.c_str(), toPathInfo.c_str());

    // fire onWillHide
    if (!isPopPage && !preDestination && navigationMode_ == NavigationMode::STACK) {
        // NavBar will be covered in STACK mode
        auto navBarNode = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
        ProcessAutoSave(navBarNode);
    }
    if (isPopPage || (preDestination &&
        (hostNode->GetLastStandardIndex() > preDestination->GetIndex() || preDestination->NeedRemoveInPush()))) {
        NotifyDestinationLifecycle(preDestination, NavDestinationLifecycle::ON_WILL_HIDE);
    }
    hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_WILL_HIDE);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto navigationManager = pipeline->GetNavigationManager();
    navigationManager->FireNavigationUpdateCallback();
    auto overlayManager = pipeline->GetOverlayManager();
    if (overlayManager) {
        overlayManager->RemoveAllModalInOverlay(false);
    }
    if (topDestination) {
        NotifyDialogChange(NavDestinationLifecycle::ON_WILL_SHOW, true);
    }
    if (isNotNeedAnimation) {
        FireShowAndHideLifecycle(preDestination, topDestination, isPopPage, false);
        TransitionWithOutAnimation(preDestination, topDestination, isPopPage, isNeedVisible);
        return;
    }
    pipeline->AddAfterLayoutTask([weakNavigation = WeakClaim(this),
        weakPreDestination = WeakPtr<NavDestinationGroupNode>(preDestination),
        weakTopDestination = WeakPtr<NavDestinationGroupNode>(topDestination),
        isPopPage, isNeedVisible]() {
        auto navigationPattern = AceType::DynamicCast<NavigationPattern>(weakNavigation.Upgrade());
        CHECK_NULL_VOID(navigationPattern);
        auto preDestination = weakPreDestination.Upgrade();
        auto topDestination = weakTopDestination.Upgrade();
        navigationPattern->FireShowAndHideLifecycle(preDestination, topDestination, isPopPage, true);
        navigationPattern->TransitionWithAnimation(preDestination, topDestination, isPopPage, isNeedVisible);
    });
}

void NavigationPattern::ProcessAutoSave(const RefPtr<FrameNode>& node)
{
    CHECK_NULL_VOID(node);
    if (!node->NeedRequestAutoSave()) {
        return;
    }
    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    container->RequestAutoSave(node);
}

void NavigationPattern::NotifyDestinationLifecycle(const RefPtr<UINode>& uiNode,
    NavDestinationLifecycle lifecycle, NavDestinationActiveReason reason)
{
    auto curDestination =
        AceType::DynamicCast<NavDestinationGroupNode>(NavigationGroupNode::GetNavDestinationNode(uiNode));
    CHECK_NULL_VOID(curDestination);
    auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    if (lifecycle == NavDestinationLifecycle::ON_WILL_DISAPPEAR) {
        NavigationPattern::FireNavigationLifecycleChange(curDestination, lifecycle);
        eventHub->FireOnWillDisAppear();
        return;
    }
    auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    if (lifecycle == NavDestinationLifecycle::ON_INACTIVE) {
        FireOnInactiveLifecycle(curDestination, reason);
        return;
    }
    if (lifecycle == NavDestinationLifecycle::ON_ACTIVE) {
        FireOnActiveLifecycle(curDestination, reason);
        return;
    }
    if ((navDestinationPattern->GetIsOnShow() && (lifecycle == NavDestinationLifecycle::ON_SHOW ||
            lifecycle == NavDestinationLifecycle::ON_WILL_SHOW)) ||
        (!navDestinationPattern->GetIsOnShow() && (lifecycle == NavDestinationLifecycle::ON_HIDE ||
            lifecycle == NavDestinationLifecycle::ON_WILL_HIDE))) {
        return;
    }
    if (lifecycle == NavDestinationLifecycle::ON_WILL_SHOW) {
        eventHub->FireOnWillShow();
        NavigationPattern::FireNavigationLifecycleChange(curDestination, lifecycle);
        return;
    }
    if (lifecycle == NavDestinationLifecycle::ON_SHOW) {
        FireOnShowLifecycle(curDestination);
        return;
    }
    NavigationPattern::FireNavigationLifecycleChange(curDestination, lifecycle);
    if (lifecycle == NavDestinationLifecycle::ON_WILL_HIDE) {
        eventHub->FireOnWillHide();
        return;
    }
    if (lifecycle == NavDestinationLifecycle::ON_HIDE) {
        eventHub->FireOnHiddenEvent(navDestinationPattern->GetName());
        NotifyPageHide(navDestinationPattern->GetName());
        navDestinationPattern->SetIsOnShow(false);
    }
}

void NavigationPattern::FireOnShowLifecycle(const RefPtr<NavDestinationGroupNode>& curDestination)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    auto parentDestinationNode = navigationNode->GetParentDestinationNode().Upgrade();
    if (CheckParentDestinationIsOnhide(parentDestinationNode) && CheckDestinationIsPush(parentDestinationNode)) {
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "parentDestinationNode is onhide");
        return;
    }
    auto param = Recorder::EventRecorder::Get().IsPageParamRecordEnable() ? navigationStack_->GetRouteParam() : "";
    auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    eventHub->FireOnShownEvent(navDestinationPattern->GetName(), param);
    NotifyPageShow(navDestinationPattern->GetName());
    navDestinationPattern->SetIsOnShow(true);
    NavigationPattern::FireNavigationLifecycleChange(curDestination, NavDestinationLifecycle::ON_SHOW);
}

void NavigationPattern::FireOnActiveLifecycle(const RefPtr<NavDestinationGroupNode>& curDestination,
    NavDestinationActiveReason reason)
{
    auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    if (navDestinationPattern->IsActive() || CheckParentDestinationInactive()) {
        return;
    }
    auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    navDestinationPattern->SetIsActive(true);
    eventHub->FireOnActive(static_cast<int32_t>(reason));
    NavigationPattern::FireNavigationLifecycle(curDestination, NavDestinationLifecycle::ON_ACTIVE, reason);
}

void NavigationPattern::FireOnInactiveLifecycle(const RefPtr<NavDestinationGroupNode>& curDestination,
    NavDestinationActiveReason reason)
{
    auto navDestinationPattern = curDestination->GetPattern<NavDestinationPattern>();
    CHECK_NULL_VOID(navDestinationPattern);
    if (!navDestinationPattern->IsActive()) {
        return;
    }
    auto eventHub = curDestination->GetEventHub<NavDestinationEventHub>();
    CHECK_NULL_VOID(eventHub);
    navDestinationPattern->SetIsActive(false);
    NavigationPattern::FireNavigationLifecycle(curDestination, NavDestinationLifecycle::ON_INACTIVE, reason);
    eventHub->FireOnInactive(static_cast<int32_t>(reason));
}

std::unique_ptr<JsonValue> NavigationPattern::GetNavdestinationJsonArray()
{
    auto allNavdestinationInfo = JsonUtil::CreateArray(true);
    const auto& navdestinationNodes = GetAllNavDestinationNodes();
    for (auto iter : navdestinationNodes) {
        auto navdestinationInfo = JsonUtil::Create(true);
        auto navdestinationNode =
            AceType::DynamicCast<NavDestinationGroupNode>(NavigationGroupNode::GetNavDestinationNode(iter.second));
        if (!navdestinationNode) {
            continue;
        }
        if (!navdestinationNode->CanRecovery()) {
            continue;
        }
        auto navdestinationPattern = navdestinationNode->GetPattern<NavDestinationPattern>();
        if (!navdestinationPattern) {
            continue;
        }
        auto name = navdestinationPattern->GetName();
        auto param = navigationStack_->GetStringifyParamByIndex(navdestinationNode->GetIndex());
        auto mode = static_cast<int32_t>(navdestinationNode->GetNavDestinationMode());
        navdestinationInfo->Put("name", name.c_str());
        navdestinationInfo->Put("param", param.c_str());
        navdestinationInfo->Put("mode", mode);
        allNavdestinationInfo->Put(navdestinationInfo);
    }
    return allNavdestinationInfo;
}

void NavigationPattern::RestoreJsStackIfNeeded()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto navigationManager = pipeline->GetNavigationManager();
    CHECK_NULL_VOID(navigationManager);
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto navdestinationsInfo = navigationManager->GetNavigationRecoveryInfo(hostNode->GetCurId());
    if (navdestinationsInfo.empty()) {
        return;
    }
    navigationStack_->SetPathArray(navdestinationsInfo);
}

void NavigationPattern::PerformanceEventReport(int32_t nodeCount, int32_t depth, const std::string& navDestinationName)
{
    if (nodeCount >= PAGE_NODES) {
        EventReport::ReportPageNodeOverflow(navDestinationName, nodeCount, PAGE_NODES);
    }
    if (depth >= PAGE_DEPTH) {
        EventReport::ReportPageDepthOverflow(navDestinationName, depth, PAGE_DEPTH);
    }
}

void NavigationPattern::FireShowAndHideLifecycle(const RefPtr<NavDestinationGroupNode>& preDestination,
    const RefPtr<NavDestinationGroupNode>& topDestination, bool isPopPage, bool isAnimated)
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    // don't move position hide lifecycle is from top to end
    if (preDestination) {
        NotifyDestinationLifecycle(preDestination, NavDestinationLifecycle::ON_INACTIVE);
        auto lastStandardIndex = hostNode->GetLastStandardIndex();
        if (isPopPage || lastStandardIndex > preDestination->GetIndex() || preDestination->NeedRemoveInPush()) {
            // fire preTop Destination lifecycle
            NotifyDestinationLifecycle(preDestination, NavDestinationLifecycle::ON_HIDE);
        }
    }
    // fire remove navDestination and invisible navDestination lifecycle for pop or clear
    hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_HIDE);
    if (isPopPage || (preDestination && preDestination->NeedRemoveInPush())) {
        // fire removed preDestination lifecycle for pop many times or clear
        NotifyDestinationLifecycle(preDestination, NavDestinationLifecycle::ON_WILL_DISAPPEAR);
    }
    // fire removed navDestination lifecycle
    hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_WILL_DISAPPEAR);
    if (!isAnimated) {
        auto pipelineContext = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipelineContext);
        pipelineContext->AddAfterLayoutTask([weakNavigation = WeakClaim(this),
            weakTopDestination = WeakPtr<NavDestinationGroupNode>(topDestination)]() {
            auto navigation = weakNavigation.Upgrade();
            CHECK_NULL_VOID(navigation);
            auto topDestination = weakTopDestination.Upgrade();
            navigation->NotifyDialogChange(NavDestinationLifecycle::ON_SHOW, true);
            navigation->NotifyDestinationLifecycle(topDestination, NavDestinationLifecycle::ON_ACTIVE);
        });
    } else {
        NotifyDialogChange(NavDestinationLifecycle::ON_SHOW, true);
        NotifyDestinationLifecycle(topDestination, NavDestinationLifecycle::ON_ACTIVE);
    }
    FireInterceptionEvent(false, navigationStack_->GetTopNavPath());
}

void NavigationPattern::OnWindowSizeChanged(int32_t  /*width*/, int32_t  /*height*/, WindowSizeChangeReason type)
{
    if (WindowSizeChangeReason::ROTATION == type) {
        auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
        CHECK_NULL_VOID(hostNode);
        AbortAnimation(hostNode);
        CloseLongPressDialog();
    }
}

void NavigationPattern::OnWindowHide()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto navigationPattern = hostNode->GetPattern<NavigationPattern>();
    CHECK_NULL_VOID(navigationPattern);
    navigationPattern->SyncWithJsStackIfNeeded();
}

void NavigationPattern::NotifyPerfMonitorPageMsg(const std::string& pageName)
{
    auto container = Container::Current();
    if (container != nullptr && PerfMonitor::GetPerfMonitor() != nullptr) {
        std::string bundleName = container->GetBundleName();
        PerfMonitor::GetPerfMonitor()->ReportPageShowMsg("", bundleName, pageName);
    }
}

void NavigationPattern::RefreshFocusToDestination()
{
    auto newTopNavPath = navigationStack_->GetTopNavPath();
    if (!newTopNavPath.has_value()) {
        return;
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto navBarNode = AceType::DynamicCast<FrameNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto navBarFocus = navBarNode->GetFocusHub();
    CHECK_NULL_VOID(navBarFocus);
    if (!navBarFocus->IsCurrentFocus()) {
        return;
    }
    auto newTopNavDestination = AceType::DynamicCast<NavDestinationGroupNode>(
        NavigationGroupNode::GetNavDestinationNode(newTopNavPath->second));
    CHECK_NULL_VOID(newTopNavDestination);
    if (!GetIsFocusable(newTopNavDestination)) {
        return;
    }
    auto navDestinationFocusView = newTopNavDestination->GetPattern<FocusView>();
    CHECK_NULL_VOID(navDestinationFocusView);
    if (Container::LessThanAPIVersion(PlatformVersion::VERSION_TWELVE)) {
        navDestinationFocusView->SetIsViewRootScopeFocused(false);
    }
    navDestinationFocusView->FocusViewShow();
}

void NavigationPattern::RecoveryToLastStack(
    const RefPtr<NavDestinationGroupNode>& preTopDestination,
    const RefPtr<NavDestinationGroupNode>& newTopDestination)
{
    if (preTopDestination) {
        preTopDestination->SetIsOnAnimation(false);
    }
    if (newTopDestination) {
        newTopDestination->SetIsOnAnimation(false);
    }
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    hostNode->CleanHideNodes();
    CHECK_NULL_VOID(navigationStack_);
    navigationStack_->SetNavPathList(navigationStack_->GetRecoveryList());

    // update cached node
    auto destinationNodes = navigationStack_->GetAllNavDestinationNodes();
    for (uint32_t index = 0; index < destinationNodes.size(); index++) {
        auto childNode = destinationNodes[index];
        if (!childNode.second) {
            continue;
        }
        auto navDestinationNode = AceType::DynamicCast<NavDestinationGroupNode>(
            NavigationGroupNode::GetNavDestinationNode(childNode.second));
        if (!navDestinationNode) {
            continue;
        }
        // update pre cache node to cache node list
        auto cacheNode = navigationStack_->GetFromCacheNode(childNode.first);
        if (cacheNode && cacheNode == childNode.second) {
            navigationStack_->AddCacheNode(childNode.first, childNode.second);
        }
    }
    hostNode->UpdateNavDestinationNodeWithoutMarkDirty(nullptr, navigationModeChange_);

    // recover lifecycle state before transition
    NotifyDestinationLifecycle(preTopDestination, NavDestinationLifecycle::ON_INACTIVE);
    hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_HIDE);
    hostNode->FireHideNodeChange(NavDestinationLifecycle::ON_WILL_DISAPPEAR);
    NotifyDialogChange(NavDestinationLifecycle::ON_SHOW, true);
    NotifyDestinationLifecycle(newTopDestination, NavDestinationLifecycle::ON_ACTIVE);
    hostNode->RemoveDialogDestination();
    hostNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);

    // update name index
    navigationStack_->RecoveryNavigationStack();
    ACE_SCOPED_TRACE_COMMERCIAL("Navigation page transition end");
    PerfMonitor::GetPerfMonitor()->End(PerfConstants::ABILITY_OR_PAGE_SWITCH, true);
    hostNode->SetIsOnAnimation(false);
    auto id = hostNode->GetTopDestination() ? hostNode->GetTopDestination()->GetAccessibilityId() : -1;
    hostNode->OnAccessibilityEvent(
        AccessibilityEventType::PAGE_CHANGE, id, WindowsContentChangeTypes::CONTENT_CHANGE_TYPE_INVALID);
}

bool NavigationPattern::ExecuteAddAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination,
    bool isPopPage, const RefPtr<NavigationTransitionProxy>& proxy,
    NavigationTransition navigationTransition)
{
    // custom animation return undefined,finish this transition
    if (!navigationTransition.isValid) {
        proxy->SetIsSuccess(false);
        proxy->SetIsFinished(true);
        return false;
    }
    if (preTopNavDestination) {
        preTopNavDestination->SetIsOnAnimation(true);
        if (!isPopPage) {
            auto renderContext = preTopNavDestination->GetRenderContext();
            CHECK_NULL_RETURN(renderContext, false);
            renderContext->RemoveClipWithRRect();
        }
    }
    if (newTopNavDestination) {
        newTopNavDestination->SetIsOnAnimation(true);
    }
    auto proxyId = proxy->GetProxyId();
    proxy->SetInteractive(navigationTransition.interactive);
    // set on transition end callback
    proxy->SetEndCallback(std::move(navigationTransition.endCallback));
    proxy->SetFinishTransitionEvent([weakNavigation = WeakClaim(this),
                                        weakPreNavDestination = WeakPtr<NavDestinationGroupNode>(preTopNavDestination),
                                        weakNewNavDestination = WeakPtr<NavDestinationGroupNode>(newTopNavDestination),
                                        isPopPage, proxyId]() {
        auto pattern = weakNavigation.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto proxy = pattern->GetProxyById(proxyId);
        auto preDestination = weakPreNavDestination.Upgrade();
        auto topDestination = weakNewNavDestination.Upgrade();
        // disable render group for text node after the custom animation
        if (isPopPage && preDestination) {
            preDestination->ReleaseTextNodeList();
        }
        if (!isPopPage && topDestination) {
            topDestination->ReleaseTextNodeList();
        }

        // to avoid call finishTransition many times
        if (proxy == nullptr) {
            TAG_LOGW(AceLogTag::ACE_NAVIGATION, "custom animation proxy is empty or is finished");
            return;
        }
        TAG_LOGI(AceLogTag::ACE_NAVIGATION, "custom animation finish end");
        proxy->SetIsFinished(true);
        // update pre navigation stack
        ACE_SCOPED_TRACE_COMMERCIAL("navigation page custom transition end");
        PerfMonitor::GetPerfMonitor()->End(PerfConstants::ABILITY_OR_PAGE_SWITCH, true);
        pattern->GetNavigationStack()->ClearRecoveryList();
        pattern->OnCustomAnimationFinish(preDestination, topDestination, isPopPage);
        pattern->RemoveProxyById(proxyId);
    });
    // add timeout callback
    auto timeout = navigationTransition.timeout;
    auto pipeline = GetHost()->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto taskExecutor = pipeline->GetTaskExecutor();
    CHECK_NULL_RETURN(taskExecutor, false);
    if (timeout < 0) {
        return true;
    }
    // deal timeout callback
    taskExecutor->PostDelayedTask(
        [weakProxy = WeakPtr<NavigationTransitionProxy>(proxy)] {
            auto transitionProxy = weakProxy.Upgrade();
            CHECK_NULL_VOID(transitionProxy);
            transitionProxy->FireFinishCallback();
        },
        TaskExecutor::TaskType::UI, timeout, "ArkUINavigationTransitionProxyFinish");
    return true;
}

bool NavigationPattern::GetIsFocusable(const RefPtr<FrameNode>& frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto hostNode = AceType::DynamicCast<FrameNode>(GetHost());
    CHECK_NULL_RETURN(hostNode, false);
    auto focusHub = hostNode->GetFocusHub();
    CHECK_NULL_RETURN(focusHub, false);
    if (!focusHub->IsFocusableWholePath()) {
        return false;
    }
    auto currentFocusHub = frameNode->GetFocusHub();
    CHECK_NULL_RETURN(currentFocusHub, false);
    return currentFocusHub->IsFocusableNode();
}

bool NavigationPattern::IsLastStdChange()
{
    // check whether last std navdestination id is changed, change return true, not change return false
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_RETURN(navigationNode, false);
    auto& navPathList = navigationStack_->GetAllNavDestinationNodes();
    auto& preNavPathList = navigationStack_->GetAllNavDestinationNodesPrev();
    auto lastStdIndex = navigationNode->GetLastStandardIndex();
    auto preLastStdIndex = navigationNode->GetPreLastStandardIndex();
    if (preLastStdIndex == -1 && lastStdIndex == -1) {
        return false;
    }
    if (preLastStdIndex != -1 && lastStdIndex != -1) {
        // check new and pre std navdestination id changed or not
        auto preStd = NavigationGroupNode::GetNavDestinationNode(preNavPathList[preLastStdIndex].second.Upgrade());
        auto newStd = NavigationGroupNode::GetNavDestinationNode(navPathList[lastStdIndex].second);
        if (preStd && newStd) {
            return preStd != newStd;
        }
    }
    return true;
}

void NavigationPattern::FollowStdNavdestinationAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);
    if (preTopNavDestination && newTopNavDestination) {
        if (isPopPage) {
            navigationNode->TransitionWithDialogPop(preTopNavDestination, newTopNavDestination);
        } else {
            navigationNode->TransitionWithDialogPush(preTopNavDestination, newTopNavDestination);
        }
        return;
    }
    if (newTopNavDestination && navigationMode_ == NavigationMode::STACK) {
        auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationNode->GetNavBarNode());
        CHECK_NULL_VOID(navBarNode);
        navigationNode->TransitionWithDialogPush(navBarNode, newTopNavDestination, true);
        return;
    }
    if (preTopNavDestination) {
        if (navigationMode_ == NavigationMode::SPLIT) {
            navigationNode->TransitionWithDialogPop(preTopNavDestination, nullptr);
        }
        if (navigationMode_ == NavigationMode::STACK) {
            auto navBarNode = AceType::DynamicCast<NavBarNode>(navigationNode->GetNavBarNode());
            CHECK_NULL_VOID(navBarNode);
            navigationNode->TransitionWithDialogPop(preTopNavDestination, navBarNode, true);
        }
    }
}

void NavigationPattern::TransitionWithDialogAnimation(const RefPtr<NavDestinationGroupNode>& preTopNavDestination,
    const RefPtr<NavDestinationGroupNode>& newTopNavDestination, bool isPopPage)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(navigationNode);

    // if last standard id is not changed and new top navdestination is standard
    if (!isPopPage && !IsLastStdChange() && newTopNavDestination &&
        newTopNavDestination->GetNavDestinationMode() == NavDestinationMode::STANDARD) {
        return;
    }
    auto replaceVal = navigationStack_->GetReplaceValue();
    if (replaceVal != 0) {
        ReplaceAnimation(preTopNavDestination, newTopNavDestination);
        return;
    }
    // last std id is not change, but new dialogs came into stack and upward animation
    if (!IsLastStdChange()) {
        if (isPopPage) {
            navigationNode->StartDialogtransition(preTopNavDestination, newTopNavDestination, false);
        } else {
            if (!preTopNavDestination && navigationMode_ == NavigationMode::SPLIT) {
                // if split mode and push one dialog at the first time, no animation
                return;
            }
            navigationNode->StartDialogtransition(preTopNavDestination, newTopNavDestination, true);
        }
        return;
    }
    FollowStdNavdestinationAnimation(preTopNavDestination, newTopNavDestination, isPopPage);
}

void NavigationPattern::DumpInfo(std::unique_ptr<JsonValue>& json)
{
    if (!navigationStack_) {
        return;
    }
    json->Put("size", std::to_string(navigationStack_->Size()).c_str());
}

void NavigationPattern::CreateDragBarNode(const RefPtr<NavigationGroupNode>& navigationGroupNode)
{
    auto dragBarNode = FrameNode::GetOrCreateFrameNode("DragBar", ElementRegister::GetInstance()->MakeUniqueId(),
        []() { return AceType::MakeRefPtr<NavigationDragBarPattern>(); });
    auto dragBarLayoutProperty = dragBarNode->GetLayoutProperty();
    CHECK_NULL_VOID(dragBarLayoutProperty);
    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    auto renderContext = dragBarNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateBackBlurRadius(DRAG_BAR_BLUR_RADIUS);
    renderContext->UpdateBorderRadius(BorderRadiusProperty(DRAG_BAR_RADIUS));
    renderContext->UpdateZIndex(1);
    dragBarNode->MarkModifyDone();
    auto dragBarItem = CreateDragBarItemNode();
    dragBarItem->MountToParent(dragBarNode);
    dragBarNode->MountToParent(navigationGroupNode);
    navigationGroupNode->SetDragBarNode(dragBarNode);

    auto dragBarPattern = dragBarNode->GetPattern<NavigationDragBarPattern>();
    CHECK_NULL_VOID(dragBarPattern);
    dragBarPattern->UpdateDefaultColor();
}

RefPtr<FrameNode> NavigationPattern::CreateDragBarItemNode()
{
    auto dragBarItemNode = FrameNode::GetOrCreateFrameNode("DragBarItem",
        ElementRegister::GetInstance()->MakeUniqueId(), []() { return AceType::MakeRefPtr<Pattern>(); });
    auto dragBarItemLayoutProperty = dragBarItemNode->GetLayoutProperty();
    CHECK_NULL_RETURN(dragBarItemLayoutProperty, nullptr);
    dragBarItemLayoutProperty->UpdateAlignment(Alignment::CENTER);
    auto renderContext = dragBarItemNode->GetRenderContext();
    CHECK_NULL_RETURN(renderContext, nullptr);
    renderContext->UpdateZIndex(SECOND_ZINDEX_VALUE);
    renderContext->UpdateBorderRadius(BorderRadiusProperty(DRAG_BAR_ITEM_RADIUS));
    dragBarItemNode->MarkModifyDone();
    return dragBarItemNode;
}

RefPtr<NavigationTransitionProxy> NavigationPattern::GetProxyById(uint64_t id) const
{
    for (auto proxy : proxyList_) {
        if (proxy && proxy->GetProxyId() == id) {
            return proxy;
        }
    }
    return nullptr;
}
    
void NavigationPattern::RemoveProxyById(uint64_t id)
{
    for (auto it = proxyList_.begin(); it != proxyList_.end(); ++it) {
        if (*it && (*it)->GetProxyId() == id) {
            it = proxyList_.erase(it);
            return;
        }
    }
}

void NavigationPattern::InitTouchEvent(const RefPtr<GestureEventHub>& gestureHub)
{
    if (touchEvent_) {
        return;
    }
    auto touchTask = [weak = WeakClaim(this)](const TouchEventInfo& info) {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->HandleTouchEvent(info);
    };
    touchEvent_ = MakeRefPtr<TouchEventImpl>(std::move(touchTask));
    gestureHub->AddTouchEvent(touchEvent_);
}

void NavigationPattern::HandleTouchEvent(const TouchEventInfo& info)
{
    auto touchType = info.GetTouches().front().GetTouchType();
    if (touchType == TouchType::DOWN) {
        HandleTouchDown();
    }
    if (touchType == TouchType::UP || touchType == TouchType::CANCEL) {
        HandleTouchUp();
    }
}

void NavigationPattern::HandleTouchDown()
{
    auto dragBarNode = GetDragBarNode();
    CHECK_NULL_VOID(dragBarNode);
    auto dragPattern = dragBarNode->GetPattern<NavigationDragBarPattern>();
    CHECK_NULL_VOID(dragPattern);
    dragPattern->UpdateActiveColor();

    auto dividerNode = GetDividerNode();
    CHECK_NULL_VOID(dividerNode);
    auto dividerRenderContext = dividerNode->GetRenderContext();
    CHECK_NULL_VOID(dividerRenderContext);
    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    NG::Gradient gradient;
    gradient.CreateGradientWithType(NG::GradientType::LINEAR);
    gradient.AddColor(CreatePercentGradientColor(0, theme->GetDviderLightBlueColor()));
    gradient.AddColor(CreatePercentGradientColor(HALF_POSITION, theme->GetDviderDarkBlueColor()));
    gradient.AddColor(CreatePercentGradientColor(END_POSITION, theme->GetDviderLightBlueColor()));
    dividerRenderContext->UpdateBackgroundColor(Color::TRANSPARENT);
    dividerRenderContext->UpdateLinearGradient(gradient);
}

void NavigationPattern::HandleTouchUp()
{
    auto dragBarNode = GetDragBarNode();
    CHECK_NULL_VOID(dragBarNode);
    auto dragPattern = dragBarNode->GetPattern<NavigationDragBarPattern>();
    CHECK_NULL_VOID(dragPattern);
    dragPattern->UpdateDefaultColor();

    auto theme = NavigationGetTheme();
    CHECK_NULL_VOID(theme);
    auto dividerNode = GetDividerNode();
    CHECK_NULL_VOID(dividerNode);
    NG::Gradient gradient;
    gradient.CreateGradientWithType(NG::GradientType::LINEAR);
    gradient.AddColor(CreatePercentGradientColor(0, Color::TRANSPARENT));
    dividerNode->GetRenderContext()->UpdateLinearGradient(gradient);
    dividerNode->GetRenderContext()->UpdateBackgroundColor(theme->GetNavigationDividerColor());
}

void NavigationPattern::CheckContentNeedMeasure(const RefPtr<FrameNode>& node)
{
    auto navigationNode = AceType::DynamicCast<NavigationGroupNode>(node);
    CHECK_NULL_VOID(navigationNode);
    auto navigationLayoutProperty = navigationNode->GetLayoutProperty<NavigationLayoutProperty>();
    CHECK_NULL_VOID(navigationLayoutProperty);
    if (!NavigationLayoutAlgorithm::IsAutoHeight(navigationLayoutProperty)) {
        return;
    }
    TAG_LOGI(AceLogTag::ACE_NAVIGATION, "Navigation height is auto, content need to measure after pushAnimation ends");
    auto contentNode = navigationNode->GetContentNode();
    CHECK_NULL_VOID(contentNode);
    contentNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE);
}

void NavigationPattern::CloseLongPressDialog()
{
    auto hostNode = AceType::DynamicCast<NavigationGroupNode>(GetHost());
    CHECK_NULL_VOID(hostNode);
    auto pipeline = hostNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);

    auto navBarNode = AceType::DynamicCast<NavBarNode>(hostNode->GetNavBarNode());
    CHECK_NULL_VOID(navBarNode);
    auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navBarNode->GetTitleBarNode());
    CHECK_NULL_VOID(titleBarNode);
    auto titleBarPattern = AceType::DynamicCast<TitleBarPattern>(titleBarNode->GetPattern());
    CHECK_NULL_VOID(titleBarPattern);
    auto backButtonDialogNode = titleBarPattern->GetBackButtonDialogNode();
    if (backButtonDialogNode) {
        overlayManager->CloseDialog(backButtonDialogNode);
        titleBarPattern->SetBackButtonDialogNode(nullptr);
    }

    auto menuItemDialogNode = titleBarPattern->GetLargeFontPopUpDialogNode();
    if (menuItemDialogNode) {
        overlayManager->CloseDialog(menuItemDialogNode);
        titleBarPattern->SetLargeFontPopUpDialogNode(nullptr);
    }

    auto toolBarNode = AceType::DynamicCast<NavToolbarNode>(navBarNode->GetToolBarNode());
    CHECK_NULL_VOID(toolBarNode);
    auto toolBarPattern = AceType::DynamicCast<NavToolbarPattern>(toolBarNode->GetPattern());
    CHECK_NULL_VOID(toolBarPattern);
    auto toolBarItemDialogNode = toolBarPattern->GetDialogNode();
    if (toolBarItemDialogNode) {
        overlayManager->CloseDialog(toolBarItemDialogNode);
        toolBarPattern->SetToolBarItemDialogNode(nullptr);
    }
}

bool NavigationPattern::FindInCurStack(const RefPtr<FrameNode>& navDestinationNode)
{
    const auto& navdestinationNodes = GetAllNavDestinationNodes();
    for (auto navdestination : navdestinationNodes) {
        if (navDestinationNode == NavigationGroupNode::GetNavDestinationNode(navdestination.second)) {
            return true;
        }
    }
    return false;
}

void NavigationPattern::SetMouseStyle(MouseFormat format)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContextWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto frameNodeId = host->GetId();
    int32_t windowId = static_cast<int32_t>(pipeline->GetFocusWindowId());
#ifdef WINDOW_SCENE_SUPPORTED
    windowId = static_cast<int32_t>(WindowSceneHelper::GetFocusSystemWindowId(host));
#endif
    pipeline->SetMouseStyleHoldNode(frameNodeId);
    pipeline->ChangeMouseStyle(frameNodeId, format, windowId);
    pipeline->FreeMouseStyleHoldNode(frameNodeId);
}

void NavigationPattern::RegisterContainerModalButtonsRectChangeListener(const RefPtr<FrameNode>& hostNode)
{
    CHECK_NULL_VOID(hostNode);
    auto pipeline = hostNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto mgr = pipeline->GetNavigationManager();
    CHECK_NULL_VOID(mgr);
    mgr->AddButtonsRectChangeListener(hostNode->GetId(), [weakPattern = WeakClaim(this)]() {
        auto pattern = weakPattern.Upgrade();
        CHECK_NULL_VOID(pattern);
        if (!pattern->isFullPageNavigation_) {
            return;
        }
        pattern->MarkAllNavDestinationDirtyIfNeeded(pattern->GetHost());
    });
}

void NavigationPattern::UnregisterContainerModalButtonsRectChangeListener(const RefPtr<FrameNode>& hostNode)
{
    CHECK_NULL_VOID(hostNode);
    auto pipeline = hostNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto mgr = pipeline->GetNavigationManager();
    CHECK_NULL_VOID(mgr);
    mgr->RemoveButtonsRectChangeListener(hostNode->GetId());
}

void NavigationPattern::MarkAllNavDestinationDirtyIfNeeded(const RefPtr<FrameNode>& hostNode)
{
    auto groupNode = AceType::DynamicCast<NavigationGroupNode>(hostNode);
    CHECK_NULL_VOID(groupNode);
    auto pipeline = groupNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (!NavigationTitleUtil::NeedAvoidContainerModal(pipeline)) {
        return;
    }
    auto contentNode = AceType::DynamicCast<FrameNode>(groupNode->GetContentNode());
    CHECK_NULL_VOID(contentNode);
    auto& childrens = contentNode->GetChildren();
    for (auto& child : childrens) {
        auto navDestination = AceType::DynamicCast<NavDestinationGroupNode>(child);
        if (!navDestination) {
            continue;
        }
        if (!navDestination->IsVisible()) {
            navDestination->SetNeedForceMeasure(true);
            continue;
        }
        navDestination->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        auto titleBarNode = AceType::DynamicCast<TitleBarNode>(navDestination->GetTitleBarNode());
        if (titleBarNode) {
            titleBarNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
        }
    }
}

void NavigationPattern::FireNavigationLifecycle(const RefPtr<UINode>& uiNode, NavDestinationLifecycle lifecycle,
    NavDestinationActiveReason reason)
{
    auto frameNode = AceType::DynamicCast<FrameNode>(uiNode);
    CHECK_NULL_VOID(frameNode);
    auto context = frameNode->GetContextRefPtr();
    CHECK_NULL_VOID(context);
    auto navigationManager = context->GetNavigationManager();
    CHECK_NULL_VOID(navigationManager);
    auto navigationIds = navigationManager->FindNavigationInTargetParent(frameNode->GetId());
    for (auto navigationId: navigationIds) {
        auto navigation = AceType::DynamicCast<NavigationGroupNode>(
            FrameNode::GetFrameNode(V2::NAVIGATION_VIEW_ETS_TAG, navigationId));
        if (!navigation) {
            continue;
        }
        auto pattern = navigation->GetPattern<NavigationPattern>();
        if (!pattern) {
            continue;
        }
        auto navigationStack = pattern->GetNavigationStack();
        if (navigationStack) {
            pattern->NotifyDestinationLifecycle(AceType::DynamicCast<NavDestinationGroupNode>(
                NavigationGroupNode::GetNavDestinationNode(navigationStack->Get())), lifecycle, reason);
        }
    }
}
} // namespace OHOS::Ace::NG
