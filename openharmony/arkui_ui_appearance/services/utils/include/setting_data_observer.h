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

#ifndef UI_APPEARANCE_UTILS_SETTING_DATA_OBSERVER_H
#define UI_APPEARANCE_UTILS_SETTING_DATA_OBSERVER_H

#include "data_ability_observer_stub.h"

namespace OHOS::ArkUi::UiAppearance {
constexpr int32_t INVALID_USER_ID = -1;

class SettingDataObserver final : public AAFwk::DataAbilityObserverStub {
public:
    SettingDataObserver() = default;

    ~SettingDataObserver() override = default;

    void OnChange() override;

    void SetKey(const std::string& key);

    const std::string &GetKey() const;

    using UpdateFunc = std::function<void(const std::string&, int32_t)>;

    void SetUpdateFunc(const UpdateFunc& updateFunc);

    void SetUserId(int32_t userId);

    int32_t GetUserId() const;

private:
    std::string key_;
    UpdateFunc updateFunc_;
    int32_t userId_ = INVALID_USER_ID;
};
} // namespace OHOS::ArkUi::UiAppearance

#endif // UI_APPEARANCE_UTILS_SETTING_DATA_OBSERVER_H
