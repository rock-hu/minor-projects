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

#include "core/components_ng/base/view_abstract_model_ng.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/common/ace_engine.h"
#include "core/common/vibrator/vibrator_utils.h"
#include "core/components_ng/base/view_abstract.h"
#include "core/components_ng/event/focus_hub.h"
#include "core/components_ng/pattern/container_modal/container_modal_pattern.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/pattern/navrouter/navdestination_pattern.h"
#include "core/components_ng/pattern/overlay/overlay_manager.h"
#include "core/components_ng/pattern/overlay/sheet_manager.h"
#include "core/components_ng/pattern/overlay/sheet_presentation_pattern.h"
#include "core/components_ng/pattern/overlay/sheet_style.h"
#include "core/components_ng/pattern/stage/page_pattern.h"
#ifdef WINDOW_SCENE_SUPPORTED
#include "core/components_ng/pattern/ui_extension/ui_extension_manager.h"
#include "core/components_ng/pattern/window_scene/scene/system_window_scene.h"
#endif
#include "core/common/resource/resource_object.h"
#include "core/common/resource/resource_parse_utils.h"
#include "core/components_ng/pattern/bubble/bubble_pattern.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t LONG_PRESS_DURATION = 800;
constexpr int32_t HOVER_IMAGE_LONG_PRESS_DURATION = 250;
constexpr char KEY_CONTEXT_MENU[] = "ContextMenu";
constexpr char KEY_MENU[] = "Menu";
} // namespace

void StartVirator(const MenuParam& menuParam, bool isMenu, const std::string& menuHapticFeedback)
{
    if (menuParam.hapticFeedbackMode == HapticFeedbackMode::ENABLED) {
        VibratorUtils::StartViratorDirectly(menuHapticFeedback);
        return;
    }
    if (menuParam.hapticFeedbackMode == HapticFeedbackMode::AUTO) {
        if (menuParam.maskEnable.has_value() && menuParam.maskEnable.value()) {
            VibratorUtils::StartViratorDirectly(menuHapticFeedback);
            return;
        }
    }
    if (isMenu) {
        return;
    }
    if (menuParam.hapticFeedbackMode == HapticFeedbackMode::AUTO && menuParam.previewMode != MenuPreviewMode::NONE) {
        VibratorUtils::StartViratorDirectly(menuHapticFeedback);
    }
}

void ViewAbstractModelNG::BindMenuGesture(
    std::vector<NG::OptionParam>&& params, std::function<void()>&& buildFunc, const MenuParam& menuParam)
{
    auto targetNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(targetNode);
    GestureEventFunc showMenu;
    auto weakTarget = AceType::WeakClaim(targetNode);
    if (!params.empty()) {
        showMenu = [params, weakTarget, menuParam](GestureEvent& info) mutable {
            auto targetNode = weakTarget.Upgrade();
            CHECK_NULL_VOID(targetNode);
            if (NG::ViewAbstractModelNG::CheckSkipMenuShow(targetNode)) {
                TAG_LOGI(AceLogTag::ACE_MENU, "skip menu show with params %{public}d", targetNode->GetId());
                return;
            }
            auto pipelineContext = targetNode->GetContext();
            CHECK_NULL_VOID(pipelineContext);
            auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
            CHECK_NULL_VOID(menuTheme);
            NG::OffsetF menuPosition { info.GetGlobalLocation().GetX() + menuParam.positionOffset.GetX(),
                info.GetGlobalLocation().GetY() + menuParam.positionOffset.GetY() };
            StartVirator(menuParam, true, menuTheme->GetMenuHapticFeedback());
            NG::ViewAbstract::BindMenuWithItems(std::move(params), targetNode, menuPosition, menuParam);
        };
    } else if (buildFunc) {
        showMenu = [builderFunc = std::move(buildFunc), weakTarget, menuParam](const GestureEvent& info) mutable {
            auto targetNode = weakTarget.Upgrade();
            CHECK_NULL_VOID(targetNode);
            if (NG::ViewAbstractModelNG::CheckSkipMenuShow(targetNode)) {
                TAG_LOGI(AceLogTag::ACE_MENU, "skip menu show with builder %{public}d", targetNode->GetId());
                return;
            }
            auto pipelineContext = targetNode->GetContext();
            CHECK_NULL_VOID(pipelineContext);
            auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
            CHECK_NULL_VOID(menuTheme);
            NG::OffsetF menuPosition { info.GetGlobalLocation().GetX() + menuParam.positionOffset.GetX(),
                info.GetGlobalLocation().GetY() + menuParam.positionOffset.GetY() };
            StartVirator(menuParam, true, menuTheme->GetMenuHapticFeedback());
            std::function<void()> previewBuildFunc;
            NG::ViewAbstract::BindMenuWithCustomNode(
                std::move(builderFunc), targetNode, menuPosition, menuParam, std::move(previewBuildFunc));
        };
    } else {
        return;
    }
    auto gestureHub = targetNode->GetOrCreateGestureEventHub();
    gestureHub->BindMenu(std::move(showMenu));
    BindMenuTouch(targetNode, gestureHub);
}

