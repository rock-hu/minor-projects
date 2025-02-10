/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "adapter/ohos/entrance/mmi_event_convertor.h"

#include "input_manager.h"

#include "adapter/ohos/entrance/ace_extra_input_data.h"
#include "adapter/ohos/entrance/ace_container.h"
#include "adapter/ohos/entrance/tsa_advanced_feature.h"

namespace OHOS::Ace::Platform {
namespace {
constexpr int32_t ANGLE_0 = 0;
constexpr int32_t ANGLE_90 = 90;
constexpr int32_t ANGLE_180 = 180;
constexpr int32_t ANGLE_270 = 270;
constexpr double SIZE_DIVIDE = 2.0;
} // namespace

SourceTool GetSourceTool(int32_t orgToolType)
{
    switch (orgToolType) {
        case OHOS::MMI::PointerEvent::TOOL_TYPE_FINGER:
            return SourceTool::FINGER;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_PEN:
            return SourceTool::PEN;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_RUBBER:
            return SourceTool::RUBBER;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_BRUSH:
            return SourceTool::BRUSH;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_PENCIL:
            return SourceTool::PENCIL;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_AIRBRUSH:
            return SourceTool::AIRBRUSH;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_MOUSE:
            return SourceTool::MOUSE;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_LENS:
            return SourceTool::LENS;
        case OHOS::MMI::PointerEvent::TOOL_TYPE_TOUCHPAD:
            return SourceTool::TOUCHPAD;
        default:
            LOGW("unknown tool type");
            return SourceTool::UNKNOWN;
    }
}

TouchPoint ConvertTouchPoint(const MMI::PointerEvent::PointerItem& pointerItem)
{
    TouchPoint touchPoint;
    // just get the max of width and height
    touchPoint.size = std::max(pointerItem.GetWidth(), pointerItem.GetHeight()) / 2.0;
    touchPoint.id = pointerItem.GetPointerId();
    touchPoint.downTime = TimeStamp(std::chrono::microseconds(pointerItem.GetDownTime()));
    touchPoint.x = pointerItem.GetWindowX();
    touchPoint.y = pointerItem.GetWindowY();
    touchPoint.screenX = pointerItem.GetDisplayX();
    touchPoint.screenY = pointerItem.GetDisplayY();
    touchPoint.isPressed = pointerItem.IsPressed();
    touchPoint.force = static_cast<float>(pointerItem.GetPressure());
    touchPoint.tiltX = pointerItem.GetTiltX();
    touchPoint.tiltY = pointerItem.GetTiltY();
    touchPoint.sourceTool = GetSourceTool(pointerItem.GetToolType());
    touchPoint.originalId = pointerItem.GetOriginPointerId();
    touchPoint.width = pointerItem.GetWidth();
    touchPoint.height = pointerItem.GetHeight();
    touchPoint.operatingHand = pointerItem.GetBlobId() & (OPERATING_HAND_LEFT | OPERATING_HAND_RIGHT);
    return touchPoint;
}

void UpdateTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, TouchEvent& touchEvent)
{
    CHECK_NULL_VOID(pointerEvent);
    auto ids = pointerEvent->GetPointerIds();
    for (auto&& id : ids) {
        MMI::PointerEvent::PointerItem item;
        bool ret = pointerEvent->GetPointerItem(id, item);
        if (!ret) {
            LOGE("get pointer item failed.");
            continue;
        }
        auto touchPoint = ConvertTouchPoint(item);
        touchEvent.pointers.emplace_back(std::move(touchPoint));
    }
    touchEvent.CovertId();
}

Offset GetTouchEventOriginOffset(const TouchEvent& event)
{
    auto pointerEvent = event.pointerEvent;
    if (!pointerEvent) {
        return Offset();
    }
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        return Offset();
    } else {
        return Offset(item.GetWindowX(), item.GetWindowY());
    }
}

TimeStamp GetTouchEventOriginTimeStamp(const TouchEvent& event)
{
    auto pointerEvent = event.pointerEvent;
    if (!pointerEvent) {
        return event.time;
    }
    std::chrono::microseconds microseconds(pointerEvent->GetActionTime());
    TimeStamp time(microseconds);
    return time;
}

