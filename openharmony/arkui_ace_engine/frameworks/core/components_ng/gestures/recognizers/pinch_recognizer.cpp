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

#include "core/components_ng/gestures/recognizers/pinch_recognizer.h"
#include "core/components_ng/manager/event/json_child_report.h"
#include "core/common/reporter/reporter.h"
#include "core/components_ng/event/event_constants.h"
#include "core/components_ng/manager/event/json_report.h"

#include "base/ressched/ressched_report.h"
#include "core/event/ace_events.h"

namespace OHOS::Ace::NG {

namespace {

constexpr int32_t MAX_PINCH_FINGERS = 5;
constexpr int32_t DEFAULT_PINCH_FINGERS = 2;
constexpr Dimension DEFAULT_PINCH_DISTANCE = Dimension(5.0, DimensionUnit::VP);
constexpr double SCALE_PER_AXIS_EVENT = 0.1f;

} // namespace

PinchRecognizer::PinchRecognizer(int32_t fingers, double distance, bool isLimitFingerCount)
    : MultiFingersRecognizer(fingers, isLimitFingerCount), distance_(distance)
{
    if (fingers_ > MAX_PINCH_FINGERS || fingers_ < DEFAULT_PINCH_FINGERS) {
        fingers_ = DEFAULT_PINCH_FINGERS;
    }
    if (distance_ <= 0) {
        distance_ = DEFAULT_PINCH_DISTANCE.ConvertToPx();
    }
}

void PinchRecognizer::OnAccepted()
{
    int64_t acceptTime = GetSysTimestamp();
    int64_t inputTime = acceptTime;
    if (firstInputTime_.has_value()) {
        inputTime = static_cast<int64_t>(firstInputTime_.value().time_since_epoch().count());
    }
    if (SystemProperties::GetTraceInputEventEnabled()) {
        ACE_SCOPED_TRACE("UserEvent InputTime:%lld AcceptTime:%lld InputType:PinchGesture",
            static_cast<long long>(inputTime), static_cast<long long>(acceptTime));
    }
    
    auto node = GetAttachedNode().Upgrade();
    TAG_LOGI(AceLogTag::ACE_INPUTKEYFLOW, "Pinch accepted, tag = %{public}s",
        node ? node->GetTag().c_str() : "null");
    ResSchedReport::GetInstance().ResSchedDataReport("click");
    lastRefereeState_ = refereeState_;
    refereeState_ = RefereeState::SUCCEED;
    isLastPinchFinished_ = false;
    TouchEvent touchPoint = {};
    if (!touchPoints_.empty()) {
        touchPoint = touchPoints_.begin()->second;
    }
    localMatrix_ = NGGestureRecognizer::GetTransformMatrix(GetAttachedNode(), false,
        isPostEventResult_, touchPoint.postEventNodeId);
    SendCallbackMsg(onActionStart_, GestureCallbackType::START);
    isNeedResetVoluntarily_ = false;
}

void PinchRecognizer::OnRejected()
{
    if (refereeState_ == RefereeState::SUCCEED) {
        return;
    }
    SendRejectMsg();
    lastRefereeState_ = refereeState_;
    refereeState_ = RefereeState::FAIL;
    firstInputTime_.reset();
}

bool PinchRecognizer::IsCtrlBeingPressed(const AxisEvent& event)
{
    return std::any_of(event.pressedCodes.begin(), event.pressedCodes.end(),
        [](const KeyCode& code) { return code == KeyCode::KEY_CTRL_LEFT || code == KeyCode::KEY_CTRL_RIGHT; });
}

void PinchRecognizer::HandleTouchDownEvent(const TouchEvent& event)
{
    extraInfo_ = "";
    if (touchPoints_.size() == 1 && refereeState_ == RefereeState::FAIL) {
        lastRefereeState_ = RefereeState::READY;
        refereeState_ = RefereeState::READY;
    }
    touchPoints_[event.id] = event;
    if (!firstInputTime_.has_value()) {
        firstInputTime_ = event.time;
    }

    if (static_cast<int32_t>(activeFingers_.size()) >= fingers_) {
        return;
    }

    if (fingersId_.find(event.id) == fingersId_.end()) {
        fingersId_.insert(event.id);
    }
    activeFingers_.emplace_back(event.id);
    lastTouchEvent_ = event;

    if (static_cast<int32_t>(activeFingers_.size()) >= fingers_ && refereeState_ != RefereeState::FAIL) {
        initialDev_ = ComputeAverageDeviation();
        pinchCenter_ = ComputePinchCenter();
        lastRefereeState_ = refereeState_;
        refereeState_ = RefereeState::DETECTING;
    }
}

void PinchRecognizer::HandleTouchDownEvent(const AxisEvent& event)
{
    if (!firstInputTime_.has_value()) {
        firstInputTime_ = event.time;
    }
    if (event.isRotationEvent) {
        return;
    }
    touchPoints_[event.id] = TouchEvent();
    UpdateTouchPointWithAxisEvent(event);
    lastAxisEvent_ = event;
    if (IsRefereeFinished()) {
        return;
    }
    if (refereeState_ == RefereeState::READY &&
        (NearEqual(event.pinchAxisScale, 1.0) ||
            (IsCtrlBeingPressed(event) && event.sourceTool != SourceTool::TOUCHPAD))) {
        scale_ = 1.0f;
        pinchCenter_ = Offset(event.x, event.y);
        lastRefereeState_ = refereeState_;
        refereeState_ = RefereeState::DETECTING;
    }
}

void PinchRecognizer::HandleTouchUpEvent(const TouchEvent& event)
{
    if (fingersId_.find(event.id) != fingersId_.end()) {
        fingersId_.erase(event.id);
    }
    if (fingersId_.empty()) {
        isLastPinchFinished_ = true;
    }
    if (isNeedResetVoluntarily_ && currentFingers_ == 1) {
        ResetStateVoluntarily();
        isNeedResetVoluntarily_ = false;
        activeFingers_.remove(event.id);
        return;
    }
    if (!IsActiveFinger(event.id)) {
        return;
    }

    touchPoints_[event.id] = event;
    extraInfo_ = "activeSize: " + std::to_string(static_cast<int32_t>(activeFingers_.size()));
    if (static_cast<int32_t>(activeFingers_.size()) < fingers_ && refereeState_ != RefereeState::SUCCEED) {
        extraInfo_ += "activeFinger size not satisify.";
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        activeFingers_.remove(event.id);
        return;
    }

    lastTouchEvent_ = event;
    if ((refereeState_ != RefereeState::SUCCEED) && (refereeState_ != RefereeState::FAIL)) {
        extraInfo_ += "refereeState not satisfy.";
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        activeFingers_.remove(event.id);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED && static_cast<int32_t>(activeFingers_.size()) == fingers_) {
        SendCallbackMsg(onActionEnd_, GestureCallbackType::END);
        int64_t overTime = GetSysTimestamp();
        int64_t inputTime = overTime;
        if (firstInputTime_.has_value()) {
            inputTime = static_cast<int64_t>(firstInputTime_.value().time_since_epoch().count());
        }
        if (SystemProperties::GetTraceInputEventEnabled()) {
            ACE_SCOPED_TRACE("UserEvent InputTime:%lld OverTime:%lld InputType:PinchGesture",
                static_cast<long long>(inputTime), static_cast<long long>(overTime));
        }
        firstInputTime_.reset();
        isNeedResetVoluntarily_ = true;
    }
    activeFingers_.remove(event.id);
}

void PinchRecognizer::HandleTouchUpEvent(const AxisEvent& event)
{
    // if axisEvent received rotateEvent, no need to active Pinch recognizer.
    if (isPinchEnd_ || event.isRotationEvent) {
        return;
    }
    UpdateTouchPointWithAxisEvent(event);
    lastAxisEvent_ = event;
    if ((refereeState_ != RefereeState::SUCCEED) && (refereeState_ != RefereeState::FAIL)) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }
    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionEnd_, GestureCallbackType::END);
        isPinchEnd_ = true;
        int64_t overTime = GetSysTimestamp();
        int64_t inputTime = overTime;
        if (firstInputTime_.has_value()) {
            inputTime = static_cast<int64_t>(firstInputTime_.value().time_since_epoch().count());
        }
        if (SystemProperties::GetTraceInputEventEnabled()) {
            ACE_SCOPED_TRACE("UserEvent InputTime:%lld OverTime:%lld InputType:PinchGesture",
                static_cast<long long>(inputTime), static_cast<long long>(overTime));
        }
        firstInputTime_.reset();
    }
}

