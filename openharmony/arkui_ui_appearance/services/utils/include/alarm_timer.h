/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef UI_APPEARANCE_UTILS_ALARM_TIMER_H
#define UI_APPEARANCE_UTILS_ALARM_TIMER_H

#include "time_service_client.h"

namespace OHOS::ArkUi::UiAppearance {
class AlarmTimer final : public MiscServices::ITimerInfo {
public:
    AlarmTimer() = default;

    ~AlarmTimer() override = default;

    void OnTrigger() override;

    void SetType(const int& type) override;

    void SetRepeat(bool repeat) override;

    void SetInterval(const uint64_t& interval) override;

    void SetWantAgent(std::shared_ptr<AbilityRuntime::WantAgent::WantAgent> wantAgent) override;

    void SetCallbackInfo(const std::function<void()>& callBack);

private:
    std::function<void()> callBack_;
};
} // namespace OHOS::ArkUi::UiAppearance
#endif // UI_APPEARANCE_UTILS_ALARM_TIMER_H
