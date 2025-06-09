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

#include "adapter/ohos/entrance/subwindow/subwindow_ohos.h"

#include "display_info.h"
#include "dm/display_manager.h"
#include "interfaces/inner_api/ace/viewport_config.h"
#include "render_service_client/core/ui/rs_surface_node.h"
#include "window.h"

#include "adapter/ohos/entrance/ace_application_info.h"
#include "base/geometry/rect.h"
#include "core/components/root/root_element.h"
#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/base/ui_node.h"
#include "core/components_ng/property/property.h"
#include "core/components_v2/inspector/inspector_constants.h"
#include "core/pipeline_ng/pipeline_context.h"

#if defined(ENABLE_ROSEN_BACKEND) and !defined(UPLOAD_GPU_DISABLED)
#include "adapter/ohos/entrance/ace_rosen_sync_task.h"
#endif

#include "bundlemgr/bundle_mgr_interface.h"
#include "iservice_registry.h"

#include "adapter/ohos/entrance/ace_view_ohos.h"
#include "adapter/ohos/entrance/dialog_container.h"
#include "adapter/ohos/entrance/ui_content_impl.h"
#include "adapter/ohos/entrance/utils.h"
#include "base/log/frame_report.h"
#include "base/subwindow/subwindow_manager.h"
#include "base/utils/system_properties.h"
#include "base/utils/utils.h"
#include "core/common/connect_server_manager.h"
#include "core/common/container_scope.h"
#include "core/common/frontend.h"
#include "core/common/hdc_register.h"
#include "core/common/text_field_manager.h"
#include "core/components/bubble/bubble_component.h"
#include "core/components/popup/popup_component.h"
#include "core/components_ng/pattern/menu/menu_view.h"
#include "core/components_ng/pattern/menu/wrapper/menu_wrapper_pattern.h"
#include "core/components_ng/pattern/overlay/overlay_manager.h"
#include "core/components_ng/render/adapter/rosen_render_context.h"
#include "core/components_ng/render/adapter/rosen_window.h"
#include "frameworks/bridge/common/utils/engine_helper.h"
#include "frameworks/bridge/declarative_frontend/declarative_frontend.h"

#ifdef OS_ACCOUNT_EXISTS
#include "os_account_manager.h"
#endif

#include "system_ability_definition.h"

namespace OHOS::Ace {
namespace {
const Rect MIN_WINDOW_HOT_AREA = Rect(0.0f, 0.0f, 1.0f, 1.0f);
constexpr uint32_t ENABLE_SYSTEM_WINDOW_AVOID_AREA = 1;
constexpr uint32_t ENABLE_APP_SUB_WINDOW_AVOID_AREA = 1 << 1;
#ifndef NG_BUILD
constexpr int32_t PLATFORM_VERSION_TEN = 10;
#endif
constexpr uint64_t DEFAULT_DISPLAY_ID = 0;
} // namespace

int32_t SubwindowOhos::id_ = 0;
static std::atomic<int32_t> gToastDialogId = 0;

class SwitchFreeMultiWindowListener : public OHOS::Rosen::ISwitchFreeMultiWindowListener {
public:
    explicit SwitchFreeMultiWindowListener(int32_t instanceId) : instanceId_(instanceId) {}
    ~SwitchFreeMultiWindowListener() = default;

    void OnSwitchFreeMultiWindow(bool enable)
    {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Window status changes, freeMultiWindow is %{public}d", enable);
        auto container = Platform::AceContainer::GetContainer(instanceId_);
        CHECK_NULL_VOID(container);
        auto subWindow = SubwindowManager::GetInstance()->GetSubwindowById(instanceId_);
        CHECK_NULL_VOID(subWindow);

        auto taskExecutor = container->GetTaskExecutor();
        CHECK_NULL_VOID(taskExecutor);
        ContainerScope scope(instanceId_);
        taskExecutor->PostTask(
            [subWindow, enable]() {
                CHECK_NULL_VOID(subWindow);
                subWindow->OnFreeMultiWindowSwitch(enable);
                subWindow->SwitchFollowParentWindowLayout(enable);
            },
            TaskExecutor::TaskType::UI, "ArkUIFreeMultiWindowSwitch");
    }

private:
    int32_t instanceId_ = -1;
};

RefPtr<Subwindow> Subwindow::CreateSubwindow(int32_t instanceId)
{
    return AceType::MakeRefPtr<SubwindowOhos>(instanceId);
}

SubwindowOhos::SubwindowOhos(int32_t instanceId) : windowId_(id_), parentContainerId_(instanceId)
{
    SetSubwindowId(windowId_);
    id_++;
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Create Subwindow, subwindow id %{public}d, container id %{public}d", windowId_,
        instanceId);
}

Rosen::WindowType SubwindowOhos::GetToastRosenType(bool IsSceneBoardEnabled)
{
    auto toastType = GetToastWindowType();
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW,
        "GetToastRosenType windowType: %{public}d, IsSceneBoardEnabled: %{public}d",
        toastType, IsSceneBoardEnabled);
    if (toastType == ToastWindowType::TOAST_IN_TYPE_APP_SUB_WINDOW) {
        if (!IsSceneBoardEnabled && !GetIsSelectOverlaySubWindow()) {
            return Rosen::WindowType::WINDOW_TYPE_TOAST;
        }
        return Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
    } else if (toastType == ToastWindowType::TOAST_IN_TYPE_SYSTEM_SUB_WINDOW) {
        return Rosen::WindowType::WINDOW_TYPE_TOAST;
    } else if (toastType == ToastWindowType::TOAST_IN_TYPE_SYSTEM_FLOAT) {
        return Rosen::WindowType::WINDOW_TYPE_SYSTEM_FLOAT;
    }
    return Rosen::WindowType::WINDOW_TYPE_TOAST;
}

void SubwindowOhos::SetToastWindowOption(RefPtr<Platform::AceContainer>& parentContainer,
    OHOS::sptr<OHOS::Rosen::WindowOption>& windowOption,
    const Rosen::WindowType& toastWindowType, uint32_t mainWindowId)
{
    if (toastWindowType == Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW) {
        windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
        if (GetIsSelectOverlaySubWindow()) {
            windowOption->AddWindowFlag(Rosen::WindowFlag::WINDOW_FLAG_IS_TEXT_MENU);
        } else {
            windowOption->AddWindowFlag(Rosen::WindowFlag::WINDOW_FLAG_IS_TOAST);
        }
    }
    windowOption->SetWindowType(toastWindowType);
    if (parentContainer->IsUIExtensionWindow()) {
        auto parentPipeline = parentContainer->GetPipelineContext();
        CHECK_NULL_VOID(parentPipeline);
        auto hostWindowId = parentPipeline->GetFocusWindowId();
        windowOption->SetIsUIExtAnySubWindow(true);
        windowOption->SetParentId(hostWindowId);
        SetUIExtensionHostWindowId(hostWindowId);
    } else {
        windowOption->SetParentId(mainWindowId);
    }
}

void SetUIExtensionSubwindowFlag(OHOS::sptr<OHOS::Rosen::WindowOption>& windowOption,
    bool isAppSubwindow, sptr<OHOS::Rosen::Window>& parentWindow)
{
    if (isAppSubwindow && (parentWindow->GetIsUIExtFirstSubWindow() ||
        parentWindow->GetIsUIExtAnySubWindow())) {
        windowOption->SetIsUIExtAnySubWindow(true);
    }
}

void SetSubWindowCutout(const RefPtr<PipelineBase> parentPipeline, int32_t childContainerId)
{
    auto parentPipelineContext = AceType::DynamicCast<NG::PipelineContext>(parentPipeline);
    CHECK_NULL_VOID(parentPipelineContext);
    auto parentSafeAreaManager = parentPipelineContext->GetSafeAreaManager();
    CHECK_NULL_VOID(parentSafeAreaManager);
    auto parentUseCutout = parentSafeAreaManager->GetUseCutout();

    auto subPipelineContext = Platform::AceContainer::GetContainer(childContainerId)->GetPipelineContext();
    auto subPipelineContextNG = AceType::DynamicCast<NG::PipelineContext>(subPipelineContext);
    CHECK_NULL_VOID(subPipelineContextNG);
    auto subSafeAreaManager = subPipelineContextNG->GetSafeAreaManager();
    CHECK_NULL_VOID(subSafeAreaManager);
    subSafeAreaManager->SetUseCutout(parentUseCutout);
}

Size GetSubWindowSize(int32_t parentContainerId, uint32_t displayId)
{
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDisplayById(displayId);
    CHECK_NULL_RETURN(defaultDisplay, Size());

    auto size = Size(defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
    if (!SystemProperties::IsSuperFoldDisplayDevice()) {
        return size;
    }

    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId);
    CHECK_NULL_RETURN(parentContainer, size);
    if (parentContainer->GetCurrentFoldStatus() == FoldStatus::EXPAND) {
        return size;
    }

    auto isCrossWindow = parentContainer->IsCrossAxisWindow();
    auto isSceneBoard = parentContainer->IsSceneBoardWindow();
    if (isCrossWindow || isSceneBoard) {
        auto display = Rosen::DisplayManager::GetInstance().GetVisibleAreaDisplayInfoById(DEFAULT_DISPLAY_ID);
        if (!display) {
            TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "failed to GetVisibleAreaDisplayInfoById");
            return size;
        }
        size = Size(display->GetWidth(), display->GetHeight());
    }

    auto parentWindowId = parentContainer->GetWindowId();
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "parentWindow windowId: %{public}d isSceneBoard: %{public}d isCrossWindow: %{public}d displaySize: %{public}s",
        parentWindowId, isSceneBoard, isCrossWindow, size.ToString().c_str());
    return size;
}

void SubwindowOhos::InitWindowRSUIDirector(const RefPtr<Platform::AceContainer>& container)
{
#ifdef ENABLE_ROSEN_BACKEND
    if (!SystemProperties::GetMultiInstanceEnabled()) {
        rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
        if (rsUiDirector != nullptr) {
            rsUiDirector->SetRSSurfaceNode(window_->GetSurfaceNode());
            auto context = DynamicCast<PipelineContext>(container->GetPipelineContext());
            if (context != nullptr) {
                context->SetRSUIDirector(rsUiDirector);
            }
            rsUiDirector->Init();
            TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "UIContent Init Rosen Backend");
        }
    } else {
        rsUiDirector = window_->GetRSUIDirector();
        if (!rsUiDirector) {
            rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
        }
        rsUiDirector->SetRSSurfaceNode(window_->GetSurfaceNode());
        auto context = DynamicCast<PipelineContext>(container->GetPipelineContext());
        if (context != nullptr) {
            context->SetRSUIDirector(rsUiDirector);
        }
        rsUiDirector->Init(true, true);
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "UIContent Init Rosen Backend");
    }
#endif
}

