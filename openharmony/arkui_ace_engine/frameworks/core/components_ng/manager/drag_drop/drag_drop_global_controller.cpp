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

#include "core/components_ng/manager/drag_drop/drag_drop_global_controller.h"

#include <csignal>
#include <thread>

#include "core/components_ng/base/frame_node.h"

namespace OHOS::Ace::NG {

DragDropGlobalController::~DragDropGlobalController() {}

DragDropGlobalController& DragDropGlobalController::GetInstance()
{
    static DragDropGlobalController singleInstance;
    return singleInstance;
}

void DragDropGlobalController::UpdateMenuShowingStatus(bool isShowing)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    isContextMenuShowing_ = isShowing;
}

bool DragDropGlobalController::IsMenuShowing() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return isContextMenuShowing_;
}

void DragDropGlobalController::UpdateDragDropInitiatingStatus(const RefPtr<FrameNode>& frameNode,
    const DragDropInitiatingStatus& dragStatus)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    CHECK_NULL_VOID(frameNode);
    if (dragStatus == DragDropInitiatingStatus::MOVING) {
        currentDragNode_ = frameNode;
    }
}

bool DragDropGlobalController::IsInMoving() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return currentDragNode_;
}

void DragDropGlobalController::ResetDragDropInitiatingStatus()
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    currentDragNode_ = nullptr;
}

void DragDropGlobalController::SetPrepareDragFrameNode(const WeakPtr<FrameNode>& prepareDragFrameNode)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    prepareDragFrameNode_ = prepareDragFrameNode;
}

const WeakPtr<FrameNode> DragDropGlobalController::GetPrepareDragFrameNode() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return prepareDragFrameNode_;
}

void DragDropGlobalController::SetPreDragStatus(PreDragStatus preDragStatus)
{
    std::unique_lock<std::shared_mutex> lock(mutex_);
    preDragStatus_ = preDragStatus;
}

PreDragStatus DragDropGlobalController::GetPreDragStatus() const
{
    std::shared_lock<std::shared_mutex> lock(mutex_);
    return preDragStatus_;
}
} // namespace OHOS::Ace
