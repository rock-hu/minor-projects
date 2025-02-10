/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "core/components_ng/pattern/ui_extension/dynamic_component/dynamic_pattern.h"

#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/osal/want_wrap_ohos.h"
#include "base/log/log_wrapper.h"
#include "base/log/dump_log.h"
#include "core/components_ng/render/animation_utils.h"
#include "core/pipeline_ng/pipeline_context.h"
#include "display_manager.h"
#include "parameters.h"

namespace OHOS::Ace::NG {
namespace {
constexpr char DC_DEPTH_PREFIX[] = "dcDepth_";
constexpr char PARAM_NAME_WORKER_HAS_USED[] = "workerHasUsed";
constexpr char PARAM_MSG_WORKER_HAS_USED[] = "Two worker are not allowed to run at the same time";
constexpr char PARAM_NAME_DC_ONLY_ON_SCB[] = "onlyRunOnSCB";
constexpr char PARAM_MSG_DC_ONLY_ON_SCB[] = "DC only run on SCB";
constexpr char PARAM_NAME_INTERNAL_ERROR[] = "internalError";
constexpr char PARAM_MSG_INTERNAL_ERROR[] = "Internal error";
constexpr char PARAM_NAME_PARAM_ERROR[] = "paramError";
constexpr char PARAM_MSG_PARAM_ERROR[] = "Param error";
const char ENABLE_DEBUG_DC_KEY[] = "persist.ace.debug.dc.enabled";

bool IsDebugDCEnabled()
{
    return OHOS::system::GetParameter(ENABLE_DEBUG_DC_KEY, "false") == "true";
}
}

int32_t DynamicPattern::dynamicGenerator_ = 0;

DynamicPattern::DynamicPattern()
    : PlatformPattern(AceLogTag::ACE_DYNAMIC_COMPONENT, ++dynamicGenerator_)
{
    uiExtensionId_ = UIExtensionIdUtility::GetInstance().ApplyExtensionId();
    instanceId_ = -1;
    PLATFORM_LOGI("The DynamicPattern is created.");
}

DynamicPattern::~DynamicPattern()
{
    UIExtensionIdUtility::GetInstance().RecycleExtensionId(uiExtensionId_);
    PLATFORM_LOGI("The DynamicPattern is destroyed.");
}

int64_t DynamicPattern::WrapExtensionAbilityId(int64_t extensionOffset, int64_t abilityId)
{
    return uiExtensionId_ * extensionOffset + abilityId;
}

RefPtr<AccessibilitySessionAdapter> DynamicPattern::GetAccessibilitySessionAdapter()
{
    return accessibilitySessionAdapter_;
}

void DynamicPattern::InitializeDynamicComponent(
    const std::string& hapPath, const std::string& abcPath, const std::string& entryPoint, void* runtime)
{
    if (entryPoint.empty() || runtime == nullptr) {
        PLATFORM_LOGE("The param empty.");
        HandleErrorCallback(DCResultCode::DC_PARAM_ERROE);
        return;
    }

    curDynamicInfo_.entryPoint = entryPoint;
    InitializeRender(runtime);
}

void DynamicPattern::HandleErrorCallback(DCResultCode resultCode)
{
    switch (resultCode) {
        case DCResultCode::DC_WORKER_HAS_USED_ERROR:
            FireOnErrorCallbackOnUI(
                resultCode, PARAM_NAME_WORKER_HAS_USED, PARAM_MSG_WORKER_HAS_USED);
            break;
        case DCResultCode::DC_ONLY_RUN_ON_SCB:
            FireOnErrorCallbackOnUI(
                resultCode, PARAM_NAME_DC_ONLY_ON_SCB, PARAM_MSG_DC_ONLY_ON_SCB);
            break;
        case DCResultCode::DC_INTERNAL_ERROR:
            FireOnErrorCallbackOnUI(
                resultCode, PARAM_NAME_INTERNAL_ERROR, PARAM_MSG_INTERNAL_ERROR);
            break;
        case DCResultCode::DC_PARAM_ERROE:
            FireOnErrorCallbackOnUI(
                resultCode, PARAM_NAME_PARAM_ERROR, PARAM_MSG_PARAM_ERROR);
            break;
        default:
            PLATFORM_LOGI("HandleErrorCallback code: %{public}d is invalid.", resultCode);
    }
}

DCResultCode DynamicPattern::CheckConstraint()
{
    auto instanceId = GetHostInstanceId();
    PLATFORM_LOGI("CheckConstraint instanceId: %{public}d.", instanceId);
    auto container = Platform::AceContainer::GetContainer(instanceId);
    if (!container) {
        PLATFORM_LOGE("container is null.");
        return DCResultCode::DC_INTERNAL_ERROR;
    }

    if (container->IsScenceBoardWindow()) {
        return DCResultCode::DC_NO_ERRORS;
    }

    return IsDebugDCEnabled() ? DCResultCode::DC_NO_ERRORS : DCResultCode::DC_ONLY_RUN_ON_SCB;
}

void DynamicPattern::InitializeRender(void* runtime)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    dynamicDumpInfo_.createLimitedWorkerTime = GetCurrentTimestamp();
#if !defined(PREVIEW)
    DCResultCode code = CheckConstraint();
    if (code != DCResultCode::DC_NO_ERRORS) {
        HandleErrorCallback(code);
        PLATFORM_LOGE("CheckConstraint failed, code: %{public}d.", code);
        return;
    }