void ViewAbstractModelNG::BindMenuTouch(FrameNode* targetNode, const RefPtr<GestureEventHub>& gestrueHub)
{
    CHECK_NULL_VOID(targetNode);
    auto weakTarget = AceType::WeakClaim(targetNode);
    auto touchCallback = [weakTarget](const TouchEventInfo& info) {
        auto targetNode = weakTarget.Upgrade();
        CHECK_NULL_VOID(targetNode);
        const auto& touches = info.GetTouches();
        CHECK_EQUAL_VOID(touches.empty(), true);
        auto touchType = touches.front().GetTouchType();
        auto targetId = targetNode->GetId();
        auto subwindow =
            SubwindowManager::GetInstance()->GetSubwindowByType(Container::CurrentId(), SubwindowType::TYPE_MENU);
        CHECK_NULL_VOID(subwindow);
        auto childContainerId = subwindow->GetChildContainerId();
        auto childContainer = AceEngine::Get().GetContainer(childContainerId);
        CHECK_NULL_VOID(childContainer);
        auto subwindowPipline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
        CHECK_NULL_VOID(subwindowPipline);
        auto subwindowOverlay = subwindowPipline->GetOverlayManager();
        CHECK_NULL_VOID(subwindowOverlay);
        if (touchType == TouchType::DOWN) {
            auto wrapperNode = subwindowOverlay->GetMenuNode(targetId);
            CHECK_NULL_VOID(wrapperNode);
            auto wrapperPattern = wrapperNode->GetPattern<MenuWrapperPattern>();
            CHECK_NULL_VOID(wrapperPattern);
            if (wrapperPattern->IsShow() || wrapperPattern->GetMenuStatus() == MenuStatus::ON_HIDE_ANIMATION) {
                TAG_LOGI(AceLogTag::ACE_MENU, "skip menu show, target %{public}d", targetId);
                subwindowOverlay->SkipMenuShow(targetId);
            }
        } else if ((touchType == TouchType::UP || touchType == TouchType::CANCEL) &&
                   subwindowOverlay->CheckSkipMenuShow(targetId)) {
            auto weakOverlay = AceType::WeakClaim(AceType::RawPtr(subwindowOverlay));
            auto taskExecutor = Container::CurrentTaskExecutor();
            CHECK_NULL_VOID(taskExecutor);
            TAG_LOGI(AceLogTag::ACE_MENU, "post resume menu show task, target %{public}d", targetId);
            taskExecutor->PostTask(
                [weakOverlay, targetId]() mutable {
                    TAG_LOGI(AceLogTag::ACE_MENU, "execute resume menu show, target %{public}d", targetId);
                    auto subwindowOverlay = weakOverlay.Upgrade();
                    CHECK_NULL_VOID(subwindowOverlay);
                    subwindowOverlay->ResumeMenuShow(targetId);
                }, TaskExecutor::TaskType::PLATFORM, "ArkUIResumeMenuShow");
        }
    };
    gestrueHub->RegisterMenuOnTouch(touchCallback);
}

bool ViewAbstractModelNG::CheckSkipMenuShow(const RefPtr<FrameNode>& targetNode)
{
    CHECK_NULL_RETURN(targetNode, false);
    auto subwindow =
        SubwindowManager::GetInstance()->GetSubwindowByType(Container::CurrentId(), SubwindowType::TYPE_MENU);
    CHECK_NULL_RETURN(subwindow, false);
    auto childContainerId = subwindow->GetChildContainerId();
    auto childContainer = AceEngine::Get().GetContainer(childContainerId);
    CHECK_NULL_RETURN(childContainer, false);
    auto subwindowPipline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
    CHECK_NULL_RETURN(subwindowPipline, false);
    auto subwindowOverlay = subwindowPipline->GetOverlayManager();
    CHECK_NULL_RETURN(subwindowOverlay, false);
    return subwindowOverlay->CheckSkipMenuShow(targetNode->GetId());
}

bool ViewAbstractModelNG::CheckMenuIsShow(
    const MenuParam& menuParam, int32_t targetId, const RefPtr<FrameNode>& targetNode)
{
    RefPtr<NG::PipelineContext> pipeline = nullptr;
    if (menuParam.isShowInSubWindow) {
        auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(
            Container::CurrentId(), SubwindowType::TYPE_MENU);
        CHECK_NULL_RETURN(subwindow, false);
        auto childContainerId = subwindow->GetChildContainerId();
        auto childContainer = AceEngine::Get().GetContainer(childContainerId);
        CHECK_NULL_RETURN(childContainer, false);
        pipeline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
    } else {
        CHECK_NULL_RETURN(targetNode, false);
        pipeline = targetNode->GetContextRefPtr();
    }
    CHECK_NULL_RETURN(pipeline, false);
    auto overlayManager = pipeline->GetOverlayManager();
    CHECK_NULL_RETURN(overlayManager, false);
    auto menuNode = overlayManager->GetMenuNode(targetId);
    CHECK_NULL_RETURN(menuNode, false);
    auto wrapperPattern = menuNode->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_RETURN(wrapperPattern, false);
    if (menuParam.hasTransitionEffect) {
        auto renderContext = menuNode->GetRenderContext();
        CHECK_NULL_RETURN(renderContext, false);
        renderContext->UpdateChainedTransition(menuParam.transition);
    }
    if (wrapperPattern->IsShow() && menuParam.setShow && !menuParam.isShow && !wrapperPattern->GetIsOpenMenu()) {
        TAG_LOGI(AceLogTag::ACE_MENU, "execute hide menu.");
        overlayManager->HideMenu(menuNode, targetId, false, HideMenuType::IS_SHOW);
    }
    return true;
}

void ViewAbstractModelNG::BindMenu(
    std::vector<NG::OptionParam>&& params, std::function<void()>&& buildFunc, const MenuParam& menuParam)
{
    auto targetNode = AceType::Claim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(targetNode);
    auto targetId = targetNode->GetId();
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, IsBindOverlay, true);
    if (CheckMenuIsShow(menuParam, targetId, targetNode)) {
        TAG_LOGI(AceLogTag::ACE_MENU, "hide menu done %{public}d %{public}d.", menuParam.isShowInSubWindow, targetId);
    } else if (menuParam.isShow) {
        auto pipelineContext = targetNode->GetContext();
        CHECK_NULL_VOID(pipelineContext);
        auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
        CHECK_NULL_VOID(menuTheme);
        StartVirator(menuParam, true, menuTheme->GetMenuHapticFeedback());
        if (!params.empty()) {
            NG::ViewAbstract::BindMenuWithItems(std::move(params), targetNode, menuParam.positionOffset, menuParam);
        } else if (buildFunc) {
            std::function<void()> previewBuildFunc;
            NG::ViewAbstract::BindMenuWithCustomNode(
                std::move(buildFunc), targetNode, menuParam.positionOffset, menuParam, std::move(previewBuildFunc));
        }
    }
    if (!menuParam.setShow) {
        BindMenuGesture(std::move(params), std::move(buildFunc), menuParam);
    }
    // delete menu when target node destroy
    auto pipeline = PipelineBase::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);
    auto expandDisplay = theme->GetExpandDisplay();
    if (!menuParam.isShowInSubWindow && expandDisplay) {
        expandDisplay = false;
    }
    if (!expandDisplay) {
        auto destructor = [id = targetNode->GetId(), params]() mutable {
            params.clear();
            auto pipeline = NG::PipelineContext::GetCurrentContext();
            CHECK_NULL_VOID(pipeline);
            auto overlayManager = pipeline->GetOverlayManager();
            CHECK_NULL_VOID(overlayManager);
            overlayManager->DeleteMenu(id);
        };
        targetNode->PushDestroyCallbackWithTag(destructor, KEY_MENU);
    } else {
        auto destructor = [id = targetNode->GetId(), containerId = Container::CurrentId(), params]() mutable {
            params.clear();
            auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(containerId, SubwindowType::TYPE_MENU);
            CHECK_NULL_VOID(subwindow);
            auto childContainerId = subwindow->GetChildContainerId();
            auto childContainer = AceEngine::Get().GetContainer(childContainerId);
            CHECK_NULL_VOID(childContainer);
            auto pipeline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
            CHECK_NULL_VOID(pipeline);
            auto overlayManager = pipeline->GetOverlayManager();
            CHECK_NULL_VOID(overlayManager);
            overlayManager->DeleteMenu(id);
        };
        targetNode->PushDestroyCallbackWithTag(destructor, KEY_MENU);
    }
}