void SubwindowOhos::InitContainer()
{
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentContainer);
    auto parentPipeline = parentContainer->GetPipelineContext();
    CHECK_NULL_VOID(parentPipeline);
    if (!window_) {
        OHOS::sptr<OHOS::Rosen::WindowOption> windowOption = new OHOS::Rosen::WindowOption();
        auto parentWindowName = parentContainer->GetWindowName();
        auto parentWindowId = parentContainer->GetWindowId();
        sptr<OHOS::Rosen::Window> parentWindow = parentContainer->GetUIWindow(parentContainerId_);
        CHECK_NULL_VOID(parentWindow);
        parentWindow_ = parentWindow;
        auto windowType = parentWindow->GetType();
        std::string windowTag = "";
        bool isAppSubwindow = false;
        bool needFollowScreen = false;
        if (IsSystemTopMost()) {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_SYSTEM_TOAST);
            windowTag = "TOAST_SYSTEM_";
            needFollowScreen = true;
        } else if (GetAboveApps()) {
            auto toastWindowType = GetToastRosenType(parentContainer->IsSceneBoardEnabled());
            isAppSubwindow = toastWindowType == Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW;
            auto isSelectOverlay = GetIsSelectOverlaySubWindow();
            auto mainWindowId = isSelectOverlay ? parentWindowId : GetMainWindowId();
            SetToastWindowOption(parentContainer, windowOption, toastWindowType, mainWindowId);
            windowTag = isSelectOverlay ? "TEXT_MENU_" : "TOAST_TOPMOST_";
        } else if (parentContainer->IsSceneBoardWindow() || windowType == Rosen::WindowType::WINDOW_TYPE_DESKTOP) {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_SYSTEM_FLOAT);
            needFollowScreen = true;
        } else if (windowType == Rosen::WindowType::WINDOW_TYPE_UI_EXTENSION) {
            auto hostWindowId = parentPipeline->GetFocusWindowId();
            windowOption->SetIsUIExtFirstSubWindow(true);
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
            windowOption->SetParentId(hostWindowId);
            SetUIExtensionHostWindowId(hostWindowId);
            isAppSubwindow = true;
        } else if (windowType >= Rosen::WindowType::SYSTEM_WINDOW_BASE) {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_SYSTEM_SUB_WINDOW);
            windowOption->SetParentId(parentWindowId);
            needFollowScreen = true;
        } else {
            windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_SUB_WINDOW);
            windowOption->SetParentId(parentWindowId);
            isAppSubwindow = true;
        }
        auto displayId = parentContainer->GetCurrentDisplayId();
        auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDisplayById(displayId);
        if (!defaultDisplay) {
            TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "DisplayManager failed to getDisplay by id: %{public}u",
                (uint32_t)displayId);
        }
        CHECK_NULL_VOID(defaultDisplay);

        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Parent window displayId: %{public}u width: %{public}d height: %{public}d",
            (uint32_t)displayId, defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
        auto windowSize = GetSubWindowSize(parentContainerId_, displayId);
        windowOption->SetWindowRect(
            { 0, 0, static_cast<uint32_t>(windowSize.Width()), static_cast<uint32_t>(windowSize.Height()) });
        windowOption->SetWindowRect({ 0, 0, static_cast<uint32_t>(defaultDisplay->GetWidth()),
            static_cast<uint32_t>(defaultDisplay->GetHeight()) });
        windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FLOATING);
        SetUIExtensionSubwindowFlag(windowOption, isAppSubwindow, parentWindow);
        windowOption->SetDisplayId(displayId);
        OHOS::Rosen::WMError ret;
        window_ = OHOS::Rosen::Window::Create("ARK_APP_SUBWINDOW_" + windowTag + parentWindowName +
            std::to_string(windowId_), windowOption, parentWindow->GetContext(), ret);
        if (!window_ || ret != OHOS::Rosen::WMError::WM_OK) {
            SetIsRosenWindowCreate(false);
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Window create failed, errCode is %{public}d", ret);
        }
        CHECK_NULL_VOID(window_);
        window_->RegisterWindowAttachStateChangeListener(new MenuWindowSceneListener(WeakClaim(this)));
        window_->SetFollowScreenChange(needFollowScreen);
        defaultDisplayId_ = displayId;
        window_->SetSubWindowSource(Rosen::SubWindowSource::SUB_WINDOW_SOURCE_ARKUI);
    }
    std::string url = "";
    auto subSurface = window_->GetSurfaceNode();
    CHECK_NULL_VOID(subSurface);
    subSurface->SetShadowElevation(0.0f);
    window_->NapiSetUIContent(url, (napi_env)nullptr, (napi_value)nullptr, Rosen::BackupAndRestoreType::NONE);
    childContainerId_ = SubwindowManager::GetInstance()->GetContainerId(window_->GetWindowId());
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "Window child containerId : %{public}d", childContainerId_);
    SubwindowManager::GetInstance()->AddParentContainerId(childContainerId_, parentContainerId_);

    auto container = Platform::AceContainer::GetContainer(childContainerId_);
    if (!container) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Window get ace container failed");
    }
    CHECK_NULL_VOID(container);

    auto parentToken = parentContainer->GetToken();
    container->SetToken(parentToken);
    container->SetWindowId(window_->GetWindowId());
    container->SetParentId(parentContainerId_);
    container->GetSettings().SetUsingSharedRuntime(true);
    container->SetSharedRuntime(parentContainer->GetSharedRuntime());
    container->Initialize();
    container->SetAssetManager(parentContainer->GetAssetManager());
    container->SetResourceConfiguration(parentContainer->GetResourceConfiguration());
    container->SetPackagePathStr(parentContainer->GetPackagePathStr());
    container->SetHapPath(parentContainer->GetHapPath());
    container->SetIsSubContainer(true);
    container->InitializeSubContainer(parentContainerId_);
    SetIsRosenWindowCreate(true);
    ViewportConfig config;
    // create ace_view
    auto aceView =
        Platform::AceViewOhos::CreateView(childContainerId_, false, container->GetSettings().usePlatformAsUIThread);
    Platform::AceViewOhos::SurfaceCreated(aceView, window_);

    int32_t width = static_cast<int32_t>(window_->GetRequestRect().width_);
    int32_t height = static_cast<int32_t>(window_->GetRequestRect().height_);
    auto density = parentPipeline->GetDensity();
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "UIContent Initialize: width: %{public}d, height: %{public}d, density: %{public}lf", width, height, density);

    Ace::Platform::UIEnvCallback callback = nullptr;
    // set view
    Platform::AceContainer::SetView(aceView, density, width, height, window_, callback);
    Platform::AceViewOhos::SurfaceChanged(aceView, width, height, config.Orientation());

    auto uiContentImpl = reinterpret_cast<UIContentImpl*>(window_->GetUIContent());
    CHECK_NULL_VOID(uiContentImpl);
    uiContentImpl->SetFontScaleAndWeightScale(container, childContainerId_);
    freeMultiWindowListener_ = new SwitchFreeMultiWindowListener(childContainerId_);
    window_->RegisterSwitchFreeMultiWindowListener(freeMultiWindowListener_);
    window_->SetAvoidAreaOption(ENABLE_SYSTEM_WINDOW_AVOID_AREA | ENABLE_APP_SUB_WINDOW_AVOID_AREA);

#ifndef NG_BUILD
#ifdef ENABLE_ROSEN_BACKEND
    if (SystemProperties::GetRosenBackendEnabled()) {
        InitWindowRSUIDirector(container);
    }
#endif
#endif
#ifdef NG_BUILD
    auto subPipelineContextNG = AceType::DynamicCast<NG::PipelineContext>(
        Platform::AceContainer::GetContainer(childContainerId_)->GetPipelineContext());
    CHECK_NULL_VOID(subPipelineContextNG);
    subPipelineContextNG->SetParentPipeline(parentContainer->GetPipelineContext());
    subPipelineContextNG->SetupSubRootElement();
    subPipelineContextNG->SetMinPlatformVersion(parentPipeline->GetMinPlatformVersion());
    subPipelineContextNG->SetDragNodeGrayscale(parentPipeline->GetDragNodeGrayscale());
    subPipelineContextNG->SetMaxAppFontScale(parentPipeline->GetMaxAppFontScale());
    subPipelineContextNG->SetFollowSystem(parentPipeline->IsFollowSystem());
    subPipelineContextNG->SetFontScale(parentPipeline->GetFontScale());
    subPipelineContextNG->SetApiTargetVersion(parentPipeline->GetApiTargetVersion());
#else
    if (container->IsCurrentUseNewPipeline()) {
        auto subPipelineContextNG = AceType::DynamicCast<NG::PipelineContext>(
            Platform::AceContainer::GetContainer(childContainerId_)->GetPipelineContext());
        CHECK_NULL_VOID(subPipelineContextNG);
        subPipelineContextNG->SetParentPipeline(parentContainer->GetPipelineContext());
        subPipelineContextNG->SetupSubRootElement();
        subPipelineContextNG->SetMinPlatformVersion(parentPipeline->GetMinPlatformVersion());
        subPipelineContextNG->SetKeyboardAnimationConfig(parentPipeline->GetKeyboardAnimationConfig());
        subPipelineContextNG->SetDragNodeGrayscale(parentPipeline->GetDragNodeGrayscale());
        subPipelineContextNG->SetMaxAppFontScale(parentPipeline->GetMaxAppFontScale());
        subPipelineContextNG->SetFollowSystem(parentPipeline->IsFollowSystem());
        subPipelineContextNG->SetFontScale(parentPipeline->GetFontScale());
        subPipelineContextNG->SetApiTargetVersion(parentPipeline->GetApiTargetVersion());
        return;
    }
    auto subPipelineContext =
        DynamicCast<PipelineContext>(Platform::AceContainer::GetContainer(childContainerId_)->GetPipelineContext());
    CHECK_NULL_VOID(subPipelineContext);
    subPipelineContext->SetParentPipeline(parentContainer->GetPipelineContext());
    subPipelineContext->SetupSubRootElement();
    subPipelineContext->SetMinPlatformVersion(parentPipeline->GetMinPlatformVersion());
    subPipelineContext->SetKeyboardAnimationConfig(parentPipeline->GetKeyboardAnimationConfig());
    subPipelineContext->SetDragNodeGrayscale(parentPipeline->GetDragNodeGrayscale());
    subPipelineContext->SetMaxAppFontScale(parentPipeline->GetMaxAppFontScale());
    subPipelineContext->SetFollowSystem(parentPipeline->IsFollowSystem());
    subPipelineContext->SetFontScale(parentPipeline->GetFontScale());
    subPipelineContext->SetApiTargetVersion(parentPipeline->GetApiTargetVersion());
#endif
}

RefPtr<PipelineBase> SubwindowOhos::GetChildPipelineContext() const
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(aceContainer, nullptr);
    return aceContainer->GetPipelineContext();
}

std::function<void()> SubwindowOhos::GetInitToastDelayTask(const NG::ToastInfo& toastInfo,
    std::function<void(int32_t)>&& callback)
{
    return [toastInfo, callbackParam = std::move(callback)]() {
        int32_t posX = 0;
        int32_t posY = 0;
        int32_t width = 0;
        int32_t height = 0;
        float density = 1.0f;
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        subwindowOhos->GetToastDialogWindowProperty(width, height, posX, posY, density);
        auto childContainerId = subwindowOhos->GetChildContainerId();
        auto window = Platform::DialogContainer::GetUIWindow(childContainerId);
        auto dialogWindow = subwindowOhos->GetDialogWindow();
        if (!dialogWindow || !window || !subwindowOhos->IsToastWindow()) {
            bool ret = subwindowOhos->InitToastDialogWindow(width, height, posX, posY, true);
            if (!ret) {
                TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog window failed");
                return;
            }
            ret = subwindowOhos->InitToastDialogView(width, height, density);
            if (!ret) {
                TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog view failed");
                return;
            }
            subwindowOhos->SetIsToastWindow(true);
        }
        childContainerId = subwindowOhos->GetChildContainerId();
        ContainerScope scope(childContainerId);
        subwindowOhos->UpdateAceView(width, height, density, childContainerId);
        TAG_LOGD(AceLogTag::ACE_SUB_WINDOW,
            "update ace view width : %{public}d,  height : %{public}d, density : %{public}f,childContainerId : "
            "%{public}d",
            width, height, density, childContainerId);
        auto container = Platform::AceContainer::GetContainer(childContainerId);
        CHECK_NULL_VOID(container);
        container->SetFontScaleAndWeightScale(childContainerId);
        auto ret = subwindowOhos->InitToastServiceConfig();
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast service conf failed");
        }
        Platform::DialogContainer::ShowToastDialogWindow(childContainerId, posX, posY, width, height, true);
        Platform::DialogContainer::ShowToast(childContainerId, toastInfo,
            std::move(const_cast<std::function<void(int32_t)>&&>(callbackParam)));
    };
}

