/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef UI_APPEARANCE_ABILITY_PROXY_H
#define UI_APPEARANCE_ABILITY_PROXY_H

#include "iremote_proxy.h"
#include "ui_appearance_ability_interface.h"

namespace OHOS {
namespace ArkUi::UiAppearance {
class UiAppearanceAbilityProxy : public IRemoteProxy<UiAppearanceAbilityInterface> {
public:
    int32_t SetDarkMode(DarkMode mode) override;
    int32_t GetDarkMode() override;
    int32_t SetFontScale(std::string& fontScale) override;
    int32_t GetFontScale(std::string& fontScale) override;
    int32_t SetFontWeightScale(std::string& fontWeightScale) override;
    int32_t GetFontWeightScale(std::string& fontWeightScale) override;

    explicit UiAppearanceAbilityProxy(const sptr<IRemoteObject>& impl)
        : IRemoteProxy<UiAppearanceAbilityInterface>(impl)
    {}
    ~UiAppearanceAbilityProxy() = default;

private:
    static inline BrokerDelegator<UiAppearanceAbilityProxy> delegator_;
};
} // namespace ArkUi::UiAppearance
} // namespace OHOS
#endif // UI_APPEARANCE_ABILITY_PROXY_H