void CreateCustomMenuWithPreview(
    std::function<void()>& buildFunc, const MenuParam& menuParam, std::function<void()>& previewBuildFunc)
{
    auto targetNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(targetNode);
    if (menuParam.previewMode == MenuPreviewMode::IMAGE) {
        auto context = targetNode->GetRenderContext();
        CHECK_NULL_VOID(context);
        auto gestureHub = targetNode->GetOrCreateEventHub<EventHub>()->GetOrCreateGestureEventHub();
        CHECK_NULL_VOID(gestureHub);
        auto pixelMap = context->GetThumbnailPixelMap();
        gestureHub->SetPixelMap(pixelMap);
    }
    auto refTargetNode = AceType::Claim<NG::FrameNode>(targetNode);
    auto pipelineContext = targetNode->GetContext();
    CHECK_NULL_VOID(pipelineContext);
    auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
    CHECK_NULL_VOID(menuTheme);
    StartVirator(menuParam, false, menuTheme->GetMenuHapticFeedback());
    NG::ViewAbstract::BindMenuWithCustomNode(
        std::move(buildFunc), refTargetNode, menuParam.positionOffset, menuParam, std::move(previewBuildFunc));
}

void UpdateIsShowStatusForMenu(int32_t targetId, bool isShow)
{
    auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(
        Container::CurrentId(), SubwindowType::TYPE_MENU);
    CHECK_NULL_VOID(subwindow);
    auto overlayManager = subwindow->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    auto menuNode = overlayManager->GetMenuNode(targetId);
    CHECK_NULL_VOID(menuNode);
    auto wrapperPattern = menuNode->GetPattern<MenuWrapperPattern>();
    CHECK_NULL_VOID(wrapperPattern);
    wrapperPattern->SetIsShowFromUser(isShow);
}

void BindContextMenuSingle(
    std::function<void()>& buildFunc, const MenuParam& menuParam, std::function<void()>& previewBuildFunc)
{
    auto targetNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(targetNode);
    ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, IsBindOverlay, true);
    auto targetId = targetNode->GetId();
    TAG_LOGD(AceLogTag::ACE_OVERLAY, "target %{public}d menu isShow %{public}d", targetId, menuParam.isShow);
    auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(
        Container::CurrentId(), SubwindowType::TYPE_MENU);
    if (subwindow) {
        auto childContainerId = subwindow->GetChildContainerId();
        auto childContainer = AceEngine::Get().GetContainer(childContainerId);
        CHECK_NULL_VOID(childContainer);
        auto pipeline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        auto menuNode = overlayManager->GetMenuNode(targetId);
        if (menuNode) {
            TAG_LOGI(AceLogTag::ACE_OVERLAY, "menuNode already exist");
            auto wrapperPattern = menuNode->GetPattern<MenuWrapperPattern>();
            CHECK_NULL_VOID(wrapperPattern);
            // If menu is shown or in show animation, set isShow to false will close menu. If menu is not shown or
            // in close animation, wrapperPattern->IsShow() is false, set isShow to false will not trigger close again.
            if (wrapperPattern->IsShow() && !menuParam.isShow) {
                TAG_LOGI(AceLogTag::ACE_MENU, "will hide menu, tagetNode id %{public}d.", targetId);
                SubwindowManager::GetInstance()->HideMenuNG(menuNode, targetId);
                UpdateIsShowStatusForMenu(targetId, false);
            } else if (!wrapperPattern->IsShow() && menuParam.isShow &&
                       wrapperPattern->GetIsShowFromUser() != menuParam.isShow) {
                // If click outside to close menu during show animation, and isShow is always true without changing,
                // then show new menu will result in an incorrect isShow state because onDisappear not be triggered.
                // The menu only show if isShow is manually set from false to true.
                CreateCustomMenuWithPreview(buildFunc, menuParam, previewBuildFunc);
                UpdateIsShowStatusForMenu(targetId, true);
            }
        } else if (menuParam.isShow && buildFunc) {
            CreateCustomMenuWithPreview(buildFunc, menuParam, previewBuildFunc);
            UpdateIsShowStatusForMenu(targetId, true);
        }
    } else {
        // first response for build subwindow and menu
        if (menuParam.isShow && buildFunc) {
            CreateCustomMenuWithPreview(buildFunc, menuParam, previewBuildFunc);
            UpdateIsShowStatusForMenu(targetId, true);
        }
    }
}

void ViewAbstractModelNG::BindContextMenu(ResponseType type, std::function<void()>& buildFunc,
    const MenuParam& menuParam, std::function<void()>& previewBuildFunc)
{
    auto targetNode = AceType::Claim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    BindContextMenu(targetNode, type, buildFunc, menuParam, previewBuildFunc);
}