void SubwindowOhos::ResizeWindow()
{
    CHECK_NULL_VOID(window_);
    auto windowSize = GetSubWindowSize(parentContainerId_, window_->GetDisplayId());
    auto ret = window_->Resize(windowSize.Width(), windowSize.Height());
    if (ret != Rosen::WMError::WM_OK) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Resize window by default display failed with errCode: %{public}d",
            static_cast<int32_t>(ret));
        return;
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "SubwindowOhos window rect is resized to x: %{public}d, y: %{public}d, width: %{public}u, height: %{public}u",
        window_->GetRect().posX_, window_->GetRect().posY_, window_->GetRect().width_, window_->GetRect().height_);
}

void SubwindowOhos::ResizeWindowForMenu()
{
    CHECK_NULL_VOID(window_);
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentContainer);
    auto pipeline = DynamicCast<NG::PipelineContext>(parentContainer->GetPipelineContext());
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);

    Rosen::WMError ret;
    if (!(theme->GetExpandDisplay() || parentContainer->IsFreeMultiWindow()) &&
        SystemProperties::GetDeviceOrientation() == DeviceOrientation::LANDSCAPE) {
        if (parentContainer->IsUIExtensionWindow()) {
            auto subwindow =
                SubwindowManager::GetInstance()->GetSubwindowByType(childContainerId_, SubwindowType::TYPE_MENU);
            CHECK_NULL_VOID(subwindow);
            auto rect = subwindow->GetUIExtensionHostWindowRect();
            ret = window_->Resize(rect.Width(), rect.Height());
        } else {
            auto rect = pipeline->GetDisplayWindowRectInfo();
            ret = window_->Resize(rect.Width(), rect.Height());
        }
    } else {
        auto windowSize = GetSubWindowSize(parentContainerId_, window_->GetDisplayId());
        ret = window_->Resize(windowSize.Width(), windowSize.Height());
    }
    if (ret != Rosen::WMError::WM_OK) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Resize window by default display failed with errCode: %{public}d",
            static_cast<int32_t>(ret));
        return;
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "SubwindowOhos window rect is resized for menu to x: %{public}d, y: %{public}d, width: %{public}u, height: "
        "%{public}u",
        window_->GetRect().posX_, window_->GetRect().posY_, window_->GetRect().width_, window_->GetRect().height_);
}

void SubwindowOhos::SetRect(const NG::RectF& rect)
{
    windowRect_ = rect;
}

NG::RectF SubwindowOhos::GetRect()
{
    return windowRect_;
}

void SubwindowOhos::ResizeDialogSubwindow()
{
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    CHECK_NULL_VOID(defaultDisplay);
    if (!(NearEqual(defaultDisplay->GetWidth(), window_->GetRect().width_) &&
        NearEqual(defaultDisplay->GetHeight(), window_->GetRect().height_))) {
        auto container = Container::Current();
        CHECK_NULL_VOID(container);
        auto taskExecutor = container->GetTaskExecutor();
        CHECK_NULL_VOID(taskExecutor);
        taskExecutor->PostTask(
            [this]() {
                ResizeWindow();
            },
            TaskExecutor::TaskType::UI, "ArkUIResizeDialogSubwindow");
    }
}

void SubwindowOhos::ShowPopup(const RefPtr<Component>& newComponent, bool disableTouchEvent)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show popup enter");
#ifndef NG_BUILD
    ShowWindow();
    auto stack = GetStack();
    CHECK_NULL_VOID(stack);
    auto popup = AceType::DynamicCast<TweenComponent>(newComponent);
    CHECK_NULL_VOID(popup);
    stack->PopPopup(popup->GetId());
    stack->PushComponent(newComponent, disableTouchEvent);
    auto bubble = AceType::DynamicCast<BubbleComponent>(popup->GetChild());
    if (bubble) {
        bubble->SetWeakStack(WeakClaim(RawPtr(stack)));
    }
#endif
}

bool SubwindowOhos::CancelPopup(const std::string& id)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "cancel popup enter");
#ifndef NG_BUILD
    auto stack = GetStack();
    CHECK_NULL_RETURN(stack, false);
    stack->PopPopup(id);
    auto context = stack->GetContext().Upgrade();
    CHECK_NULL_RETURN(context, false);
    context->FlushPipelineImmediately();
    HideWindow();
#endif
    return true;
}

void SubwindowOhos::ShowPopupNG(int32_t targetId, const NG::PopupInfo& popupInfo,
    const std::function<void(int32_t)>&& onWillDismiss, bool interactiveDismiss)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show popup ng enter, subwindowId: %{public}d", window_->GetWindowId());
    popupTargetId_ = targetId;
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    ResizeWindow();
    ShowWindow(popupInfo.focusable);
    CHECK_NULL_VOID(window_);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    needAvoidKeyboard_ = popupInfo.isAvoidKeyboard;
    overlayManager->ShowPopup(targetId, popupInfo, std::move(onWillDismiss), interactiveDismiss);
    window_->SetFocusable(true);
}

void SubwindowOhos::HidePopupNG(int32_t targetId)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "hide popup ng enter, subwindowId: %{public}d, subwindowName: %{public}s",
        window_->GetWindowId(), window_->GetWindowName().c_str());
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    auto popupInfo = overlayManager->GetPopupInfo(targetId == -1 ? popupTargetId_ : targetId);
    popupInfo.markNeedUpdate = true;
    ContainerScope scope(childContainerId_);
    overlayManager->HidePopup(targetId == -1 ? popupTargetId_ : targetId, popupInfo);
    context->FlushPipelineImmediately();
    HideEventColumn();
    HidePixelMap();
}

void SubwindowOhos::ShowTipsNG(int32_t targetId, const NG::PopupInfo& popupInfo, int32_t appearingTime,
    int32_t appearingTimeWithContinuousOperation, bool isSubwindow)
{
    popupTargetId_ = targetId;
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    ResizeWindow();
    ShowWindow(popupInfo.focusable);
    CHECK_NULL_VOID(window_);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    overlayManager->ShowTips(targetId, popupInfo, appearingTime, appearingTimeWithContinuousOperation, isSubwindow);
    window_->SetFocusable(true);
}

void SubwindowOhos::HideTipsNG(int32_t targetId, int32_t disappearingTime)
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    auto popupInfo = overlayManager->GetPopupInfo(targetId == -1 ? popupTargetId_ : targetId);
    popupInfo.markNeedUpdate = true;
    ContainerScope scope(childContainerId_);
    overlayManager->HideTips(targetId == -1 ? popupTargetId_ : targetId, popupInfo, disappearingTime);
    context->FlushPipelineImmediately();
    HideEventColumn();
    HidePixelMap();
}

void SubwindowOhos::GetPopupInfoNG(int32_t targetId, NG::PopupInfo& popupInfo)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "get popup info ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    popupInfo = overlayManager->GetPopupInfo(targetId);
}

const RefPtr<NG::OverlayManager> SubwindowOhos::GetOverlayManager()
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(aceContainer, nullptr);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_RETURN(context, nullptr);
    return context->GetOverlayManager();
}

void SubwindowOhos::ShowWindow(bool needFocus)
{
    CHECK_NULL_VOID(window_);
    if (isShowed_) {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "subwindow id:%{public}u is on display", window_->GetWindowId());
        if (needFocus) {
            window_->SetFocusable(needFocus);
            RequestFocus();
        }
        return;
    }
    // Set min window hot area so that sub window can transparent event.
    std::vector<Rosen::Rect> hotAreas;
    Rosen::Rect rosenRect {};
    RectConverter(MIN_WINDOW_HOT_AREA, rosenRect);
    hotAreas.emplace_back(rosenRect);
    window_->SetTouchHotAreas(hotAreas);

    window_->SetNeedDefaultAnimation(false);
    auto ret = window_->SetFocusable(needFocus);
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW,
            "subwindow id:%{public}u set focusable %{public}d failed with WMError: %{public}d", window_->GetWindowId(),
            needFocus, static_cast<int32_t>(ret));
    }
    ret = window_->Show(0, false, needFocus);
    attachState_ = MenuWindowState::ATTACHING;
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "Show subwindow id:%{public}u failed with WMError: %{public}d",
            window_->GetWindowId(), static_cast<int32_t>(ret));
        return;
    }
    if (needFocus) {
        RequestFocus();
    }

    InitializeSafeArea();
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = aceContainer->GetPipelineContext();
    CHECK_NULL_VOID(context);
    AccessibilityEvent event;
    event.type = AccessibilityEventType::PAGE_CHANGE;
    event.windowId = context->GetWindowId();
    event.windowChangeTypes = WINDOW_UPDATE_ADDED;
    context->SendEventToAccessibility(event);
    isShowed_ = true;
    if (ifNeedSetCurrentWindow_) {
        SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));
    }
}

void SubwindowOhos::HideWindow()
{
    CHECK_NULL_VOID(window_);
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Hide the subwindow %{public}s", window_->GetWindowName().c_str());

    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "get container failed, child containerId: %{public}d", childContainerId_);
        return;
    }

#ifdef NG_BUILD
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto rootNode = context->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    if (!rootNode->GetChildren().empty() &&
        !(rootNode->GetChildren().size() == 1 && rootNode->GetLastChild()->GetTag() == V2::KEYBOARD_ETS_TAG)) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Subwindow has other node, the last child is %{public}s",
            rootNode->GetLastChild()->GetTag().c_str());
        auto lastChildId = rootNode->GetLastChild()->GetId();
        auto iter = hotAreasMap_.find(lastChildId);
        if (iter != hotAreasMap_.end()) {
            auto hotAreaRect = iter->second;
            OHOS::Rosen::WMError ret = window_->SetTouchHotAreas(hotAreaRect);
            if (ret != OHOS::Rosen::WMError::WM_OK) {
                TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Set hot areas failed with errCode: %{public}d",
                    static_cast<int32_t>(ret));
            }
        }
        return;
    }
    auto focusHub = rootNode->GetFocusHub();
    CHECK_NULL_VOID(focusHub);
    focusHub->SetIsDefaultHasFocused(false);
    context->SetIsFocusActive(false);
#else
    if (Container::IsCurrentUseNewPipeline()) {
        auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
        CHECK_NULL_VOID(context);
        auto rootNode = context->GetRootElement();
        CHECK_NULL_VOID(rootNode);
        if (!rootNode->GetChildren().empty() &&
            !(rootNode->GetChildren().size() == 1 && rootNode->GetLastChild()->GetTag() == V2::KEYBOARD_ETS_TAG)) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Subwindow has other node, the last child is %{public}s",
                rootNode->GetLastChild()->GetTag().c_str());
            return;
        }
        auto focusHub = rootNode->GetFocusHub();
        CHECK_NULL_VOID(focusHub);
        focusHub->SetIsDefaultHasFocused(false);
        context->SetIsFocusActive(false);
    } else {
        auto context = DynamicCast<PipelineContext>(aceContainer->GetPipelineContext());
        CHECK_NULL_VOID(context);
        auto rootNode = context->GetRootElement();
        CHECK_NULL_VOID(rootNode);
        rootNode->SetIsDefaultHasFocused(false);
    }
#endif
    if (!window_->IsFocused()) {
        ContainerModalUnFocus();
    }
    OHOS::Rosen::WMError ret = window_->Hide();
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    if (!parentContainer) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "get container failed, parent containerId: %{public}d", parentContainerId_);
        return;
    }
    if (parentContainer->IsSceneBoardWindow()) {
        window_->SetTouchable(true);
    }

    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "Hide window failed with errCode: %{public}d", static_cast<int32_t>(ret));
        return;
    }
    if (isShowed_) {
        detachState_ = MenuWindowState::DETACHING;
    }
    isShowed_ = false;
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Hide the subwindow successfully.");
#ifndef NG_BUILD
    auto context = aceContainer->GetPipelineContext();
    CHECK_NULL_VOID(context);
