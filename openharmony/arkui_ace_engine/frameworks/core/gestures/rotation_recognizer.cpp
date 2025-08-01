/*
* Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "core/gestures/rotation_recognizer.h"
#include "core/pipeline/base/constants.h"

namespace OHOS::Ace {

namespace {

constexpr int32_t MAX_ROTATION_FINGERS = 5;

} // namespace

void RotationRecognizer::OnAccepted()
{
    SendCallbackMsg(onActionStart_);

    if (pendingEnd_) {
        SendCallbackMsg(onActionEnd_);
        Reset();
    } else if (pendingCancel_) {
        SendCallbackMsg(onActionCancel_);
        Reset();
    }
}

void RotationRecognizer::OnRejected()
{
    Reset();
}

void RotationRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    if (fingers_ > MAX_ROTATION_FINGERS) {
        return;
    }

    if (state_ == DetectState::READY) {
        AddToReferee(event.id, AceType::Claim(this));
        touchPoints_[event.id] = event;
        if (static_cast<int32_t>(touchPoints_.size()) == fingers_) {
            initialAngle_ = ComputeAngle();
            state_ = DetectState::DETECTING;
        }
    }
}

void RotationRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    touchPoints_.erase(itr);

    if (state_ != DetectState::DETECTED) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    GestureEvent info;
    info.SetTimeStamp(time_);
    info.SetAngle(resultAngle_);
    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionEnd_);
        Reset();
    } else {
        pendingEnd_ = true;
    }
}

void RotationRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    auto itr = touchPoints_.find(event.id);
    if (itr == touchPoints_.end()) {
        return;
    }

    if (state_ == DetectState::READY) {
        touchPoints_[event.id] = event;
        return;
    }

    touchPoints_[event.id] = event;
    currentAngle_ = ComputeAngle();
    time_ = event.time;

    if (state_ == DetectState::DETECTING) {
        double diffAngle = fabs((currentAngle_ - initialAngle_));
        if (GreatOrEqual(diffAngle, angle_)) {
            resultAngle_ = ChangeValueRange(currentAngle_ - initialAngle_);
            state_ = DetectState::DETECTED;
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        }
    } else if (state_ == DetectState::DETECTED && refereeState_ == RefereeState::SUCCEED) {
        resultAngle_ = ChangeValueRange(currentAngle_ - initialAngle_);
        SendCallbackMsg(onActionUpdate_);
    }
}

void RotationRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    if (state_ == DetectState::READY || state_ == DetectState::DETECTING) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionCancel_);
        Reset();
    } else {
        pendingCancel_ = true;
    }
}

double RotationRecognizer::ComputeAngle()
{
    double fx = touchPoints_[0].x;
    double fy = touchPoints_[0].y;
    double sx = touchPoints_[1].x;
    double sy = touchPoints_[1].y;
    double angle = atan2(fy - sy, fx - sx) * 180.0 / ACE_PI;
    return angle;
}

// Map the value range to -180 to 180
double RotationRecognizer::ChangeValueRange(double value)
{
    double result = 0.0;
    if (LessOrEqual(value, -180.0)) {
        result = value + 360.0;
    } else if (GreatNotEqual(value, 180.0)) {
        result = value - 360.0;
    } else {
        result = value;
    }

    return result;
}

void RotationRecognizer::Reset()
{
    touchPoints_.clear();
    state_ = DetectState::READY;
    pendingEnd_ = false;
    pendingCancel_ = false;
}

void RotationRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback)
{
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        info.SetAngle(resultAngle_);
        info.SetDeviceId(deviceId_);
        info.SetSourceDevice(deviceType_);
        info.SetTarget(GetEventTarget().value_or(EventTarget()));
        TouchEvent touchPoint = {};
        if (!touchPoints_.empty()) {
            touchPoint = touchPoints_.begin()->second;
        }
        info.SetForce(touchPoint.force);
        if (touchPoint.tiltX.has_value()) {
            info.SetTiltX(touchPoint.tiltX.value());
        }
        if (touchPoint.tiltY.has_value()) {
            info.SetTiltY(touchPoint.tiltY.value());
        }
        info.SetSourceTool(touchPoint.sourceTool);
        info.SetPressedKeyCodes(touchPoint.pressedKeyCodes_);
        (*callback)(info);
    }
}

bool RotationRecognizer::ReconcileFrom(const RefPtr<GestureRecognizer>& recognizer)
{
    RefPtr<RotationRecognizer> curr = AceType::DynamicCast<RotationRecognizer>(recognizer);
    if (!curr) {
        Reset();
        return false;
    }

    if (curr->fingers_ != fingers_ || !NearEqual(curr->angle_, angle_) || curr->priorityMask_ != priorityMask_) {
        Reset();
        return false;
    }

    onActionStart_ = std::move(curr->onActionStart_);
    onActionUpdate_ = std::move(curr->onActionUpdate_);
    onActionEnd_ = std::move(curr->onActionEnd_);
    onActionCancel_ = std::move(curr->onActionCancel_);

    return true;
}

} // namespace OHOS::Ace