void ViewAbstractModelNG::BindContextMenu(const RefPtr<FrameNode>& targetNode, ResponseType type,
    std::function<void()>& buildFunc, const NG::MenuParam& menuParam, std::function<void()>& previewBuildFunc)
{
    CHECK_NULL_VOID(targetNode);
    auto targetId = targetNode->GetId();
    auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(
        Container::CurrentId(), SubwindowType::TYPE_MENU);
    if (subwindow) {
        auto childContainerId = subwindow->GetChildContainerId();
        auto childContainer = AceEngine::Get().GetContainer(childContainerId);
        CHECK_NULL_VOID(childContainer);
        auto pipeline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        auto menuNode = overlayManager->GetMenuNode(targetId);
        if (menuNode) {
            TAG_LOGI(AceLogTag::ACE_OVERLAY, "menuNode already exist");
            auto menuWrapperPattern = menuNode->GetPattern<NG::MenuWrapperPattern>();
            CHECK_NULL_VOID(menuWrapperPattern);
            menuWrapperPattern->SetMenuTransitionEffect(menuNode, menuParam);
            menuWrapperPattern->RegisterMenuStateChangeCallback(menuParam.onStateChange);
        }
    }
    if (menuParam.contextMenuRegisterType == ContextMenuRegisterType::CUSTOM_TYPE) {
        BindContextMenuSingle(buildFunc, menuParam, previewBuildFunc);
    } else {
        auto hub = targetNode->GetOrCreateGestureEventHub();
        CHECK_NULL_VOID(hub);
        auto weakTarget = AceType::WeakClaim(AceType::RawPtr(targetNode));
        if (type == ResponseType::RIGHT_CLICK) {
            OnMouseEventFunc event = [builderF = buildFunc, weakTarget, menuParam](MouseInfo& info) mutable {
                auto taskExecutor = Container::CurrentTaskExecutor();
                CHECK_NULL_VOID(taskExecutor);
                if (info.GetButton() == MouseButton::RIGHT_BUTTON && info.GetAction() == MouseAction::RELEASE) {
                    TAG_LOGI(AceLogTag::ACE_MENU, "Post rightClick task for menu");
                    info.SetStopPropagation(true);
                    taskExecutor->PostTask(
                        [builder = builderF, weakTarget, menuParam, info]() mutable {
                            auto targetNode = weakTarget.Upgrade();
                            CHECK_NULL_VOID(targetNode);
                            NG::OffsetF menuPosition { info.GetGlobalLocation().GetX() +
                                                           menuParam.positionOffset.GetX(),
                                info.GetGlobalLocation().GetY() + menuParam.positionOffset.GetY() };
                            std::function<void()> previewBuildFunc;
                            TAG_LOGI(AceLogTag::ACE_MENU, "Execute rightClick task for menu");
                            auto pipelineContext = targetNode->GetContext();
                            CHECK_NULL_VOID(pipelineContext);
                            auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
                            CHECK_NULL_VOID(menuTheme);
                            StartVirator(menuParam, false, menuTheme->GetMenuHapticFeedback());
                            NG::ViewAbstract::BindMenuWithCustomNode(
                                std::move(builder), targetNode, menuPosition, menuParam, std::move(previewBuildFunc));
                        },
                        TaskExecutor::TaskType::PLATFORM, "ArkUIRightClickCreateCustomMenu");
                }
            };
            auto inputHub = targetNode->GetOrCreateInputEventHub();
            CHECK_NULL_VOID(inputHub);
            inputHub->BindContextMenu(std::move(event));
        } else if (type == ResponseType::LONG_PRESS) {
            auto gestureHub = targetNode->GetOrCreateEventHub<EventHub>()->GetOrCreateGestureEventHub();
            CHECK_NULL_VOID(gestureHub);
            gestureHub->SetPreviewMode(menuParam.previewMode);
            // create or show menu on long press
            auto event =
                [builderF = buildFunc, weakTarget, menuParam, previewBuildFunc](const GestureEvent& info) mutable {
                TAG_LOGI(AceLogTag::ACE_MENU, "Trigger longPress event for menu");
                auto taskExecutor = Container::CurrentTaskExecutor();
                CHECK_NULL_VOID(taskExecutor);
                taskExecutor->PostTask(
                    [builder = builderF, weakTarget, menuParam, previewBuildFunc, info]() mutable {
                        TAG_LOGI(AceLogTag::ACE_MENU, "Execute longPress task for menu");
                        auto targetNode = weakTarget.Upgrade();
                        CHECK_NULL_VOID(targetNode);
                        auto pipelineContext = targetNode->GetContext();
                        CHECK_NULL_VOID(pipelineContext);
                        auto menuTheme = pipelineContext->GetTheme<NG::MenuTheme>();
                        CHECK_NULL_VOID(menuTheme);
                        if (pipelineContext->IsDragging()) {
                            TAG_LOGI(AceLogTag::ACE_MENU, "TargetNode is dragging, menu is no longer show");
                            return;
                        }
                        if (menuParam.previewMode == MenuPreviewMode::IMAGE || menuParam.isShowHoverImage) {
                            auto context = targetNode->GetRenderContext();
                            CHECK_NULL_VOID(context);
                            auto gestureHub = targetNode->GetOrCreateEventHub<EventHub>()->GetOrCreateGestureEventHub();
                            CHECK_NULL_VOID(gestureHub);
                            auto pixelMap = context->GetThumbnailPixelMap();
                            gestureHub->SetPixelMap(pixelMap);
                        }
                        NG::OffsetF menuPosition { info.GetGlobalLocation().GetX() + menuParam.positionOffset.GetX(),
                            info.GetGlobalLocation().GetY() + menuParam.positionOffset.GetY() };
                        StartVirator(menuParam, false, menuTheme->GetMenuHapticFeedback());
                        NG::ViewAbstract::BindMenuWithCustomNode(
                            std::move(builder), targetNode, menuPosition, menuParam, std::move(previewBuildFunc));
                    },
                    TaskExecutor::TaskType::PLATFORM, "ArkUILongPressCreateCustomMenu");
            };
            auto longPress = AceType::MakeRefPtr<NG::LongPressEvent>(std::move(event));
            ACE_UPDATE_LAYOUT_PROPERTY(LayoutProperty, IsBindOverlay, true);
            auto longPressDuration = menuParam.isShowHoverImage ? HOVER_IMAGE_LONG_PRESS_DURATION : LONG_PRESS_DURATION;
            hub->SetLongPressEvent(longPress, false, true, longPressDuration);
        } else {
            return;
        }
        RegisterContextMenuKeyEvent(targetNode, buildFunc, menuParam);
    }

    // delete menu when target node destroy
    auto destructor = [id = targetNode->GetId(), containerId = Container::CurrentId()]() {
        auto subwindow = SubwindowManager::GetInstance()->GetSubwindowByType(containerId, SubwindowType::TYPE_MENU);
        CHECK_NULL_VOID(subwindow);
        auto childContainerId = subwindow->GetChildContainerId();
        auto childContainer = AceEngine::Get().GetContainer(childContainerId);
        CHECK_NULL_VOID(childContainer);
        auto pipeline = AceType::DynamicCast<NG::PipelineContext>(childContainer->GetPipelineContext());
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        overlayManager->DeleteMenu(id);
    };
    targetNode->PushDestroyCallbackWithTag(destructor, KEY_CONTEXT_MENU);
}