#endif
    AccessibilityEvent event;
    event.type = AccessibilityEventType::PAGE_CHANGE;
    event.windowId = context->GetWindowId();
    event.windowChangeTypes = WINDOW_UPDATE_REMOVED;
    context->SendEventToAccessibility(event);
}

void SubwindowOhos::ContainerModalUnFocus()
{
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentContainer);
    auto parentWindowName = parentContainer->GetWindowName();
    sptr<OHOS::Rosen::Window> parentWindow = OHOS::Rosen::Window::Find(parentWindowName);
    CHECK_NULL_VOID(parentWindow);
    if (parentWindow->GetFocusable() && !parentWindow->IsFocused()) {
        auto pipelineContext = parentContainer->GetPipelineContext();
        CHECK_NULL_VOID(pipelineContext);
        pipelineContext->ContainerModalUnFocus();
    }
}

void SubwindowOhos::AddMenu(const RefPtr<Component>& newComponent)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "add menu enter");
#ifndef NG_BUILD
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Subwindow push new component start.");
    auto stack = GetStack();
    CHECK_NULL_VOID(stack);
    // Push the component
    stack->PopMenu();
    stack->PushComponent(newComponent);
    popup_ = AceType::DynamicCast<SelectPopupComponent>(newComponent);
#endif
}

void SubwindowOhos::ClearMenu()
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "clear menu enter");
    if (haveDialog_) {
        return;
    }
#ifndef NG_BUILD
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Subwindow Clear menu start.");
    auto stack = GetStack();
    CHECK_NULL_VOID(stack);
    // Pop the component
    stack->PopMenu();
    auto context = stack->GetContext().Upgrade();
    CHECK_NULL_VOID(context);
    context->FlushPipelineImmediately();
    HideWindow();
#endif
}

bool SubwindowOhos::ShowPreviewNG(bool isStartDraggingFromSubWindow)
{
    CHECK_NULL_RETURN(window_, false);
    ResizeWindow();
    ShowWindow(false);
    if (!isStartDraggingFromSubWindow) {
        window_->SetTouchable(false);
    }
    return true;
}

void SubwindowOhos::HidePreviewNG()
{
    auto overlayManager = GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    overlayManager->RemovePixelMap();
    overlayManager->RemovePreviewBadgeNode();
    overlayManager->RemoveGatherNode();
    overlayManager->RemoveEventColumn();
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto pipeline = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(pipeline);
    pipeline->FlushPipelineImmediately();
    HideSubWindowNG();
}

void SubwindowOhos::ShowMenuNG(const RefPtr<NG::FrameNode> customNode, const NG::MenuParam& menuParam,
    const RefPtr<NG::FrameNode>& targetNode, const NG::OffsetF& offset)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show menu ng enter");
    CHECK_NULL_VOID(customNode);
    if (!targetNode) {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "targetNode is nullptr");
        return;
    }
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    auto menuNode = customNode;
    if (customNode->GetTag() != V2::MENU_WRAPPER_ETS_TAG) {
        menuNode = NG::MenuView::Create(customNode, targetNode->GetId(), targetNode->GetTag(), menuParam, true);
        auto menuWrapperPattern = menuNode->GetPattern<NG::MenuWrapperPattern>();
        CHECK_NULL_VOID(menuWrapperPattern);
        menuWrapperPattern->RegisterMenuCallback(menuNode, menuParam);
        menuWrapperPattern->SetMenuTransitionEffect(menuNode, menuParam);
    }
    ResizeWindowForMenu();
    ShowWindow();
    CHECK_NULL_VOID(window_);
    window_->SetTouchable(true);
    overlay->ShowMenuInSubWindow(targetNode->GetId(), offset, menuNode);
}

void SubwindowOhos::ShowMenuNG(std::function<void()>&& buildFunc, std::function<void()>&& previewBuildFunc,
    const NG::MenuParam& menuParam, const RefPtr<NG::FrameNode>& targetNode, const NG::OffsetF& offset)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show menu ng enter");
    if (!targetNode) {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "targetNode is nullptr");
        return;
    }
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ResizeWindowForMenu();
    ShowWindow();
    CHECK_NULL_VOID(window_);
    window_->SetTouchable(true);
    NG::ScopedViewStackProcessor builderViewStackProcessor;
    buildFunc();
    auto customNode = NG::ViewStackProcessor::GetInstance()->Finish();
    RefPtr<NG::UINode> previewCustomNode;
    if (previewBuildFunc && menuParam.previewMode == MenuPreviewMode::CUSTOM) {
        previewBuildFunc();
        previewCustomNode = NG::ViewStackProcessor::GetInstance()->Finish();
    }
    auto menuNode =
        NG::MenuView::Create(customNode, targetNode->GetId(), targetNode->GetTag(), menuParam, true, previewCustomNode);
    auto menuWrapperPattern = menuNode->GetPattern<NG::MenuWrapperPattern>();
    CHECK_NULL_VOID(menuWrapperPattern);
    menuWrapperPattern->RegisterMenuCallback(menuNode, menuParam);
    menuWrapperPattern->SetMenuTransitionEffect(menuNode, menuParam);
    overlay->ShowMenuInSubWindow(targetNode->GetId(), offset, menuNode);
}

void SubwindowOhos::HideMenuNG(bool showPreviewAnimation, bool startDrag)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "hide menu ng enter");
    if (!isShowed_) {
        return;
    }
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    overlay->HideMenuInSubWindow(showPreviewAnimation, startDrag);
    HideEventColumn();
    HidePixelMap(startDrag, 0, 0, false);
}

void SubwindowOhos::HideMenuNG(const RefPtr<NG::FrameNode>& menu, int32_t targetId)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "hide menu ng enter");
    if (!isShowed_) {
        return;
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "Subwindow hide menu for target id %{public}d", targetId);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    overlay->HideMenuInSubWindow(menu, targetId);
    HideEventColumn();
    HidePixelMap(false, 0, 0, false);
}

void SubwindowOhos::UpdatePreviewPosition()
{
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    if (overlay->GetHasPixelMap()) {
        return;
    }
    overlay->UpdatePixelMapPosition(true);
}

bool SubwindowOhos::GetMenuPreviewCenter(NG::OffsetF& offset)
{
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_RETURN(overlay, false);
    return overlay->GetMenuPreviewCenter(offset);
}

void SubwindowOhos::UpdateHideMenuOffsetNG(
    const NG::OffsetF& offset, float menuScale, bool isRedragStart, int32_t menuWrapperId)
{
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    if (overlay->IsContextMenuDragHideFinished()) {
        return;
    }
    overlay->UpdateContextMenuDisappearPosition(offset, menuScale, isRedragStart, menuWrapperId);
}

void SubwindowOhos::ContextMenuSwitchDragPreviewAnimationtNG(const RefPtr<NG::FrameNode>& dragPreviewNode,
    const NG::OffsetF& offset)
{
    CHECK_NULL_VOID(dragPreviewNode);
    ContainerScope scope(childContainerId_);
    auto overlay = GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    overlay->ContextMenuSwitchDragPreviewAnimation(dragPreviewNode, offset);
}

void SubwindowOhos::ClearMenuNG(int32_t targetId, bool inWindow, bool showAnimation)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "clear menu ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    if (showAnimation) {
        overlay->CleanMenuInSubWindowWithAnimation();
    } else {
        overlay->CleanMenuInSubWindow(targetId);
    }
    HideWindow();
    if (overlay->GetMenuNode(targetId)) {
        context->FlushPipelineImmediately();
    }
    overlay->EraseMenuInfo(targetId);
    if (inWindow) {
        HideEventColumn();
    }
    HidePixelMap(false, 0, 0, false);
}

void SubwindowOhos::ClearPopupNG()
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "clear popup ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    overlay->CleanPopupInSubWindow();
    HideWindow();
    context->FlushPipelineImmediately();
}

void SubwindowOhos::ClearPopupNG(bool isForceClear)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "clear popup ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    overlay->CleanPopupInSubWindow(isForceClear);
    HideWindow();
    context->FlushPipelineImmediately();
}

void SubwindowOhos::ShowMenu(const RefPtr<Component>& newComponent)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show menu enter");
    ShowWindow();
    AddMenu(newComponent);
}

void SubwindowOhos::CloseMenu()
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "close menu enter");
#ifndef NG_BUILD
    if (!isShowed_) {
        return;
    }
    if (popup_) {
        popup_->CloseContextMenu();
    }
#endif
}

RefPtr<StackElement> SubwindowOhos::GetStack()
{
#ifndef NG_BUILD
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(aceContainer, nullptr);

    auto context = DynamicCast<PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_RETURN(context, nullptr);
    return context->GetLastStack();
#else
    return nullptr;
#endif
}

void SubwindowOhos::DeleteHotAreas(int32_t nodeId)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "delete hot area %{public}d", nodeId);
    CHECK_NULL_VOID(window_);
    hotAreasMap_.erase(nodeId);
    if (hotAreasMap_.size() == 0) {
        // Set min window hot area so that sub window can transparent event.
        std::vector<Rosen::Rect> hotAreas;
        Rosen::Rect rosenRect {};
        RectConverter(MIN_WINDOW_HOT_AREA, rosenRect);
        hotAreas.emplace_back(rosenRect);
        window_->SetTouchHotAreas(hotAreas);
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "hotAreasMap_ has no item");
        return;
    }
    std::vector<Rosen::Rect> hotAreas;
    for (auto it = hotAreasMap_.begin(); it != hotAreasMap_.end(); it++) {
        hotAreas.insert(hotAreas.end(), it->second.begin(), it->second.end());
    }
    CHECK_NULL_VOID(window_);
    window_->SetTouchHotAreas(hotAreas);
}

void SubwindowOhos::SetHotAreas(const std::vector<Rect>& rects, int32_t nodeId)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "set hot areas enter");
    CHECK_NULL_VOID(window_);

    std::vector<Rosen::Rect> hotAreas;
    Rosen::Rect rosenRect {};
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "set hot area %{public}d", nodeId);
    for (const auto& rect : rects) {
        TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "set hot area rect %{public}s", rect.ToString().c_str());
        RectConverter(rect, rosenRect);
        hotAreas.emplace_back(rosenRect);
    }
    if (nodeId >= 0) {
        hotAreasMap_[nodeId] = hotAreas;
    }

    std::vector<Rosen::Rect> hotAreasNow;
    for (auto it = hotAreasMap_.begin(); it != hotAreasMap_.end(); it++) {
        hotAreasNow.insert(hotAreasNow.end(), it->second.begin(), it->second.end());
    }
    window_->SetTouchHotAreas(hotAreasNow);
}

void SubwindowOhos::RectConverter(const Rect& rect, Rosen::Rect& rosenRect)
{
    rosenRect.posX_ = static_cast<int>(rect.GetOffset().GetX());
    rosenRect.posY_ = static_cast<int>(rect.GetOffset().GetY());
    rosenRect.width_ = static_cast<uint32_t>(rect.GetSize().Width());
    rosenRect.height_ = static_cast<uint32_t>(rect.GetSize().Height());
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "Convert rect to rosenRect, x is %{public}d, y is %{public}d, width is %{public}d, height is %{public}d",
        rosenRect.posX_, rosenRect.posY_, rosenRect.width_, rosenRect.height_);
}