    if (!dynamicComponentRenderer_) {
        ContainerScope scope(instanceId_);
        SetHostNode(host);
        dynamicComponentRenderer_ =
            DynamicComponentRenderer::Create(GetHost(), runtime, curDynamicInfo_);
        CHECK_NULL_VOID(dynamicComponentRenderer_);
        if (dynamicComponentRenderer_->HasWorkerUsing(runtime)) {
            HandleErrorCallback(DCResultCode::DC_WORKER_HAS_USED_ERROR);
            return;
        }

        dynamicComponentRenderer_->SetUIContentType(UIContentType::DYNAMIC_COMPONENT);
        dynamicComponentRenderer_->SetAdaptiveSize(adaptiveWidth_, adaptiveHeight_);
        dynamicComponentRenderer_->SetBackgroundTransparent(backgroundTransparent_);
        dynamicComponentRenderer_->CreateContent();
        accessibilitySessionAdapter_ =
            AceType::MakeRefPtr<AccessibilitySessionAdapterIsolatedComponent>(dynamicComponentRenderer_);
        SetContainerHostPattern(WeakClaim(this));
        auto eventHub = host->GetEventHub<EventHub>();
        CHECK_NULL_VOID(eventHub);
        OnAreaChangedFunc onAreaChangedFunc = [renderer = dynamicComponentRenderer_](
            const RectF& oldRect,
            const OffsetF& oldOrigin,
            const RectF& rect,
            const OffsetF& origin) {
                CHECK_NULL_VOID(renderer);
                renderer->UpdateParentOffsetToWindow(origin + rect.GetOffset());
        };
        eventHub->AddInnerOnAreaChangedCallback(host->GetId(), std::move(onAreaChangedFunc));
    }
#else
    PLATFORM_LOGE("DynamicComponent not support preview.");
#endif
}

void DynamicPattern::SetBackgroundTransparent(bool backgroundTransparent)
{
    backgroundTransparent_ = backgroundTransparent;
}

void DynamicPattern::FireOnErrorCallbackOnUI(
    int32_t code, const std::string& name, const std::string& msg)
{
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    auto uiTaskExecutor = SingleTaskExecutor::Make(
        pipeline->GetTaskExecutor(), TaskExecutor::TaskType::UI);
    uiTaskExecutor.PostTask([weak = WeakClaim(this), code, name, msg] {
        auto pattern = weak.Upgrade();
        CHECK_NULL_VOID(pattern);
        pattern->FireOnErrorCallback(code, name, msg);
        }, "FireOnErrorCallback");
}