void ViewAbstractModelNG::BindDragWithContextMenuParams(const NG::MenuParam& menuParam)
{
    auto targetNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    BindDragWithContextMenuParams(targetNode, menuParam);
}

void ViewAbstractModelNG::BindDragWithContextMenuParams(FrameNode* targetNode, const NG::MenuParam& menuParam)
{
    CHECK_NULL_VOID(targetNode);

    auto gestureHub = targetNode->GetOrCreateGestureEventHub();
    if (gestureHub) {
        if (menuParam.contextMenuRegisterType == ContextMenuRegisterType::CUSTOM_TYPE) {
            gestureHub->SetBindMenuStatus(true, menuParam.isShow, menuParam.previewMode);
        } else if (menuParam.menuBindType == MenuBindingType::LONG_PRESS) {
            gestureHub->SetBindMenuStatus(false, false, menuParam.previewMode);
        }
        gestureHub->SetPreviewMode(menuParam.previewMode);
        gestureHub->SetContextMenuShowStatus(menuParam.isShow);
        gestureHub->SetMenuBindingType(menuParam.menuBindType);
        // set menu preview scale to drag.
        if (menuParam.menuBindType != MenuBindingType::RIGHT_CLICK) {
            auto menuPreviewScale = LessOrEqual(menuParam.previewAnimationOptions.scaleTo, 0.0)
                                        ? DEFALUT_DRAG_PPIXELMAP_SCALE
                                        : menuParam.previewAnimationOptions.scaleTo;
            gestureHub->SetMenuPreviewScale(menuPreviewScale);
        }
    } else {
        TAG_LOGW(AceLogTag::ACE_DRAG, "Can not get gestureEventHub!");
    }
}

void ViewAbstractModelNG::SetToolbarBuilder(std::function<void()>&& buildFunc)
{
    auto pipelineContext = NG::PipelineContext::GetMainPipelineContext();
    CHECK_NULL_VOID(pipelineContext);
    auto rootNode = pipelineContext->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto rootNodeChild = rootNode->GetChildren();
    CHECK_NULL_VOID(!rootNodeChild.empty());
    auto containerMode = AceType::DynamicCast<NG::FrameNode>(rootNodeChild.front());
    CHECK_NULL_VOID(containerMode);
    auto pattern = containerMode->GetPattern<NG::ContainerModalPattern>();
    CHECK_NULL_VOID(pattern);
    auto frameNode = AceType::WeakClaim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    auto parent = frameNode.Upgrade();
    CHECK_NULL_VOID(parent);
    if (buildFunc == nullptr) {
        pattern->SetToolbarBuilder(parent, nullptr);
        return;
    }
    auto buildNodeFunc = [func = std::move(buildFunc)]() -> RefPtr<UINode> {
        NG::ScopedViewStackProcessor builderViewStackProcessor;
        func();
        auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        return customNode;
    };
    pattern->SetToolbarBuilder(parent, std::move(buildNodeFunc));
}

void ViewAbstractModelNG::BindBackground(std::function<void()>&& buildFunc, const Alignment& align)
{
    SetBackground(std::move(buildFunc));
    NG::ViewAbstract::SetBackgroundAlign(align);
}

void ViewAbstractModelNG::SetBackground(std::function<void()>&& buildFunc)
{
    std::function<RefPtr<UINode>()> buildNodeFunc;
    if (buildFunc) {
        buildNodeFunc = [buildFunc = std::move(buildFunc)]() -> RefPtr<UINode> {
            NG::ScopedViewStackProcessor builderViewStackProcessor;
            buildFunc();
            auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
            return customNode;
        };
    }

    auto targetNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(targetNode);
    targetNode->SetBackgroundFunction(std::move(buildNodeFunc));
}

void ViewAbstractModelNG::SetCustomBackgroundColor(const Color& color)
{
    NG::ViewAbstract::SetCustomBackgroundColor(color);
}

void ViewAbstractModelNG::SetBackgroundIgnoresLayoutSafeAreaEdges(const uint32_t edges)
{
    NG::ViewAbstract::SetBackgroundIgnoresLayoutSafeAreaEdges(edges);
}

void ViewAbstractModelNG::SetPivot(const Dimension& x, const Dimension& y, const Dimension& z)
{
    DimensionOffset center(x, y);
    if (!NearZero(z.Value())) {
        center.SetZ(z);
    }
    ViewAbstract::SetPivot(center);
}

void ViewAbstractModelNG::SetScale(float x, float y, float z)
{
    VectorF scale(x, y);
    ViewAbstract::SetScale(scale);
}

void ViewAbstractModelNG::BindContentCover(bool isShow, std::function<void(const std::string&)>&& callback,
    std::function<void()>&& buildFunc, NG::ModalStyle& modalStyle, std::function<void()>&& onAppear,
    std::function<void()>&& onDisappear, std::function<void()>&& onWillAppear, std::function<void()>&& onWillDisappear,
    const NG::ContentCoverParam& contentCoverParam)
{
    auto targetNode = AceType::Claim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(targetNode);
    auto buildNodeFunc = [buildFunc]() -> RefPtr<UINode> {
        NG::ScopedViewStackProcessor builderViewStackProcessor;
        buildFunc();
        auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        return customNode;
    };
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);

    // delete full screen modal when target node destroy
    auto destructor = [id = targetNode->GetId()]() {
        auto pipeline = NG::PipelineContext::GetCurrentContext();
        CHECK_NULL_VOID(pipeline);
        auto overlayManager = pipeline->GetOverlayManager();
        CHECK_NULL_VOID(overlayManager);
        overlayManager->DeleteModal(id);
    };
    targetNode->PushDestroyCallbackWithTag(destructor, V2::MODAL_PAGE_TAG);

    overlayManager->BindContentCover(isShow, std::move(callback), std::move(buildNodeFunc), modalStyle,
        std::move(onAppear), std::move(onDisappear), std::move(onWillAppear), std::move(onWillDisappear),
        contentCoverParam, targetNode);
}