void SubwindowOhos::ShowBindSheetNG(bool isShow, std::function<void(const std::string&)>&& callback,
    std::function<RefPtr<NG::UINode>()>&& buildNodeFunc, std::function<RefPtr<NG::UINode>()>&& buildtitleNodeFunc,
    NG::SheetStyle& sheetStyle, std::function<void()>&& onAppear, std::function<void()>&& onDisappear,
    std::function<void()>&& shouldDismiss, std::function<void(const int32_t)>&& onWillDismiss,
    std::function<void()>&& onWillAppear, std::function<void()>&& onWillDisappear,
    std::function<void(const float)>&& onHeightDidChange,
    std::function<void(const float)>&& onDetentsDidChange,
    std::function<void(const float)>&& onWidthDidChange,
    std::function<void(const float)>&& onTypeDidChange,
    std::function<void()>&& sheetSpringBack, const RefPtr<NG::FrameNode>& targetNode)
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ResizeWindow();
    ShowWindow();
    CHECK_NULL_VOID(window_);
    window_->SetFullScreen(true);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    overlay->OnBindSheet(isShow, std::move(callback), std::move(buildNodeFunc),
        std::move(buildtitleNodeFunc), sheetStyle, std::move(onAppear), std::move(onDisappear),
        std::move(shouldDismiss), std::move(onWillDismiss),
        std::move(onWillAppear), std::move(onWillDisappear), std::move(onHeightDidChange),
        std::move(onDetentsDidChange), std::move(onWidthDidChange), std::move(onTypeDidChange),
        std::move(sheetSpringBack), targetNode);
}

RefPtr<NG::FrameNode> SubwindowOhos::ShowDialogNG(
    const DialogProperties& dialogProps, std::function<void()>&& buildFunc)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show dialog ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(aceContainer, nullptr);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_RETURN(context, nullptr);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_RETURN(overlay, nullptr);
    auto parentAceContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_RETURN(parentAceContainer, nullptr);
    std::map<int32_t, RefPtr<NG::FrameNode>> DialogMap(overlay->GetDialogMap().begin(), overlay->GetDialogMap().end());
    int dialogMapSize = static_cast<int>(DialogMap.size());
    if (dialogMapSize == 0) {
        auto parentcontext = DynamicCast<NG::PipelineContext>(parentAceContainer->GetPipelineContext());
        CHECK_NULL_RETURN(parentcontext, nullptr);
        auto parentOverlay = parentcontext->GetOverlayManager();
        CHECK_NULL_RETURN(parentOverlay, nullptr);
        parentOverlay->SetSubWindowId(childContainerId_);
    }
    auto dialogTheme = context->GetTheme<DialogTheme>();
    CHECK_NULL_RETURN(dialogTheme, nullptr);
    if (dialogTheme->GetExpandDisplay() || parentAceContainer->IsFreeMultiWindow()) {
        SetFollowParentWindowLayoutEnabled(false);
        ResizeWindow();
    } else {
        SetFollowParentWindowLayoutEnabled(true);
    }
    ShowWindow(dialogProps.focusable);
    CHECK_NULL_RETURN(window_, nullptr);
    window_->SetFullScreen(true);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    auto dialog = overlay->ShowDialog(dialogProps, std::move(buildFunc));
    CHECK_NULL_RETURN(dialog, nullptr);
    if (parentAceContainer->IsUIExtensionWindow() && dialogProps.isModal) {
        window_->SetFollowParentWindowLayoutEnabled(true);
        SetNodeId(dialog->GetId());
        SubwindowManager::GetInstance()->AddSubwindow(
            parentContainerId_, SubwindowType::TYPE_DIALOG, AceType::Claim(this), dialog->GetId());
    }
    haveDialog_ = true;
    return dialog;
}

RefPtr<NG::FrameNode> SubwindowOhos::ShowDialogNGWithNode(
    const DialogProperties& dialogProps, const RefPtr<NG::UINode>& customNode)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show dialog ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(aceContainer, nullptr);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_RETURN(context, nullptr);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_RETURN(overlay, nullptr);
    auto parentAceContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_RETURN(parentAceContainer, nullptr);
    std::map<int32_t, RefPtr<NG::FrameNode>> DialogMap(overlay->GetDialogMap().begin(), overlay->GetDialogMap().end());
    int dialogMapSize = static_cast<int>(DialogMap.size());
    if (dialogMapSize == 0) {
        auto parentcontext = DynamicCast<NG::PipelineContext>(parentAceContainer->GetPipelineContext());
        CHECK_NULL_RETURN(parentcontext, nullptr);
        auto parentOverlay = parentcontext->GetOverlayManager();
        CHECK_NULL_RETURN(parentOverlay, nullptr);
        parentOverlay->SetSubWindowId(childContainerId_);
    }
    auto dialogTheme = context->GetTheme<DialogTheme>();
    CHECK_NULL_RETURN(dialogTheme, nullptr);
    if (dialogTheme->GetExpandDisplay() || parentAceContainer->IsFreeMultiWindow()) {
        SetFollowParentWindowLayoutEnabled(false);
        ResizeWindow();
    } else {
        SetFollowParentWindowLayoutEnabled(true);
    }
    ShowWindow(dialogProps.focusable);
    CHECK_NULL_RETURN(window_, nullptr);
    window_->SetFullScreen(true);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    auto dialog = overlay->ShowDialogWithNode(dialogProps, customNode);
    CHECK_NULL_RETURN(dialog, nullptr);
    if (parentAceContainer->IsUIExtensionWindow() && dialogProps.isModal) {
        window_->SetFollowParentWindowLayoutEnabled(true);
        SetNodeId(dialog->GetId());
        SubwindowManager::GetInstance()->AddSubwindow(
            parentContainerId_, SubwindowType::TYPE_DIALOG, AceType::Claim(this), dialog->GetId());
    }
    haveDialog_ = true;
    return dialog;
}

void SubwindowOhos::CloseDialogNG(const RefPtr<NG::FrameNode>& dialogNode)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "close dialog ng enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->CloseDialog(dialogNode);
}

void SubwindowOhos::OpenCustomDialogNG(const DialogProperties& dialogProps, std::function<void(int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "open customDialog ng subwindow enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    auto parentAceContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentAceContainer);
    std::map<int32_t, RefPtr<NG::FrameNode>> DialogMap(overlay->GetDialogMap().begin(), overlay->GetDialogMap().end());
    int dialogMapSize = static_cast<int>(DialogMap.size());
    if (dialogMapSize == 0) {
        auto parentcontext = DynamicCast<NG::PipelineContext>(parentAceContainer->GetPipelineContext());
        CHECK_NULL_VOID(parentcontext);
        auto parentOverlay = parentcontext->GetOverlayManager();
        CHECK_NULL_VOID(parentOverlay);
        parentOverlay->SetSubWindowId(childContainerId_);
        TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "overlay in parent container %{public}d, SetSubWindowId %{public}d",
            parentContainerId_, childContainerId_);
    }
    auto dialogTheme = context->GetTheme<DialogTheme>();
    CHECK_NULL_VOID(dialogTheme);
    if (dialogTheme->GetExpandDisplay() || parentAceContainer->IsFreeMultiWindow()) {
        SetFollowParentWindowLayoutEnabled(false);
        ResizeWindow();
    } else {
        SetFollowParentWindowLayoutEnabled(true);
    }
    ShowWindow(dialogProps.focusable);
    CHECK_NULL_VOID(window_);
    window_->SetFullScreen(true);
    window_->SetTouchable(true);
    ContainerScope scope(childContainerId_);
    auto dialog = overlay->OpenCustomDialog(dialogProps, std::move(callback));
    CHECK_NULL_VOID(dialog);
    if (parentAceContainer->IsUIExtensionWindow() && dialogProps.isModal) {
        window_->SetFollowParentWindowLayoutEnabled(true);
        SetNodeId(dialog->GetId());
        SubwindowManager::GetInstance()->AddSubwindow(
            parentContainerId_, SubwindowType::TYPE_DIALOG, AceType::Claim(this), dialog->GetId());
    }
    haveDialog_ = true;
}

void SubwindowOhos::CloseCustomDialogNG(int32_t dialogId)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "close customDialog ng subwindow enter, child container id %{public}d",
        childContainerId_);
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->CloseCustomDialog(dialogId);
}

void SubwindowOhos::CloseCustomDialogNG(const WeakPtr<NG::UINode>& node, std::function<void(int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "close customDialog ng subwindow enter, child container id %{public}d",
        childContainerId_);
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->CloseCustomDialog(node, std::move(callback));
}

void SubwindowOhos::UpdateCustomDialogNG(
    const WeakPtr<NG::UINode>& node, const DialogProperties& dialogProps, std::function<void(int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "update customDialog ng subwindow enter");
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->UpdateCustomDialog(node, dialogProps, std::move(callback));
}

void SubwindowOhos::HideSubWindowNG()
{
    ContainerScope scope(childContainerId_);
    auto container = Container::Current();
    CHECK_NULL_VOID(container);
    if (container->IsDialogContainer()) {
        if (IsToastWindow()) {
            Platform::AceContainer::HideWindow(Container::CurrentId());
        } else {
            Platform::AceContainer::CloseWindow(Container::CurrentId());
            Platform::AceContainer::DestroyContainer(Container::CurrentId());
        }
    } else {
        auto context = container->GetPipelineContext();
        if (context) {
            context->FlushPipelineImmediately();
        }
        HideWindow();
    }
}

void SubwindowOhos::GetToastDialogWindowProperty(
    int32_t& width, int32_t& height, int32_t& posX, int32_t& posY, float& density) const
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "get toast dialog window property enter");
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    auto subwindowOhos = AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
    if (subwindowOhos) {
        auto dialogWindow = subwindowOhos->GetDialogWindow();
        if (dialogWindow) {
            auto currentDisplay = Rosen::DisplayManager::GetInstance().GetDisplayById(dialogWindow->GetDisplayId());
            defaultDisplay = currentDisplay ? currentDisplay : defaultDisplay;
        }
    }

    if (defaultDisplay) {
        posX = 0;
        posY = 0;
        width = defaultDisplay->GetWidth();
        height = defaultDisplay->GetHeight();
        density = defaultDisplay->GetVirtualPixelRatio();
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "Toast posX: %{public}d, posY: %{public}d, width: %{public}d, height: %{public}d, density: %{public}f", posX,
        posY, width, height, density);
}

void SubwindowOhos::InitDialogWindowRSUIDirector(const RefPtr<Platform::AceContainer>& container)
{
#ifdef ENABLE_ROSEN_BACKEND
    if (!SystemProperties::GetMultiInstanceEnabled()) {
        rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
        if (rsUiDirector != nullptr) {
            rsUiDirector->SetRSSurfaceNode(dialogWindow_->GetSurfaceNode());
            auto context = DynamicCast<PipelineContext>(container->GetPipelineContext());
            if (context != nullptr) {
                context->SetRSUIDirector(rsUiDirector);
            }
            rsUiDirector->Init();
        }
    } else {
        rsUiDirector = dialogWindow_->GetRSUIDirector();
        if (!rsUiDirector) {
            rsUiDirector = OHOS::Rosen::RSUIDirector::Create();
        }
        rsUiDirector->SetRSSurfaceNode(dialogWindow_->GetSurfaceNode());
        auto context = DynamicCast<PipelineContext>(container->GetPipelineContext());
        if (context != nullptr) {
            context->SetRSUIDirector(rsUiDirector);
        }
        rsUiDirector->Init(true, true);
    }
#endif
}