void UpdatePressedKeyCodes(std::vector<KeyCode>& pressedKeyCodes)
{
    auto inputManager = MMI::InputManager::GetInstance();
    CHECK_NULL_VOID(inputManager);

    std::vector<int32_t> pressedKeys;
    std::map<int32_t, int32_t> specialKeysState;
    pressedKeyCodes.clear();
    auto ret = inputManager->GetKeyState(pressedKeys, specialKeysState);
    if (ret == 0) {
        for (const auto& curCode : pressedKeys) {
            pressedKeyCodes.emplace_back(static_cast<KeyCode>(curCode));
        }
    }
}

void UpdateMouseEventForPen(const MMI::PointerEvent::PointerItem& pointerItem, MouseEvent& mouseEvent)
{
    if (mouseEvent.sourceType != SourceType::TOUCH || mouseEvent.sourceTool != SourceTool::PEN) {
        return;
    }
    mouseEvent.id = TOUCH_TOOL_BASE_ID + static_cast<int32_t>(mouseEvent.sourceTool);
    // Pen use type double XY position.
    mouseEvent.x = pointerItem.GetWindowXPos();
    mouseEvent.y = pointerItem.GetWindowYPos();
    mouseEvent.screenX = pointerItem.GetDisplayXPos();
    mouseEvent.screenY = pointerItem.GetDisplayYPos();
    mouseEvent.originalId = mouseEvent.id;
}

TouchEvent ConvertTouchEventFromTouchPoint(TouchPoint touchPoint)
{
    TouchEvent event;
    event.SetId(touchPoint.id)
        .SetX(touchPoint.x)
        .SetY(touchPoint.y)
        .SetScreenX(touchPoint.screenX)
        .SetScreenY(touchPoint.screenY)
        .SetType(TouchType::UNKNOWN)
        .SetPullType(TouchType::UNKNOWN)
        .SetSize(touchPoint.size)
        .SetForce(touchPoint.force)
        .SetTiltX(touchPoint.tiltX)
        .SetTiltY(touchPoint.tiltY)
        .SetSourceType(SourceType::NONE)
        .SetSourceTool(touchPoint.sourceTool)
        .SetOriginalId(touchPoint.originalId)
        .SetSourceType(SourceType::NONE)
        .SetOperatingHand(touchPoint.operatingHand)
        .SetPressedTime(touchPoint.downTime)
        .SetWidth(touchPoint.width)
        .SetHeight(touchPoint.height);
    return event;
}

TouchEvent ConvertTouchEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent)
{
    CHECK_NULL_RETURN(pointerEvent, TouchEvent());
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        LOGE("get pointer item failed.");
        return TouchEvent();
    }
    auto touchPoint = ConvertTouchPoint(item);
    TouchEvent event = ConvertTouchEventFromTouchPoint(touchPoint);
    std::chrono::microseconds microseconds(pointerEvent->GetActionTime());
    TimeStamp time(microseconds);
    event.SetTime(time)
        .SetDeviceId(pointerEvent->GetDeviceId())
        .SetTargetDisplayId(pointerEvent->GetTargetDisplayId())
        .SetTouchEventId(pointerEvent->GetId());
    AceExtraInputData::ReadToTouchEvent(pointerEvent, event);
    event.pointerEvent = pointerEvent;
    int32_t orgDevice = pointerEvent->GetSourceType();
    GetEventDevice(orgDevice, event);
    int32_t orgAction = pointerEvent->GetPointerAction();
    SetTouchEventType(orgAction, event);
    event.isPrivacyMode = pointerEvent->HasFlag(OHOS::MMI::InputEvent::EVENT_FLAG_PRIVACY_MODE);
    UpdateTouchEvent(pointerEvent, event);
    if (event.sourceType == SourceType::TOUCH && event.sourceTool == SourceTool::PEN) {
        // Pen use type double XY position.
        event.x = item.GetWindowXPos();
        event.y = item.GetWindowYPos();
        event.screenX = item.GetDisplayXPos();
        event.screenY = item.GetDisplayYPos();
    }
    event.pressedKeyCodes_.clear();
    for (const auto& curCode : pointerEvent->GetPressedKeys()) {
        event.pressedKeyCodes_.emplace_back(static_cast<KeyCode>(curCode));
    }
    return event;
}