void ViewAbstractModelNG::RegisterContextMenuKeyEvent(
    const RefPtr<FrameNode>& targetNode, std::function<void()>& buildFunc, const MenuParam& menuParam)
{
    auto focusHub = targetNode->GetOrCreateFocusHub();
    CHECK_NULL_VOID(focusHub);
    auto onKeyEvent = [wp = AceType::WeakClaim(AceType::RawPtr(targetNode)), builder = buildFunc, param = menuParam](
                          const KeyEvent& event) mutable -> bool {
        if (event.action != KeyAction::DOWN) {
            return false;
        }
        if (event.code == KeyCode::KEY_MENU || event.keyIntention == KeyIntention::INTENTION_MENU) {
            auto targetNode = wp.Upgrade();
            CHECK_NULL_RETURN(targetNode, false);
            if (!param.placement.has_value()) {
                param.placement = Placement::BOTTOM_LEFT;
            }
            std::function<void()> previewBuildFunc = nullptr;
            NG::ViewAbstract::BindMenuWithCustomNode(
                std::move(builder), targetNode, OffsetF(), param, std::move(previewBuildFunc));
            return true;
        }
        return false;
    };
    focusHub->SetOnKeyEventInternal(std::move(onKeyEvent), OnKeyEventType::CONTEXT_MENU);
}

RefPtr<PipelineContext> ViewAbstractModelNG::GetSheetContext(NG::SheetStyle& sheetStyle)
{
    RefPtr<PipelineContext> context;
    if (sheetStyle.instanceId.has_value()) {
        TAG_LOGD(AceLogTag::ACE_SHEET, "Sheet uiContext id is: %{public}d", sheetStyle.instanceId.value());
        auto container = AceEngine::Get().GetContainer(sheetStyle.instanceId.value());
        CHECK_NULL_RETURN(container, nullptr);
        auto contextBase = container->GetPipelineContext();
        CHECK_NULL_RETURN(contextBase, nullptr);
        context = AceType::DynamicCast<PipelineContext>(contextBase);
    } else {
        context = PipelineContext::GetCurrentContext();
    }
    return context;
}

void ViewAbstractModelNG::BindSheet(bool isShow, std::function<void(const std::string&)>&& callback,
    std::function<void()>&& buildFunc, std::function<void()>&& titleBuildFunc, NG::SheetStyle& sheetStyle,
    std::function<void()>&& onAppear, std::function<void()>&& onDisappear, std::function<void()>&& shouldDismiss,
    std::function<void(const int32_t info)>&& onWillDismiss, std::function<void()>&& onWillAppear,
    std::function<void()>&& onWillDisappear, std::function<void(const float)>&& onHeightDidChange,
    std::function<void(const float)>&& onDetentsDidChange, std::function<void(const float)>&& onWidthDidChange,
    std::function<void(const float)>&& onTypeDidChange, std::function<void()>&& sheetSpringBack)
{
    auto targetNode = AceType::Claim(NG::ViewStackProcessor::GetInstance()->GetMainFrameNode());
    CHECK_NULL_VOID(targetNode);
    auto instanceId = sheetStyle.instanceId.has_value() && !sheetStyle.showInSubWindow.value_or(false) ?
        sheetStyle.instanceId.value() : Container::CurrentId();
    auto buildNodeFunc = [buildFunc, instanceId]() -> RefPtr<UINode> {
        NG::ScopedViewStackProcessor builderViewStackProcess(instanceId);
        buildFunc();
        auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        return customNode;
    };
    auto buildTitleNodeFunc = [titleBuildFunc, instanceId]() -> RefPtr<UINode> {
        CHECK_NULL_RETURN(titleBuildFunc, nullptr);
        NG::ScopedViewStackProcessor builderViewStackProcess(instanceId);
        titleBuildFunc();
        auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        return customNode;
    };
    auto context = GetSheetContext(sheetStyle);
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    if (sheetStyle.showInPage.value_or(false)) {
        overlayManager = SheetManager::FindPageNodeOverlay(targetNode, isShow);
    }
    CHECK_NULL_VOID(overlayManager);

    // delete Sheet when target node destroy
    SheetManager::GetInstance().RegisterDestroyCallback(targetNode, sheetStyle, instanceId);

    if (sheetStyle.showInSubWindow.value_or(false)) {
        if (isShow) {
            SubwindowManager::GetInstance()->ShowBindSheetNG(isShow, std::move(callback), std::move(buildNodeFunc),
                std::move(buildTitleNodeFunc), sheetStyle, std::move(onAppear), std::move(onDisappear),
                std::move(shouldDismiss), std::move(onWillDismiss),
                std::move(onWillAppear), std::move(onWillDisappear), std::move(onHeightDidChange),
                std::move(onDetentsDidChange), std::move(onWidthDidChange), std::move(onTypeDidChange),
                std::move(sheetSpringBack), targetNode);
        } else {
            SheetManager::GetInstance().CloseSheetInSubWindow(SheetKey(targetNode->GetId()));
        }
        return;
    }

    overlayManager->BindSheet(isShow, std::move(callback), std::move(buildNodeFunc), std::move(buildTitleNodeFunc),
        sheetStyle, std::move(onAppear), std::move(onDisappear), std::move(shouldDismiss), std::move(onWillDismiss),
        std::move(onWillAppear), std::move(onWillDisappear), std::move(onHeightDidChange),
        std::move(onDetentsDidChange), std::move(onWidthDidChange), std::move(onTypeDidChange),
        std::move(sheetSpringBack), targetNode);
}

void ViewAbstractModelNG::DismissSheet()
{
    auto sheetId = SheetManager::GetInstance().GetDismissSheet();
    auto sheet = FrameNode::GetFrameNode(V2::SHEET_PAGE_TAG, sheetId);
    CHECK_NULL_VOID(sheet);
    auto sheetPattern = sheet->GetPattern<SheetPresentationPattern>();
    CHECK_NULL_VOID(sheetPattern);
    sheetPattern->OverlayDismissSheet();
}

void ViewAbstractModelNG::DismissContentCover()
{
    auto context = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    overlayManager->DismissContentCover();
}

void ViewAbstractModelNG::SheetSpringBack()
{
    auto sheetId = SheetManager::GetInstance().GetDismissSheet();
    auto sheet = FrameNode::GetFrameNode(V2::SHEET_PAGE_TAG, sheetId);
    CHECK_NULL_VOID(sheet);
    auto sheetPattern = sheet->GetPattern<SheetPresentationPattern>();
    CHECK_NULL_VOID(sheetPattern);
    sheetPattern->OverlaySheetSpringBack();
}

void ViewAbstractModelNG::SetAccessibilityGroup(bool accessible)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityGroup(accessible);
}

void ViewAbstractModelNG::SetAccessibilityTextPreferred(bool accessibilityTextPreferred)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTextPreferred(accessibilityTextPreferred);
}

void ViewAbstractModelNG::SetAccessibilityText(const std::string& text)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTextWithEvent(text);
}

