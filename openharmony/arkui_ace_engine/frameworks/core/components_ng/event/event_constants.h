/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_CONSTANTS_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_CONSTANTS_H

#include <cstddef>
#include <cstdint>
enum class MenuPreviewMode {
    NONE,
    IMAGE,
    CUSTOM,
};

enum class MenuBindingType {
    LONG_PRESS,
    RIGHT_CLICK,
};

namespace OHOS::Ace::NG {

enum class HitTestMode {
    /**
     *  Both self and children respond to the hit test for touch events,
     *  but block hit test of the other nodes which is masked by this node.
     */
    HTMDEFAULT = 0,

    /**
     * Self respond to the hit test for touch events,
     * but block hit test of children and other nodes which is masked by this node.
     */
    HTMBLOCK,

    /**
     * Self and child respond to the hit test for touch events,
     * and allow hit test of other nodes which is masked by this node.
     */
    HTMTRANSPARENT,

    /**
     * Self not respond to the hit test for touch events,
     * but children respond to the hit test for touch events.
     */
    HTMNONE,

    /**
     * Self and child respond to the hit test for touch events,
     * when self consumed allow hit test of other nodes which is masked by this node,
     * when child consumed block hit test of other nodes.
     */
    HTMTRANSPARENT_SELF,
};

enum class TouchTestStrategy { DEFAULT = 0, FORWARD_COMPETITION, FORWARD };

enum class HitTestResult {
    // The touch point is located outside the current component area;
    OUT_OF_REGION,
    // node consumption events and prevent bubbling;
    STOP_BUBBLING,
    // node process events and bubble;
    BUBBLING,
    // node process events and bubble;
    SELF_TRANSPARENT,
};

enum class DragFuncType {
    DRAG_ENTER,
    DRAG_LEAVE,
    DRAG_MOVE,
    DRAG_DROP,
    DRAG_END,
};

enum class EventTreeType {
    TOUCH = 0,
    POST_EVENT,
};

} // namespace OHOS::Ace::NG

namespace OHOS::Ace {

static const int32_t TOUCH_TOOL_BASE_ID = 100;

enum class TouchType : size_t {
    DOWN = 0,
    UP,
    MOVE,
    CANCEL,
    PULL_DOWN,
    PULL_UP,
    PULL_MOVE,
    PULL_IN_WINDOW,
    PULL_OUT_WINDOW,
    HOVER_ENTER,
    HOVER_MOVE,
    HOVER_EXIT,
    HOVER_CANCEL,
    PROXIMITY_IN,
    PROXIMITY_OUT,
    UNKNOWN,
};

enum class UIInputEventType {
    NONE = 0,
    TOUCH,
    AXIS,
    KEY,
    NON_POINTER_AXIS,
};

enum class KeyIntention : int32_t {
    INTENTION_UNKNOWN = -1,
    INTENTION_UP = 1,
    INTENTION_DOWN = 2,
    INTENTION_LEFT = 3,
    INTENTION_RIGHT = 4,
    INTENTION_SELECT = 5,
    INTENTION_ESCAPE = 6,
    INTENTION_BACK = 7,
    INTENTION_FORWARD = 8,
    INTENTION_MENU = 9,
    INTENTION_HOME = 10,
    INTENTION_PAGE_UP = 11,
    INTENTION_PAGE_DOWN = 12,
    INTENTION_ZOOM_OUT = 13,
    INTENTION_ZOOM_IN = 14,

    INTENTION_MEDIA_PLAY_PAUSE = 100,
    INTENTION_MEDIA_FAST_FORWARD = 101,
    INTENTION_MEDIA_FAST_REWIND = 102,
    INTENTION_MEDIA_FAST_PLAYBACK = 103,
    INTENTION_MEDIA_NEXT = 104,
    INTENTION_MEDIA_PREVIOUS = 105,
    INTENTION_MEDIA_MUTE = 106,
    INTENTION_VOLUTE_UP = 107,
    INTENTION_VOLUTE_DOWN = 108,

    INTENTION_CALL = 200,
    INTENTION_ENDCALL = 201,
    INTENTION_REJECTCALL = 202,

    INTENTION_CAMERA = 300,
};
} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_EVENT_CONSTANTS_H