void SetTouchEventType(int32_t orgAction, TouchEvent& event)
{
    std::map<int32_t, TouchType> actionMap = {
        { OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL, TouchType::CANCEL },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN, TouchType::DOWN },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE, TouchType::MOVE },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_UP, TouchType::UP },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_DOWN, TouchType::PULL_DOWN },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_MOVE, TouchType::PULL_MOVE },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_UP, TouchType::PULL_UP },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW, TouchType::PULL_IN_WINDOW },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW, TouchType::PULL_OUT_WINDOW },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_HOVER_ENTER, TouchType::HOVER_ENTER },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_HOVER_MOVE, TouchType::HOVER_MOVE },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_HOVER_EXIT, TouchType::HOVER_EXIT },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_HOVER_CANCEL, TouchType::HOVER_CANCEL },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PROXIMITY_IN, TouchType::PROXIMITY_IN },
        { OHOS::MMI::PointerEvent::POINTER_ACTION_PROXIMITY_OUT, TouchType::PROXIMITY_OUT },
    };
    auto typeIter = actionMap.find(orgAction);
    if (typeIter == actionMap.end()) {
        TAG_LOGI(AceLogTag::ACE_INPUTKEYFLOW, "unknown touch type");
        return;
    }
    event.type = typeIter->second;
    if (typeIter->second == TouchType::PULL_DOWN || typeIter->second == TouchType::PULL_MOVE ||
        typeIter->second == TouchType::PULL_UP || typeIter->second == TouchType::PULL_IN_WINDOW ||
        typeIter->second == TouchType::PULL_OUT_WINDOW) {
        event.pullType = typeIter->second;
    }
}

void GetMouseEventAction(int32_t action, MouseEvent& events, bool isScenceBoardWindow)
{
    switch (action) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_DOWN:
            events.action = MouseAction::PRESS;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_BUTTON_UP:
            events.action = MouseAction::RELEASE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_ENTER_WINDOW:
            events.action = MouseAction::WINDOW_ENTER;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_LEAVE_WINDOW:
            events.action = MouseAction::WINDOW_LEAVE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE:
            events.action = MouseAction::MOVE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_DOWN:
            events.action = MouseAction::PRESS;
            events.pullAction = MouseAction::PULL_DOWN;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_MOVE:
            events.action = MouseAction::MOVE;
            events.pullAction = MouseAction::PULL_MOVE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW:
            events.action = MouseAction::WINDOW_ENTER;
            events.pullAction = MouseAction::PULL_MOVE;
            return;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW:
            events.action = MouseAction::WINDOW_LEAVE;
            events.pullAction = MouseAction::PULL_MOVE;
            return;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_UP:
            events.action = MouseAction::RELEASE;
            events.pullAction = MouseAction::PULL_UP;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL:
            events.action = MouseAction::CANCEL;
            break;
        default:
            events.action = MouseAction::NONE;
            break;
    }
}

MouseButton GetMouseEventButton(int32_t button)
{
    switch (button) {
        case OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT:
            return MouseButton::LEFT_BUTTON;
        case OHOS::MMI::PointerEvent::MOUSE_BUTTON_RIGHT:
            return MouseButton::RIGHT_BUTTON;
        case OHOS::MMI::PointerEvent::MOUSE_BUTTON_MIDDLE:
            return MouseButton::MIDDLE_BUTTON;
        case OHOS::MMI::PointerEvent::MOUSE_BUTTON_SIDE:
            return MouseButton::BACK_BUTTON;
        case OHOS::MMI::PointerEvent::MOUSE_BUTTON_EXTRA:
            return MouseButton::FORWARD_BUTTON;
        default:
            return MouseButton::NONE_BUTTON;
    }
}