void ViewAbstractModelNG::SetAccessibilityTextHint(const std::string& text)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTextHint(text);
}

void ViewAbstractModelNG::SetAccessibilityDescription(const std::string& description)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityDescriptionWithEvent(description);
}

void ViewAbstractModelNG::SetAccessibilityNextFocusId(const std::string& nextFocusId)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityNextFocusInspectorKey(nextFocusId);
}

void ViewAbstractModelNG::SetAccessibilityImportance(const std::string& importance)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityLevel(importance);
}

void ViewAbstractModelNG::SetAccessibilityDefaultFocus(bool isFocus)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    accessibilityManager->AddFrameNodeToDefaultFocusList(AceType::Claim(frameNode), isFocus);
}

void ViewAbstractModelNG::SetAccessibilityUseSamePage(const std::string& pageMode)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (pageMode == accessibilityProperty->GetAccessibilitySamePage()) {
        return;
    }
    accessibilityProperty->SetAccessibilitySamePage(pageMode);
#ifdef WINDOW_SCENE_SUPPORTED
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto uiExtManager = pipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtManager);
    uiExtManager->SendPageModeToProvider(frameNode->GetId(), pageMode);
#endif
}

void ViewAbstractModelNG::SetAccessibilityText(FrameNode* frameNode, const std::string& text)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTextWithEvent(text);
}

void ViewAbstractModelNG::SetAccessibilityImportance(FrameNode* frameNode, const std::string& importance)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityLevel(importance);
}

void ViewAbstractModelNG::SetAccessibilityVirtualNode(std::function<void()>&& buildFunc)
{
    auto buildNodeFunc = [buildFunc = std::move(buildFunc)]() -> RefPtr<UINode> {
        NG::ScopedViewStackProcessor builderViewStackProcessor;
        buildFunc();
        auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
        return customNode;
    };
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto virtualNode = buildNodeFunc();
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    auto virtualFrameNode = AceType::DynamicCast<NG::FrameNode>(virtualNode);
    CHECK_NULL_VOID(virtualFrameNode);
    virtualFrameNode->SetAccessibilityNodeVirtual();
    virtualFrameNode->SetAccessibilityVirtualNodeParent(AceType::Claim(AceType::DynamicCast<NG::UINode>(frameNode)));
    virtualFrameNode->SetFirstAccessibilityVirtualNode();
    frameNode->HasAccessibilityVirtualNode(true);
    accessibilityProperty->SaveAccessibilityVirtualNode(virtualNode);
}

void ViewAbstractModelNG::SetAccessibilitySelected(bool selected, bool resetValue)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserSelected();
    } else {
        accessibilityProperty->SetUserSelected(selected);
    }
}

void ViewAbstractModelNG::SetAccessibilityChecked(bool checked, bool resetValue)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserCheckedType();
        accessibilityProperty->ResetUserCheckable();
    } else {
        accessibilityProperty->SetUserCheckedType(checked);
        accessibilityProperty->SetUserCheckable(true);
    }
}

void ViewAbstractModelNG::SetAccessibilityScrollTriggerable(bool triggerable, bool resetValue)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserScrollTriggerable();
    } else {
        accessibilityProperty->SetUserScrollTriggerable(triggerable);
    }
}

void ViewAbstractModelNG::SetAccessibilityFocusDrawLevel(int32_t drawLevel)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetFocusDrawLevel(drawLevel);
}

void ViewAbstractModelNG::SetAccessibilityRole(const std::string& role, bool resetValue)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue) {
        accessibilityProperty->ResetAccessibilityCustomRole();
    } else {
        accessibilityProperty->SetAccessibilityCustomRole(role);
    }
}

void ViewAbstractModelNG::SetOnAccessibilityFocus(
    NG::OnAccessibilityFocusCallbackImpl&& onAccessibilityFocusCallbackImpl)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetUserOnAccessibilityFocusCallback(onAccessibilityFocusCallbackImpl);
}

void ViewAbstractModelNG::ResetOnAccessibilityFocus()
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->ResetUserOnAccessibilityFocusCallback();
}

void ViewAbstractModelNG::SetOnAccessibilityActionIntercept(
    NG::ActionAccessibilityActionIntercept&& onActionAccessibilityActionIntercept)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityActionIntercept(onActionAccessibilityActionIntercept);
}

void ViewAbstractModelNG::SetOnAccessibilityHoverTransparent(TouchEventFunc&& touchEventFunc)
{
    auto frameNode = NG::ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTransparentCallback(touchEventFunc);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    accessibilityManager->AddHoverTransparentCallback(AceType::Claim(frameNode));
}

void ViewAbstractModelNG::SetAccessibilityDescription(FrameNode* frameNode, const std::string& description)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityDescriptionWithEvent(description);
}

void ViewAbstractModelNG::SetAccessibilityNextFocusId(FrameNode* frameNode, const std::string& nextFocusId)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityNextFocusInspectorKey(nextFocusId);
}

void ViewAbstractModelNG::SetAccessibilityGroup(FrameNode* frameNode, bool accessible)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityGroup(accessible);
}

void ViewAbstractModelNG::SetAccessibilityTextPreferred(FrameNode* frameNode, bool accessibilityTextPreferred)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTextPreferred(accessibilityTextPreferred);
}

void ViewAbstractModelNG::SetAccessibilityDefaultFocus(FrameNode* frameNode, bool isFocus)
{
    CHECK_NULL_VOID(frameNode);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    accessibilityManager->AddFrameNodeToDefaultFocusList(AceType::Claim(frameNode), isFocus);
}

void ViewAbstractModelNG::SetAccessibilityUseSamePage(FrameNode* frameNode, const std::string& pageMode)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (pageMode == accessibilityProperty->GetAccessibilitySamePage()) {
        return;
    }
    accessibilityProperty->SetAccessibilitySamePage(pageMode);
#ifdef WINDOW_SCENE_SUPPORTED
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto uiExtManager = pipeline->GetUIExtensionManager();
    CHECK_NULL_VOID(uiExtManager);
    uiExtManager->SendPageModeToProvider(frameNode->GetId(), pageMode);
#endif
}

bool ViewAbstractModelNG::GetAccessibilityGroup(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, false);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, false);
    return accessibilityProperty->IsAccessibilityGroup();
}

std::string ViewAbstractModelNG::GetAccessibilityText(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, "");
    return accessibilityProperty->GetAccessibilityText();
}

