/*
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/container_modal/container_modal_pattern.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/manager/toolbar/toolbar_manager.h"
#include "core/components_ng/pattern/button/button_event_hub.h"
#include "core/components_ng/pattern/button/button_layout_property.h"
#include "core/components_ng/pattern/container_modal/container_modal_theme.h"
#include "core/components_ng/pattern/flex/flex_layout_styles.h"
#include "core/components_ng/pattern/image/image_layout_property.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_property.h"
#include "core/components_ng/pattern/linear_layout/linear_layout_pattern.h"
#include "core/components_ng/pattern/toolbaritem/toolbaritem_pattern.h"
#include "ui/base/ace_type.h"
#include "ui/base/referenced.h"
#include "ui/base/utils/utils.h"

namespace OHOS::Ace::NG {

namespace {
constexpr int32_t LEFT_SPLIT_BUTTON_INDEX = 0;
constexpr int32_t MAX_RECOVER_BUTTON_INDEX = 1;
constexpr int32_t MINIMIZE_BUTTON_INDEX = 2;
constexpr int32_t CLOSE_BUTTON_INDEX = 3;
constexpr int32_t TITLE_POPUP_DURATION = 200;
constexpr double MOUSE_MOVE_POPUP_DISTANCE = 5.0; // 5.0px
constexpr double MOVE_POPUP_DISTANCE_X = 40.0;    // 40.0px
constexpr double MOVE_POPUP_DISTANCE_Y = 20.0;    // 20.0px
constexpr double TITLE_POPUP_DISTANCE = 37.0;     // 37vp height of title
constexpr float TITLE_ITEM_HEIGT_S = 56.0;     // 56vp height of title
constexpr float TITLE_ITEM_HEIGT_M = 64.0;     // 64vp height of title
constexpr float TITLE_ITEM_HEIGT_L = 72.0;     // 72vp height of title
} // namespace

void ContainerModalPattern::ShowTitle(bool isShow, bool hasDeco, bool needUpdate)
{
    auto containerNode = GetHost();
    CHECK_NULL_VOID(containerNode);
    auto customTitleRow = GetCustomTitleRow();
    CHECK_NULL_VOID(customTitleRow);
    auto floatingTitleRow = GetFloatingTitleRow();
    CHECK_NULL_VOID(floatingTitleRow);
    if (needUpdate) {
        TAG_LOGI(AceLogTag::ACE_APPBAR, "title is need update, isFocus_: %{public}d", isFocus_);
        ChangeCustomTitle(isFocus_);
        ChangeControlButtons(isFocus_);
        return;
    }

    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto theme = pipelineContext->GetTheme<ContainerModalTheme>();
    auto stackNode = GetStackNode();
    CHECK_NULL_VOID(stackNode);
    auto windowManager = pipelineContext->GetWindowManager();
    CHECK_NULL_VOID(windowManager);
    windowMode_ = windowManager->GetWindowMode();
    hasDeco_ = hasDeco;
    TAG_LOGI(AceLogTag::ACE_APPBAR, "ShowTitle isShow: %{public}d, windowMode: %{public}d, hasDeco: %{public}d",
        isShow, windowMode_, hasDeco_);
    if (!hasDeco_) {
        isShow = false;
    }
    isTitleShow_ = isShow;
    // update container modal padding and border
    auto layoutProperty = containerNode->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateAlignment(Alignment::TOP_LEFT);
    PaddingProperty padding;
    if (isShow && customTitleSettedShow_) {
        padding = { CalcLength(CONTENT_PADDING), CalcLength(CONTENT_PADDING), std::nullopt,
            CalcLength(CONTENT_PADDING), std::nullopt, std::nullopt };
    }
    layoutProperty->UpdatePadding(padding);
    BorderWidthProperty borderWidth;
    layoutProperty->UpdateBorderWidth(borderWidth);

    auto renderContext = containerNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->SetClipToBounds(true);
    renderContext->UpdateBackgroundColor(GetContainerColor(isFocus_));
    BorderRadiusProperty borderRadius;
    borderRadius.SetRadius(isShow ? CONTAINER_OUTER_RADIUS : 0.0_vp);
    renderContext->UpdateBorderRadius(borderRadius);
    BorderColorProperty borderColor;
    borderColor.SetColor(isShow ? CONTAINER_BORDER_COLOR : Color::TRANSPARENT);
    renderContext->UpdateBorderColor(borderColor);

    // update stack content border
    auto stackLayoutProperty = stackNode->GetLayoutProperty();
    CHECK_NULL_VOID(stackLayoutProperty);
    stackLayoutProperty->UpdateLayoutWeight(1.0f);
    auto stackRenderContext = stackNode->GetRenderContext();
    CHECK_NULL_VOID(stackRenderContext);
    stackRenderContext->SetClipToBounds(true);

    auto customTitleLayoutProperty = customTitleRow->GetLayoutProperty();
    CHECK_NULL_VOID(customTitleLayoutProperty);
    customTitleLayoutProperty->UpdateVisibility(
        (isShow && customTitleSettedShow_) ? VisibleType::VISIBLE : VisibleType::GONE);
    auto floatingLayoutProperty = floatingTitleRow->GetLayoutProperty();
    CHECK_NULL_VOID(floatingLayoutProperty);
    floatingLayoutProperty->UpdateVisibility(VisibleType::GONE);

    auto controlButtonsNode = GetControlButtonRow();
    CHECK_NULL_VOID(controlButtonsNode);
    auto controlButtonsLayoutProperty = controlButtonsNode->GetLayoutProperty();
    CHECK_NULL_VOID(controlButtonsLayoutProperty);
    AddPanEvent(controlButtonsNode);
    ChangeFloatingTitle(isFocus_);
    ChangeControlButtons(isFocus_);

    auto controlButtonsContext = controlButtonsNode->GetRenderContext();
    CHECK_NULL_VOID(controlButtonsContext);
    controlButtonsLayoutProperty->UpdateVisibility(isShow ? VisibleType::VISIBLE : VisibleType::GONE);
}

void ContainerModalPattern::InitContainerEvent()
{
    bool isChangeTitleStyle = SystemProperties::GetTitleStyleEnabled();
    if (isChangeTitleStyle) {
        return;
    }
    auto containerNode = GetHost();
    CHECK_NULL_VOID(containerNode);
    auto touchEventHub = containerNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(touchEventHub);
    auto controlButtonsNode = GetControlButtonRow();
    CHECK_NULL_VOID(controlButtonsNode);
    auto controlButtonsLayoutProperty = controlButtonsNode->GetLayoutProperty();
    CHECK_NULL_VOID(controlButtonsLayoutProperty);
    auto controlButtonsContext = controlButtonsNode->GetRenderContext();
    CHECK_NULL_VOID(controlButtonsContext);

    auto floatingTitleRow = GetFloatingTitleRow();
    CHECK_NULL_VOID(floatingTitleRow);
    auto floatingLayoutProperty = floatingTitleRow->GetLayoutProperty();
    CHECK_NULL_VOID(floatingLayoutProperty);
    auto floatingContext = floatingTitleRow->GetRenderContext();
    CHECK_NULL_VOID(floatingContext);

    auto containerNodeContext = containerNode->GetContext();
    CHECK_NULL_VOID(containerNodeContext);
    auto titlePopupDistance = TITLE_POPUP_DISTANCE * containerNodeContext->GetDensity();
    AnimationOption option;
    option.SetDuration(TITLE_POPUP_DURATION);
    option.SetCurve(Curves::EASE_IN_OUT);

    // init touch event
    touchEventHub->SetTouchEvent([controlButtonsLayoutProperty, floatingLayoutProperty, controlButtonsContext,
                                     floatingContext, option, titlePopupDistance,
                                     weak = WeakClaim(this)](TouchEventInfo& info) {
        auto container = weak.Upgrade();
        CHECK_NULL_VOID(container);
        if (!container->hasDeco_) {
            return;
        }
        if (info.GetChangedTouches().begin()->GetGlobalLocation().GetY() <= titlePopupDistance) {
            // step1. Record the coordinates of the start of the touch.
            if (info.GetChangedTouches().begin()->GetTouchType() == TouchType::DOWN) {
                container->moveX_ = static_cast<float>(info.GetChangedTouches().begin()->GetGlobalLocation().GetX());
                container->moveY_ = static_cast<float>(info.GetChangedTouches().begin()->GetGlobalLocation().GetY());
                return;
            }
            if (info.GetChangedTouches().begin()->GetTouchType() != TouchType::MOVE ||
                !container->CanShowFloatingTitle()) {
                return;
            }

            // step2. Calculate the coordinates of touch move relative to touch down.
            auto deltaMoveX = fabs(info.GetChangedTouches().begin()->GetGlobalLocation().GetX() - container->moveX_);
            auto deltaMoveY = info.GetChangedTouches().begin()->GetGlobalLocation().GetY() - container->moveY_;
            // step3. If the horizontal distance of the touch move does not exceed 10px and the vertical distance
            // exceeds 20px, the floating title will be displayed.
            if (deltaMoveX <= MOVE_POPUP_DISTANCE_X && deltaMoveY >= MOVE_POPUP_DISTANCE_Y) {
                controlButtonsContext->OnTransformTranslateUpdate(
                    { 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
                controlButtonsLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
                AnimationUtils::Animate(option, [controlButtonsContext]() {
                    controlButtonsContext->OnTransformTranslateUpdate({ 0.0f, 0.0f, 0.0f });
                });
                floatingContext->OnTransformTranslateUpdate({ 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
                floatingLayoutProperty->UpdateVisibility(
                    container->floatingTitleSettedShow_ ? VisibleType::VISIBLE : VisibleType::GONE);
                AnimationUtils::Animate(option, [floatingContext]() {
                    floatingContext->OnTransformTranslateUpdate({ 0.0f, 0.0f, 0.0f });
                });
            }
            return;
        }
        if (info.GetChangedTouches().begin()->GetTouchType() != TouchType::DOWN) {
            return;
        }
        if (floatingLayoutProperty->GetVisibilityValue(VisibleType::GONE) != VisibleType::VISIBLE) {
            return;
        }
        // step4. Touch other area to hide floating title.
        AnimationUtils::Animate(
            option,
            [controlButtonsContext, floatingContext, titlePopupDistance]() {
                controlButtonsContext->OnTransformTranslateUpdate(
                    { 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
                floatingContext->OnTransformTranslateUpdate({ 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
            },
            [floatingLayoutProperty, id = Container::CurrentId()]() {
                ContainerScope scope(id);
                floatingLayoutProperty->UpdateVisibility(VisibleType::GONE);
            });
    });

    // init mouse event
    auto mouseEventHub = containerNode->GetOrCreateInputEventHub();
    CHECK_NULL_VOID(mouseEventHub);
    mouseEventHub->SetMouseEvent([controlButtonsLayoutProperty, floatingLayoutProperty, controlButtonsContext,
                                     floatingContext, option, titlePopupDistance,
                                     weak = WeakClaim(this)](MouseInfo& info) {
        auto container = weak.Upgrade();
        CHECK_NULL_VOID(container);
        auto action = info.GetAction();
        if ((action != MouseAction::MOVE && action != MouseAction::WINDOW_LEAVE) || !container->hasDeco_) {
            return;
        }
        if (info.GetLocalLocation().GetY() <= MOUSE_MOVE_POPUP_DISTANCE && container->CanShowFloatingTitle()) {
            controlButtonsContext->OnTransformTranslateUpdate({ 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
            controlButtonsLayoutProperty->UpdateVisibility(VisibleType::VISIBLE);
            AnimationUtils::Animate(option, [controlButtonsContext]() {
                controlButtonsContext->OnTransformTranslateUpdate({ 0.0f, 0.0f, 0.0f });
            });
            floatingContext->OnTransformTranslateUpdate({ 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
            floatingLayoutProperty->UpdateVisibility(
                container->floatingTitleSettedShow_ ? VisibleType::VISIBLE : VisibleType::GONE);
            AnimationUtils::Animate(option, [floatingContext]() {
                floatingContext->OnTransformTranslateUpdate({ 0.0f, 0.0f, 0.0f });
            });
        }

        if (!container->CanHideFloatingTitle()) {
            return;
        }
        if ((info.GetLocalLocation().GetY() >= titlePopupDistance || action == MouseAction::WINDOW_LEAVE) &&
            floatingLayoutProperty->GetVisibilityValue(VisibleType::GONE) == VisibleType::VISIBLE) {
            AnimationUtils::Animate(
                option,
                [controlButtonsContext, floatingContext, titlePopupDistance]() {
                    controlButtonsContext->OnTransformTranslateUpdate(
                        { 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
                    floatingContext->OnTransformTranslateUpdate(
                        { 0.0f, static_cast<float>(-titlePopupDistance), 0.0f });
                },
                [floatingLayoutProperty, id = Container::CurrentId()]() {
                    ContainerScope scope(id);
                    floatingLayoutProperty->UpdateVisibility(VisibleType::GONE);
                });
        }
    });
}

void ContainerModalPattern::AddPanEvent(const RefPtr<FrameNode>& controlButtonsNode)
{
    auto eventHub = controlButtonsNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(eventHub);
    PanDirection panDirection;
    panDirection.type = PanDirection::ALL;

    if (!panEvent_) {
        auto pipeline = PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto windowManager = pipeline->GetWindowManager();
        CHECK_NULL_VOID(windowManager);
        // touch the title to move the floating window
        auto panActionStart = [wk = WeakClaim(RawPtr(windowManager))](const GestureEvent& event) {
            auto windowManager = wk.Upgrade();
            CHECK_NULL_VOID(windowManager);
            auto currentWindowMode = windowManager->GetCurrentWindowMaximizeMode();
            TAG_LOGI(AceLogTag::ACE_APPBAR, "container window pan recognized. currentWindowMode = %{public}d",
                currentWindowMode);
            if ((windowManager->GetCurrentWindowMaximizeMode() != MaximizeMode::MODE_AVOID_SYSTEM_BAR) &&
                (event.GetSourceTool() != SourceTool::TOUCHPAD)) {
                windowManager->WindowStartMove();
                SubwindowManager::GetInstance()->ClearToastInSubwindow();
            }
        };
        panEvent_ = MakeRefPtr<PanEvent>(std::move(panActionStart), nullptr, nullptr, nullptr);
    }
    PanDistanceMap distanceMap = { { SourceTool::UNKNOWN, DEFAULT_PAN_DISTANCE.ConvertToPx() },
        { SourceTool::PEN, DEFAULT_PEN_PAN_DISTANCE.ConvertToPx() } };
    eventHub->AddPanEvent(panEvent_, panDirection, DEFAULT_PAN_FINGER, distanceMap);
}

void ContainerModalPattern::RemovePanEvent(const RefPtr<FrameNode>& controlButtonsNode)
{
    auto eventHub = controlButtonsNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(eventHub);

    if (!panEvent_) {
        return;
    }
    eventHub->RemovePanEvent(panEvent_);
}

void ContainerModalPattern::OnWindowActivated()
{
    WindowFocus(true);
}

void ContainerModalPattern::OnWindowDeactivated()
{
    WindowFocus(false);
}

void ContainerModalPattern::OnWindowForceUnfocused() {}

void ContainerModalPattern::WindowFocus(bool isFocus)
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto theme = pipelineContext->GetTheme<ContainerModalTheme>();
    isFocus_ = isFocus;
    auto containerNode = GetHost();
    CHECK_NULL_VOID(containerNode);

    // update container modal background
    auto renderContext = containerNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    renderContext->UpdateBackgroundColor(GetContainerColor(isFocus_));
    BorderColorProperty borderColor;
    borderColor.SetColor(isFocus ? CONTAINER_BORDER_COLOR : CONTAINER_BORDER_COLOR_LOST_FOCUS);
    renderContext->UpdateBorderColor(borderColor);

    ChangeCustomTitle(isFocus);
    ChangeFloatingTitle(isFocus);
    ChangeControlButtons(isFocus);
}

void ContainerModalPattern::ChangeCustomTitle(bool isFocus)
{
    // update custom title label
    auto customTitleNode = GetCustomTitleNode();
    CHECK_NULL_VOID(customTitleNode);
    isFocus ? customTitleNode->FireOnWindowFocusedCallback() : customTitleNode->FireOnWindowUnfocusedCallback();
}

void ContainerModalPattern::ChangeControlButtons(bool isFocus)
{
    auto containerNode = GetHost();
    CHECK_NULL_VOID(containerNode);
    auto controlButtonsNode = GetControlButtonRow();
    CHECK_NULL_VOID(controlButtonsNode);

    // update leftSplit button
    auto leftSplitButton =
        AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, LEFT_SPLIT_BUTTON_INDEX));
    CHECK_NULL_VOID(leftSplitButton);
    ChangeTitleButtonIcon(leftSplitButton,
        isFocus ? InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_SPLIT_LEFT
                : InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_DEFOCUS_SPLIT_LEFT,
        isFocus, false);

    // hide leftSplit button when window mode is WINDOW_MODE_SPLIT_PRIMARY type or split button can not show
    bool hideLeftSplit = hideSplitButton_ || windowMode_ == WindowMode::WINDOW_MODE_SPLIT_PRIMARY;
    leftSplitButton->GetLayoutProperty()->UpdateVisibility(hideLeftSplit ? VisibleType::GONE : VisibleType::VISIBLE);

    // update maximize button
    auto maximizeButton =
        AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, MAX_RECOVER_BUTTON_INDEX));
    CHECK_NULL_VOID(maximizeButton);
    auto pipeline = PipelineContext::GetCurrentContext();
    auto windowManager = pipeline->GetWindowManager();
    MaximizeMode mode = windowManager->GetCurrentWindowMaximizeMode();
    InternalResource::ResourceId maxId;
    if (mode == MaximizeMode::MODE_AVOID_SYSTEM_BAR || windowMode_ == WindowMode::WINDOW_MODE_FULLSCREEN) {
        maxId = InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_RECOVER;
    } else {
        maxId = InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MAXIMIZE;
    }

    ChangeTitleButtonIcon(maximizeButton, maxId, isFocus, false);
    // update minimize button
    auto minimizeButton =
        AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, MINIMIZE_BUTTON_INDEX));
    CHECK_NULL_VOID(minimizeButton);
    ChangeTitleButtonIcon(minimizeButton,
        InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_MINIMIZE, isFocus, false);

    // update close button
    auto closeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, CLOSE_BUTTON_INDEX));
    CHECK_NULL_VOID(closeButton);
    ChangeTitleButtonIcon(closeButton,
        InternalResource::ResourceId::CONTAINER_MODAL_WINDOW_CLOSE, isFocus, true);
}

void ContainerModalPattern::ChangeFloatingTitle(bool isFocus)
{
    // update floating custom title label
    auto customFloatingTitleNode = GetFloatingTitleNode();
    CHECK_NULL_VOID(customFloatingTitleNode);
    isFocus ? customFloatingTitleNode->FireOnWindowFocusedCallback()
            : customFloatingTitleNode->FireOnWindowUnfocusedCallback();
}

void ContainerModalPattern::ChangeTitleButtonIcon(
    const RefPtr<FrameNode>& buttonNode, InternalResource::ResourceId icon, bool isFocus, bool isCloseBtn)
{
    auto theme = PipelineContext::GetCurrentContext()->GetTheme<ContainerModalTheme>();
    auto renderContext = buttonNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    auto colorType = isFocus ? ControlBtnColorType::NORMAL : ControlBtnColorType::UNFOCUS;
    auto color = theme->GetControlBtnColor(isCloseBtn, colorType);
    renderContext->UpdateBackgroundColor(color);
    auto buttonIcon = AceType::DynamicCast<FrameNode>(buttonNode->GetChildren().front());
    CHECK_NULL_VOID(buttonIcon);
    ImageSourceInfo imageSourceInfo;
    imageSourceInfo.SetResourceId(icon);
    colorType = isFocus ? ControlBtnColorType::NORMAL_FILL : ControlBtnColorType::UNFOCUS_FILL;
    color = theme->GetControlBtnColor(isCloseBtn,  colorType);
    imageSourceInfo.SetFillColor(color);
    auto imageLayoutProperty = buttonIcon->GetLayoutProperty<ImageLayoutProperty>();
    imageLayoutProperty->UpdateImageSourceInfo(imageSourceInfo);
    buttonIcon->MarkModifyDone();
    buttonNode->MarkModifyDone();
}

bool ContainerModalPattern::CanShowFloatingTitle()
{
    auto floatingTitleRow = GetFloatingTitleRow();
    CHECK_NULL_RETURN(floatingTitleRow, false);
    auto floatingLayoutProperty = floatingTitleRow->GetLayoutProperty();
    CHECK_NULL_RETURN(floatingLayoutProperty, false);

    if (windowMode_ != WindowMode::WINDOW_MODE_FULLSCREEN && windowMode_ != WindowMode::WINDOW_MODE_SPLIT_PRIMARY &&
        windowMode_ != WindowMode::WINDOW_MODE_SPLIT_SECONDARY) {
        TAG_LOGI(AceLogTag::ACE_APPBAR, "Window is not full screen or split screen, can not show floating title.");
        return false;
    }

    if (floatingLayoutProperty->GetVisibilityValue(VisibleType::GONE) == VisibleType::VISIBLE) {
        TAG_LOGI(AceLogTag::ACE_APPBAR, "Floating tittle is visible now, no need to show again.");
        return false;
    }
    return true;
}

void ContainerModalPattern::SetAppTitle(const std::string& title)
{
    TAG_LOGI(AceLogTag::ACE_APPBAR, "SetAppTitle successfully");
    auto customTitleNode = GetCustomTitleNode();
    CHECK_NULL_VOID(customTitleNode);
    customTitleNode->FireAppTitleCallback(title);

    // call setTitle() callback for backButton bar
    auto controllButtonRow = GetCustomButtonNode();
    CHECK_NULL_VOID(controllButtonRow);
    controllButtonRow->FireAppTitleCallback(title);

    auto customFloatingTitleNode = GetFloatingTitleNode();
    CHECK_NULL_VOID(customFloatingTitleNode);
    customFloatingTitleNode->FireAppTitleCallback(title);
}

void ContainerModalPattern::SetAppIcon(const RefPtr<PixelMap>& icon)
{
    CHECK_NULL_VOID(icon);
    TAG_LOGI(AceLogTag::ACE_APPBAR, "SetAppIcon successfully");
    auto customTitleNode = GetCustomTitleNode();
    CHECK_NULL_VOID(customTitleNode);
    customTitleNode->FireAppIconCallback(icon);

    auto customFloatingTitleNode = GetFloatingTitleNode();
    CHECK_NULL_VOID(customFloatingTitleNode);
    customFloatingTitleNode->FireAppIconCallback(icon);
}

void ContainerModalPattern::SetTitleButtonHide(
    const RefPtr<FrameNode>& controlButtonsNode, bool hideSplit, bool hideMaximize, bool hideMinimize, bool hideClose)
{
    auto leftSplitButton =
        AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, LEFT_SPLIT_BUTTON_INDEX));
    CHECK_NULL_VOID(leftSplitButton);
    leftSplitButton->GetLayoutProperty()->UpdateVisibility(hideSplit ? VisibleType::GONE : VisibleType::VISIBLE);
    leftSplitButton->MarkDirtyNode();

    auto maximizeButton =
        AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, MAX_RECOVER_BUTTON_INDEX));
    CHECK_NULL_VOID(maximizeButton);
    maximizeButton->GetLayoutProperty()->UpdateVisibility(hideMaximize ? VisibleType::GONE : VisibleType::VISIBLE);
    maximizeButton->MarkDirtyNode();

    auto minimizeButton = AceType::DynamicCast<FrameNode>(
        GetTitleItemByIndex(controlButtonsNode, MINIMIZE_BUTTON_INDEX));
    CHECK_NULL_VOID(minimizeButton);
    minimizeButton->GetLayoutProperty()->UpdateVisibility(hideMinimize ? VisibleType::GONE : VisibleType::VISIBLE);
    minimizeButton->MarkDirtyNode();

    auto closeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsNode, CLOSE_BUTTON_INDEX));
    CHECK_NULL_VOID(closeButton);
    closeButton->GetLayoutProperty()->UpdateVisibility(hideClose ? VisibleType::GONE : VisibleType::VISIBLE);
    closeButton->MarkDirtyNode();
}

void ContainerModalPattern::SetContainerButtonHide(bool hideSplit, bool hideMaximize, bool hideMinimize, bool hideClose)
{
    auto controlButtonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(controlButtonsRow);
    SetTitleButtonHide(controlButtonsRow, hideSplit, hideMaximize, hideMinimize, hideClose);
    hideSplitButton_ = hideSplit;
    TAG_LOGI(AceLogTag::ACE_APPBAR,
        "Set containerModal button status successfully, "
        "hideSplit: %{public}d, hideMaximize: %{public}d, "
        "hideMinimize: %{public}d, hideClose: %{public}d",
        hideSplit, hideMaximize, hideMinimize, hideClose);
}

void ContainerModalPattern::SetCloseButtonStatus(bool isEnabled)
{
    auto controlButtonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(controlButtonsRow);

    // set closeButton enable or disable
    auto closeButton = AceType::DynamicCast<FrameNode>(GetTitleItemByIndex(controlButtonsRow, CLOSE_BUTTON_INDEX));
    CHECK_NULL_VOID(closeButton);
    auto buttonEvent = closeButton->GetOrCreateEventHub<ButtonEventHub>();
    CHECK_NULL_VOID(buttonEvent);
    buttonEvent->SetEnabled(isEnabled);
    TAG_LOGI(AceLogTag::ACE_APPBAR, "Set close button status %{public}s", isEnabled ? "enable" : "disable");
}

void ContainerModalPattern::SetWindowContainerColor(const Color& activeColor, const Color& inactiveColor)
{
    auto theme = PipelineContext::GetCurrentContext()->GetTheme<ContainerModalTheme>();
    auto containerNode = GetHost();
    CHECK_NULL_VOID(containerNode);
    // update container modal background
    auto renderContext = containerNode->GetRenderContext();
    CHECK_NULL_VOID(renderContext);
    activeColor_ = activeColor;
    inactiveColor_ = inactiveColor;
    isCustomColor_ = true;
    renderContext->UpdateBackgroundColor(GetContainerColor(isFocus_));
}

Color ContainerModalPattern::GetContainerColor(bool isFocus)
{
    auto theme = PipelineContext::GetCurrentContext()->GetTheme<ContainerModalTheme>();
    return isCustomColor_ ? (isFocus ? activeColor_ : inactiveColor_) : theme->GetBackGroundColor(isFocus);
}

void ContainerModalPattern::UpdateGestureRowVisible()
{
    auto gestureRow = GetGestureRow();
    CHECK_NULL_VOID(gestureRow);
    auto customTitleRow = GetCustomTitleRow();
    CHECK_NULL_VOID(customTitleRow);
    auto buttonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(buttonsRow);
    auto gestureRowProp = gestureRow->GetLayoutProperty();
    auto customTitleRowProp = customTitleRow->GetLayoutProperty();
    auto buttonsRowProp = buttonsRow->GetLayoutProperty();
    if (customTitleRowProp->GetVisibilityValue(VisibleType::VISIBLE) == VisibleType::GONE &&
        buttonsRowProp->GetVisibilityValue(VisibleType::VISIBLE) == VisibleType::VISIBLE) {
        gestureRowProp->UpdateVisibility(VisibleType::VISIBLE);
    } else {
        gestureRowProp->UpdateVisibility(VisibleType::GONE);
    }
}

void ContainerModalPattern::SetContainerModalTitleVisible(bool customTitleSettedShow, bool floatingTitleSettedShow)
{
    TAG_LOGI(AceLogTag::ACE_APPBAR,
        "ContainerModal customTitleSettedShow=%{public}d, floatingTitleSettedShow=%{public}d",
        customTitleSettedShow, floatingTitleSettedShow);
    customTitleSettedShow_ = customTitleSettedShow;
    auto customTitleRow = GetCustomTitleRow();
    CHECK_NULL_VOID(customTitleRow);
    auto customTitleRowProp = customTitleRow->GetLayoutProperty();
    if (!customTitleSettedShow) {
        customTitleRowProp->UpdateVisibility(VisibleType::GONE);
    } else if (CanShowCustomTitle()) {
        customTitleRowProp->UpdateVisibility(VisibleType::VISIBLE);
    }
    floatingTitleSettedShow_ = floatingTitleSettedShow;
    auto floatingTitleRow = GetFloatingTitleRow();
    CHECK_NULL_VOID(floatingTitleRow);
    auto floatingTitleRowProp = floatingTitleRow->GetLayoutProperty();
    if (!floatingTitleSettedShow) {
        floatingTitleRowProp->UpdateVisibility(VisibleType::GONE);
    }

    auto buttonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(buttonsRow);
    buttonsRow->SetHitTestMode(HitTestMode::HTMTRANSPARENT_SELF);
    UpdateGestureRowVisible();
}

bool ContainerModalPattern::GetContainerModalTitleVisible(bool isImmersive)
{
    if (isImmersive) {
        auto floatingTitleRow = GetFloatingTitleRow();
        CHECK_NULL_RETURN(floatingTitleRow, false);
        auto floatingLayoutProperty = floatingTitleRow->GetLayoutProperty();
        CHECK_NULL_RETURN(floatingLayoutProperty, false);
        return floatingLayoutProperty->GetVisibilityValue(VisibleType::GONE) == VisibleType::VISIBLE;
    } else {
        return isTitleShow_ && customTitleSettedShow_;
    }
}

void ContainerModalPattern::SetContainerModalTitleHeight(int32_t height)
{
    TAG_LOGI(AceLogTag::ACE_APPBAR, "ContainerModal SetContainerModalTitleHeight height=%{public}d", height);
    if (height < 0) {
        height = 0;
    }
    titleHeight_ = Dimension(Dimension(height, DimensionUnit::PX).ConvertToVp(), DimensionUnit::VP);
    auto customTitleRow = GetCustomTitleRow();
    UpdateRowHeight(customTitleRow, titleHeight_);
    auto controlButtonsRow = GetControlButtonRow();
    UpdateRowHeight(controlButtonsRow, titleHeight_);
    auto gestureRow = GetGestureRow();
    UpdateRowHeight(gestureRow, titleHeight_);
    CallButtonsRectChange();
}

int32_t ContainerModalPattern::GetContainerModalTitleHeight()
{
    return static_cast<int32_t>(ceil(titleHeight_.ConvertToPx()));
}

bool ContainerModalPattern::GetContainerModalButtonsRect(RectF& containerModal, RectF& buttons)
{
    auto column = GetColumnNode();
    CHECK_NULL_RETURN(column, false);
    auto columnRect = column->GetGeometryNode()->GetFrameRect();
    containerModal = columnRect;
    if (columnRect.Width() == 0) {
        TAG_LOGW(AceLogTag::ACE_APPBAR, "Get rect of buttons failed, the rect is measuring.");
        return false;
    }

    auto controlButtonsRow = GetControlButtonRow();
    CHECK_NULL_RETURN(controlButtonsRow, false);
    auto controlButtonsRowLayoutProperty = controlButtonsRow->GetLayoutProperty();
    CHECK_NULL_RETURN(controlButtonsRowLayoutProperty, false);
    if (controlButtonsRowLayoutProperty->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE) {
        TAG_LOGW(AceLogTag::ACE_APPBAR, "Get rect of buttons failed, buttonRow are hidden");
        return false;
    }
    auto children = controlButtonsRow->GetChildren();
    RectF firstButtonRect;
    RectF lastButtonRect;
    for (auto& child : children) {
        auto node = AceType::DynamicCast<FrameNode>(child);
        if (node->GetLayoutProperty()->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE) {
            continue;
        }
        auto rect = node->GetGeometryNode()->GetFrameRect();
        if (firstButtonRect.Width() == 0) {
            firstButtonRect = rect;
        }
        lastButtonRect = rect;
    }
    buttons = firstButtonRect.CombineRectT(lastButtonRect);
    if (buttons.Width() == 0) {
        TAG_LOGW(AceLogTag::ACE_APPBAR, "Get rect of buttons failed, buttons are hidden");
        return false;
    }

    auto widthByPx = (TITLE_PADDING_START + TITLE_PADDING_END).ConvertToPx() + buttons.Width();
    auto isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
    if (isRtl) {
        buttons.SetLeft(0);
    } else {
        buttons.SetLeft(containerModal.Width() - widthByPx);
    }
    buttons.SetTop(0);
    buttons.SetWidth(widthByPx);
    buttons.SetHeight(titleHeight_.ConvertToPx());
    return true;
}

void ContainerModalPattern::SubscribeContainerModalButtonsRectChange(
    std::function<void(RectF& containerModal, RectF& buttons)>&& callback)
{
    controlButtonsRectChangeCallback_ = std::move(callback);
}

void ContainerModalPattern::GetWindowPaintRectWithoutMeasureAndLayout(RectInt& rect)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    auto titleHeight = round(GetCustomTitleHeight().ConvertToPx());
    auto padding = layoutProperty->CreatePaddingAndBorder();
    rect.SetRect(padding.Offset().GetX(), padding.Offset().GetY() + titleHeight, rect.Width() - padding.Width(),
        rect.Height() - padding.Height() - titleHeight);
}

void ContainerModalPattern::GetWindowPaintRectWithoutMeasureAndLayout(Rect& rect, bool isContainerModal)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto layoutProperty = host->GetLayoutProperty();
    CHECK_NULL_VOID(layoutProperty);
    auto titleHeight = GetCustomTitleHeight().ConvertToPx();
    auto padding = layoutProperty->CreatePaddingAndBorder();
    rect.SetRect(
        padding.Offset().GetX(),
        isContainerModal ? padding.Offset().GetY() + titleHeight : padding.Offset().GetY(),
        rect.Width() - padding.Width(),
        rect.Height() - padding.Height() - titleHeight);
}

void ContainerModalPattern::CallButtonsRectChange()
{
    RectF containerModal;
    RectF buttons;
    GetContainerModalButtonsRect(containerModal, buttons);
    NotifyButtonsRectChange(containerModal, buttons);
    CHECK_NULL_VOID(controlButtonsRectChangeCallback_);
    if (AceApplicationInfo::GetInstance().GreatOrEqualTargetAPIVersion(PlatformVersion::VERSION_FIFTEEN)) {
        if (isInitButtonsRect_ && buttonsRect_ == buttons) {
            return;
        }
        isInitButtonsRect_ = true;
    }
    buttonsRect_ = buttons;
    auto taskExecutor = Container::CurrentTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(
        [containerModal, buttons, cb = controlButtonsRectChangeCallback_]() mutable {
            if (cb) {
                cb(containerModal, buttons);
            }
        },
        TaskExecutor::TaskType::JS, "ArkUIContainerModalButtonsRectChange");
}

void ContainerModalPattern::InitTitle()
{
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto themeManager = pipeline->GetThemeManager();
    CHECK_NULL_VOID(themeManager);
    auto themeConstants = themeManager->GetThemeConstants();
    CHECK_NULL_VOID(themeConstants);
    auto id = pipeline->GetWindowManager()->GetAppIconId();
    auto pixelMap = themeConstants->GetPixelMap(id);
    if (pixelMap) {
        RefPtr<PixelMap> icon = PixelMap::CreatePixelMap(&pixelMap);
        SetAppIcon(icon);
    } else {
        TAG_LOGW(AceLogTag::ACE_APPBAR, "Cannot get pixelmap, try media path."); // use themeConstants GetMediaPath
    }
    SetAppTitle(themeConstants->GetString(pipeline->GetWindowManager()->GetAppLabelId()));
}

void ContainerModalPattern::Init()
{
    InitContainerColor();
    InitContainerEvent();
    InitTitle();
    InitLayoutProperty();
}

void ContainerModalPattern::InitContainerColor()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto theme = pipelineContext->GetTheme<ContainerModalTheme>();
    activeColor_ = theme->GetBackGroundColor(true);
    inactiveColor_ = theme->GetBackGroundColor(false);
}

void ContainerModalPattern::OnColorConfigurationUpdate()
{
    WindowFocus(isFocus_);
}

void ContainerModalPattern::InitLayoutProperty()
{
    auto containerModal = GetHost();
    CHECK_NULL_VOID(containerModal);
    auto column = GetColumnNode();
    CHECK_NULL_VOID(column);
    auto stack = GetStackNode();
    CHECK_NULL_VOID(stack);
    auto content = GetContentNode();
    CHECK_NULL_VOID(content);
    auto buttonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(buttonsRow);
    auto contentProperty = content->GetLayoutProperty();
    CHECK_NULL_VOID(contentProperty);
    auto buttonsRowProperty = buttonsRow->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(buttonsRowProperty);
    containerModal->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
    column->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
    stack->GetLayoutProperty()->UpdateMeasureType(MeasureType::MATCH_PARENT);
    contentProperty->UpdateMeasureType(MeasureType::MATCH_CONTENT);
    contentProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(1.0, DimensionUnit::PERCENT), CalcLength(1.0, DimensionUnit::PERCENT)));
    buttonsRowProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    auto buttonHeight = (CONTAINER_TITLE_HEIGHT == titleHeight_) ? CONTAINER_TITLE_HEIGHT : titleHeight_;
    buttonsRowProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(1.0, DimensionUnit::PERCENT), CalcLength(buttonHeight)));
    buttonsRowProperty->UpdateMainAxisAlign(FlexAlign::FLEX_END);
    buttonsRowProperty->UpdateCrossAxisAlign(FlexAlign::CENTER);

    InitTitleRowLayoutProperty(GetCustomTitleRow(), false);
    InitTitleRowLayoutProperty(GetFloatingTitleRow(), true);
    InitButtonsLayoutProperty();

    containerModal->MarkModifyDone();
}

void ContainerModalPattern::InitTitleRowLayoutProperty(RefPtr<FrameNode> titleRow, bool isFloating)
{
    CHECK_NULL_VOID(titleRow);
    auto titleRowProperty = titleRow->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(titleRowProperty);
    titleRowProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    auto rowHeight = CONTAINER_TITLE_HEIGHT;
    if (!isFloating) {
        rowHeight = (CONTAINER_TITLE_HEIGHT == titleHeight_) ? CONTAINER_TITLE_HEIGHT : titleHeight_;
    }
    titleRowProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(1.0, DimensionUnit::PERCENT), CalcLength(rowHeight)));
    titleRowProperty->UpdateMainAxisAlign(FlexAlign::FLEX_START);
    titleRowProperty->UpdateCrossAxisAlign(FlexAlign::CENTER);
    auto isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
    PaddingProperty padding;
    auto sidePadding = isRtl ? &padding.left : &padding.right;
    *sidePadding = GetControlButtonRowWidth();
    titleRowProperty->UpdatePadding(padding);
}

void ContainerModalPattern::InitAllTitleRowLayoutProperty()
{
    auto containerModal = GetHost();
    CHECK_NULL_VOID(containerModal);
    auto customTitleRow = GetCustomTitleRow();
    CHECK_NULL_VOID(customTitleRow);
    auto floatingTitleRow = GetFloatingTitleRow();
    CHECK_NULL_VOID(floatingTitleRow);
    InitTitleRowLayoutProperty(customTitleRow, false);
    customTitleRow->MarkModifyDone();
    customTitleRow->MarkDirtyNode(NG::PROPERTY_UPDATE_MEASURE);
    InitTitleRowLayoutProperty(floatingTitleRow, true);
    floatingTitleRow->MarkModifyDone();
    floatingTitleRow->MarkDirtyNode(NG::PROPERTY_UPDATE_MEASURE);
}

CalcLength ContainerModalPattern::GetControlButtonRowWidth()
{
    auto row = GetControlButtonRow();
    // default
    int32_t buttonNum = 0;
    const auto& children = row->GetChildren();
    for (const auto& child : children) {
        auto childButton = AceType::DynamicCast<FrameNode>(child);
        if (childButton && childButton->IsVisible()) {
            buttonNum++;
        }
    }
    return CalcLength(TITLE_ELEMENT_MARGIN_HORIZONTAL * (buttonNum - 1) + TITLE_BUTTON_SIZE * buttonNum +
                      TITLE_PADDING_START + TITLE_PADDING_END);
}

void ContainerModalPattern::InitColumnTouchTestFunc()
{
    auto column = GetColumnNode();
    CHECK_NULL_VOID(column);
    auto eventHub = column->GetOrCreateGestureEventHub();
    bool defaultResEnable = enableContainerModalCustomGesture_;
    auto func = [defaultResEnable](const std::vector<TouchTestInfo>& touchInfo) -> TouchResult {
        TouchResult touchRes;
        TouchResult defaultRes;
        touchRes.strategy = TouchTestStrategy::FORWARD_COMPETITION;
        defaultRes.strategy = TouchTestStrategy::DEFAULT;
        defaultRes.id = "";
        if (defaultResEnable) {
            return defaultRes;
        }
        for (auto info : touchInfo) {
            if (info.id.compare(CONTAINER_MODAL_STACK_ID) == 0) {
                touchRes.id = info.id;
                return touchRes;
            }
        }
        return defaultRes;
    };
    eventHub->SetOnTouchTestFunc(func);
}

void ContainerModalPattern::InitButtonsLayoutProperty()
{
    auto buttonsRow = GetControlButtonRow();
    CHECK_NULL_VOID(buttonsRow);
    auto isRtl = AceApplicationInfo::GetInstance().IsRightToLeft();
    auto buttons = buttonsRow->GetChildren();
    for (uint64_t index = 0; index < buttons.size(); index++) {
        auto space = (index == buttons.size() - 1) ? TITLE_PADDING_END : TITLE_ELEMENT_MARGIN_HORIZONTAL;
        MarginProperty margin;
        if (isRtl) {
            margin.left = CalcLength(space);
            margin.right = CalcLength();
        } else {
            margin.left = CalcLength();
            margin.right = CalcLength(space);
        }
        auto button = AceType::DynamicCast<FrameNode>(buttonsRow->GetChildAtIndex(index));
        CHECK_NULL_VOID(button);
        auto layoutProp = button->GetLayoutProperty<ButtonLayoutProperty>();
        layoutProp->UpdateMargin(margin);
        button->MarkModifyDone();
        button->MarkDirtyNode();
    }
}

void ContainerModalPattern::OnLanguageConfigurationUpdate()
{
    InitTitle();
    InitLayoutProperty();
}

Dimension ContainerModalPattern::GetCustomTitleHeight()
{
    auto customTitleRow = GetCustomTitleRow();
    Dimension zeroHeight;
    CHECK_NULL_RETURN(customTitleRow, zeroHeight);
    auto property = customTitleRow->GetLayoutProperty();
    if (property->GetVisibilityValue(VisibleType::VISIBLE) != VisibleType::VISIBLE) {
        return zeroHeight;
    }
    return titleHeight_;
}

bool ContainerModalPattern::CanShowCustomTitle()
{
    auto buttonsRow = GetControlButtonRow();
    CHECK_NULL_RETURN(buttonsRow, false);
    auto visibility = buttonsRow->GetLayoutProperty()->GetVisibilityValue(VisibleType::GONE);
    return visibility == VisibleType::VISIBLE;
}

bool ContainerModalPattern::OnDirtyLayoutWrapperSwap(
    const RefPtr<LayoutWrapper>& dirty,
    const DirtySwapConfig& config)
{
    CallButtonsRectChange();

    auto considerFloatingWindow = false;
    CallSetContainerWindow(considerFloatingWindow);
    
    return false;
}

void ContainerModalPattern::CallSetContainerWindow(bool considerFloatingWindow)
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto curWindowRect = pipelineContext->GetCurrentWindowRect();
    auto isContainerModal = pipelineContext->GetWindowModal() == WindowModal::CONTAINER_MODAL;
    GetWindowPaintRectWithoutMeasureAndLayout(curWindowRect, isContainerModal);

    auto borderRadius = 0.0_vp;
    if (considerFloatingWindow) {
        auto windowManager = pipelineContext->GetWindowManager();
        CHECK_NULL_VOID(windowManager);
        bool isFloatingWindow = windowManager->GetWindowMode() == WindowMode::WINDOW_MODE_FLOATING;
        borderRadius = (isFloatingWindow && isTitleShow_) ? CONTAINER_OUTER_RADIUS : 0.0_vp;
    } else {
        borderRadius = isTitleShow_ ? CONTAINER_OUTER_RADIUS : 0.0_vp;
    }
    auto borderRadiusValue = borderRadius.ConvertToPx();

    auto expectRect = RRect::MakeRect(curWindowRect);
    expectRect.SetRectWithSimpleRadius(curWindowRect, borderRadiusValue, borderRadiusValue);
    if (windowPaintRect_ == expectRect) {
        return;
    }

    // set container window show state to RS
    pipelineContext->SetContainerWindow(isTitleShow_, expectRect);
    windowPaintRect_ = expectRect;
}
void ContainerModalPattern::UpdateRowHeight(const RefPtr<FrameNode>& row, Dimension height)
{
    CHECK_NULL_VOID(row);
    auto layoutProperty = row->GetLayoutProperty<LinearLayoutProperty>();
    CHECK_NULL_VOID(layoutProperty);
    layoutProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(1.0, DimensionUnit::PERCENT), CalcLength(height)));
    row->MarkModifyDone();
    row->MarkDirtyNode();
}

void ContainerModalPattern::EnableContainerModalCustomGesture(RefPtr<PipelineContext> pipeline, bool enable)
{
    CHECK_NULL_VOID(pipeline);
    if (!pipeline || pipeline->GetWindowModal() != WindowModal::CONTAINER_MODAL) {
        return;
    }
    auto rootNode = pipeline->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto containerNode = AceType::DynamicCast<FrameNode>(rootNode->GetChildren().front());
    CHECK_NULL_VOID(containerNode);
    auto containerPattern = containerNode->GetPattern<ContainerModalPattern>();
    CHECK_NULL_VOID(containerPattern);
    containerPattern->SetEnableContainerModalCustomGesture(enable);
    containerPattern->InitColumnTouchTestFunc();
}

void ContainerModalPattern::InitToolBarManager()
{
    if (!toolbarManager_) {
        auto pipeline = GetHost()->GetContext();
        CHECK_NULL_VOID(pipeline);
        toolbarManager_ = pipeline->GetToolbarManager();

        std::function<void()> func = [weak = WeakClaim(this)]() {
            auto pattern = weak.Upgrade();
            CHECK_NULL_VOID(pattern);
            pattern->OnToolBarLayoutChange();
        };
        toolbarManager_->SetToolBarChangeCallback(std::move(func));

        std::function<void()> getTypeOfItem = [weak = WeakClaim(this)]() {
            auto pattern = weak.Upgrade();
            pattern->PrasePlaceMentType();
        };
        toolbarManager_->SetModifyDoneCallback(std::move(getTypeOfItem));
    }
}

void ContainerModalPattern::SetToolbarBuilder(
    const RefPtr<FrameNode>& parent, std::function<RefPtr<UINode>()>&& builder)
{
    auto node = builder();
    CHECK_NULL_VOID(node);
    auto it = itemsWillOnTree_.find(parent);
    if (it != itemsWillOnTree_.end()) {
        it->second.clear();
    }
    it = itemsOnTree_.find(parent);
    if (it != itemsOnTree_.end()) {
        it->second.clear();
    }
    auto children = node->GetChildren();

    for (auto& item : children) {
        itemsWillOnTree_[parent].push_back(item);
        itemsOnTree_[parent].push_back(item);
    }
    auto callback = [weak = WeakClaim(this), frame = WeakClaim(RawPtr(parent))]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        auto frameNode = frame.Upgrade();
        CHECK_NULL_VOID(frameNode);
        pattern->RemoveToolbarItem(frameNode);
    };
    parent->SetDetachRelatedNodeCallback(std::move(callback));
    InitToolBarManager();

    auto pipeline = GetContext();
    CHECK_NULL_VOID(pipeline);
    pipeline->AddAfterRenderTask([weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->PrasePlaceMentType();
    });
}

void ContainerModalPattern::PrasePlaceMentType()
{
    bool hasItem = false;
    for (auto it = itemsWillOnTree_.begin(); it != itemsWillOnTree_.end();) {
        auto parent = it->first;
        auto& list = it->second;
        if (HandleToolbarItemList(parent, list)) {
            it = itemsWillOnTree_.erase(it);
            hasItem = true;
        } else {
            it++;
        }
    }
    if (hasItem) {
        AddToolbarItemToContainer();
    }
}

ItemPlacementType ContainerModalPattern::GetItemTypeFromTag(const std::string& tag, uint32_t placement)
{
    if (tag == V2::SIDE_BAR_ETS_TAG)
        return placement ? ItemPlacementType::SIDE_BAR_END : ItemPlacementType::SIDE_BAR_START;
    if (tag == V2::NAVBAR_ETS_TAG)
        return placement ? ItemPlacementType::NAV_BAR_END : ItemPlacementType::NAV_BAR_START;
    if (tag == V2::NAVDESTINATION_VIEW_ETS_TAG)
        return placement ? ItemPlacementType::NAVDEST_END : ItemPlacementType::NAVDEST_START;
    return ItemPlacementType::NONE;
}

bool ContainerModalPattern::HandleToolbarItemList(const RefPtr<FrameNode>& parentNode, std::list<RefPtr<UINode>>& list)
{
    CHECK_NULL_RETURN(parentNode, true);

    auto parent = AceType::DynamicCast<UINode>(parentNode);
    CHECK_NULL_RETURN(parent, true);
    std::string tag = "";
    while (parent && parent->GetTag() != V2::ROOT_ETS_TAG) {
        if (parent->GetTag() == V2::SIDE_BAR_ETS_TAG || parent->GetTag() == V2::NAVBAR_ETS_TAG ||
            parent->GetTag() == V2::NAVDESTINATION_VIEW_ETS_TAG) {
            tag = parent->GetTag();
            break;
        }
        parent = parent->GetParent();
    }
    if (tag == "") {
        return false;
    }

    for (auto& item : list) {
        auto frameNode = AceType::DynamicCast<FrameNode>(item);

        auto pattern = frameNode->GetPattern<ToolBarItemPattern>();
        int32_t placement = 0;
        if (pattern) {
            placement = pattern->GetPlacement();
            auto id = GetItemTypeFromTag(tag, placement);
            itemWillAdd_[id].emplace_back(frameNode);
        }
    }
    return true;
}

void ContainerModalPattern::RemoveToolbarItem(const RefPtr<FrameNode>& frameNode)
{
    auto& list = itemsOnTree_[frameNode];
    for (auto& item : list) {
        auto parent = item->GetParent();
        if (parent) {
            parent->RemoveChild(item);
            parent->MarkNeedSyncRenderTree();
            parent->RebuildRenderContextTree();
        }
    }
    list.clear();
    itemsOnTree_.erase(frameNode);
    if (navbarRow_) {
        navbarRow_->MarkDirtyNode(
            PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        if (leftNavRow_) {
            leftNavRow_->MarkDirtyNode(
                PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
        if (rightNavRow_) {
            rightNavRow_->MarkDirtyNode(
                PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
    }
    if (navDestbarRow_) {
        navDestbarRow_->MarkDirtyNode(
            PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        if (leftNavDestRow_) {
            leftNavDestRow_->MarkDirtyNode(
                PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
        if (rightNavDestRow_) {
            rightNavDestRow_->MarkDirtyNode(
                PROPERTY_UPDATE_LAYOUT | PROPERTY_UPDATE_RENDER | PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
    }
    toolbarItemMaxHeight_ = 0.0f;
    for (auto it = itemsOnTree_.begin(); it != itemsOnTree_.end(); it++) {
        auto& list = it->second;
        for (auto iit = list.begin(); iit != list.end(); iit++) {
            auto toolbarNode = AceType::DynamicCast<FrameNode>(*iit);
            LayoutConstraintF Constraint;
            toolbarNode->Measure(Constraint);
            auto toolbarItemHeight = Dimension(toolbarNode->GetGeometryNode()->GetFrameSize().Height()).ConvertToVp();
            if (toolbarItemHeight > toolbarItemMaxHeight_) {
                toolbarItemMaxHeight_ = toolbarItemHeight;
            }
        }
    }
    OnToolBarLayoutChange();
    AdjustContainerModalTitleHeight();
}

void ContainerModalPattern::AddToolbarItemToContainer()
{
    for (auto it = itemWillAdd_.begin(); it != itemWillAdd_.end(); it++) {
        auto placementType = it->first;
        if (placementType == ItemPlacementType::NONE) {
            continue;
        }
        auto& list = it->second;
        for (auto iit = list.begin(); iit != list.end();) {
            if (AddToolbarItemToRow(placementType, *iit)) {
                iit = list.erase(iit);
            } else {
                iit++;
            }
        }
    }
}

bool ContainerModalPattern::AddToolbarItemToRow(ItemPlacementType placeMent, const RefPtr<FrameNode>& frameNode)
{
    if ((!toolbarManager_->HasNavBar() &&
            (placeMent == ItemPlacementType::NAV_BAR_END || placeMent == ItemPlacementType::NAV_BAR_START)) ||
        (!toolbarManager_->HasNavDest() &&
            (placeMent == ItemPlacementType::NAVDEST_START || placeMent == ItemPlacementType::NAVDEST_END))) {
        return false;
    }
    CHECK_NULL_RETURN(frameNode, false);
    LayoutConstraintF Constraint;
    frameNode->Measure(Constraint);
    auto toolbarItemHeight = Dimension(frameNode->GetGeometryNode()->GetFrameSize().Height()).ConvertToVp();
    if (toolbarItemHeight > toolbarItemMaxHeight_) {
        toolbarItemMaxHeight_ = toolbarItemHeight;
    }
    auto toolbarItemPattern = frameNode->GetPattern<ToolBarItemPattern>();
    CHECK_NULL_RETURN(toolbarItemPattern, false);
    if (!navbarRow_ || !navDestbarRow_) {
        AddToolbarRowContainers();
    }
    return AddToolbarItemToSpecificRow(placeMent, frameNode);
}

bool ContainerModalPattern::AddToolbarItemToSpecificRow(ItemPlacementType placeMent, const RefPtr<FrameNode>& frameNode)
{
    bool ref = false;
    switch (placeMent) {
        case ItemPlacementType::NAV_BAR_START:
            ref = AddToolbarItemToNavBarStart(frameNode);
            break;
        case ItemPlacementType::NAV_BAR_END:
            ref = AddToolbarItemToNavBarEnd(frameNode);
            break;
        case ItemPlacementType::NAVDEST_START:
            ref = AddToolbarItemToNavDestStart(frameNode);
            break;
        case ItemPlacementType::NAVDEST_END:
            ref = AddToolbarItemToNavDestEnd(frameNode);
            break;
        default:
            TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "Unknown placement");
            return false;
    }
    auto overlayTask = [weak = WeakClaim(this)]() {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->AdjustContainerModalTitleHeight();
    };
    auto pipeline = GetContext();
    CHECK_NULL_RETURN(pipeline, true);
    pipeline->AddAfterRenderTask(overlayTask);
    return ref;
}

bool ContainerModalPattern::AddToolbarItemToNavBarStart(const RefPtr<FrameNode>& frameNode)
{
    if (navbarRow_) {
        if (!leftNavRow_) {
            AddToolbarRowContainers();
        }
        leftNavRow_->AddChild(frameNode);
        leftNavRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
        return true;
    }
    return false;
}

bool ContainerModalPattern::AddToolbarItemToNavBarEnd(const RefPtr<FrameNode>& frameNode)
{
    if (navbarRow_) {
        if (!rightNavRow_) {
            AddToolbarRowContainers();
        }
        rightNavRow_->AddChild(frameNode);
        rightNavRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
        return true;
    }
    return false;
}

bool ContainerModalPattern::AddToolbarItemToNavDestStart(const RefPtr<FrameNode>& frameNode)
{
    if (navDestbarRow_) {
        if (!leftNavDestRow_) {
            AddToolbarRowContainers();
        }
        leftNavDestRow_->AddChild(frameNode);
        leftNavDestRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
        return true;
    }
    return false;
}

bool ContainerModalPattern::AddToolbarItemToNavDestEnd(const RefPtr<FrameNode>& frameNode)
{
    if (navDestbarRow_) {
        if (!rightNavDestRow_) {
            AddToolbarRowContainers();
        }
        rightNavDestRow_->AddChild(frameNode);
        rightNavDestRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
        return true;
    }
    return false;
}

void ContainerModalPattern::AddToolbarRowContainers()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);

    auto customTitleRow = GetCustomTitleRow();
    CHECK_NULL_VOID(customTitleRow);

    auto sideBarInfo = toolbarManager_->GetSideBarInfo();
    auto navBarInfo = toolbarManager_->GetNavBarInfo();
    auto navDestInfo = toolbarManager_->GetNavDestInfo();

    AddSideBarDivider(customTitleRow, sideBarInfo);
    AddNavBarRow(customTitleRow, navBarInfo, sideBarInfo);
    AddNavDestBarRow(customTitleRow, navDestInfo);

    customTitleRow->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
}

void ContainerModalPattern::AddSideBarDivider(const RefPtr<FrameNode>& customTitleRow, const ToolbarInfo& sideBarInfo)
{
    if (sideBarInfo.isShow && !sideBarDivider_) {
        sideBarDivider_ = FrameNode::CreateFrameNode(
            V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(true));
        auto customTitleNode = GetCustomTitleNode();
        CHECK_NULL_VOID(customTitleNode);
        customTitleRow->AddChildAfter(sideBarDivider_, customTitleNode->GetParent());
        sideBarDivider_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    }
}

void ContainerModalPattern::AddNavBarRow(
    const RefPtr<FrameNode>& customTitleRow, const ToolbarInfo& navBarInfo, const ToolbarInfo& sideBarInfo)
{
    if (navBarInfo.isShow) {
        if (!navbarRow_) {
            navbarRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                MakeRefPtr<LinearLayoutPattern>(false));
            navbarRow_->UpdateInspectorId("NavBar");
            auto layout = navbarRow_->GetLayoutProperty<LinearLayoutProperty>();
            CHECK_NULL_VOID(layout);
            layout->UpdateMainAxisAlign(FlexAlign::SPACE_BETWEEN);
            navbarRow_->GetRenderContext()->SetClipToFrame(true);
            if (sideBarInfo.isShow) {
                customTitleRow->AddChildAfter(navbarRow_, sideBarDivider_);
            } else {
                auto customTitleNode = GetCustomTitleNode();
                CHECK_NULL_VOID(customTitleNode);
                customTitleRow->AddChildAfter(navbarRow_, customTitleNode->GetParent());
            }
            AddNavBarDivider(customTitleRow);
            navbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
        if (navbarRow_) {
            if (!leftNavRow_) {
                leftNavRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG,
                    ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(false));
                leftNavRow_->UpdateInspectorId("LeftNavRow");
                navbarRow_->AddChild(leftNavRow_);
                leftNavRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            }
            if (!rightNavRow_) {
                rightNavRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG,
                    ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(false));
                rightNavRow_->UpdateInspectorId("RightNavRow");
                navbarRow_->AddChild(rightNavRow_);
                auto layout = rightNavRow_->GetLayoutProperty<LinearLayoutProperty>();
                CHECK_NULL_VOID(layout);
                layout->UpdateMainAxisAlign(FlexAlign::FLEX_END);
                rightNavRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            }
            navbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }
        AdjustNavbarRowWidth();
    }
}

void ContainerModalPattern::AddNavBarDivider(const RefPtr<FrameNode>& customTitleRow)
{
    if (!navBarDivider_) {
        navBarDivider_ = FrameNode::CreateFrameNode(
            V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(false));
        navBarDivider_->UpdateInspectorId("NavBarDivider");
        customTitleRow->AddChildAfter(navBarDivider_, navbarRow_);
        navBarDivider_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    }
}

void ContainerModalPattern::AddNavDestBarRow(const RefPtr<FrameNode>& customTitleRow, const ToolbarInfo& navDestInfo)
{
    if (navDestInfo.isShow) {
        if (!navDestbarRow_) {
            navDestbarRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG, ElementRegister::GetInstance()->MakeUniqueId(),
                MakeRefPtr<LinearLayoutPattern>(false));
            navDestbarRow_->UpdateInspectorId("NavDestBar");
            auto layout = navDestbarRow_->GetLayoutProperty<LinearLayoutProperty>();
            CHECK_NULL_VOID(layout);
            layout->UpdateMainAxisAlign(FlexAlign::SPACE_BETWEEN);
            navDestbarRow_->GetRenderContext()->SetClipToFrame(true);
            customTitleRow->AddChild(navDestbarRow_);
            navDestbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
        }

        if (navDestbarRow_) {
            if (!leftNavDestRow_) {
                leftNavDestRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG,
                    ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(false));
                leftNavDestRow_->UpdateInspectorId("LeftNavDest");
                navDestbarRow_->AddChild(leftNavDestRow_);
                leftNavDestRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            }
            if (!rightNavDestRow_) {
                rightNavDestRow_ = FrameNode::CreateFrameNode(V2::ROW_ETS_TAG,
                    ElementRegister::GetInstance()->MakeUniqueId(), MakeRefPtr<LinearLayoutPattern>(false));
                rightNavDestRow_->UpdateInspectorId("RightNavDest");
                auto layout = rightNavDestRow_->GetLayoutProperty<LinearLayoutProperty>();
                CHECK_NULL_VOID(layout);
                layout->UpdateMainAxisAlign(FlexAlign::FLEX_END);
                navDestbarRow_->AddChild(rightNavDestRow_);
                rightNavDestRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            }
            navDestbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF);
            AdjustNavDestRowWidth();
        }
    }
}

void ContainerModalPattern::OnToolBarLayoutChange()
{
    if (!toolbarManager_) {
        return;
    }

    if (toolbarManager_->GetNavBarInfo().isShow) {
        AddToolbarItemToContainer();
        AdjustNavbarRowWidth();
    }
    if (toolbarManager_->GetNavDestInfo().isShow) {
        AddToolbarItemToContainer();
        AdjustNavDestRowWidth();
    }
}

void ContainerModalPattern::AdjustNavbarRowWidth()
{
    auto sideBarInfo = toolbarManager_->GetSideBarInfo();
    auto sideBarDividerInfo = toolbarManager_->GetSideBarDividerInfo();
    auto navbarInfo = toolbarManager_->GetNavBarInfo();
    auto navbarDividerInfo = toolbarManager_->GetNavBarDividerInfo();

    float titleNodeWidth = sideBarInfo.isShow ? sideBarInfo.width : 0;
    auto customTitleNode = GetCustomTitleNode();
    CHECK_NULL_VOID(customTitleNode);
    auto titleNode = AceType::DynamicCast<FrameNode>(customTitleNode->GetParent());
    CHECK_NULL_VOID(titleNode);

    auto titleNodeProperty = titleNode->GetLayoutProperty<LayoutProperty>();
    CHECK_NULL_VOID(titleNodeProperty);

    titleNodeProperty->UpdateMeasureType(MeasureType::MATCH_PARENT);
    titleNode->GetRenderContext()->SetClipToFrame(true);
    titleNodeProperty->UpdateUserDefinedIdealSize(
        CalcSize(CalcLength(titleNodeWidth), CalcLength(1.0, DimensionUnit::PERCENT)));
    titleNode->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
    titleNode->MarkDirtyNode(PROPERTY_UPDATE_RENDER);

    float sideBarDividerWidth = sideBarInfo.isShow ? sideBarDividerInfo.width : 0;
    if (sideBarDivider_) {
        auto sideBarDividerProperty = sideBarDivider_->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(sideBarDividerProperty);
        sideBarDividerProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(sideBarDividerWidth), CalcLength(1.0, DimensionUnit::PERCENT)));
        sideBarDivider_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    }
    if (navbarInfo.isShow && navbarRow_) {
        auto navbarRowProperty = navbarRow_->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(navbarRowProperty);
        navbarRowProperty->UpdateUserDefinedIdealSize(CalcSize(CalcLength(navbarInfo.width), std::nullopt));
        navbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);

        auto navBarDividerProperty = navBarDivider_->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(navBarDividerProperty);
        navBarDividerProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(navbarDividerInfo.width), CalcLength(1.0, DimensionUnit::PERCENT)));
        navBarDivider_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_PARENT);
    }
}

void ContainerModalPattern::AdjustNavDestRowWidth()
{
    auto pipelineContext = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipelineContext);
    auto navDestInfo = toolbarManager_->GetNavDestInfo();
    auto controlButtonsWidth = GetControlButtonRowWidth();

    if (navDestInfo.isShow && navDestbarRow_) {
        float navDestbarRowAvailableWidth = navDestInfo.width - controlButtonsWidth.GetDimension().ConvertToPx();
        auto navDestbarRowProperty = navDestbarRow_->GetLayoutProperty<LinearLayoutProperty>();
        CHECK_NULL_VOID(navDestbarRowProperty);
        navDestbarRowProperty->UpdateUserDefinedIdealSize(
            CalcSize(CalcLength(navDestbarRowAvailableWidth), std::nullopt));
        navDestbarRow_->MarkDirtyNode(PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
    }
}

void ContainerModalPattern::AdjustContainerModalTitleHeight()
{
    if (itemsOnTree_.empty()) {
        SetContainerModalTitleHeight(CONTAINER_TITLE_HEIGHT.ConvertToPx());
    }

    if (toolbarItemMaxHeight_ <= TITLE_ITEM_HEIGT_S) {
        titleHeight_ = Dimension(TITLE_ITEM_HEIGT_S, DimensionUnit::VP);
    } else if (toolbarItemMaxHeight_ > TITLE_ITEM_HEIGT_S && toolbarItemMaxHeight_ <= TITLE_ITEM_HEIGT_M) {
        titleHeight_ = Dimension(TITLE_ITEM_HEIGT_M, DimensionUnit::VP);
    } else if (toolbarItemMaxHeight_ > TITLE_ITEM_HEIGT_M) {
        titleHeight_ = Dimension(TITLE_ITEM_HEIGT_L, DimensionUnit::VP);
    }
    SetContainerModalTitleHeight(titleHeight_.ConvertToPx());
}
} // namespace OHOS::Ace::NG