void DynamicPattern::DispatchPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    CHECK_NULL_VOID(pointerEvent);
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->TransferPointerEvent(pointerEvent);
}

void DynamicPattern::DispatchFocusActiveEvent(bool isFocusActive)
{
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->TransferFocusActiveEvent(isFocusActive);
}

bool DynamicPattern::HandleKeyEvent(const KeyEvent& event)
{
    CHECK_NULL_RETURN(event.rawKeyEvent, false);
    CHECK_NULL_RETURN(dynamicComponentRenderer_, false);
    return dynamicComponentRenderer_->TransferKeyEvent(event);
}

void DynamicPattern::HandleFocusEvent()
{
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto pipeline = host->GetContext();
    CHECK_NULL_VOID(pipeline);
    if (pipeline->GetIsFocusActive()) {
        dynamicComponentRenderer_->TransferFocusActiveEvent(true);
    }
    dynamicComponentRenderer_->TransferFocusState(true);
}

void DynamicPattern::HandleBlurEvent()
{
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->TransferFocusActiveEvent(false);
    dynamicComponentRenderer_->TransferFocusState(false);
}

void DynamicPattern::OnAttachToFrameNode()
{
    ContainerScope scope(instanceId_);
    auto pipeline = PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(pipeline);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    pipeline->AddOnAreaChangeNode(host->GetId());
}

bool DynamicPattern::OnDirtyLayoutWrapperSwap(const RefPtr<LayoutWrapper>& dirty, const DirtySwapConfig& config)
{
    if (config.skipLayout || config.skipMeasure) {
        return false;
    }
    CHECK_NULL_RETURN(dynamicComponentRenderer_, false);
    auto& node = dirty->GetGeometryNode();
    CHECK_NULL_RETURN(node, false);
    auto size = node->GetContentSize();
    float density = 1.0f;
    int32_t orientation = 0;
    auto defaultDisplay = Rosen::DisplayManager::GetInstance().GetDefaultDisplay();
    if (defaultDisplay) {
        density = defaultDisplay->GetVirtualPixelRatio();
        orientation = static_cast<int32_t>(defaultDisplay->GetOrientation());
    }

    auto host = GetHost();
    CHECK_NULL_RETURN(host, false);
    auto pipeline = host->GetContext();
    CHECK_NULL_RETURN(pipeline, false);
    auto animationOption = pipeline->GetSyncAnimationOption();
    auto parentGlobalOffset = dirty->GetParentGlobalOffsetWithSafeArea(true, true) +
        dirty->GetFrameRectWithSafeArea(true).GetOffset();
    dynamicComponentRenderer_->UpdateViewportConfig(size, density, orientation, animationOption, parentGlobalOffset);
    return false;
}

void DynamicPattern::SetIsReportFrameEvent(bool isReportFrameEvent)
{
    hostConfig_.isReportFrameEvent = isReportFrameEvent;
}

void DynamicPattern::OnDetachFromFrameNode(FrameNode* frameNode)
{
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->DestroyContent();
    dynamicComponentRenderer_ = nullptr;
}

void DynamicPattern::SetAdaptiveWidth(bool state)
{
    adaptiveWidth_ = state;
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->SetAdaptiveSize(adaptiveWidth_, adaptiveHeight_);
}

void DynamicPattern::SetAdaptiveHeight(bool state)
{
    adaptiveHeight_ = state;
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    dynamicComponentRenderer_->SetAdaptiveSize(adaptiveWidth_, adaptiveHeight_);
}

void DynamicPattern::OnAttachContext(PipelineContext *context)
{
    CHECK_NULL_VOID(context);
    auto newInstanceId = context->GetInstanceId();
    PLATFORM_LOGI("OnAttachContext newInstanceId: %{public}d, oldInstanceId: %{public}d.",
        newInstanceId, instanceId_);
    if (newInstanceId != instanceId_) {
        UnRegisterPipelineEvent(instanceId_);
        RegisterPipelineEvent(newInstanceId);
        instanceId_ = newInstanceId;
    }
}