bool SubwindowOhos::InitToastDialogWindow(int32_t& width, int32_t& height, int32_t posX, int32_t posY, bool isToast)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "init toast dialog window enter");
    OHOS::sptr<OHOS::Rosen::WindowOption> windowOption = new OHOS::Rosen::WindowOption();
    if (isToast) {
        auto windowType =
            IsSystemTopMost() ? Rosen::WindowType::WINDOW_TYPE_SYSTEM_TOAST : Rosen::WindowType::WINDOW_TYPE_TOAST;
        windowOption->SetWindowType(windowType);
    } else {
        windowOption->SetWindowType(Rosen::WindowType::WINDOW_TYPE_APP_MAIN_WINDOW);
    }
    windowOption->SetWindowRect({ posX, posY, static_cast<uint32_t>(width), static_cast<uint32_t>(height) });
    windowOption->SetWindowMode(Rosen::WindowMode::WINDOW_MODE_FULLSCREEN);
    windowOption->SetFocusable(!isToast);
    int32_t dialogId = gToastDialogId.fetch_add(1, std::memory_order_relaxed);
    std::string windowName = "ARK_APP_SUBWINDOW_TOAST_DIALOG_" + std::to_string(dialogId);
    if (isToast) {
        auto context = OHOS::AbilityRuntime::Context::GetApplicationContext();
        dialogWindow_ = OHOS::Rosen::Window::Create(windowName, windowOption, context);
    } else {
        dialogWindow_ = OHOS::Rosen::Window::Create(windowName, windowOption);
    }
    CHECK_NULL_RETURN(dialogWindow_, false);
    dialogWindow_->SetLayoutFullScreen(true);
    dialogWindow_->SetSubWindowSource(Rosen::SubWindowSource::SUB_WINDOW_SOURCE_ARKUI);
    auto focusWindowId = dialogWindow_->GetDisplayId();
    auto focusDisplayInfo = Rosen::DisplayManager::GetInstance().GetDisplayById(focusWindowId);
    CHECK_NULL_RETURN(focusDisplayInfo, false);
    width = focusDisplayInfo->GetWidth();
    height = focusDisplayInfo->GetHeight();
    return true;
}

bool SubwindowOhos::InitToastDialogView(int32_t width, int32_t height, float density)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "init toast dialog view enter");
#ifndef NG_BUILD
    dialogWindow_->NapiSetUIContent("", (napi_env)nullptr, (napi_value)nullptr, Rosen::BackupAndRestoreType::NONE);
    childContainerId_ = SubwindowManager::GetInstance()->GetContainerId(dialogWindow_->GetWindowId());
    SubwindowManager::GetInstance()->AddParentContainerId(childContainerId_, parentContainerId_);
    ContainerScope scope(childContainerId_);

    auto container = Platform::DialogContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(container, false);
    // create ace_view
    auto aceView = Platform::AceViewOhos::CreateView(childContainerId_, true, true);
    Platform::AceViewOhos::SurfaceCreated(aceView, dialogWindow_);
    // set view
    Platform::DialogContainer::SetView(aceView, density, width, height, dialogWindow_);
    Ace::Platform::DialogContainer::SetUIWindow(childContainerId_, dialogWindow_);
    ViewportConfig config(width, height, density);
    Platform::AceViewOhos::SetViewportMetrics(aceView, config);
    Platform::AceViewOhos::SurfaceChanged(aceView, width, height, 0);

#ifdef ENABLE_ROSEN_BACKEND
    if (SystemProperties::GetRosenBackendEnabled()) {
        InitDialogWindowRSUIDirector(container);
    }
#endif

    auto pipelineContext = DynamicCast<NG::PipelineContext>(container->GetPipelineContext());
    CHECK_NULL_RETURN(pipelineContext, false);
    pipelineContext->SetupSubRootElement();
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    if (parentContainer) {
        auto parentPipeline = parentContainer->GetPipelineContext();
        CHECK_NULL_RETURN(parentPipeline, false);
        pipelineContext->SetMinPlatformVersion(parentPipeline->GetMinPlatformVersion());
        pipelineContext->SetApiTargetVersion(parentPipeline->GetApiTargetVersion());
    } else {
        pipelineContext->SetMinPlatformVersion(PLATFORM_VERSION_TEN);
        pipelineContext->SetApiTargetVersion(container->GetApiTargetVersion());
    }
    return true;
#else
    return true;
#endif
}

bool SubwindowOhos::InitToastServiceConfig()
{
    auto context = OHOS::AbilityRuntime::Context::GetApplicationContext();
    CHECK_NULL_RETURN(context, false);
    auto config = context->GetConfiguration();
    CHECK_NULL_RETURN(config, false);
    auto maxAppFontScale = config->GetItem(OHOS::AAFwk::GlobalConfigurationKey::APP_FONT_MAX_SCALE);
    auto followSystem = config->GetItem(OHOS::AAFwk::GlobalConfigurationKey::APP_FONT_SIZE_SCALE);
    auto container = Platform::DialogContainer::GetContainer(childContainerId_);
    CHECK_NULL_RETURN(container, false);
    auto pipelineContext = container->GetPipelineContext();
    CHECK_NULL_RETURN(pipelineContext, false);
    auto isFollowSystem = followSystem == "followSystem";
    if (!followSystem.empty()) {
        pipelineContext->SetFollowSystem(isFollowSystem);
    }
    if (!maxAppFontScale.empty()) {
        pipelineContext->SetMaxAppFontScale(StringUtils::StringToFloat(maxAppFontScale));
    }
    if (!isFollowSystem) {
        pipelineContext->SetFontScale(1.0f);
    }
    auto fontScale = config->GetItem(OHOS::AAFwk::GlobalConfigurationKey::SYSTEM_FONT_SIZE_SCALE);
    if (!fontScale.empty()) {
        pipelineContext->SetFontScale(StringUtils::StringToFloat(fontScale));
    }

    return true;
}

bool SubwindowOhos::CreateEventRunner()
{
    std::lock_guard<std::mutex> lock(eventRunnerMutex_);
    if (!eventLoop_) {
        eventLoop_ = AppExecFwk::EventRunner::Create("Subwindow_Toast_Dialog");
        CHECK_NULL_RETURN(eventLoop_, false);
        handler_ = std::make_shared<AppExecFwk::EventHandler>(eventLoop_);
        CHECK_NULL_RETURN(handler_, false);
    }
    return true;
}

void SubwindowOhos::ClearToast()
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "clear toast enter");
    if (!IsToastWindow()) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "default toast needs not to be clear");
        return;
    }
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlayManager = context->GetOverlayManager();
    CHECK_NULL_VOID(overlayManager);
    ContainerScope scope(childContainerId_);
    overlayManager->ClearToast();
    context->FlushPipelineImmediately();
    HideWindow();
}

void SubwindowOhos::ShowToastForAbility(const NG::ToastInfo& toastInfo, std::function<void(int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show toast for ability enter, containerId : %{public}d", childContainerId_);
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentContainer);
    SetIsToastWindow(toastInfo.showMode == NG::ToastShowMode::TOP_MOST ||
                     toastInfo.showMode == NG::ToastShowMode::SYSTEM_TOP_MOST || parentContainer->IsSceneBoardWindow());
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "get container failed, child containerId : %{public}d", childContainerId_);
        return;
    }

    auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
    RefPtr<Framework::FrontendDelegate> delegate;
    if (!engine) {
        auto frontend = AceType::DynamicCast<DeclarativeFrontend>(aceContainer->GetFrontend());
        CHECK_NULL_VOID(frontend);
        delegate = frontend->GetDelegate();
        if (!delegate) {
            TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "get engine failed, containerId : %{public}d",
                aceContainer->GetInstanceId());
            return;
        }
    } else {
        delegate = engine->GetFrontend();
        if (!delegate) {
            TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "get frontend failed, child containerId : %{public}d",
                childContainerId_);
            return;
        }
    }
    ContainerScope scope(childContainerId_);
    if (parentContainer->IsSceneBoardWindow() || toastInfo.showMode == NG::ToastShowMode::TOP_MOST ||
        toastInfo.showMode == NG::ToastShowMode::SYSTEM_TOP_MOST) {
        ResizeWindow();
        ifNeedSetCurrentWindow_ = false;
        ShowWindow(false);
        CHECK_NULL_VOID(window_);
        window_->SetTouchable(false);
    }
    delegate->ShowToast(toastInfo, std::move(callback));
}

void SubwindowOhos::ShowToastForService(const NG::ToastInfo& toastInfo, std::function<void(int32_t)>&& callback)
{
    bool ret = CreateEventRunner();
    if (!ret) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create event runner failed");
        return;
    }

    SubwindowManager::GetInstance()->SetCurrentDialogSubwindow(AceType::Claim(this));
    if (!handler_->PostTask(GetInitToastDelayTask(toastInfo, std::move(callback)))) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create show dialog callback failed");
        return;
    }
}

void SubwindowOhos::ShowToast(const NG::ToastInfo& toastInfo, std::function<void(int32_t)>&& callback)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show toast, window parent id is %{public}d", parentContainerId_);
    auto isTopMost = toastInfo.showMode == NG::ToastShowMode::TOP_MOST;
    // for pa service
    if ((isTopMost && parentContainerId_ >= MIN_PA_SERVICE_ID && parentContainerId_ < MIN_SUBCONTAINER_ID) ||
        (!isTopMost && parentContainerId_ >= MIN_PA_SERVICE_ID) || parentContainerId_ < 0) {
        ShowToastForService(toastInfo, std::move(callback));
    } else {
        ShowToastForAbility(toastInfo, std::move(callback));
    }
}

void SubwindowOhos::CloseToast(const int32_t toastId, std::function<void(int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "close toast enter");
    if (parentContainerId_ >= MIN_PA_SERVICE_ID || parentContainerId_ < 0) {
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        auto childContainerId = subwindowOhos->GetChildContainerId();
        CHECK_NULL_VOID(childContainerId);
        ContainerScope scope(childContainerId);
        Platform::DialogContainer::CloseToast(childContainerId, toastId, std::move(callback));
    } else {
        auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
        CHECK_NULL_VOID(aceContainer);
        auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
        auto delegate = engine->GetFrontend();
        CHECK_NULL_VOID(delegate);
        ContainerScope scope(childContainerId_);
        delegate->CloseToast(toastId, std::move(callback));
    }
}

void SubwindowOhos::ShowDialogForAbility(const std::string& title, const std::string& message,
    const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
    const std::set<std::string>& callbacks)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show dialog for ability enter");
    SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));

    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get container failed, child containerId : %{public}d", childContainerId_);
        return;
    }

    auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
    auto delegate = engine->GetFrontend();
    if (!delegate) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get frontend failed, child containerId : %{public}d", childContainerId_);
        return;
    }
    delegate->ShowDialog(title, message, buttons, autoCancel, std::move(callback), callbacks);
}

void SubwindowOhos::ShowDialogForService(const std::string& title, const std::string& message,
    const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
    const std::set<std::string>& callbacks)
{
    bool ret = CreateEventRunner();
    if (!ret) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create event runner failed");
        return;
    }

    SubwindowManager::GetInstance()->SetCurrentDialogSubwindow(AceType::Claim(this));
    auto showDialogCallback = [title, message, &buttons, autoCancel, callbackParam = std::move(callback),
                                  &callbacks]() {
        int32_t posX = 0;
        int32_t posY = 0;
        int32_t width = 0;
        int32_t height = 0;
        float density = 1.0f;
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        subwindowOhos->GetToastDialogWindowProperty(width, height, posX, posY, density);
        bool ret = subwindowOhos->InitToastDialogWindow(width, height, posX, posY);
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog window failed");
            return;
        }
        ret = subwindowOhos->InitToastDialogView(width, height, density);
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog view failed");
            return;
        }
        auto childContainerId = subwindowOhos->GetChildContainerId();
        ContainerScope scope(childContainerId);
        auto container = Platform::AceContainer::GetContainer(childContainerId);
        CHECK_NULL_VOID(container);
        container->SetFontScaleAndWeightScale(childContainerId);
        Platform::DialogContainer::ShowToastDialogWindow(childContainerId, posX, posY, width, height);
        Platform::DialogContainer::ShowDialog(childContainerId, title, message, buttons, autoCancel,
            std::move(const_cast<std::function<void(int32_t, int32_t)>&&>(callbackParam)), callbacks);
    };
    isToastWindow_ = false;
    if (!handler_->PostTask(showDialogCallback)) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create show dialog callback failed");
        return;
    }
}

