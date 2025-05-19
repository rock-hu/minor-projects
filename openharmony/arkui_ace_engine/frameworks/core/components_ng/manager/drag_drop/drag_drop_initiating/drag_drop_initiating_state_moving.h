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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MANAGER_DRAG_DROP_DRAG_DROP_INITIATING_STATE_MOVING_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MANAGER_DRAG_DROP_DRAG_DROP_INITIATING_STATE_MOVING_H

#include "core/components_ng/base/frame_node.h"
#include "core/components_ng/event/touch_event.h"
#include "core/components_ng/gestures/gesture_info.h"
#include "core/components_ng/manager/drag_drop/drag_drop_global_controller.h"
#include "core/components_ng/manager/drag_drop/drag_drop_initiating/drag_drop_initiating_state_base.h"
#include "core/event/pointer_event.h"
#include "core/gestures/gesture_info.h"

namespace OHOS::Ace::NG {
class DragDropInitiatingStateMachine;

class DragDropInitiatingStateMoving : public DragDropInitiatingStateBase {
    DECLARE_ACE_TYPE(DragDropInitiatingStateMoving, DragDropInitiatingStateBase);

public:
    DragDropInitiatingStateMoving(const WeakPtr<DragDropInitiatingStateMachine>& machine)
        : DragDropInitiatingStateBase(machine)
    {}
    ~DragDropInitiatingStateMoving() = default;

    void HandlePanOnActionEnd(const GestureEvent& info) override;
};
} // namespace OHOS::Ace::NG
#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_MANAGER_DRAG_DROP_DRAG_DROP_INITIATING_STATE_MOVING_H