void PinchRecognizer::HandleTouchMoveEvent(const TouchEvent& event)
{
    touchPoints_[event.id] = event;
    if (!IsActiveFinger(event.id)) {
        return;
    }

    lastTouchEvent_ = event;
    currentDev_ = ComputeAverageDeviation();
    time_ = event.time;

    if (static_cast<int32_t>(touchPoints_.size()) < fingers_ ||
        static_cast<int32_t>(activeFingers_.size()) < fingers_) {
        return;
    }
    if (refereeState_ == RefereeState::DETECTING) {
        if (GreatOrEqual(fabs(currentDev_ - initialDev_), distance_)) {
            scale_ = currentDev_ / initialDev_;
            auto onGestureJudgeBeginResult = TriggerGestureJudgeCallback();
            if (onGestureJudgeBeginResult == GestureJudgeResult::REJECT) {
                Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
                return;
            }
            if (CheckLimitFinger()) {
                return;
            }
            if (!isLastPinchFinished_) {
                OnAccepted();
            } else {
                Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
            }
        }
    } else if (refereeState_ == RefereeState::SUCCEED) {
        scale_ = currentDev_ / initialDev_;
        if (static_cast<int32_t>(touchPoints_.size()) > fingers_ && isLimitFingerCount_) {
            return;
        }
        if (isFlushTouchEventsEnd_) {
            SendCallbackMsg(onActionUpdate_, GestureCallbackType::UPDATE);
        }
    }
}

