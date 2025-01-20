/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "core/components_ng/manager/drag_drop/drag_drop_initiating/drag_drop_initiating_state_ready.h"

#include "base/subwindow/subwindow_manager.h"
#include "core/common/container.h"
#include "core/components_ng/manager/drag_drop/drag_drop_behavior_reporter/drag_drop_behavior_reporter.h"
#include "core/components_ng/manager/drag_drop/drag_drop_func_wrapper.h"
#include "core/components_ng/manager/drag_drop/drag_drop_initiating/drag_drop_initiating_state_machine.h"
#include "core/components_ng/manager/drag_drop/drag_drop_manager.h"
#include "core/components_ng/manager/drag_drop/utils/drag_animation_helper.h"
#include "core/gestures/drag_event.h"
#include "core/pipeline_ng/pipeline_context.h"
namespace OHOS::Ace::NG {
void DragDropInitiatingStateReady::HandleOnDragStart(RefPtr<FrameNode> frameNode, const GestureEvent& info)
{
    CHECK_NULL_VOID(frameNode);
    auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipeline);
    auto dragDropManager = pipeline->GetDragDropManager();
    CHECK_NULL_VOID(dragDropManager);
    if (!CheckStatusForPanActionBegin(frameNode, info)) {
        return;
    }
    dragDropManager->ResetDragging(DragDropMgrState::ABOUT_TO_PREVIEW);
    frameNode->MarkModifyDone();
    dragDropManager->SetIsShowBadgeAnimation(true);
    HideEventColumn();
    HidePixelMap(true, info.GetGlobalLocation().GetX(), info.GetGlobalLocation().GetY());
    DragDropFuncWrapper::RecordMenuWrapperNodeForDrag(frameNode->GetId());
    UpdateDragPreviewOptionFromModifier();
    auto gestureHub = frameNode->GetOrCreateGestureEventHub();
    CHECK_NULL_VOID(gestureHub);
    auto gestureEvent = info;
    gestureHub->HandleOnDragStart(gestureEvent);
}

void DragDropInitiatingStateReady::HandlePanOnActionStart(const GestureEvent& info)
{
    auto machine = GetStateMachine();
    CHECK_NULL_VOID(machine);
    auto params = machine->GetDragDropInitiatingParams();
    auto frameNode = params.frameNode.Upgrade();
    CHECK_NULL_VOID(frameNode);
    if (info.GetSourceDevice() == SourceType::MOUSE) {
        HandleOnDragStart(frameNode, info);
    }
}

void DragDropInitiatingStateReady::HandleLongPressOnAction(const GestureEvent& info)
{
    auto machine = GetStateMachine();
    CHECK_NULL_VOID(machine);
    auto params = machine->GetDragDropInitiatingParams();
    auto frameNode = params.frameNode.Upgrade();
    CHECK_NULL_VOID(frameNode);
    auto focusHub = frameNode->GetFocusHub();
    if (focusHub ? focusHub->FindContextMenuOnKeyEvent(OnKeyEventType::CONTEXT_MENU) : false) {
        auto pipeline = PipelineContext::GetCurrentContextSafelyWithCheck();
        CHECK_NULL_VOID(pipeline);
        auto dragDropManager = pipeline->GetDragDropManager();
        CHECK_NULL_VOID(dragDropManager);
        dragDropManager->ResetContextMenuDragPosition();
        dragDropManager->SetDragDampStartPoint(info.GetGlobalPoint());
        dragDropManager->SetDraggingPointer(info.GetPointerId());
    }
    machine->RequestStatusTransition(AceType::Claim(this), static_cast<int32_t>(DragDropInitiatingStatus::PRESS));
}

void DragDropInitiatingStateReady::HandlePanOnReject()
{
    TAG_LOGI(AceLogTag::ACE_DRAG, "Trigger pan onReject");
    auto pipelineContext = PipelineContext::GetCurrentContextSafelyWithCheck();
    CHECK_NULL_VOID(pipelineContext);
    auto manager = pipelineContext->GetOverlayManager();
    CHECK_NULL_VOID(manager);
    if (manager->IsGatherWithMenu()) {
        return;
    }
    manager->RemoveGatherNode();
}

void DragDropInitiatingStateReady::HandleSequenceOnActionCancel(const GestureEvent& info)
{
    auto machine = GetStateMachine();
    CHECK_NULL_VOID(machine);
    machine->RequestStatusTransition(AceType::Claim(this), static_cast<int32_t>(DragDropInitiatingStatus::IDLE));
}
} // namespace OHOS::Ace::NG