void SubwindowOhos::ShowDialogForAbility(const PromptDialogAttr& dialogAttr, const std::vector<ButtonInfo>& buttons,
    std::function<void(int32_t, int32_t)>&& callback, const std::set<std::string>& callbacks)
{
    SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));

    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get ace container failed");
        return;
    }

    auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
    auto delegate = engine->GetFrontend();
    if (!delegate) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get frontend failed");
        return;
    }
    delegate->ShowDialog(dialogAttr, buttons, std::move(callback), callbacks);
}

void SubwindowOhos::ShowDialogForService(const PromptDialogAttr& dialogAttr, const std::vector<ButtonInfo>& buttons,
    std::function<void(int32_t, int32_t)>&& callback, const std::set<std::string>& callbacks)
{
    bool ret = CreateEventRunner();
    if (!ret) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create event runner failed");
        return;
    }

    SubwindowManager::GetInstance()->SetCurrentDialogSubwindow(AceType::Claim(this));
    auto showDialogCallback = [dialogAttr, &buttons, callbackParam = std::move(callback), &callbacks]() {
        int32_t posX = 0;
        int32_t posY = 0;
        int32_t width = 0;
        int32_t height = 0;
        float density = 1.0f;
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        subwindowOhos->GetToastDialogWindowProperty(width, height, posX, posY, density);
        bool ret = subwindowOhos->InitToastDialogWindow(width, height, posX, posY);
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog window failed");
            return;
        }
        ret = subwindowOhos->InitToastDialogView(width, height, density);
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog view failed");
            return;
        }
        auto childContainerId = subwindowOhos->GetChildContainerId();
        ContainerScope scope(childContainerId);
        Platform::DialogContainer::ShowToastDialogWindow(childContainerId, posX, posY, width, height);
        Platform::DialogContainer::ShowDialog(childContainerId, dialogAttr, buttons,
            std::move(const_cast<std::function<void(int32_t, int32_t)>&&>(callbackParam)), callbacks);
    };
    isToastWindow_ = false;
    if (!handler_->PostTask(showDialogCallback)) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create show dialog callback failed");
        return;
    }
}

void SubwindowOhos::ShowDialog(const std::string& title, const std::string& message,
    const std::vector<ButtonInfo>& buttons, bool autoCancel, std::function<void(int32_t, int32_t)>&& callback,
    const std::set<std::string>& callbacks)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show dialog, window parent id is %{public}d", parentContainerId_);
    if (parentContainerId_ >= MIN_PA_SERVICE_ID || parentContainerId_ < 0) {
        ShowDialogForService(title, message, buttons, autoCancel, std::move(callback), callbacks);
    } else {
        ShowDialogForAbility(title, message, buttons, autoCancel, std::move(callback), callbacks);
    }
}

void SubwindowOhos::ShowDialog(const PromptDialogAttr& dialogAttr, const std::vector<ButtonInfo>& buttons,
    std::function<void(int32_t, int32_t)>&& callback, const std::set<std::string>& callbacks)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show dialog with attr, window parent id is %{public}d", parentContainerId_);
    if (parentContainerId_ >= MIN_PA_SERVICE_ID || parentContainerId_ < 0) {
        ShowDialogForService(dialogAttr, buttons, std::move(callback), callbacks);
    } else {
        ShowDialogForAbility(dialogAttr, buttons, std::move(callback), callbacks);
    }
}

void SubwindowOhos::OpenCustomDialogForAbility(
    const PromptDialogAttr& dialogAttr, std::function<void(int32_t)>&& callback)
{
    SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));

    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGW(
            AceLogTag::ACE_SUB_WINDOW, "open dialog fail, the container %{public}d can not find", childContainerId_);
        return;
    }

    auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
    auto delegate = engine->GetFrontend();
    if (!delegate) {
        return;
    }
    delegate->OpenCustomDialog(dialogAttr, std::move(callback));
}

void SubwindowOhos::OpenCustomDialogForService(
    const PromptDialogAttr& dialogAttr, std::function<void(int32_t)>&& callback)
{
    // temporary not support
    TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "temporary not support for service by promptAction with CustomBuilder");
}

void SubwindowOhos::OpenCustomDialog(const PromptDialogAttr& dialogAttr, std::function<void(int32_t)>&& callback)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "open custom dialog, window parent id is %{public}d", parentContainerId_);
    if (parentContainerId_ >= MIN_PA_SERVICE_ID || parentContainerId_ < 0) {
        OpenCustomDialogForService(dialogAttr, std::move(callback));
    } else {
        OpenCustomDialogForAbility(dialogAttr, std::move(callback));
    }
}

void SubwindowOhos::CloseCustomDialog(const int32_t dialogId)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "close custom dialog with id, childContainerId_ is %{public}d",
        childContainerId_);
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->CloseCustomDialog(dialogId);
}

void SubwindowOhos::CloseCustomDialog(const WeakPtr<NG::UINode>& node, std::function<void(int32_t)>&& callback)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "close custom dialog with node, childContainerId_ is %{public}d",
        childContainerId_);
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto overlay = context->GetOverlayManager();
    CHECK_NULL_VOID(overlay);
    ContainerScope scope(childContainerId_);
    return overlay->CloseCustomDialog(node, std::move(callback));
}

void SubwindowOhos::ShowActionMenuForAbility(
    const std::string& title, const std::vector<ButtonInfo>& button, std::function<void(int32_t, int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show action menu for ability enter");
    SubwindowManager::GetInstance()->SetCurrentSubwindow(AceType::Claim(this));

    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    if (!aceContainer) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get container failed");
        return;
    }

    auto engine = EngineHelper::GetEngine(aceContainer->GetInstanceId());
    auto delegate = engine->GetFrontend();
    if (!delegate) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "get frontend failed");
        return;
    }
    delegate->ShowActionMenu(title, button, std::move(callback));
}

void SubwindowOhos::ShowActionMenuForService(
    const std::string& title, const std::vector<ButtonInfo>& button, std::function<void(int32_t, int32_t)>&& callback)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "show action menu for service enter");
    bool ret = CreateEventRunner();
    if (!ret) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create event runner failed");
        return;
    }

    SubwindowManager::GetInstance()->SetCurrentDialogSubwindow(AceType::Claim(this));
    auto showDialogCallback = [title, &button, callbackParam = std::move(callback)]() {
        int32_t posX = 0;
        int32_t posY = 0;
        int32_t width = 0;
        int32_t height = 0;
        float density = 1.0f;
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        subwindowOhos->GetToastDialogWindowProperty(width, height, posX, posY, density);
        bool ret = subwindowOhos->InitToastDialogWindow(width, height, posX, posY);
        if (!ret) {
            return;
        }
        ret = subwindowOhos->InitToastDialogView(width, height, density);
        if (!ret) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "init toast dialog view failed");
            return;
        }
        auto childContainerId = subwindowOhos->GetChildContainerId();
        ContainerScope scope(childContainerId);
        auto container = Platform::AceContainer::GetContainer(childContainerId);
        CHECK_NULL_VOID(container);
        container->SetFontScaleAndWeightScale(childContainerId);
        Platform::DialogContainer::ShowToastDialogWindow(childContainerId, posX, posY, width, height);
        Platform::DialogContainer::ShowActionMenu(childContainerId, title, button,
            std::move(const_cast<std::function<void(int32_t, int32_t)>&&>(callbackParam)));
    };
    isToastWindow_ = false;
    if (!handler_->PostTask(showDialogCallback)) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "create show dialog callback failed");
        return;
    }
}

void SubwindowOhos::CloseDialog(int32_t instanceId)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "close dialog enter");
    Platform::DialogContainer::CloseWindow(instanceId);
}

void SubwindowOhos::UpdateAceView(int32_t width, int32_t height, float density, int32_t containerId)
{
    auto container = Platform::DialogContainer::GetContainer(containerId);
    CHECK_NULL_VOID(container);
    auto aceView = AceType::DynamicCast<Platform::AceViewOhos>(container->GetAceView());
    CHECK_NULL_VOID(aceView);
    if (aceView->GetWidth() != width || aceView->GetHeight() != height) {
        ViewportConfig config(width, height, density);
        Platform::AceViewOhos::SetViewportMetrics(aceView, config);
        Platform::AceViewOhos::SurfaceChanged(aceView, width, height, 0);
    }
}

void SubwindowOhos::ShowActionMenu(
    const std::string& title, const std::vector<ButtonInfo>& button, std::function<void(int32_t, int32_t)>&& callback)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "show custom dialog, window parent id is %{public}d", parentContainerId_);
    if (parentContainerId_ >= MIN_PA_SERVICE_ID || parentContainerId_ < 0) {
        ShowActionMenuForService(title, button, std::move(callback));
    } else {
        ShowActionMenuForAbility(title, button, std::move(callback));
    }
}

Rect SubwindowOhos::GetParentWindowRect() const
{
    Rect rect;
    CHECK_NULL_RETURN(parentWindow_, rect);
    auto parentWindowRect = parentWindow_->GetRect();
    return Rect(parentWindowRect.posX_, parentWindowRect.posY_, parentWindowRect.width_, parentWindowRect.height_);
}

Rect SubwindowOhos::GetUIExtensionHostWindowRect() const
{
    Rect rect;
    CHECK_NULL_RETURN(parentWindow_, rect);
    auto id = GetUIExtensionHostWindowId();
    auto hostWindowRect = parentWindow_->GetHostWindowRect(id);
    return Rect(hostWindowRect.posX_, hostWindowRect.posY_, hostWindowRect.width_, hostWindowRect.height_);
}

Rect SubwindowOhos::GetFoldExpandAvailableRect() const
{
    CHECK_NULL_RETURN(window_, Rect());
    Rosen::DMRect rect;
    Rosen::DMError ret = Rosen::DisplayManager::GetInstance().GetExpandAvailableArea(DEFAULT_DISPLAY_ID, rect);
    if (ret != Rosen::DMError::DM_OK) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "failed to get expandAvailableArea");
        return Rect();
    }

    return Rect(rect.posX_, rect.posY_, rect.width_, rect.height_);
}

NG::RectF SubwindowOhos::GetWindowRect() const
{
    NG::RectF rect;
    CHECK_NULL_RETURN(window_, rect);
    rect.SetRect(window_->GetRect().posX_, window_->GetRect().posY_,
        window_->GetRect().width_, window_->GetRect().height_);
    return rect;
}

void SubwindowOhos::RequestFocus()
{
    CHECK_NULL_VOID(window_);
    if (window_->IsFocused()) {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "subwindow id:%{public}u already focused", window_->GetWindowId());
        // already focused, no need to focus
        return;
    }
    OHOS::Rosen::WMError ret = window_->RequestFocus();
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "subindow id:%{public}u request focus failed with WMError: %{public}d",
            window_->GetWindowId(), static_cast<int32_t>(ret));
        return;
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "subwindow id:%{public}u request focus successfully.", window_->GetWindowId());
}

bool SubwindowOhos::IsFocused()
{
    CHECK_NULL_RETURN(window_, false);
    return window_->IsFocused();
}

void SubwindowOhos::HidePixelMap(bool startDrag, double x, double y, bool showAnimation)
{
    auto parentAceContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentAceContainer);
    auto parentPipeline = DynamicCast<NG::PipelineContext>(parentAceContainer->GetPipelineContext());
    CHECK_NULL_VOID(parentPipeline);
    auto manager = parentPipeline->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    ContainerScope scope(parentContainerId_);
    if (!startDrag) {
        manager->RemovePreviewBadgeNode();
        manager->RemoveGatherNodeWithAnimation();
    }
    if (showAnimation) {
        manager->RemovePixelMapAnimation(startDrag, x, y, true);
    } else {
        manager->RemovePixelMap();
    }
}