void PinchRecognizer::OnFlushTouchEventsBegin()
{
    isFlushTouchEventsEnd_ = false;
}

void PinchRecognizer::OnFlushTouchEventsEnd()
{
    isFlushTouchEventsEnd_ = true;
}

void PinchRecognizer::HandleTouchMoveEvent(const AxisEvent& event)
{
    if (event.isRotationEvent || isPinchEnd_) {
        return;
    }
    if (ProcessAxisAbnormalCondition(event)) {
        return;
    }
    UpdateTouchPointWithAxisEvent(event);
    lastTouchEvent_ = touchPoints_[event.id];
    time_ = event.time;
    lastAxisEvent_ = event;
    if (refereeState_ == RefereeState::DETECTING || refereeState_ == RefereeState::SUCCEED) {
        if (event.pinchAxisScale != 0.0) {
            scale_ = event.pinchAxisScale;
        } else {
            if (GreatNotEqual(event.verticalAxis, 0.0) || GreatNotEqual(event.horizontalAxis, 0.0)) {
                scale_ -= SCALE_PER_AXIS_EVENT;
            } else if (LessNotEqual(event.verticalAxis, 0.0) || LessNotEqual(event.horizontalAxis, 0)) {
                scale_ += SCALE_PER_AXIS_EVENT;
            }
            if (LessNotEqual(scale_, 0.0)) {
                scale_ = 0.0;
            }
        }
        if (refereeState_ == RefereeState::DETECTING) {
            auto onGestureJudgeBeginResult = TriggerGestureJudgeCallback();
            if (onGestureJudgeBeginResult == GestureJudgeResult::REJECT) {
                Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
                return;
            }
            Adjudicate(AceType::Claim(this), GestureDisposal::ACCEPT);
        }
        SendCallbackMsg(onActionUpdate_, GestureCallbackType::UPDATE);
    }
}

void PinchRecognizer::HandleTouchCancelEvent(const TouchEvent& event)
{
    if (!IsActiveFinger(event.id)) {
        return;
    }
    touchPoints_[event.id] = event;
    if ((refereeState_ != RefereeState::SUCCEED) && (refereeState_ != RefereeState::FAIL)) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED && static_cast<int32_t>(activeFingers_.size()) == fingers_) {
        SendCallbackMsg(onActionCancel_, GestureCallbackType::CANCEL);
        lastRefereeState_ = RefereeState::READY;
        refereeState_ = RefereeState::READY;
    } else if (refereeState_ == RefereeState::SUCCEED) {
        TAG_LOGI(AceLogTag::ACE_INPUTKEYFLOW,
            "PinchRecognizer activeFingers size not equal fingers_, not send cancel callback.");
    }
}

void PinchRecognizer::HandleTouchCancelEvent(const AxisEvent& event)
{
    UpdateTouchPointWithAxisEvent(event);
    if ((refereeState_ != RefereeState::SUCCEED) && (refereeState_ != RefereeState::FAIL)) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return;
    }

    if (refereeState_ == RefereeState::SUCCEED) {
        SendCallbackMsg(onActionCancel_, GestureCallbackType::CANCEL);
    }
}

double PinchRecognizer::ComputeAverageDeviation()
{
    // compute the coordinate of focal point
    double sumOfX = 0.0;
    double sumOfY = 0.0;
    for (auto& id : activeFingers_) {
        sumOfX = sumOfX + touchPoints_[id].x;
        sumOfY = sumOfY + touchPoints_[id].y;
    }
    double focalX = sumOfX / fingers_;
    double focalY = sumOfY / fingers_;

    // compute average deviation
    double devX = 0.0;
    double devY = 0.0;
    for (auto& id : activeFingers_) {
        devX = devX + fabs(touchPoints_[id].x - focalX);
        devY = devY + fabs(touchPoints_[id].y - focalY);
    }
    double aveDevX = devX / fingers_;
    double aveDevY = devY / fingers_;

    // compute zoom distance
    double zoomDistance = sqrt(pow(aveDevX, 2) + pow(aveDevY, 2));

    return zoomDistance;
}