void ConvertMouseEvent(
    const std::shared_ptr<MMI::PointerEvent>& pointerEvent, MouseEvent& events, bool isScenceBoardWindow)
{
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        LOGE("get pointer: %{public}d item failed.", pointerID);
        return;
    }
    events.id = pointerID;
    events.x = item.GetWindowX();
    events.y = item.GetWindowY();
    events.screenX = item.GetDisplayX();
    events.screenY = item.GetDisplayY();
    events.rawDeltaX = item.GetRawDx();
    events.rawDeltaY = item.GetRawDy();
    int32_t orgAction = pointerEvent->GetPointerAction();
    GetMouseEventAction(orgAction, events, isScenceBoardWindow);
    int32_t orgButton = pointerEvent->GetButtonId();
    events.button = GetMouseEventButton(orgButton);
    int32_t orgDevice = pointerEvent->GetSourceType();
    GetEventDevice(orgDevice, events);
    events.isPrivacyMode = pointerEvent->HasFlag(OHOS::MMI::InputEvent::EVENT_FLAG_PRIVACY_MODE);
    events.targetDisplayId = pointerEvent->GetTargetDisplayId();
    events.originalId = item.GetOriginPointerId();
    events.deviceId = pointerEvent->GetDeviceId();

    std::set<int32_t> pressedSet = pointerEvent->GetPressedButtons();
    uint32_t pressedButtons = 0;
    if (pressedSet.find(OHOS::MMI::PointerEvent::MOUSE_BUTTON_LEFT) != pressedSet.end()) {
        pressedButtons &= static_cast<uint32_t>(MouseButton::LEFT_BUTTON);
    }
    if (pressedSet.find(OHOS::MMI::PointerEvent::MOUSE_BUTTON_RIGHT) != pressedSet.end()) {
        pressedButtons &= static_cast<uint32_t>(MouseButton::RIGHT_BUTTON);
    }
    if (pressedSet.find(OHOS::MMI::PointerEvent::MOUSE_BUTTON_MIDDLE) != pressedSet.end()) {
        pressedButtons &= static_cast<uint32_t>(MouseButton::MIDDLE_BUTTON);
    }
    events.pressedButtons = static_cast<int32_t>(pressedButtons);

    for (const auto& pressedButton : pressedSet) {
        events.pressedButtonsArray.emplace_back(GetMouseEventButton(pressedButton));
    }
    events.time = TimeStamp(std::chrono::microseconds(pointerEvent->GetActionTime()));
    events.pointerEvent = pointerEvent;
    events.sourceTool = GetSourceTool(item.GetToolType());
    UpdateMouseEventForPen(item, events);
    events.touchEventId = pointerEvent->GetId();
    events.pressedKeyCodes_.clear();
    for (const auto& curCode : pointerEvent->GetPressedKeys()) {
        events.pressedKeyCodes_.emplace_back(static_cast<KeyCode>(curCode));
    }
}

void GetAxisEventAction(int32_t action, AxisEvent& event)
{
    switch (action) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN:
        case OHOS::MMI::PointerEvent::POINTER_ACTION_ROTATE_BEGIN:
            event.action = AxisAction::BEGIN;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE:
        case OHOS::MMI::PointerEvent::POINTER_ACTION_ROTATE_UPDATE:
            event.action = AxisAction::UPDATE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_END:
        case OHOS::MMI::PointerEvent::POINTER_ACTION_ROTATE_END:
            event.action = AxisAction::END;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL:
            event.action = AxisAction::CANCEL;
            break;
        default:
            event.action = AxisAction::NONE;
            break;
    }
}

void GetNonPointerAxisEventAction(int32_t action, NG::FocusAxisEvent& event)
{
    switch (action) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN:
            event.action = AxisAction::BEGIN;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE:
            event.action = AxisAction::UPDATE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_AXIS_END:
            event.action = AxisAction::END;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL:
            event.action = AxisAction::CANCEL;
            break;
        default:
            event.action = AxisAction::NONE;
            break;
    }
}

void ConvertCrownEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, CrownEvent& event)
{
    event.touchEventId = pointerEvent->GetId();
    int32_t pointerAction = pointerEvent->GetPointerAction();
    if (pointerAction == MMI::PointerEvent::POINTER_ACTION_AXIS_BEGIN) {
        event.action = Ace::CrownAction::BEGIN;
    } else if (pointerAction == MMI::PointerEvent::POINTER_ACTION_AXIS_UPDATE) {
        event.action = Ace::CrownAction::UPDATE;
    } else if (pointerAction == MMI::PointerEvent::POINTER_ACTION_AXIS_END) {
        event.action = Ace::CrownAction::END;
    } else {
        event.action = Ace::CrownAction::UNKNOWN;
    }
    int32_t orgDevice = pointerEvent->GetSourceType();
    GetEventDevice(orgDevice, event);
    event.angularVelocity = pointerEvent->GetVelocity();
    event.degree =  pointerEvent->GetAxisValue(MMI::PointerEvent::AXIS_TYPE_SCROLL_VERTICAL);
    std::chrono::microseconds microseconds(pointerEvent->GetActionTime());
    TimeStamp time(microseconds);
    event.timeStamp = time;
    event.SetPointerEvent(pointerEvent);
}

void ConvertAxisEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, AxisEvent& event)
{
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        LOGE("get pointer: %{public}d item failed.", pointerID);
        return;
    }

    event.id = item.GetPointerId();
    event.x = static_cast<float>(item.GetWindowX());
    event.y = static_cast<float>(item.GetWindowY());
    event.screenX = static_cast<float>(item.GetDisplayX());
    event.screenY = static_cast<float>(item.GetDisplayY());
    event.horizontalAxis = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_SCROLL_HORIZONTAL);
    event.verticalAxis = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_SCROLL_VERTICAL);
    event.pinchAxisScale = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_PINCH);
    event.rotateAxisAngle = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ROTATE);
    int32_t orgAction = pointerEvent->GetPointerAction();
    GetAxisEventAction(orgAction, event);
    event.isRotationEvent = (orgAction >= MMI::PointerEvent::POINTER_ACTION_ROTATE_BEGIN) &&
                            (orgAction <= MMI::PointerEvent::POINTER_ACTION_ROTATE_END);
    event.scrollStep = pointerEvent->GetScrollRows();
    int32_t orgDevice = pointerEvent->GetSourceType();
    GetEventDevice(orgDevice, event);
    event.sourceTool = GetSourceTool(item.GetToolType());
    event.pointerEvent = pointerEvent;
    event.originalId = item.GetOriginPointerId();
    event.deviceId = pointerEvent->GetDeviceId();

    std::chrono::microseconds microseconds(pointerEvent->GetActionTime());
    TimeStamp time(microseconds);
    event.time = time;
    event.touchEventId = pointerEvent->GetId();
    event.targetDisplayId = pointerEvent->GetTargetDisplayId();
    event.pressedCodes.clear();
    for (const auto& curCode : pointerEvent->GetPressedKeys()) {
        event.pressedCodes.emplace_back(static_cast<KeyCode>(curCode));
    }
}

void ConvertKeyEvent(const std::shared_ptr<MMI::KeyEvent>& keyEvent, KeyEvent& event)
{
    CHECK_NULL_VOID(keyEvent);
    event.rawKeyEvent = keyEvent;
    event.code = static_cast<KeyCode>(keyEvent->GetKeyCode());
    event.numLock = keyEvent->GetFunctionKey(MMI::KeyEvent::NUM_LOCK_FUNCTION_KEY);
    event.keyIntention = static_cast<KeyIntention>(keyEvent->GetKeyIntention());
    if (keyEvent->GetKeyAction() == OHOS::MMI::KeyEvent::KEY_ACTION_UP) {
        event.action = KeyAction::UP;
    } else if (keyEvent->GetKeyAction() == OHOS::MMI::KeyEvent::KEY_ACTION_DOWN) {
        event.action = KeyAction::DOWN;
    } else {
        event.action = KeyAction::UNKNOWN;
    }
    auto keyItems = keyEvent->GetKeyItems();
    for (auto item = keyItems.rbegin(); item != keyItems.rend(); item++) {
        if (item->GetKeyCode() == keyEvent->GetKeyCode()) {
            event.unicode = item->GetUnicode();
            break;
        } else {
            event.unicode = 0;
        }
    }

    std::chrono::microseconds microseconds(keyEvent->GetActionTime());
    TimeStamp time(microseconds);
    event.timeStamp = time;
    event.key.assign(MMI::KeyEvent::KeyCodeToString(keyEvent->GetKeyCode()));
    event.deviceId = keyEvent->GetDeviceId();
    int32_t orgDevice = keyEvent->GetSourceType();
    event.sourceType =
        orgDevice == MMI::PointerEvent::SOURCE_TYPE_JOYSTICK ? SourceType::JOYSTICK : SourceType::KEYBOARD;
#ifdef SECURITY_COMPONENT_ENABLE
    event.enhanceData = keyEvent->GetEnhanceData();
#endif
    event.pressedCodes.clear();
    for (const auto& curCode : keyEvent->GetPressedKeys()) {
        event.pressedCodes.emplace_back(static_cast<KeyCode>(curCode));
    }
    event.enableCapsLock = keyEvent->GetFunctionKey(MMI::KeyEvent::CAPS_LOCK_FUNCTION_KEY);
    event.numLock = keyEvent->GetFunctionKey(MMI::KeyEvent::NUM_LOCK_FUNCTION_KEY);
}

void ConvertFocusAxisEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, NG::FocusAxisEvent& event)
{
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        LOGE("get pointer: %{public}d item failed.", pointerID);
        return;
    }

    event.id = item.GetPointerId();
    event.absXValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_X);
    event.absYValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_Y);
    event.absZValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_Z);
    event.absRzValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_RZ);
    event.absHat0XValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0X);
    event.absHat0YValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_HAT0Y);
    event.absBrakeValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_BRAKE);
    event.absGasValue = pointerEvent->GetAxisValue(OHOS::MMI::PointerEvent::AxisType::AXIS_TYPE_ABS_GAS);
    int32_t orgAction = pointerEvent->GetPointerAction();
    GetNonPointerAxisEventAction(orgAction, event);
    int32_t orgDevice = pointerEvent->GetSourceType();
    GetEventDevice(orgDevice, event);
    event.sourceTool = SourceTool::JOYSTICK;
    event.pointerEvent = pointerEvent;
    event.originalId = item.GetOriginPointerId();
    event.deviceId = pointerEvent->GetDeviceId();

    std::chrono::microseconds microseconds(pointerEvent->GetActionTime());
    TimeStamp time(microseconds);
    event.time = time;
    event.touchEventId = pointerEvent->GetId();
    event.targetDisplayId = pointerEvent->GetTargetDisplayId();
    event.pressedCodes.clear();
    for (const auto& curCode : pointerEvent->GetPressedKeys()) {
        event.pressedCodes.emplace_back(static_cast<KeyCode>(curCode));
    }
}

void GetPointerEventAction(int32_t action, DragPointerEvent& event)
{
    switch (action) {
        case OHOS::MMI::PointerEvent::POINTER_ACTION_CANCEL:
            event.action = PointerAction::CANCEL;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN:
            event.action = PointerAction::DOWN;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_MOVE:
            event.action = PointerAction::MOVE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_UP:
            event.action = PointerAction::UP;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_MOVE:
            event.action = PointerAction::PULL_MOVE;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_UP:
            event.action = PointerAction::PULL_UP;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW:
            event.action = PointerAction::PULL_IN_WINDOW;
            break;
        case OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW:
            event.action = PointerAction::PULL_OUT_WINDOW;
            break;
        default:
            event.action = PointerAction::UNKNOWN;
            break;
    }
}

/**
 * Only for UIExtension to convert drag event type and dispatch.
 */
void UpdatePointerAction(std::shared_ptr<MMI::PointerEvent>& pointerEvent, const PointerAction action)
{
    if (action == PointerAction::PULL_IN_WINDOW) {
        pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_IN_WINDOW);
    }
    if (action == PointerAction::PULL_OUT_WINDOW) {
        pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_OUT_WINDOW);
    }
    if (action == PointerAction::UP) {
        pointerEvent->SetPointerAction(OHOS::MMI::PointerEvent::POINTER_ACTION_PULL_UP);
    }
}

bool GetPointerEventToolType(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, int32_t& toolType)
{
    int32_t pointerID = pointerEvent->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = pointerEvent->GetPointerItem(pointerID, item);
    if (!ret) {
        TAG_LOGE(AceLogTag::ACE_INPUTTRACKING, "get pointer: %{public}d item failed.", pointerID);
        return false;
    }
    toolType = item.GetToolType();
    return true;
}

void ConvertPointerEvent(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, DragPointerEvent& event)
{
    event.rawPointerEvent = pointerEvent;
    event.pointerEventId = pointerEvent->GetId();
    event.pointerId = pointerEvent->GetPointerId();
    event.pullId = pointerEvent->GetPullId();
    MMI::PointerEvent::PointerItem pointerItem;
    pointerEvent->GetPointerItem(pointerEvent->GetPointerId(), pointerItem);
    event.pressed = pointerItem.IsPressed();
    event.windowX = pointerItem.GetWindowX();
    event.windowY = pointerItem.GetWindowY();
    event.displayX = pointerItem.GetDisplayX();
    event.displayY = pointerItem.GetDisplayY();
    event.size = std::max(pointerItem.GetWidth(), pointerItem.GetHeight()) / SIZE_DIVIDE;
    event.force = static_cast<float>(pointerItem.GetPressure());
    event.deviceId = pointerItem.GetDeviceId();
    event.downTime = TimeStamp(std::chrono::microseconds(pointerItem.GetDownTime()));
    event.time = TimeStamp(std::chrono::microseconds(pointerEvent->GetActionTime()));
    event.sourceTool = GetSourceTool(pointerItem.GetToolType());
    event.targetWindowId = pointerItem.GetTargetWindowId();
    event.x = event.windowX;
    event.y = event.windowY;
    event.pressedKeyCodes.clear();
    for (const auto& curCode : pointerEvent->GetPressedKeys()) {
        event.pressedKeyCodes.emplace_back(static_cast<KeyCode>(curCode));
    }
    int32_t orgAction = pointerEvent->GetPointerAction();
    GetPointerEventAction(orgAction, event);
}