void SubwindowOhos::HideEventColumn()
{
    auto parentAceContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentAceContainer);
    auto parentPipeline = DynamicCast<NG::PipelineContext>(parentAceContainer->GetPipelineContext());
    CHECK_NULL_VOID(parentPipeline);
    auto manager = parentPipeline->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    ContainerScope scope(parentContainerId_);
    manager->RemoveEventColumn();
}

void SubwindowOhos::ResizeWindowForFoldStatus(int32_t parentContainerId)
{
    auto callback = []() {
        auto subwindowOhos =
            AceType::DynamicCast<SubwindowOhos>(SubwindowManager::GetInstance()->GetCurrentDialogWindow());
        CHECK_NULL_VOID(subwindowOhos);
        auto childContainerId = subwindowOhos->GetChildContainerId();
        ContainerScope scope(childContainerId);
        auto window = Platform::DialogContainer::GetUIWindow(childContainerId);
        CHECK_NULL_VOID(window);
        auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplaySync();
        CHECK_NULL_VOID(defaultDisplay);
        auto ret = window->Resize(defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
        if (ret != Rosen::WMError::WM_OK) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Resize window by default display failed with errCode: %{public}d",
                static_cast<int32_t>(ret));
            return;
        }
        auto container = Platform::DialogContainer::GetContainer(childContainerId);
        CHECK_NULL_VOID(container);
        // get ace_view
        auto aceView = AceType::DynamicCast<Platform::AceViewOhos>(container->GetAceView());
        CHECK_NULL_VOID(aceView);
        Platform::AceViewOhos::SurfaceChanged(aceView, defaultDisplay->GetWidth(), defaultDisplay->GetHeight(), 0);
    };
    if (parentContainerId > 0 && parentContainerId < MIN_PA_SERVICE_ID) {
        ResizeWindowForFoldStatus();
        return;
    }
    if (!handler_->PostTask(callback)) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Resize Toast Window failed");
    }
}

void SubwindowOhos::ResizeWindowForFoldStatus()
{
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplaySync();
    CHECK_NULL_VOID(defaultDisplay);
    CHECK_NULL_VOID(window_);
    auto ret = window_->Resize(defaultDisplay->GetWidth(), defaultDisplay->GetHeight());
    if (ret != Rosen::WMError::WM_OK) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Resize window by default display failed with errCode: %{public}d",
            static_cast<int32_t>(ret));
        return;
    }
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "foldStatus is changed, subwindowOhos is resized to [%{public}d, %{public}d, %{public}u, %{public}u]",
        window_->GetRect().posX_, window_->GetRect().posY_, window_->GetRect().width_, window_->GetRect().height_);
}

void SubwindowOhos::MarkDirtyDialogSafeArea()
{
    auto aceContainer = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(aceContainer);
    auto context = DynamicCast<NG::PipelineContext>(aceContainer->GetPipelineContext());
    CHECK_NULL_VOID(context);
    auto rootNode = context->GetRootElement();
    CHECK_NULL_VOID(rootNode);
    auto lastChild = rootNode->GetLastChild();
    CHECK_NULL_VOID(lastChild);
    lastChild->MarkDirtyNode(NG::PROPERTY_UPDATE_MEASURE);
}

bool SubwindowOhos::CheckHostWindowStatus() const
{
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_RETURN(parentContainer, false);
    sptr<OHOS::Rosen::Window> parentWindow = parentContainer->GetUIWindow(parentContainerId_);
    CHECK_NULL_RETURN(parentWindow, false);
    if (parentWindow->GetType() == Rosen::WindowType::WINDOW_TYPE_UI_EXTENSION) {
        auto parentPipeline = parentContainer->GetPipelineContext();
        CHECK_NULL_RETURN(parentPipeline, false);
        auto hostWindowId = parentPipeline->GetFocusWindowId();
        auto hostWindowRect = parentWindow->GetHostWindowRect(hostWindowId);
        auto isValid = GreatNotEqual(hostWindowRect.width_, 0) && GreatNotEqual(hostWindowRect.height_, 0);
        if (!isValid) {
            TAG_LOGW(AceLogTag::ACE_SUB_WINDOW,
                "UIExtension Window failed to obtain host window information. Please check if permissions are enabled");
            return false;
        }
    }
    return true;
}

bool SubwindowOhos::Close()
{
    // prevent repeated closure when subWindow's container is destroying
    if (isClosing_) {
        TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "current window is closing.");
        return false;
    }

    CHECK_NULL_RETURN(window_, false);
    window_->UnregisterSwitchFreeMultiWindowListener(freeMultiWindowListener_);
    isClosing_ = true;
    OHOS::Rosen::WMError ret = window_->Destroy();
    isClosing_ = false;
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "SubwindowOhos failed to close the dialog subwindow.");
        return false;
    }
    sptr<OHOS::Rosen::Window> uiWindow = nullptr;
    Ace::Platform::DialogContainer::SetUIWindow(childContainerId_, uiWindow);
    return true;
}

bool SubwindowOhos::IsFreeMultiWindow() const
{
    if (!parentWindow_) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW, "Window is null, failed to get freeMultiWindow status");
        return false;
    }

    return parentWindow_->GetFreeMultiWindowModeEnabledState();
}

void SubwindowOhos::OnFreeMultiWindowSwitch(bool enable)
{
    for (auto&& [id, callback] : freeMultiWindowSwitchCallbackMap_) {
        if (callback) {
            callback(enable);
        }
    }
}

int32_t SubwindowOhos::RegisterFreeMultiWindowSwitchCallback(std::function<void(bool)>&& callback)
{
    if (callback) {
        freeMultiWindowSwitchCallbackMap_.emplace(++callbackId_, std::move(callback));
        return callbackId_;
    }
    return 0;
}

void SubwindowOhos::UnRegisterFreeMultiWindowSwitchCallback(int32_t callbackId)
{
    freeMultiWindowSwitchCallbackMap_.erase(callbackId);
}

bool SubwindowOhos::IsToastSubWindow()
{
    CHECK_NULL_RETURN(window_, false);
    auto windowType = window_->GetType();
    return windowType == Rosen::WindowType::WINDOW_TYPE_SYSTEM_TOAST ||
           windowType == Rosen::WindowType::WINDOW_TYPE_TOAST;
}

void SubwindowOhos::DestroyWindow()
{
    CHECK_NULL_VOID(window_);
    OHOS::Rosen::WMError ret = window_->Destroy();
    if (ret != OHOS::Rosen::WMError::WM_OK) {
        TAG_LOGE(AceLogTag::ACE_SUB_WINDOW, "SubwindowOhos failed to destroy the dialog subwindow.");
        return;
    }
}

uint64_t SubwindowOhos::GetDisplayId()
{
    if (window_) {
        return window_->GetDisplayId();
    }
    return 0;
}

bool SubwindowOhos::IsSameDisplayWithParentWindow(bool useInitializedId)
{
    CHECK_NULL_RETURN(window_, false);
    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_RETURN(parentContainer, false);
    auto parentWindow = parentContainer->GetUIWindow(parentContainerId_);
    CHECK_NULL_RETURN(parentWindow, false);
    auto displayId = useInitializedId ? defaultDisplayId_ : window_->GetDisplayId();
    return displayId == parentWindow->GetDisplayId();
}

void SubwindowOhos::InitializeSafeArea()
{
    CHECK_NULL_VOID(window_);

    auto container = Platform::AceContainer::GetContainer(childContainerId_);
    CHECK_NULL_VOID(container);
    auto pipeline = DynamicCast<NG::PipelineContext>(container->GetPipelineContext());
    CHECK_NULL_VOID(pipeline);
    auto theme = pipeline->GetTheme<SelectTheme>();
    CHECK_NULL_VOID(theme);

    auto parentContainer = Platform::AceContainer::GetContainer(parentContainerId_);
    CHECK_NULL_VOID(parentContainer);

    std::optional<NG::RectF> windowRect;
    if (theme->GetExpandDisplay() || parentContainer->IsFreeMultiWindow()) {
        auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDisplayById(window_->GetDisplayId());
        CHECK_NULL_VOID(defaultDisplay);
        windowRect = { 0.0, 0.0, static_cast<float>(defaultDisplay->GetWidth()),
            static_cast<float>(defaultDisplay->GetHeight()) };
    }

    auto systemSafeArea = container->GetViewSafeAreaByType(Rosen::AvoidAreaType::TYPE_SYSTEM, windowRect);
    auto navSafeArea = container->GetViewSafeAreaByType(Rosen::AvoidAreaType::TYPE_NAVIGATION_INDICATOR, windowRect);
    pipeline->UpdateSystemSafeArea(systemSafeArea);
    pipeline->UpdateNavSafeArea(navSafeArea);
    auto cutoutSafeArea = container->GetViewSafeAreaByType(Rosen::AvoidAreaType::TYPE_CUTOUT, windowRect);
    pipeline->UpdateCutoutSafeArea(cutoutSafeArea);

    auto safeAreaInsets = pipeline->GetSafeAreaWithoutProcess();
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW, "initializeSafeArea by windowRect: %{public}s, safeAreaInsets: %{public}s",
        windowRect.value_or(NG::RectF()).ToString().c_str(), safeAreaInsets.ToString().c_str());
}

bool SubwindowOhos::ShowSelectOverlay(const RefPtr<NG::FrameNode>& overlayNode)
{
    TAG_LOGD(AceLogTag::ACE_SUB_WINDOW, "Show selectOverlay enter");
    ContainerScope scope(childContainerId_);
    CHECK_NULL_RETURN(window_, false);
    ResizeWindow();
    ShowWindow(false);
    if (!isShowed_) {
        TAG_LOGW(AceLogTag::ACE_SUB_WINDOW,
            "Show selectOverlay subwindow failed, subwindow not show after showWindow called.");
        return false;
    }
    auto context = NG::PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_RETURN(context, false);
    auto rootNode = context->GetRootElement();
    CHECK_NULL_RETURN(rootNode, false);
    CHECK_NULL_RETURN(overlayNode, false);
    overlayNode->MountToParent(rootNode);
    rootNode->MarkDirtyNode(NG::PROPERTY_UPDATE_MEASURE_SELF_AND_CHILD);
    window_->SetTouchable(true);
    window_->KeepKeyboardOnFocus(true);
    window_->SetFocusable(false);
    return true;
}

void SubwindowOhos::SwitchFollowParentWindowLayout(bool freeMultiWindowEnable)
{
    TAG_LOGI(AceLogTag::ACE_SUB_WINDOW,
        "subwindow switch followParentWindowLayout, enable: %{public}d", freeMultiWindowEnable);
    auto expandDisplay = SubwindowManager::GetInstance()->GetIsExpandDisplay();
    if (NeedFollowParentWindowLayout() && !expandDisplay && !freeMultiWindowEnable) {
        SetFollowParentWindowLayoutEnabled(true);
    } else {
        SetFollowParentWindowLayoutEnabled(false);
        ResizeWindow();
    }
}

void SubwindowOhos::AddFollowParentWindowLayoutNode(int32_t nodeId)
{
    followParentWindowLayoutNodeIds_.emplace_back(nodeId);
    if (followParentWindowLayoutNodeIds_.size() == 1) {
        bool freeMultiWindowEnable = IsFreeMultiWindow();
        SwitchFollowParentWindowLayout(freeMultiWindowEnable);
    }
}

void SubwindowOhos::RemoveFollowParentWindowLayoutNode(int32_t nodeId)
{
    followParentWindowLayoutNodeIds_.remove(nodeId);
    if (followParentWindowLayoutNodeIds_.empty()) {
        bool freeMultiWindowEnable = IsFreeMultiWindow();
        SwitchFollowParentWindowLayout(freeMultiWindowEnable);
    }
}
} // namespace OHOS::Ace