std::string ViewAbstractModelNG::GetAccessibilityDescription(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, "");
    return accessibilityProperty->GetAccessibilityDescription();
}

std::string ViewAbstractModelNG::GetAccessibilityImportance(FrameNode* frameNode)
{
    CHECK_NULL_RETURN(frameNode, "");
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_RETURN(accessibilityProperty, "");
    return accessibilityProperty->GetAccessibilityLevel();
}

void ViewAbstractModelNG::SetAccessibilitySelected(FrameNode* frameNode, bool selected, bool resetValue)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserSelected();
    } else {
        accessibilityProperty->SetUserSelected(selected);
    }
}

void ViewAbstractModelNG::SetAccessibilityChecked(FrameNode* frameNode, bool checked, bool resetValue)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserCheckedType();
        accessibilityProperty->ResetUserCheckable();
    } else {
        accessibilityProperty->SetUserCheckedType(checked);
        accessibilityProperty->SetUserCheckable(true);
    }
}

void ViewAbstractModelNG::SetAccessibilityScrollTriggerable(FrameNode* frameNode, bool triggerable, bool resetValue)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue == true) {
        accessibilityProperty->ResetUserScrollTriggerable();
    } else {
        accessibilityProperty->SetUserScrollTriggerable(triggerable);
    }
}

void ViewAbstractModelNG::SetAccessibilityFocusDrawLevel(FrameNode* frameNode, int32_t drawLevel)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetFocusDrawLevel(drawLevel);
}

void ViewAbstractModelNG::SetAccessibilityRole(FrameNode* frameNode, const std::string& role, bool resetValue)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    if (resetValue) {
        accessibilityProperty->ResetAccessibilityCustomRole();
    } else {
        accessibilityProperty->SetAccessibilityCustomRole(role);
    }
}

void ViewAbstractModelNG::SetOnAccessibilityFocus(
    FrameNode* frameNode, NG::OnAccessibilityFocusCallbackImpl&& onAccessibilityFocusCallbackImpl)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetUserOnAccessibilityFocusCallback(onAccessibilityFocusCallbackImpl);
}

void ViewAbstractModelNG::ResetOnAccessibilityFocus(FrameNode* frameNode)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->ResetUserOnAccessibilityFocusCallback();
}

void ViewAbstractModelNG::SetOnAccessibilityActionIntercept(
    FrameNode* frameNode, NG::ActionAccessibilityActionIntercept&& onActionAccessibilityActionIntercept)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityActionIntercept(onActionAccessibilityActionIntercept);
}

void ViewAbstractModelNG::SetOnAccessibilityHoverTransparent(FrameNode* frameNode, TouchEventFunc&& touchEventFunc)
{
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetAccessibilityTransparentCallback(touchEventFunc);
    auto pipeline = frameNode->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto accessibilityManager = pipeline->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    accessibilityManager->AddHoverTransparentCallback(AceType::Claim(frameNode));
}

std::string ViewAbstractModelNG::PopupTypeStr(PopupType& type)
{
    switch (type) {
        case PopupType::POPUPTYPE_TEXTCOLOR:
            return "TextColor";
        case PopupType::POPUPTYPE_POPUPCOLOR:
            return "PopupColor";
        case POPUPTYPE_MASKCOLOR:
            return "MaskColor";
        default:
            return "";
    }
}

void ViewAbstractModelNG::UpdateColor(PopupType& type, const Color& color)
{
    auto frameNode = ViewStackProcessor::GetInstance()->GetMainFrameNode();
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<BubblePattern>();
    CHECK_NULL_VOID(pattern);
    switch (type) {
        case POPUPTYPE_TEXTCOLOR:
            pattern->UpdateBubbleText(color);
            break;
        case POPUPTYPE_POPUPCOLOR:
            pattern->UpdateBubbleBackGroundColor(color);
            break;
        case POPUPTYPE_MASKCOLOR:
            pattern->UpdateMaskColor(color);
            break;
        default:
            break;
    }
}

void ViewAbstractModelNG::CreateWithColorResourceObj(
    const RefPtr<NG::FrameNode>& frameNode, const RefPtr<ResourceObject>& ColorResObj, PopupType& type)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<BubblePattern>();
    CHECK_NULL_VOID(pattern);
    if (ColorResObj) {
        std::string key = "popup" + PopupTypeStr(type);
        auto&& updateFunc = [&](const RefPtr<ResourceObject>& ColorResObj) {
            std::string color = pattern->GetResCacheMapByKey(key);
            Color result;
            if (color.empty()) {
                ResourceParseUtils::ParseResColor(ColorResObj, result);
                pattern->AddResCache(key, result.ColorToString());
            } else {
                result = Color::FromString(color);
            }
            UpdateColor(type, result);
        };
        updateFunc(ColorResObj);
        pattern->AddResObj(key, ColorResObj, std::move(updateFunc));
    }
}

void ViewAbstractModelNG::CreateWithBoolResourceObj(
    const RefPtr<NG::FrameNode>& frameNode, const RefPtr<ResourceObject>& maskResObj)
{
    CHECK_NULL_VOID(frameNode);
    auto pattern = frameNode->GetPattern<BubblePattern>();
    CHECK_NULL_VOID(pattern);
    if (maskResObj) {
        std::string key = "popupMask";
        auto&& updateFunc = [&](const RefPtr<ResourceObject>& maskResObj) {
            std::string mask = pattern->GetResCacheMapByKey(key);
            bool result;
            if (mask.empty()) {
                ResourceParseUtils::ParseResBool(maskResObj, result);
                std::string maskValue = result ? "true" : "false";
                pattern->AddResCache(key, maskValue);
            } else {
                result = mask == "true";
            }
            pattern->UpdateMask(result);
        };
        updateFunc(maskResObj);
        pattern->AddResObj(key, maskResObj, std::move(updateFunc));
    }
}

void ViewAbstractModelNG::CreateWithResourceObj(
    const RefPtr<NG::FrameNode>& frameNode, const RefPtr<ResourceObject>& resourceObj, PopupType type)
{
    CHECK_NULL_VOID(frameNode);
    CreateWithColorResourceObj(frameNode, resourceObj, type);
}
void ViewAbstractModelNG::CreateWithResourceObj(
    const RefPtr<NG::FrameNode>& frameNode, const RefPtr<ResourceObject>& resourceObj)
{
    CHECK_NULL_VOID(frameNode);
    CreateWithBoolResourceObj(frameNode, resourceObj);
}
} // namespace OHOS::Ace::NG