void LogPointInfo(const std::shared_ptr<MMI::PointerEvent>& pointerEvent, int32_t instanceId)
{
    if (pointerEvent->GetPointerAction() == OHOS::MMI::PointerEvent::POINTER_ACTION_DOWN) {
        auto container = Platform::AceContainer::GetContainer(instanceId);
        if (container) {
            auto pipelineContext = container->GetPipelineContext();
            if (pipelineContext) {
                uint32_t windowId = pipelineContext->GetWindowId();
                TAG_LOGI(AceLogTag::ACE_INPUTTRACKING, "pointdown windowId: %{public}u", windowId);
            }
        }
    }
    if (SystemProperties::GetDebugEnabled()) {
        TAG_LOGD(AceLogTag::ACE_DRAG, "point source: %{public}d", pointerEvent->GetSourceType());
        auto actionId = pointerEvent->GetPointerId();
        MMI::PointerEvent::PointerItem item;
        if (pointerEvent->GetPointerItem(actionId, item)) {
            TAG_LOGD(AceLogTag::ACE_DRAG,
                "action point info: id: %{public}d, pointerId: %{public}d, action: "
                "%{public}d, pressure: %{public}f, tiltX: %{public}f, tiltY: %{public}f",
                pointerEvent->GetId(), actionId, pointerEvent->GetPointerAction(),
                item.GetPressure(), item.GetTiltX(), item.GetTiltY());
        }
        auto ids = pointerEvent->GetPointerIds();
        for (auto&& id : ids) {
            MMI::PointerEvent::PointerItem item;
            if (pointerEvent->GetPointerItem(id, item)) {
                TAG_LOGD(AceLogTag::ACE_UIEVENT,
                    "all point info: id: %{public}d, x: %{public}d, y: %{public}d, isPressed: %{public}d, pressure: "
                    "%{public}f, tiltX: %{public}f, tiltY: %{public}f",
                    actionId, item.GetWindowX(), item.GetWindowY(), item.IsPressed(), item.GetPressure(),
                    item.GetTiltX(), item.GetTiltY());
            }
        }
    }
}

void CalculatePointerEvent(const std::shared_ptr<MMI::PointerEvent>& point, const RefPtr<NG::FrameNode>& frameNode,
    bool useRealtimeMatrix)
{
    CHECK_NULL_VOID(point);
    int32_t pointerId = point->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = point->GetPointerItem(pointerId, item);
    if (ret) {
        float xRelative = item.GetWindowX();
        float yRelative = item.GetWindowY();
        if (point->GetSourceType() == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
            item.GetToolType() == OHOS::MMI::PointerEvent::TOOL_TYPE_PEN) {
            xRelative = item.GetWindowXPos();
            yRelative = item.GetWindowYPos();
        }
        NG::PointF transformPoint(xRelative, yRelative);
        NG::NGGestureRecognizer::Transform(transformPoint, frameNode, useRealtimeMatrix);
        item.SetWindowX(static_cast<int32_t>(transformPoint.GetX()));
        item.SetWindowY(static_cast<int32_t>(transformPoint.GetY()));
        item.SetWindowXPos(transformPoint.GetX());
        item.SetWindowYPos(transformPoint.GetY());
        point->UpdatePointerItem(pointerId, item);
    }
}