Offset PinchRecognizer::ComputePinchCenter()
{
    if (touchPoints_.empty()) {
        return Offset();
    }
    double sumOfX = 0.0;
    double sumOfY = 0.0;
    for (auto& id : activeFingers_) {
        sumOfX = sumOfX + touchPoints_[id].x;
        sumOfY = sumOfY + touchPoints_[id].y;
    }
    double focalX = sumOfX / fingers_;
    double focalY = sumOfY / fingers_;

    PointF localPoint(focalX, focalY);
    TransformForRecognizer(localPoint, GetAttachedNode(), false,
        isPostEventResult_, touchPoints_.begin()->second.postEventNodeId);
    Offset pinchCenter = Offset(localPoint.GetX(), localPoint.GetY());

    return pinchCenter;
}

void PinchRecognizer::OnResetStatus()
{
    MultiFingersRecognizer::OnResetStatus();
    initialDev_ = 0.0;
    currentDev_ = 0.0;
    scale_ = 1.0;
    isFlushTouchEventsEnd_ = false;
    isPinchEnd_ = false;
    isLastPinchFinished_ = true;
    localMatrix_.clear();
    pinchCenter_ = Offset();
}

void PinchRecognizer::SendCallbackMsg(const std::unique_ptr<GestureEventFunc>& callback, GestureCallbackType type)
{
    std::string callbackName = GetCallbackName(callback);
    ACE_SCOPED_TRACE("PinchRecognizer %s, currentDev_: %f", callbackName.c_str(), currentDev_);
    if (gestureInfo_ && gestureInfo_->GetDisposeTag()) {
        return;
    }
    if (callback && *callback) {
        GestureEvent info;
        info.SetTimeStamp(time_);
        UpdateFingerListInfo();
        info.SetFingerList(fingerList_);
        info.SetScale(scale_);
        info.SetPinchCenter(pinchCenter_);
        info.SetDeviceId(deviceId_);
        info.SetSourceDevice(deviceType_);
        info.SetTarget(GetEventTarget().value_or(EventTarget()));
        info.SetGestureTypeName(GestureTypeName::PINCH_GESTURE);
        info.SetForce(lastTouchEvent_.force);
        if (lastTouchEvent_.tiltX.has_value()) {
            info.SetTiltX(lastTouchEvent_.tiltX.value());
        }
        if (lastTouchEvent_.tiltY.has_value()) {
            info.SetTiltY(lastTouchEvent_.tiltY.value());
        }
        if (lastTouchEvent_.rollAngle.has_value()) {
            info.SetRollAngle(lastTouchEvent_.rollAngle.value());
        }
        if (inputEventType_ == InputEventType::AXIS) {
            info.SetVerticalAxis(lastAxisEvent_.verticalAxis);
            info.SetHorizontalAxis(lastAxisEvent_.horizontalAxis);
            info.SetSourceTool(lastAxisEvent_.sourceTool);
            info.SetPressedKeyCodes(lastAxisEvent_.pressedCodes);
            info.CopyConvertInfoFrom(lastAxisEvent_.convertInfo);
        } else {
            info.CopyConvertInfoFrom(lastTouchEvent_.convertInfo);
            info.SetSourceTool(lastTouchEvent_.sourceTool);
            info.SetPressedKeyCodes(lastTouchEvent_.pressedKeyCodes_);
        }
        info.SetPointerEvent(lastPointEvent_);
        info.SetInputEventType(inputEventType_);
        // callback may be overwritten in its invoke so we copy it first
        auto callbackFunction = *callback;
        HandleGestureAccept(info, type);
        callbackFunction(info);
        HandleReports(info, type);
    }
}

void PinchRecognizer::HandleReports(const GestureEvent& info, GestureCallbackType type)
{
    if (type == GestureCallbackType::ACTION || type == GestureCallbackType::UPDATE) {
        return;
    }
    auto frameNode = GetAttachedNode().Upgrade();
    CHECK_NULL_VOID(frameNode);
    PinchJsonReport pinchReport;
    pinchReport.SetCallbackType(type);
    pinchReport.SetGestureType(GetRecognizerType());
    pinchReport.SetId(frameNode->GetId());
    pinchReport.SetFingerList(info.GetFingerList());
    pinchReport.SetScale(info.GetScale());
    Reporter::GetInstance().HandleUISessionReporting(pinchReport);
}

