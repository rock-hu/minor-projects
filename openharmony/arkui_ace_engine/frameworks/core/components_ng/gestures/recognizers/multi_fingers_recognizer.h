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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_MULTI_FINGERS_RECOGNIZER_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_MULTI_FINGERS_RECOGNIZER_H

#include <list>
#include <map>

#include "core/components_ng/gestures/recognizers/gesture_recognizer.h"
#include "core/event/touch_event.h"

namespace OHOS::Ace::NG {

class MultiFingersRecognizer : public NGGestureRecognizer {
    DECLARE_ACE_TYPE(MultiFingersRecognizer, NGGestureRecognizer);

public:
    MultiFingersRecognizer() = default;
    explicit MultiFingersRecognizer(int32_t fingers, bool isLimitFingerCount = false);

    ~MultiFingersRecognizer() override = default;

    void UpdateFingerListInfo();

    bool CheckTouchId(int32_t touchId) override
    {
        return touchPoints_.find(touchId) != touchPoints_.end();
    }

    std::map<int32_t, TouchEvent> GetTouchPoints() override
    {
        return touchPoints_;
    }

    int GetFingers()
    {
        return fingers_;
    }

    void SetLimitFingerCount(bool limitFingerCount)
    {
        isLimitFingerCount_ = limitFingerCount;
    }

    bool GetLimitFingerCount() const
    {
        return isLimitFingerCount_;
    }

    bool CheckLimitFinger()
    {
        if (isLimitFingerCount_) {
            if (static_cast<int32_t>(touchPoints_.size()) != fingers_) {
                return true;
            }
        }
        return false;
    }

    void ForceCleanRecognizer() override
    {
        for (const auto& iter : touchPoints_) {
            touchPoints_[iter.first] = {};
        }
        fingersId_.clear();
        fingerList_.clear();
        activeFingers_.clear();
        currentFingers_ = 0;
        lastRefereeState_ = RefereeState::READY;
        refereeState_ = RefereeState::READY;
        disposal_ = GestureDisposal::NONE;
        lastPointEvent_.reset();
        backupTouchPointsForSucceedBlock_.reset();
        preventDefault_ = false;
    }

    void CleanRecognizerState() override;

    int32_t GetValidFingersCount() const
    {
        return std::count_if(touchPoints_.begin(), touchPoints_.end(),
            [](const auto& item) { return item.second.type != TouchType::UNKNOWN; });
    }

    int32_t GetTouchPointsSize() const
    {
        return static_cast<int32_t>(touchPoints_.size());
    }

    void SetTouchPointsForSucceedBlock()
    {
        backupTouchPointsForSucceedBlock_ = touchPoints_;
    }

    void ResetTouchPointsForSucceedBlock()
    {
        backupTouchPointsForSucceedBlock_.reset();
    }

protected:
    void OnBeginGestureReferee(int32_t touchId, bool needUpdateChild = false) override
    {
        touchPoints_[touchId] = {};
    }

    void RemoveUnsupportEvent(int32_t touchId) override
    {
        if (touchPoints_.empty() || touchPoints_.find(touchId) == touchPoints_.end()) {
            return;
        }
        touchPoints_.erase(touchId);
    }

    void UpdateTouchPointWithAxisEvent(const AxisEvent& event);

    void OnFinishGestureReferee(int32_t touchId, bool isBlocked) override;

    void OnResetStatus() override
    {
        touchPoints_.clear();
        fingersId_.clear();
        fingerList_.clear();
        activeFingers_.clear();
        lastPointEvent_.reset();
        currentFingers_ = 0;
        lastRefereeState_ = RefereeState::READY;
        refereeState_ = RefereeState::READY;
        disposal_ = GestureDisposal::NONE;
        backupTouchPointsForSucceedBlock_.reset();
        preventDefault_ = false;
    }

    bool IsNeedResetStatus();

    bool IsActiveFinger(int32_t touchId) const
    {
        return std::find(activeFingers_.begin(), activeFingers_.end(), touchId) != activeFingers_.end();
    }

    bool CheckFingerListInDownFingers(int32_t pointId) const;

    std::string DumpGestureInfo() const;

    std::map<int32_t, TouchEvent> touchPoints_;
    std::list<FingerInfo> fingerList_;
    std::list<int32_t> activeFingers_;
    std::shared_ptr<MMI::PointerEvent> lastPointEvent_;
    int32_t fingers_ = 1;
    bool isLimitFingerCount_ = false;
    std::optional<std::map<int32_t, TouchEvent>> backupTouchPointsForSucceedBlock_;
};

} // namespace OHOS::Ace::NG

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMPONENTS_NG_GESTURES_RECOGNIZERS_MULTI_FINGERS_RECOGNIZER_H