void CalculatePointerEvent(const NG::OffsetF& offsetF, const std::shared_ptr<MMI::PointerEvent>& point,
    const NG::VectorF& scale, int32_t udegree)
{
    CHECK_NULL_VOID(point);
    int32_t pointerId = point->GetPointerId();
    MMI::PointerEvent::PointerItem item;
    bool ret = point->GetPointerItem(pointerId, item);
    if (ret) {
        float xRelative = item.GetWindowX();
        float yRelative = item.GetWindowY();
        if (point->GetSourceType() == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
            item.GetToolType() == OHOS::MMI::PointerEvent::TOOL_TYPE_PEN) {
            xRelative = item.GetWindowXPos();
            yRelative = item.GetWindowYPos();
        }
        auto windowX = xRelative;
        auto windowY = yRelative;
        auto pipelineContext = PipelineBase::GetCurrentContextSafelyWithCheck();
        CHECK_NULL_VOID(pipelineContext);
        auto displayWindowRect = pipelineContext->GetDisplayWindowRectInfo();
        auto windowWidth = displayWindowRect.Width();
        auto windowHeight = displayWindowRect.Height();
        switch (udegree) {
            case ANGLE_0:
                windowX = xRelative - offsetF.GetX();
                windowY = yRelative - offsetF.GetY();
                break;
            case ANGLE_90:
                windowX = yRelative - offsetF.GetX();
                windowY = windowWidth - offsetF.GetY() - xRelative;
                break;
            case ANGLE_180:
                windowX = windowWidth - offsetF.GetX() - xRelative;
                windowY = windowHeight - offsetF.GetY() - yRelative;
                break;
            case ANGLE_270:
                windowX = windowHeight - offsetF.GetX() - yRelative;
                windowY = xRelative - offsetF.GetY();
                break;
            default:
                break;
        }
        windowX = NearZero(scale.x) ? windowX : windowX / scale.x;
        windowY = NearZero(scale.y) ? windowY : windowY / scale.y;

        item.SetWindowX(static_cast<int32_t>(windowX));
        item.SetWindowY(static_cast<int32_t>(windowY));
        item.SetWindowXPos(windowX);
        item.SetWindowYPos(windowY);
        point->UpdatePointerItem(pointerId, item);
    }
}

void CalculateWindowCoordinate(const NG::OffsetF& offsetF, const std::shared_ptr<MMI::PointerEvent>& point,
    const NG::VectorF& scale, const int32_t udegree)
{
    CHECK_NULL_VOID(point);
    auto ids = point->GetPointerIds();
    for (auto&& id : ids) {
        MMI::PointerEvent::PointerItem item;
        bool ret = point->GetPointerItem(id, item);
        if (!ret) {
            LOGE("get pointer:%{public}d item failed", id);
            continue;
        }
        float xRelative = item.GetDisplayX();
        float yRelative = item.GetDisplayY();
        if (point->GetSourceType() == OHOS::MMI::PointerEvent::SOURCE_TYPE_TOUCHSCREEN &&
            item.GetToolType() == OHOS::MMI::PointerEvent::TOOL_TYPE_PEN) {
            xRelative = item.GetDisplayXPos();
            yRelative = item.GetDisplayYPos();
        }
        float windowX = xRelative;
        float windowY = yRelative;
        int32_t deviceWidth = SystemProperties::GetDevicePhysicalWidth();
        int32_t deviceHeight = SystemProperties::GetDevicePhysicalHeight();

        if (udegree == ANGLE_0) {
            windowX = xRelative - offsetF.GetX();
            windowY = yRelative - offsetF.GetY();
        }
        if (udegree == ANGLE_90) {
            windowX = yRelative - offsetF.GetX();
            windowY = deviceWidth - offsetF.GetY() - xRelative;
        }
        if (udegree == ANGLE_180) {
            windowX = deviceWidth - offsetF.GetX() - xRelative;
            windowY = deviceHeight - offsetF.GetY() - yRelative;
        }
        if (udegree == ANGLE_270) {
            windowX = deviceHeight - offsetF.GetX() - yRelative;
            windowY = xRelative - offsetF.GetY();
        }

        windowX = NearZero(scale.x) ? windowX : windowX / scale.x;
        windowY = NearZero(scale.y) ? windowY : windowY / scale.y;

        item.SetWindowX(static_cast<int32_t>(windowX));
        item.SetWindowY(static_cast<int32_t>(windowY));
        item.SetWindowXPos(windowX);
        item.SetWindowYPos(windowY);
        point->UpdatePointerItem(id, item);
    }
}
} // namespace OHOS::Ace::Platform