void DynamicPattern::RegisterPipelineEvent(int32_t instanceId)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = PipelineContext::GetContextByContainerId(instanceId);
    CHECK_NULL_VOID(context);
    context->AddWindowStateChangedCallback(host->GetId());
}

void DynamicPattern::OnDetachContext(PipelineContext *context)
{
    CHECK_NULL_VOID(context);
    auto instanceId = context->GetInstanceId();
    PLATFORM_LOGI("OnAttachContext instanceId: %{public}d.", instanceId);
    UnRegisterPipelineEvent(instanceId);
}

void DynamicPattern::UnRegisterPipelineEvent(int32_t instanceId)
{
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto context = PipelineContext::GetContextByContainerId(instanceId);
    CHECK_NULL_VOID(context);
    context->RemoveWindowStateChangedCallback(host->GetId());
}

void DynamicPattern::DumpDynamicRenderer(int32_t depth, bool hasJson)
{
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    auto container = Platform::AceContainer::GetContainer(instanceId_);
    CHECK_NULL_VOID(container);
    std::vector<std::string> params = container->GetUieParams();
    std::string deptParam;
    deptParam.append(DC_DEPTH_PREFIX).append(std::to_string(depth));
    params.push_back(deptParam);
    if (hasJson) {
        params.push_back("-json");
    }
    std::vector<std::string> dumpInfo;
    dynamicComponentRenderer_->NotifyUieDump(params, dumpInfo);
}

void DynamicPattern::DumpInfo()
{
    DumpLog::GetInstance().AddDesc(std::string("dynamicId: ").append(std::to_string(platformId_)));
    DumpLog::GetInstance().AddDesc(std::string("entryPoint: ").append(curDynamicInfo_.entryPoint));
    DumpLog::GetInstance().AddDesc(std::string("createLimitedWorkerTime: ")
        .append(std::to_string(dynamicDumpInfo_.createLimitedWorkerTime)));
    CHECK_NULL_VOID(dynamicComponentRenderer_);
    RendererDumpInfo rendererDumpInfo;
    dynamicComponentRenderer_->Dump(rendererDumpInfo);
    DumpLog::GetInstance().AddDesc(std::string("createUiContenTime: ")
        .append(std::to_string(rendererDumpInfo.createUiContenTime)));
    DumpLog::GetInstance().AddDesc(std::string("limitedWorkerInitTime: ")
        .append(std::to_string(rendererDumpInfo.limitedWorkerInitTime)));
    DumpLog::GetInstance().AddDesc(std::string("loadAbcTime: ")
        .append(std::to_string(rendererDumpInfo.loadAbcTime)));
    std::string isReportFrameEvent = hostConfig_.isReportFrameEvent ? "true" : "false";
    DumpLog::GetInstance().AddDesc(std::string("isReportFrameEvent: ")
        .append(isReportFrameEvent));
}

void DynamicPattern::DumpInfo(std::unique_ptr<JsonValue>& json)
{
    json->Put("dynamicId", platformId_);
    json->Put("entryPoint", curDynamicInfo_.reourcePath.c_str());
    json->Put("reourcePath", curDynamicInfo_.entryPoint.c_str());
    json->Put("createLimitedWorkerTime", std::to_string(dynamicDumpInfo_.createLimitedWorkerTime).c_str());

    CHECK_NULL_VOID(dynamicComponentRenderer_);
    RendererDumpInfo rendererDumpInfo;
    dynamicComponentRenderer_->Dump(rendererDumpInfo);
    json->Put("createUiContenTime", std::to_string(rendererDumpInfo.createUiContenTime).c_str());
    json->Put("limitedWorkerInitTime", std::to_string(rendererDumpInfo.limitedWorkerInitTime).c_str());
    json->Put("loadAbcTime", std::to_string(rendererDumpInfo.createUiContenTime).c_str());
    std::string isReportFrameEvent = hostConfig_.isReportFrameEvent ? "true" : "false";
    json->Put("isReportFrameEvent", isReportFrameEvent.c_str());
}

