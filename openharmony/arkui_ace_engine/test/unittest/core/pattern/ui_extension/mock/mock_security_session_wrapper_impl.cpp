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
#define private public
#define protected public
#include "core/components_ng/pattern/ui_extension/security_ui_extension_component/security_session_wrapper_impl.h"

#include <cmath>
#include <memory>

#include "accessibility_event_info.h"
#include "refbase.h"
#include "session_manager/include/extension_session_manager.h"
#include "transaction/rs_sync_transaction_controller.h"
#include "transaction/rs_transaction.h"
#include "ui/rs_surface_node.h"
#include "want_params.h"
#include "wm/wm_common.h"

#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/osal/want_wrap_ohos.h"
#include "base/error/error_code.h"
#include "base/utils/utils.h"
#include "core/common/container.h"
#include "core/common/container_scope.h"
#include "core/components_ng/pattern/ui_extension/session_wrapper.h"
#include "core/components_ng/pattern/window_scene/helper/window_scene_helper.h"
#include "core/components_ng/pattern/window_scene/scene/system_window_scene.h"
#include "core/pipeline_ng/pipeline_context.h"

namespace OHOS::Ace::NG {

SecuritySessionWrapperImpl::SecuritySessionWrapperImpl(
    const WeakPtr<SecurityUIExtensionPattern>& hostPattern,
    int32_t instanceId, bool isTransferringCaller, SessionType sessionType)
    : hostPattern_(hostPattern), instanceId_(instanceId), isTransferringCaller_(isTransferringCaller),
      sessionType_(sessionType)
{
    auto pattern = hostPattern.Upgrade();
    platformId_ = pattern ? pattern->GetUiExtensionId() : 0;
    taskExecutor_ = Container::CurrentTaskExecutor();
    Rosen::SessionInfo sessionInfo = {
        .bundleName_ = "123",
        .abilityName_ = "123",
    };
    session_ = new OHOS::Rosen::ExtensionSession(sessionInfo);
}

SecuritySessionWrapperImpl::~SecuritySessionWrapperImpl() {}

void SecuritySessionWrapperImpl::CreateSession(const AAFwk::Want& want, const SessionConfig& config)
{
}

void SecuritySessionWrapperImpl::DestroySession()
{
}

bool SecuritySessionWrapperImpl::IsSessionValid()
{
    if (session_) {
        return true;
    }
    return false;
}

int32_t SecuritySessionWrapperImpl::GetSessionId() const
{
    return 1;
}

const std::shared_ptr<AAFwk::Want> SecuritySessionWrapperImpl::GetWant()
{
    RefPtr<WantWrap> wantWrap = AceType::MakeRefPtr<WantWrapOhos>("123", "123");
    auto wantWrapOhos = AceType::DynamicCast<WantWrapOhos>(wantWrap);
    auto want = wantWrapOhos->GetWant();
    return std::make_shared<AAFwk::Want>(want);
}

bool SecuritySessionWrapperImpl::NotifyFocusEventSync(bool isFocus)
{
    return true;
}
bool SecuritySessionWrapperImpl::NotifyFocusStateSync(bool focusState)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyBackPressedSync()
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyPointerEventSync(
    const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyKeyEventSync(
    const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent, bool isPreIme)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyKeyEventAsync(
    const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent, bool isPreIme)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyAxisEventSync(
    const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyFocusEventAsync(bool isFocus)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyFocusStateAsync(bool focusState)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyBackPressedAsync()
{
    return false;
}

bool SecuritySessionWrapperImpl::NotifyPointerEventAsync(
    const std::shared_ptr<OHOS::MMI::PointerEvent>& pointerEvent)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyKeyEventAsync(
    const std::shared_ptr<OHOS::MMI::KeyEvent>& keyEvent)
{
    return true;
}

bool SecuritySessionWrapperImpl::NotifyAxisEventAsync(
    const std::shared_ptr<OHOS::MMI::AxisEvent>& axisEvent)
{
    return true;
}

void SecuritySessionWrapperImpl::NotifyCreate()
{
}

void SecuritySessionWrapperImpl::NotifyForeground()
{
}

void SecuritySessionWrapperImpl::NotifyBackground(bool isHandleError)
{
}

void SecuritySessionWrapperImpl::NotifyDestroy(bool isHandleError)
{
}

void SecuritySessionWrapperImpl::NotifyConfigurationUpdate()
{
}

void SecuritySessionWrapperImpl::OnConnect()
{
}

void SecuritySessionWrapperImpl::OnDisconnect(bool isAbnormal)
{
}

void SecuritySessionWrapperImpl::OnExtensionTimeout(int32_t /* errorCode */)
{
}

void SecuritySessionWrapperImpl::OnAccessibilityEvent(
    const Accessibility::AccessibilityEventInfo& info, int64_t offset)
{
}

void SecuritySessionWrapperImpl::OnExtensionDetachToDisplay()
{
}

void SecuritySessionWrapperImpl::TransferAccessibilityHoverEvent(float pointX, float pointY, int32_t sourceType,
    int32_t eventType, int64_t timeMs)
{
}

void SecuritySessionWrapperImpl::TransferAccessibilityChildTreeRegister(
    uint32_t windowId, int32_t treeId, int64_t accessibilityId)
{
}

void SecuritySessionWrapperImpl::TransferAccessibilityChildTreeDeregister()
{
}

void SecuritySessionWrapperImpl::TransferAccessibilityDumpChildInfo(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
}

std::shared_ptr<Rosen::RSSurfaceNode> SecuritySessionWrapperImpl::GetSurfaceNode() const
{
    if (session_) {
        std::string surfaceNodeName = "SecurityUIExtensionSurfaceNode";
        struct Rosen::RSSurfaceNodeConfig surfaceNodeConfig = { .SurfaceNodeName = surfaceNodeName };
        std::shared_ptr<Rosen::RSSurfaceNode> rsSurfaceNode;
        rsSurfaceNode = std::make_shared<OHOS::Rosen::RSSurfaceNode>(surfaceNodeConfig, true);
        return rsSurfaceNode;
    }
    return nullptr;
}

void SecuritySessionWrapperImpl::NotifyDisplayArea(const RectF& displayArea)
{
}

void SecuritySessionWrapperImpl::NotifySizeChangeReason(
    WindowSizeChangeReason type, const std::shared_ptr<Rosen::RSTransaction>& rsTransaction)
{
}

void SecuritySessionWrapperImpl::NotifyOriginAvoidArea(
    const Rosen::AvoidArea& avoidArea, uint32_t type) const
{
}

bool SecuritySessionWrapperImpl::NotifyOccupiedAreaChangeInfo(
    sptr<Rosen::OccupiedAreaChangeInfo> info, bool needWaitLayout)
{
    return true;
}

void SecuritySessionWrapperImpl::SetDensityDpiImpl(bool isDensityDpi)
{
}

void SecuritySessionWrapperImpl::SendDataAsync(const AAFwk::WantParams& params) const
{
}

int32_t SecuritySessionWrapperImpl::SendDataSync(
    const AAFwk::WantParams& wantParams, AAFwk::WantParams& reWantParams) const
{
    return 1;
}

uint32_t SecuritySessionWrapperImpl::GetReasonDump() const
{
    return 1;
}

void SecuritySessionWrapperImpl::NotifyUieDump(
    const std::vector<std::string>& params, std::vector<std::string>& info)
{
}

bool SecuritySessionWrapperImpl::SendBusinessDataSyncReply(
    UIContentBusinessCode code, const AAFwk::Want& data, AAFwk::Want& reply, RSSubsystemId subSystemId)
{
    return false;
}

bool SecuritySessionWrapperImpl::SendBusinessData(
    UIContentBusinessCode code, const AAFwk::Want& data, BusinessDataSendType type, RSSubsystemId subSystemId)
{
    return false;
}

void SecuritySessionWrapperImpl::ReDispatchWantParams() {}

void SecuritySessionWrapperImpl::DispatchExtensionDataToHostWindow(uint32_t customId, const AAFwk::Want& data)
{
}
} // namespace OHOS::Ace::NG
