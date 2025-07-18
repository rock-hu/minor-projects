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

#include "core/common/ace_application_info.h"

namespace OHOS::Ace {
class MockAceApplicationInfo : public AceApplicationInfo {
    void SetLocale(const std::string& language, const std::string& countryOrRegion, const std::string& script,
        const std::string& keywordsAndValues) override
    {
        language_ = language;
    }
    void ChangeLocale(const std::string& language, const std::string& countryOrRegion) override {}

    void SetDebug(bool isDebugVersion, bool needDebugBreakpoint) override {}

    bool GetBundleInfo(const std::string& packageName, AceBundleInfo& bundleInfo) override
    {
        return false;
    }

    double GetLifeTime() const override
    {
        return 0;
    }

    std::string GetJsEngineParam(const std::string& key) const override
    {
        return "";
    }

    bool IsRightToLeft() const
    {
        return true;
    }

    void SetApiTargetVersion(int32_t apiVersion)
    {
        apiVersion_ = apiVersion;
    }

public:
    static MockAceApplicationInfo& GetInstance()
    {
        static MockAceApplicationInfo instance;
        return instance;
    }

private:
    std::string packageName_;
};

const std::string& AceApplicationInfo::GetAbilityName() const
{
    return abilityName_;
}

int32_t AceApplicationInfo::GetUid() const
{
    return uid_;
}

void AceApplicationInfo::SetPackageName(const std::string& packageName)
{
    packageName_ = packageName;
}

const std::string& AceApplicationInfo::GetPackageName() const
{
    return packageName_;
}

const std::string& AceApplicationInfo::GetProcessName() const
{
    return processName_;
}

AceApplicationInfo& AceApplicationInfo::GetInstance()
{
    return MockAceApplicationInfo::GetInstance();
}
} // namespace OHOS::Ace