void DynamicPattern::InitializeAccessibility()
{
    if (accessibilityChildTreeCallback_ != nullptr) {
        return;
    }
    ContainerScope scope(instanceId_);
    auto host = GetHost();
    CHECK_NULL_VOID(host);
    auto ngPipeline = host->GetContextRefPtr();
    CHECK_NULL_VOID(ngPipeline);
    auto frontend = ngPipeline->GetFrontend();
    CHECK_NULL_VOID(frontend);
    auto accessibilityManager = frontend->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    auto frameNode = frameNode_.Upgrade();
    CHECK_NULL_VOID(frameNode);
    int64_t accessibilityId = frameNode->GetAccessibilityId();
    accessibilityChildTreeCallback_ = std::make_shared<PlatformAccessibilityChildTreeCallback>(
        WeakClaim(this), accessibilityId);
    CHECK_NULL_VOID(accessibilityChildTreeCallback_);
    auto realHostWindowId = ngPipeline->GetRealHostWindowId();
    if (accessibilityManager->IsRegister()) {
        accessibilityChildTreeCallback_->OnRegister(
            realHostWindowId, accessibilityManager->GetTreeId());
    }
    PLATFORM_LOGI("SecurityUIExtension: %{public}" PRId64 " register child tree, realHostWindowId: %{public}u",
        accessibilityId, realHostWindowId);
    accessibilityManager->RegisterAccessibilityChildTreeCallback(accessibilityId, accessibilityChildTreeCallback_);
}

void DynamicPattern::OnAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId) const
{
    PLATFORM_LOGI("treeId: %{public}d, id: %{public}" PRId64, treeId, accessibilityId);
    if (dynamicComponentRenderer_ == nullptr) {
        PLATFORM_LOGI("dynamicComponentRenderer_ is null");
        return;
    }
    dynamicComponentRenderer_->TransferAccessibilityChildTreeRegister(windowId, treeId, accessibilityId);
}

void DynamicPattern::OnAccessibilityChildTreeDeregister() const
{
    PLATFORM_LOGI("deregister accessibility child tree");
    if (dynamicComponentRenderer_ == nullptr) {
        PLATFORM_LOGI("dynamicComponentRenderer_ is null");
        return;
    }
    dynamicComponentRenderer_->TransferAccessibilityChildTreeDeregister();
}

void DynamicPattern::OnSetAccessibilityChildTree(int32_t childWindowId, int32_t childTreeId) const
{
    auto frameNode = frameNode_.Upgrade();
    CHECK_NULL_VOID(frameNode);
    auto accessibilityProperty = frameNode->GetAccessibilityProperty<AccessibilityProperty>();
    CHECK_NULL_VOID(accessibilityProperty);
    accessibilityProperty->SetChildWindowId(childWindowId);
    accessibilityProperty->SetChildTreeId(childTreeId);
}

void DynamicPattern::OnAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info) const
{
    PLATFORM_LOGI("dump accessibility child info");
    if (dynamicComponentRenderer_ == nullptr) {
        PLATFORM_LOGI("dynamicComponentRenderer_ is null");
        return;
    }
    dynamicComponentRenderer_->TransferAccessibilityDumpChildInfo(params, info);
}

void DynamicPattern::ResetAccessibilityChildTreeCallback()
{
    CHECK_NULL_VOID(accessibilityChildTreeCallback_);
    ContainerScope scope(instanceId_);
    auto ngPipeline = NG::PipelineContext::GetCurrentContext();
    CHECK_NULL_VOID(ngPipeline);
    auto frontend = ngPipeline->GetFrontend();
    CHECK_NULL_VOID(frontend);
    auto accessibilityManager = frontend->GetAccessibilityManager();
    CHECK_NULL_VOID(accessibilityManager);
    accessibilityManager->DeregisterAccessibilityChildTreeCallback(
        accessibilityChildTreeCallback_->GetAccessibilityId());
    accessibilityChildTreeCallback_.reset();
    accessibilityChildTreeCallback_ = nullptr;
}
} // namespace OHOS::Ace::NG