GestureJudgeResult PinchRecognizer::TriggerGestureJudgeCallback()
{
    auto targetComponent = GetTargetComponent();
    CHECK_NULL_RETURN(targetComponent, GestureJudgeResult::CONTINUE);
    auto gestureRecognizerJudgeFunc = targetComponent->GetOnGestureRecognizerJudgeBegin();
    auto callback = targetComponent->GetOnGestureJudgeBeginCallback();
    if (!callback && !gestureRecognizerJudgeFunc) {
        return GestureJudgeResult::CONTINUE;
    }
    auto info = std::make_shared<PinchGestureEvent>();
    info->SetTimeStamp(time_);
    info->SetDeviceId(deviceId_);
    UpdateFingerListInfo();
    info->SetFingerList(fingerList_);
    info->SetScale(scale_);
    info->SetPinchCenter(pinchCenter_);
    info->SetSourceDevice(deviceType_);
    info->SetTarget(GetEventTarget().value_or(EventTarget()));
    info->SetForce(lastTouchEvent_.force);
    if (gestureInfo_) {
        gestureInfo_->SetInputEventType(inputEventType_);
    }
    if (lastTouchEvent_.tiltX.has_value()) {
        info->SetTiltX(lastTouchEvent_.tiltX.value());
    }
    if (lastTouchEvent_.tiltY.has_value()) {
        info->SetTiltY(lastTouchEvent_.tiltY.value());
    }
    if (lastTouchEvent_.rollAngle.has_value()) {
        info->SetRollAngle(lastTouchEvent_.rollAngle.value());
    }
    info->SetSourceTool(lastTouchEvent_.sourceTool);
    info->SetRawInputEventType(inputEventType_);
    info->SetRawInputEvent(lastPointEvent_);
    info->SetRawInputDeviceId(deviceId_);
    if (gestureRecognizerJudgeFunc) {
        return gestureRecognizerJudgeFunc(info, Claim(this), responseLinkRecognizer_);
    }
    return callback(gestureInfo_, info);
}

bool PinchRecognizer::ReconcileFrom(const RefPtr<NGGestureRecognizer>& recognizer)
{
    RefPtr<PinchRecognizer> curr = AceType::DynamicCast<PinchRecognizer>(recognizer);
    if (!curr) {
        ResetStatus();
        return false;
    }

    if (curr->fingers_ != fingers_ || curr->distance_ != distance_ || curr->priorityMask_ != priorityMask_) {
        if (refereeState_ == RefereeState::SUCCEED && static_cast<int32_t>(activeFingers_.size()) == fingers_) {
            SendCallbackMsg(onActionCancel_, GestureCallbackType::CANCEL);
        }
        ResetStatus();
        return false;
    }
    isLimitFingerCount_ = curr->isLimitFingerCount_;

    onActionStart_ = std::move(curr->onActionStart_);
    onActionUpdate_ = std::move(curr->onActionUpdate_);
    onActionEnd_ = std::move(curr->onActionEnd_);
    onActionCancel_ = std::move(curr->onActionCancel_);
    ReconcileGestureInfoFrom(recognizer);
    return true;
}

RefPtr<GestureSnapshot> PinchRecognizer::Dump() const
{
    RefPtr<GestureSnapshot> info = NGGestureRecognizer::Dump();
    std::stringstream oss;
    oss << "distance: " << distance_ << ", "
        << "fingers: " << fingers_ << ", "
        << DumpGestureInfo();
    info->customInfo = oss.str();
    return info;
}

bool PinchRecognizer::ProcessAxisAbnormalCondition(const AxisEvent& event)
{
    if (NearZero(event.pinchAxisScale) && (!IsCtrlBeingPressed(event) || event.sourceTool == SourceTool::TOUCHPAD)) {
        if (ProcessAxisReject()) {
            return true;
        }
    }
    return false;
}

bool PinchRecognizer::ProcessAxisReject()
{
    if (refereeState_ == RefereeState::DETECTING) {
        Adjudicate(AceType::Claim(this), GestureDisposal::REJECT);
        return true;
    }
    if (refereeState_ == RefereeState::SUCCEED) {
        lastRefereeState_ = RefereeState::READY;
        refereeState_ = RefereeState::READY;
        SendCallbackMsg(onActionEnd_, GestureCallbackType::END);
        isPinchEnd_ = true;
        return true;
    }
    return false;
}

} // namespace OHOS::Ace::NG
