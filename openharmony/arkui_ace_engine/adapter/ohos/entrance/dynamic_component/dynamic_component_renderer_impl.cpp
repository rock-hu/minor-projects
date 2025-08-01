/*
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "dynamic_component_renderer_impl.h"

#include "accessibility_element_info.h"

#include "adapter/ohos/entrance/dynamic_component/uv_task_wrapper_impl.h"
#include "adapter/ohos/entrance/ui_content_impl.h"
#include "bridge/card_frontend/form_frontend_declarative.h"
#include "core/components_ng/pattern/stage/page_pattern.h"
#include "core/components_ng/pattern/ui_extension/dynamic_component/dynamic_pattern.h"
#include "core/components_ng/pattern/ui_extension/isolated_component/isolated_pattern.h"
#include "core/components_ng/pattern/window_scene/helper/window_scene_helper.h"

namespace OHOS::Ace::NG {
namespace {
constexpr int32_t INVALID_WINDOW_ID = -1;
constexpr int32_t WORKER_ERROR = 10002;
constexpr size_t WORKER_MAX_NUM = 1;
constexpr int32_t WORKER_SIZE_ONE = 1;
constexpr int32_t DC_MAX_NUM_IN_WORKER = 4;
}

void ApplyAccessibilityElementInfoOffset(Accessibility::AccessibilityElementInfo& output, const OffsetF& offset)
{
    auto& rect = output.GetRectInScreen();
    Accessibility::Rect bounds;
    bounds.SetLeftTopScreenPostion(rect.GetLeftTopXScreenPostion() + offset.GetX(),
        rect.GetLeftTopYScreenPostion() + offset.GetY());
    bounds.SetRightBottomScreenPostion(rect.GetRightBottomXScreenPostion() + offset.GetX(),
        rect.GetRightBottomYScreenPostion() + offset.GetY());
    output.SetRectInScreen(bounds);
}

void ApplyAccessibilityElementInfoOffset(std::list<Accessibility::AccessibilityElementInfo>& output, size_t index,
    const OffsetF& offset)
{
    auto iterator = output.begin();
    if (index > 0) {
        std::advance(iterator, index);
    }
    for (; iterator != output.end(); ++iterator) {
        ApplyAccessibilityElementInfoOffset(*iterator, offset);
    }
}

std::map<void *, int32_t> DynamicComponentRendererImpl::usingWorkers_;
std::mutex DynamicComponentRendererImpl::usingWorkerMutex_;

DynamicComponentRendererImpl::DynamicComponentRendererImpl(
    const RefPtr<FrameNode>& host, void* runtime, const IsolatedInfo& isolatedInfo)
    : isolatedInfo_(isolatedInfo)
{
    host_ = WeakPtr(host);
    runtime_ = reinterpret_cast<NativeEngine*>(runtime);
}

RefPtr<DynamicComponentRenderer> DynamicComponentRenderer::Create(
    const RefPtr<FrameNode>& host, void* runtime, const IsolatedInfo& isolatedInfo)
{
    return AceType::MakeRefPtr<DynamicComponentRendererImpl>(host, runtime, isolatedInfo);
}

void DynamicComponentRendererImpl::SetAdaptiveSize(bool adaptiveWidth, bool adaptiveHeight)
{
    adaptiveWidth_ = adaptiveWidth;
    adaptiveHeight_ = adaptiveHeight;
}

void DynamicComponentRendererImpl::SetBackgroundTransparent(bool backgroundTransparent)
{
    backgroundTransparent_ = backgroundTransparent;
}

bool DynamicComponentRendererImpl::GetBackgroundTransparent() const
{
    return backgroundTransparent_;
}

void DynamicComponentRendererImpl::SetUIContentType(UIContentType uIContentType)
{
    uIContentType_ = uIContentType;
    if (uIContentType_ == UIContentType::ISOLATED_COMPONENT) {
        aceLogTag_ = AceLogTag::ACE_ISOLATED_COMPONENT;
    } else if (uIContentType_ == UIContentType::DYNAMIC_COMPONENT) {
        aceLogTag_ = AceLogTag::ACE_DYNAMIC_COMPONENT;
    }
}

bool DynamicComponentRendererImpl::IsRestrictedWorkerThread()
{
    if (!runtime_) {
        return false;
    }

    return runtime_->IsRestrictedWorkerThread();
}

bool DynamicComponentRendererImpl::CheckWorkerMaxConstraint(void *worker)
{
    if (worker == nullptr) {
        return true;
    }

    std::lock_guard<std::mutex> lock(usingWorkerMutex_);
    auto iter = usingWorkers_.find(worker);
    if (iter == usingWorkers_.end()) {
        return usingWorkers_.size() < WORKER_MAX_NUM;
    }

    return usingWorkers_.size() < WORKER_MAX_NUM + 1;
}

bool DynamicComponentRendererImpl::CheckDCMaxConstraintInWorker(void *worker)
{
    if (worker == nullptr) {
        return true;
    }

    std::lock_guard<std::mutex> lock(usingWorkerMutex_);
    auto iter = usingWorkers_.find(worker);
    if (iter == usingWorkers_.end()) {
        return true;
    }

    return iter->second < DC_MAX_NUM_IN_WORKER;
}

void DynamicComponentRendererImpl::AddWorkerUsing(void *worker)
{
    if (worker == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(usingWorkerMutex_);
    auto iter = usingWorkers_.find(worker);
    if (iter == usingWorkers_.end()) {
        usingWorkers_[worker] = WORKER_SIZE_ONE;
        return;
    }

    iter->second++;
}

void DynamicComponentRendererImpl::DeleteWorkerUsing(void *worker)
{
    if (worker == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(usingWorkerMutex_);
    auto iter = usingWorkers_.find(worker);
    if (iter == usingWorkers_.end()) {
        return;
    }

    if (iter->second <= WORKER_SIZE_ONE) {
        usingWorkers_.erase(iter);
        return;
    }

    iter->second--;
}

void DynamicComponentRendererImpl::CreateContent()
{
    hostInstanceId_ = Container::CurrentId();
    auto container = Platform::AceContainer::GetContainer(hostInstanceId_);
    CHECK_NULL_VOID(container);
    if (uIContentType_ == UIContentType::DYNAMIC_COMPONENT) {
        AddWorkerUsing(runtime_);
        CreateDynamicContent();
        return;
    }

    CreateIsolatedContent();
}

void DynamicComponentRendererImpl::CreateIsolatedContent()
{
    TAG_LOGI(aceLogTag_, "CreateIsolatedContent");
    CHECK_NULL_VOID(runtime_);
    auto napiEnv = reinterpret_cast<napi_env>(runtime_);
    CHECK_NULL_VOID(napiEnv);
    auto uvTaskWrapper = std::make_shared<UVTaskWrapperImpl>(napiEnv);
    uvTaskWrapper->Call([weak = WeakClaim(this)]() {
        auto renderer = weak.Upgrade();
        CHECK_NULL_VOID(renderer);
        renderer->InitUiContent(nullptr);
    });
}

void DynamicComponentRendererImpl::CreateDynamicContent()
{
    TAG_LOGI(aceLogTag_, "CreateDynamicContent");
    auto container = Platform::AceContainer::GetContainer(hostInstanceId_);
    CHECK_NULL_VOID(container);
    auto hostAbilityContext = container->GetAbilityContext();
    hostJsContext_ = container->GetJsContext();
    CHECK_NULL_VOID(runtime_);
    auto napiEnv = reinterpret_cast<napi_env>(runtime_);
    CHECK_NULL_VOID(napiEnv);
    auto uvTaskWrapper = std::make_shared<UVTaskWrapperImpl>(napiEnv);
    uvTaskWrapper->Call([weak = WeakClaim(this), hostAbilityContext]() {
        auto renderer = weak.Upgrade();
        CHECK_NULL_VOID(renderer);
        renderer->InitUiContent(hostAbilityContext.get());
    });
}

RefPtr<SystemWindowScene> DynamicComponentRendererImpl::GetWindowScene()
{
    RefPtr<SystemWindowScene> hostPattern = nullptr;
    auto host = host_.Upgrade();
    CHECK_NULL_RETURN(host, hostPattern);
    auto hostWindowNode = WindowSceneHelper::FindWindowScene(host);
    CHECK_NULL_RETURN(hostWindowNode, hostPattern);
    auto hostNode = AceType::DynamicCast<FrameNode>(hostWindowNode);
    CHECK_NULL_RETURN(hostNode, hostPattern);
    hostPattern = hostNode->GetPattern<SystemWindowScene>();
    return hostPattern;
}

int32_t DynamicComponentRendererImpl::GetWindowSceneId()
{
    auto hostPattern = GetWindowScene();
    CHECK_NULL_RETURN(hostPattern, INVALID_WINDOW_ID);
    auto hostSession = hostPattern->GetSession();
    CHECK_NULL_RETURN(hostSession, INVALID_WINDOW_ID);
    return hostSession->GetPersistentId();
}

void DynamicComponentRendererImpl::BuildDynamicInitialConfig(
    DynamicInitialConfig& dynamicInitialConfig)
{
    dynamicInitialConfig.hostInstanceId = hostInstanceId_;
    dynamicInitialConfig.abcPath = isolatedInfo_.abcPath;
    dynamicInitialConfig.hapPath = isolatedInfo_.resourcePath;
    dynamicInitialConfig.entryPoint = isolatedInfo_.entryPoint;
    dynamicInitialConfig.registerComponents = isolatedInfo_.registerComponents;
    auto container = Platform::AceContainer::GetContainer(hostInstanceId_);
    CHECK_NULL_VOID(container);
    auto context = container->GetPipelineContext();
    CHECK_NULL_VOID(context);
    if (container->IsSceneBoardWindow()) {
        dynamicInitialConfig.hostWindowInfo.focusWindowId = static_cast<uint32_t>(GetWindowSceneId());
        dynamicInitialConfig.hostWindowInfo.realHostWindowId = context->GetRealHostWindowId();
    } else {
        dynamicInitialConfig.hostWindowInfo.focusWindowId = context->GetFocusWindowId();
        dynamicInitialConfig.hostWindowInfo.realHostWindowId = context->GetRealHostWindowId();
    }
}

void DynamicComponentRendererImpl::OnAccessibilityParentRectInfoUpdate()
{
    CHECK_NULL_VOID(uiContent_);
    auto instanceId = uiContent_->GetInstanceId();
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask([instanceId] {
        auto container = Container::GetContainer(instanceId);
        CHECK_NULL_VOID(container);
        auto pipeline = container->GetPipelineContext();
        CHECK_NULL_VOID(pipeline);
        auto ngPipeline = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        CHECK_NULL_VOID(ngPipeline);
        auto uiExtensionManager = ngPipeline->GetUIExtensionManager();
        CHECK_NULL_VOID(uiExtensionManager);
        uiExtensionManager->TransferAccessibilityRectInfo();
        }, TaskExecutor::TaskType::UI, "OnAccessibilityParentRectInfoUpdate");
}

void DynamicComponentRendererImpl::InitUiContent(
    OHOS::AbilityRuntime::Context *abilityContext)
{
    rendererDumpInfo_.ReSet();
    // create UI Content
    TAG_LOGI(aceLogTag_, "create UI Content");
    uiContent_ = UIContent::Create(abilityContext, runtime_, true);
    CHECK_NULL_VOID(uiContent_);
    uiContent_->SetUIContentType(uIContentType_);
    rendererDumpInfo_.createUiContenTime = GetCurrentTimestamp();

    DynamicInitialConfig dynamicInitialConfig;
    BuildDynamicInitialConfig(dynamicInitialConfig);
    uiContent_->InitializeDynamic(dynamicInitialConfig);

    auto runtimeContext = Platform::AceContainer::GetRuntimeContext(hostInstanceId_);
    if (runtimeContext) {
        auto uiContentImpl = std::static_pointer_cast<UIContentImpl>(uiContent_);
        if (uiContentImpl) {
            uiContentImpl->UpdateFontScale(runtimeContext->GetConfiguration());
        }
    }

    ContainerScope scope(uiContent_->GetInstanceId());
    RegisterContainerHandler();
    RegisterErrorEventHandler();
    RegisterSizeChangedCallback();
    RegisterConfigChangedCallback();
    AttachRenderContext();
    auto hostTaskExecutor = GetHostTaskExecutor();
    CHECK_NULL_VOID(hostTaskExecutor);
    hostTaskExecutor->PostTask(
        [weak = WeakClaim(this)] () {
            auto render = weak.Upgrade();
            CHECK_NULL_VOID(render);
            render->SetUIContentJsContext();
        }, TaskExecutor::TaskType::UI, "HostSetUIContentJsContext");
    rendererDumpInfo_.limitedWorkerInitTime = GetCurrentTimestamp();
    TAG_LOGI(aceLogTag_, "foreground dynamic UI content");
    uiContent_->Foreground();
    std::function<void()> contentReadyCallback;
    {
        std::lock_guard<std::mutex> lock(contentReadyMutex_);
        contentReady_ = true;
        if (contentReadyCallback_) {
            contentReadyCallback = std::move(contentReadyCallback_);
        }
    }
    if (contentReadyCallback) {
        contentReadyCallback();
    }
    InitializeDynamicAccessibility();
    rendererDumpInfo_.loadAbcTime = GetCurrentTimestamp();
}

void DynamicComponentRendererImpl::RegisterContainerHandler()
{
    CHECK_NULL_VOID(uiContent_);
    auto container = Container::GetContainer(uiContent_->GetInstanceId());
    CHECK_NULL_VOID(container);
    auto aceContainer = AceType::DynamicCast<Platform::AceContainer>(container);
    CHECK_NULL_VOID(aceContainer);
    auto host = host_.Upgrade();
    CHECK_NULL_VOID(host);
    auto containerHandler = AceType::DynamicCast<ContainerHandler>(host->GetPattern());
    CHECK_NULL_VOID(containerHandler);
    aceContainer->RegisterContainerHandler(containerHandler);
}

void DynamicComponentRendererImpl::SetUIContentJsContext()
{
    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    auto jsContext = hostJsContext_.lock();
    CHECK_NULL_VOID(jsContext);
    auto hostContainer = Container::GetContainer(hostInstanceId_);
    CHECK_NULL_VOID(hostContainer);
    auto aceHostContainer = AceType::DynamicCast<Platform::AceContainer>(hostContainer);
    CHECK_NULL_VOID(aceHostContainer);
    auto data = aceHostContainer->SerializeValue(jsContext);
    if (data == nullptr) {
        TAG_LOGW(aceLogTag_, "set JsContext: data is null");
        return;
    }

    taskExecutor->PostTask(
        [instanceId = uiContent_->GetInstanceId(), data] {
            auto container = Container::GetContainer(instanceId);
            CHECK_NULL_VOID(container);
            auto aceContainer = AceType::DynamicCast<Platform::AceContainer>(container);
            CHECK_NULL_VOID(aceContainer);
            aceContainer->SetJsContextWithDeserialize(data);
        }, TaskExecutor::TaskType::UI, "WorkerSetJsContextWithDeserialize");
}

void DynamicComponentRendererImpl::RegisterErrorEventHandler()
{
    CHECK_NULL_VOID(uiContent_);
    auto errorEventHandler = [weak = WeakClaim(this)](const std::string& code, const std::string& msg) {
        auto renderer = weak.Upgrade();
        if (renderer) {
            renderer->FireOnErrorCallback(WORKER_ERROR, code, msg);
        }
    };
    uiContent_->SetErrorEventHandler(errorEventHandler);
}

void DynamicComponentRendererImpl::FireOnErrorCallback(
    int32_t code, const std::string& name, const std::string& msg)
{
    auto isolatedHost = host_.Upgrade();
    CHECK_NULL_VOID(isolatedHost);
    auto pattern = AceType::DynamicCast<IsolatedPattern>(isolatedHost->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->FireOnErrorCallbackOnUI(code, name, msg);
}

void DynamicComponentRendererImpl::RegisterSizeChangedCallback()
{
    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostSyncTask(
        [weak = WeakClaim(this), aceLogTag = aceLogTag_, instanceId = uiContent_->GetInstanceId()] {
            auto container = Container::GetContainer(instanceId);
            auto frontend = AceType::DynamicCast<OHOS::Ace::FormFrontendDeclarative>(container->GetFrontend());
            CHECK_NULL_VOID(frontend);
            auto delegate = frontend->GetDelegate();
            CHECK_NULL_VOID(delegate);
            auto pageRouterManager = delegate->GetPageRouterManager();
            CHECK_NULL_VOID(pageRouterManager);
            auto pageNode = pageRouterManager->GetCurrentPageNode();
            CHECK_NULL_VOID(pageNode);
            auto pagePattern = AceType::DynamicCast<PagePattern>(pageNode->GetPattern());
            CHECK_NULL_VOID(pagePattern);

            auto dynamicPageSizeCallback = [weak, aceLogTag](const SizeF& size) {
                TAG_LOGI(aceLogTag, "card size callback: %{public}s", size.ToString().c_str());
                auto renderer = weak.Upgrade();
                CHECK_NULL_VOID(renderer);
                CHECK_NULL_VOID(renderer->uiContent_);
                auto hostTaskExecutor = renderer->GetHostTaskExecutor();
                CHECK_NULL_VOID(hostTaskExecutor);
                hostTaskExecutor->PostTask(
                    [weak, size]() {
                        auto renderer = weak.Upgrade();
                        CHECK_NULL_VOID(renderer);
                        renderer->HandleCardSizeChangeEvent(size);
                    },
                    TaskExecutor::TaskType::UI, "ArkUIDynamicComponentSizeChanged",
                    TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
            };
            pagePattern->SetDynamicPageSizeCallback(std::move(dynamicPageSizeCallback));
        }, TaskExecutor::TaskType::UI, "ArkUIRegisterSizeChangedCallback");
}

void DynamicComponentRendererImpl::HandleCardSizeChangeEvent(const SizeF& size)
{
    if (!adaptiveWidth_ && !adaptiveHeight_) {
        return;
    }
    CHECK_NULL_VOID(uiContent_);
    auto instanceId = uiContent_->GetInstanceId();
    ContainerScope scope(hostInstanceId_);
    auto host = host_.Upgrade();
    CHECK_NULL_VOID(host);
    auto& layout = host->GetLayoutProperty();
    CHECK_NULL_VOID(layout);
    auto padding = layout->CreatePaddingAndBorder();
    auto rect = host->GetTransformRectRelativeToWindow();
    auto width = adaptiveWidth_ ? int32_t(size.Width() + padding.Width()) : rect.Width();
    auto height = adaptiveHeight_ ? int32_t(size.Height() + padding.Height()) : rect.Height();
    if (NearEqual(width, rect.Width()) && NearEqual(height, rect.Height())) {
        TAG_LOGI(aceLogTag_, "[%{public}d] component size not changed: [%{public}f x %{public}f]",
            instanceId, width, height);
        return;
    }
    TAG_LOGI(aceLogTag_,
        "[%{public}d] update component size: [%{public}f x %{public}f] -> [%{public}f x %{public}f]",
        instanceId, rect.Width(), rect.Height(), width, height);
    std::optional<CalcLength> calcWidth =
        adaptiveWidth_ ? std::optional<CalcLength>(CalcLength(width)) : std::nullopt;
    std::optional<CalcLength> calcHeight =
        adaptiveHeight_ ? std::optional<CalcLength>(CalcLength(height)) : std::nullopt;
    layout->UpdateUserDefinedIdealSize(CalcSize(calcWidth, calcHeight));
    host->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);
}

void DynamicComponentRendererImpl::RegisterConfigChangedCallback()
{
    CHECK_NULL_VOID(uiContent_);
    auto hostExecutor = GetHostTaskExecutor();
    CHECK_NULL_VOID(hostExecutor);
    hostExecutor->PostTask(
        [hostInstanceId = hostInstanceId_, subInstanceId = uiContent_->GetInstanceId(), aceLogTag = aceLogTag_]() {
            auto configChangedCallback = [subInstanceId, aceLogTag](
                                             const Platform::ParsedConfig& config, const std::string& configuration) {
                TAG_LOGI(aceLogTag, "UpdateConfiguration to subContainer");
                auto subContainer = Platform::AceContainer::GetContainer(subInstanceId);
                CHECK_NULL_VOID(subContainer);
                subContainer->GetTaskExecutor()->PostTask(
                    [weak = WeakClaim(RawPtr(subContainer)), config, configuration]() {
                        auto subContainer = weak.Upgrade();
                        CHECK_NULL_VOID(subContainer);
                        ContainerScope scope(subContainer->GetInstanceId());
                        subContainer->UpdateConfiguration(config, configuration);
                    },
                    TaskExecutor::TaskType::UI, "ArkUIDynamicComponentConfigurationChanged",
                    TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
            };

            auto hostContainer = Platform::AceContainer::GetContainer(hostInstanceId);
            CHECK_NULL_VOID(hostContainer);
            hostContainer->AddOnConfigurationChange(subInstanceId, configChangedCallback);
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentConfigurationChanged",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

void DynamicComponentRendererImpl::UnRegisterConfigChangedCallback()
{
    CHECK_NULL_VOID(uiContent_);
    auto container = Platform::AceContainer::GetContainer(hostInstanceId_);
    CHECK_NULL_VOID(container);
    container->RemoveOnConfigurationChange(uiContent_->GetInstanceId());
    container->RegisterContainerHandler(nullptr);
}

void DynamicComponentRendererImpl::AttachRenderContext()
{
    if (uIContentType_ == UIContentType::DYNAMIC_COMPONENT) {
        AttachRenderContextInDynamicComponent();
        return;
    }

    AttachRenderContextInIsolatedComponent();
}

void DynamicComponentRendererImpl::AttachRenderContextInIsolatedComponent()
{
    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetHostTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostSyncTask([weak = host_, hostInstanceId = hostInstanceId_,
        instanceId = uiContent_->GetInstanceId(), aceLogTag = aceLogTag_,
        backgroundTransparent = GetBackgroundTransparent()]() {
            ContainerScope scope(hostInstanceId);
            auto host = weak.Upgrade();
            CHECK_NULL_VOID(host);
            auto hostRenderContext = host->GetRenderContext();
            CHECK_NULL_VOID(hostRenderContext);

            auto container = Container::GetContainer(instanceId);
            CHECK_NULL_VOID(container);
            auto pipeline = container->GetPipelineContext();
            CHECK_NULL_VOID(pipeline);
            auto pipelineContext = AceType::DynamicCast<NG::PipelineContext>(pipeline);
            CHECK_NULL_VOID(pipelineContext);
            auto rootElement = pipelineContext->GetRootElement();
            CHECK_NULL_VOID(rootElement);
            auto renderContext = rootElement->GetRenderContext();
            CHECK_NULL_VOID(renderContext);

            if (backgroundTransparent) {
                pipeline->SetAppBgColor(Color::TRANSPARENT);
            }
            renderContext->SetClipToFrame(true);
            renderContext->SetClipToBounds(true);

            TAG_LOGI(aceLogTag, "add render context of dynamic component for '%{public}d'",
                instanceId);
            hostRenderContext->ClearChildren();
            hostRenderContext->AddChild(renderContext, -1);

            host->MarkDirtyNode(PROPERTY_UPDATE_LAYOUT);
            auto parent = host->GetParent();
            CHECK_NULL_VOID(parent);
            parent->MarkNeedSyncRenderTree();
            parent->RebuildRenderContextTree();
            hostRenderContext->RequestNextFrame();
        },
        TaskExecutor::TaskType::UI, "ArkUIIsolatedComponentAttachRenderContext");
}

void DynamicComponentRendererImpl::AttachRenderContextInDynamicComponent()
{
    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetHostTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostSyncTask([weak = host_, hostInstanceId = hostInstanceId_,
        instanceId = uiContent_->GetInstanceId(), aceLogTag = aceLogTag_,
        backgroundTransparent = GetBackgroundTransparent()]() {
            ContainerScope scope(hostInstanceId);
            auto host = weak.Upgrade();
            CHECK_NULL_VOID(host);
            auto surfaceProxyNode = AceType::DynamicCast<NG::SurfaceProxyNode>(host->GetPattern());
            CHECK_NULL_VOID(surfaceProxyNode);
            auto container = Platform::AceContainer::GetContainer(instanceId);
            CHECK_NULL_VOID(container);
            auto surfaceNode = container->GetFormSurfaceNode(instanceId);
            CHECK_NULL_VOID(surfaceNode);

            auto pipeline = container->GetPipelineContext();
            CHECK_NULL_VOID(pipeline);
            auto pipelineContext = AceType::DynamicCast<NG::PipelineContext>(pipeline);
            CHECK_NULL_VOID(pipelineContext);
            auto rootElement = pipelineContext->GetRootElement();
            CHECK_NULL_VOID(rootElement);
            auto renderContext = rootElement->GetRenderContext();
            CHECK_NULL_VOID(renderContext);
            if (backgroundTransparent) {
                pipeline->SetAppBgColor(Color::TRANSPARENT);
            }
            renderContext->SetClipToFrame(true);
            renderContext->SetClipToBounds(true);
            surfaceProxyNode->AddSurfaceNode(surfaceNode);
            TAG_LOGI(aceLogTag, "add render context of dynamic component for '%{public}d'",
                instanceId);
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentAttachRenderContext");
}

void DynamicComponentRendererImpl::TransferPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(
        [uiContent = uiContent_, pointerEvent]() {
            ContainerScope scope(uiContent->GetInstanceId());
            uiContent->ProcessPointerEvent(pointerEvent);
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentProcessPointer",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

bool DynamicComponentRendererImpl::TransferKeyEvent(const KeyEvent& keyEvent)
{
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_RETURN(taskExecutor, false);

    auto rawKeyEvent = keyEvent.rawKeyEvent;
    bool result = false;
    std::weak_ptr<UIContent> weak = uiContent_;
    taskExecutor->PostSyncTask(
        [weak, keyEvent, &result, aceLogTag = aceLogTag_]() {
            auto uiContent = weak.lock();
            CHECK_NULL_VOID(uiContent);
            auto subInstanceId = uiContent->GetInstanceId();
            ContainerScope scope(subInstanceId);
            result = uiContent->ProcessKeyEvent(keyEvent.rawKeyEvent);
            TAG_LOGI(aceLogTag, "send key event: %{public}s, result = %{public}d",
                keyEvent.ToString().c_str(), result);
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentProcessKey",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
    return result;
}

void DynamicComponentRendererImpl::TransferFocusState(bool isFocus)
{
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    std::weak_ptr<UIContent> weak = uiContent_;
    taskExecutor->PostTask(
        [weak, isFocus, aceLogTag = aceLogTag_]() {
            auto uiContent = weak.lock();
            CHECK_NULL_VOID(uiContent);
            ContainerScope scope(uiContent->GetInstanceId());
            TAG_LOGI(aceLogTag, "send focus state: %{public}d", isFocus);
            if (isFocus) {
                uiContent->Focus();
            } else {
                uiContent->UnFocus();
            }
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentFocusState",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

void DynamicComponentRendererImpl::TransferFocusActiveEvent(bool isFocus)
{
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    std::weak_ptr<UIContent> weak = uiContent_;
    taskExecutor->PostTask(
        [weak, isFocus, aceLogTag = aceLogTag_]() {
            auto uiContent = weak.lock();
            CHECK_NULL_VOID(uiContent);
            ContainerScope scope(uiContent->GetInstanceId());
            TAG_LOGI(aceLogTag, "send focus active event: %{public}d", isFocus);
            uiContent->SetIsFocusActive(isFocus);
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentFocusActiveEvent",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

SizeF DynamicComponentRendererImpl::ComputeAdaptiveSize(const SizeF& size) const
{
    auto width = size.Width();
    auto height = size.Height();
    if (adaptiveWidth_ || adaptiveHeight_) {
        auto host = host_.Upgrade();
        CHECK_NULL_RETURN(host, size);
        auto& layout = host->GetLayoutProperty();
        CHECK_NULL_RETURN(layout, size);
        auto& constraint = layout->GetLayoutConstraint();
        CHECK_NULL_RETURN(constraint, size);
        auto padding = layout->CreatePaddingAndBorder();
        auto maxSize = constraint->maxSize;
        if (adaptiveWidth_) {
            width = maxSize.Width() - padding.Width();
        }
        if (adaptiveHeight_) {
            height = maxSize.Height() - padding.Height();
        }
    }
    return SizeF(width, height);
}

void DynamicComponentRendererImpl::UpdateViewportConfig(
    const SizeF& size, float density, int32_t orientation, AnimationOption animationOpt, const OffsetF& offset)
{
    CHECK_NULL_VOID(uiContent_);
    auto adaptiveSize = ComputeAdaptiveSize(size);
    ViewportConfig vpConfig(adaptiveSize.Width(), adaptiveSize.Height(), density);
    vpConfig.SetPosition(0, 0);
    vpConfig.SetOrientation(orientation);
    TAG_LOGI(aceLogTag_, "[%{public}d] adaptive size: %{public}s -> [%{public}d x %{public}d]",
        uiContent_->GetInstanceId(), size.ToString().c_str(), vpConfig.Width(), vpConfig.Height());

    auto option = CopyAnimationOption(animationOpt);
    auto task = [weak = WeakClaim(this), vpConfig, option, aceLogTag = aceLogTag_, offset]() {
        auto renderer = weak.Upgrade();
        CHECK_NULL_VOID(renderer);
        auto uiContent = std::static_pointer_cast<UIContentImpl>(renderer->uiContent_);
        CHECK_NULL_VOID(uiContent);
        ContainerScope scope(uiContent->GetInstanceId());
        ViewportConfig config(vpConfig.Width(), vpConfig.Height(), vpConfig.Density());
        config.SetPosition(vpConfig.Left(), vpConfig.Top());
        config.SetOrientation(vpConfig.Orientation());
        if (renderer->uIContentType_ == UIContentType::DYNAMIC_COMPONENT) {
            renderer->UpdateParentOffsetToWindow(offset);
            config.SetPosition(offset.GetX(), offset.GetY());
        }
        if (renderer->viewport_.Width() == config.Width() && renderer->viewport_.Height() == config.Height()
            && renderer->density_ == config.Density()) {
            TAG_LOGW(aceLogTag, "card viewport not changed");
            return;
        }
        renderer->viewport_.SetWidth(config.Width());
        renderer->viewport_.SetHeight(config.Height());
        renderer->density_ = config.Density();
        TAG_LOGI(aceLogTag, "update card viewport: [%{public}d x %{public}d]",
            config.Width(), config.Height());
        uiContent->UpdateViewportConfigWithAnimation(config, Rosen::WindowSizeChangeReason::UNDEFINED, *option);
    };
    bool contentReady = false;
    {
        std::lock_guard<std::mutex> lock(contentReadyMutex_);
        contentReady = contentReady_;
        if (!contentReady) {
            contentReadyCallback_ = std::move(task);
        }
    }
    if (contentReady) {
        auto taskExecutor = GetTaskExecutor();
        CHECK_NULL_VOID(taskExecutor);
        taskExecutor->PostTask(std::move(task),
            TaskExecutor::TaskType::UI, "ArkUIDynamicComponentUpdateViewport",
            TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
    }
}

void DynamicComponentRendererImpl::UpdateParentOffsetToWindow(const OffsetF& offset)
{
    TAG_LOGI(aceLogTag_, "UpdateParentOffsetToWindow");
    auto task = [weak = WeakClaim(this), offset]() {
        auto renderer = weak.Upgrade();
        CHECK_NULL_VOID(renderer);
        auto uiContent = renderer->uiContent_;
        CHECK_NULL_VOID(uiContent);
        ContainerScope scope(uiContent->GetInstanceId());
        auto container = Container::GetContainer(uiContent->GetInstanceId());
        CHECK_NULL_VOID(container);
        auto pipeline = container->GetPipelineContext();
        CHECK_NULL_VOID(pipeline);
        auto pipelineContext = AceType::DynamicCast<NG::PipelineContext>(pipeline);
        CHECK_NULL_VOID(pipelineContext);
        pipelineContext->SetHostParentOffsetToWindow(Offset(offset.GetX(), offset.GetY()));
    };
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(std::move(task), TaskExecutor::TaskType::UI,
        "ArkUIDynamicComponentUpdateParentOffsetToWindow",
        TaskExecutor::GetPriorityTypeWithCheck(PriorityType::VIP));
}

void DynamicComponentRendererImpl::DestroyContent()
{
    OnDestroyContent();
    AfterDestroyContent();
}

void DynamicComponentRendererImpl::UnRegisterContainerHandler()
{
    CHECK_NULL_VOID(uiContent_);
    auto container = Container::GetContainer(uiContent_->GetInstanceId());
    CHECK_NULL_VOID(container);
    auto aceContainer = AceType::DynamicCast<Platform::AceContainer>(container);
    CHECK_NULL_VOID(aceContainer);
    aceContainer->RegisterContainerHandler(nullptr);
    TAG_LOGI(aceLogTag_, "UnRegisterContainerHandler");
}

void DynamicComponentRendererImpl::OnDestroyContent()
{
    CHECK_NULL_VOID(uiContent_);
    UnRegisterConfigChangedCallback();
    UnRegisterContainerHandler();
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    taskExecutor->PostTask(
        [uiContent = uiContent_, aceLogTag = aceLogTag_]() {
            ContainerScope scope(uiContent->GetInstanceId());
            TAG_LOGI(aceLogTag, "destroy dynamic UI content");
            uiContent->Destroy();
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentDestroy");
}

void DynamicComponentRendererImpl::AfterDestroyContent()
{
    if (uIContentType_ == UIContentType::DYNAMIC_COMPONENT) {
        DeleteWorkerUsing(runtime_);
        return;
    }
}

void DynamicComponentRendererImpl::SearchElementInfoByAccessibilityId(int64_t elementId, int32_t mode,
    int64_t baseParent, std::list<Accessibility::AccessibilityElementInfo>& output)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->SearchElementInfoByAccessibilityId(elementId, mode, baseParent, output);
}

void DynamicComponentRendererImpl::SearchElementInfosByText(int64_t elementId, const std::string& text,
    int64_t baseParent, std::list<Accessibility::AccessibilityElementInfo>& output)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->SearchElementInfosByText(elementId, text, baseParent, output);
}

void DynamicComponentRendererImpl::FindFocusedElementInfo(int64_t elementId, int32_t focusType, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& output)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->FindFocusedElementInfo(elementId, focusType, baseParent, output);
}

void DynamicComponentRendererImpl::FocusMoveSearch(int64_t elementId, int32_t direction, int64_t baseParent,
    Accessibility::AccessibilityElementInfo& output)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->FocusMoveSearch(elementId, direction, baseParent, output);
}

bool DynamicComponentRendererImpl::NotifyExecuteAction(int64_t elementId, const std::map<std::string,
    std::string>& actionArguments, int32_t action, int64_t offset)
{
    CHECK_NULL_RETURN(uiContent_, false);
    return uiContent_->NotifyExecuteAction(elementId, actionArguments, action, offset);
}

void DynamicComponentRendererImpl::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
    CHECK_NULL_VOID(uiContent_);
    auto host = host_.Upgrade();
    CHECK_NULL_VOID(host);
    auto pattern = host->GetPattern();
    CHECK_NULL_VOID(pattern);
    auto container = Container::GetContainer(uiContent_->GetInstanceId());
    CHECK_NULL_VOID(container);
    auto front = container->GetFrontend();
    CHECK_NULL_VOID(front);
    auto jsAccessibilityManager = front->GetAccessibilityManager();
    CHECK_NULL_VOID(jsAccessibilityManager);
    auto uiExtensionId = pattern->GetUiExtensionId();
    TAG_LOGI(aceLogTag_, "uiExtensionId: %{public}d", uiExtensionId);
    jsAccessibilityManager->SetUiextensionId(uiExtensionId);
    uiContent_->HandleAccessibilityHoverEvent(pointX, pointY, sourceType, eventType, timeMs);
}

RefPtr<TaskExecutor> DynamicComponentRendererImpl::GetTaskExecutor()
{
    CHECK_NULL_RETURN(uiContent_, nullptr);
    auto container = Container::GetContainer(uiContent_->GetInstanceId());
    CHECK_NULL_RETURN(container, nullptr);
    return container->GetTaskExecutor();
}

RefPtr<TaskExecutor> DynamicComponentRendererImpl::GetHostTaskExecutor()
{
    auto container = Container::GetContainer(hostInstanceId_);
    CHECK_NULL_RETURN(container, nullptr);
    return container->GetTaskExecutor();
}

void DynamicComponentRendererImpl::Dump(RendererDumpInfo &rendererDumpInfo)
{
    rendererDumpInfo = rendererDumpInfo_;
}

void DynamicComponentRendererImpl::NotifyUieDump(const std::vector<std::string>& params,
    std::vector<std::string>& info)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->DumpInfo(params, info);
}

void DynamicComponentRendererImpl::TransferAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->RegisterAccessibilityChildTree(windowId, treeId, accessibilityId);
}

void DynamicComponentRendererImpl::TransferAccessibilityChildTreeDeregister()
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->DeregisterAccessibilityChildTree();
}

void DynamicComponentRendererImpl::TransferAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
    CHECK_NULL_VOID(uiContent_);
    uiContent_->AccessibilityDumpChildInfo(params, info);
}

void DynamicComponentRendererImpl::InitializeDynamicAccessibility()
{
    if (uIContentType_ != UIContentType::DYNAMIC_COMPONENT) {
        return;
    }
    auto dynamicHost = host_.Upgrade();
    CHECK_NULL_VOID(dynamicHost);
    auto pattern = AceType::DynamicCast<DynamicPattern>(dynamicHost->GetPattern());
    CHECK_NULL_VOID(pattern);
    pattern->InitializeAccessibility();
}

void DynamicComponentRendererImpl::NotifyForeground()
{
    if (isForeground_) {
        return;
    }

    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    isForeground_ = true;
    taskExecutor->PostTask(
        [uiContent = uiContent_, aceLogTag = aceLogTag_]() {
            CHECK_NULL_VOID(uiContent);
            ContainerScope scope(uiContent->GetInstanceId());
            TAG_LOGI(aceLogTag, "NotifyForeground");
            uiContent->Foreground();
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentNotifyForeground");
}

void DynamicComponentRendererImpl::NotifyBackground()
{
    if (!isForeground_) {
        return;
    }

    CHECK_NULL_VOID(uiContent_);
    auto taskExecutor = GetTaskExecutor();
    CHECK_NULL_VOID(taskExecutor);
    isForeground_ = false;
    taskExecutor->PostTask(
        [uiContent = uiContent_, aceLogTag = aceLogTag_]() {
            CHECK_NULL_VOID(uiContent);
            ContainerScope scope(uiContent->GetInstanceId());
            TAG_LOGI(aceLogTag, "NotifyBackground");
            uiContent->Background();
        },
        TaskExecutor::TaskType::UI, "ArkUIDynamicComponentNotifyBackground");
}
} // namespace OHOS::Ace::NG
