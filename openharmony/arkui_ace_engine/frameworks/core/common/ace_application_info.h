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

#ifndef FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H
#define FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H

#include <chrono>
#include <cstdint>
#include <functional>
#include <mutex>
#include <set>
#include <shared_mutex>
#include <string>
#include <vector>

#include "interfaces/inner_api/ace/ace_forward_compatibility.h"

#include "base/json/json_util.h"
#include "base/utils/macros.h"
#include "base/utils/noncopyable.h"
#include "base/utils/string_utils.h"

namespace OHOS::Ace {

enum class PlatformVersion {
    VERSION_FIVE = 5,
    VERSION_SIX,
    VERSION_SEVEN,
    VERSION_EIGHT,
    VERSION_NINE,
    VERSION_TEN,
    VERSION_ELEVEN,
    VERSION_TWELVE,
    VERSION_THIRTEEN,
    VERSION_FOURTEEN,
    VERSION_FIFTEEN,
    VERSION_SIXTEEN,
    VERSION_SEVENTEEN,
    VERSION_EIGHTEEN,
    VERSION_NINETEEN,
    VERSION_TWENTY
};
struct AceBundleInfo {
    uint32_t versionCode = 0;
    std::string versionName;
};

enum class TouchPassMode: int32_t {
    DEFAULT = 0,
    PASS_THROUGH,
    ACCELERATE,
};

struct TextMenuInfo {
    uint32_t disableFlags = 0;
    std::function<bool()> menuOnChangeCallback;
};

class ACE_FORCE_EXPORT AceApplicationInfo : public NonCopyable {
public:
    ACE_EXPORT static AceApplicationInfo& GetInstance();

    virtual void SetLocale(const std::string& language, const std::string& countryOrRegion, const std::string& script,
        const std::string& keywordsAndValues) = 0;
    virtual void ChangeLocale(const std::string& language, const std::string& countryOrRegion) = 0;
    virtual void SetDebug(bool isDebugVersion, bool needDebugBreakpoint) = 0;

    void SetUserId(int userId)
    {
        userId_ = userId;
    }

    int GetUserId() const
    {
        return userId_;
    }

    void SetPackageName(const std::string& packageName);
    const std::string& GetPackageName() const;

    void SetUid(int32_t uid);
    int32_t GetUid() const;

    void SetProcessName(const std::string& processName);
    const std::string& GetProcessName() const;

    void SetAbilityName(const std::string& abilityName_);
    const std::string& GetAbilityName() const;

    void SetDataFileDirPath(const std::string& dataDirFilePath)
    {
        dataDirFilePath_ = dataDirFilePath;
    }
    const std::string& GetDataFileDirPath() const
    {
        return dataDirFilePath_;
    }

    void SetApiTargetVersion(int32_t apiVersion)
    {
        apiVersion_ = apiVersion;
    }
    int32_t GetApiTargetVersion() const
    {
        return apiVersion_;
    }

    bool GreatOrEqualTargetAPIVersion(PlatformVersion version)
    {
        return (apiVersion_ % 1000) >= static_cast<int32_t>(version);
    }

    void SetAppVersionName(const std::string& versionName)
    {
        versionName_ = versionName;
    }

    const std::string& GetAppVersionName() const
    {
        return versionName_;
    }

    void SetAppVersionCode(uint32_t versionCode)
    {
        versionCode_ = versionCode;
    }

    uint32_t GetAppVersionCode() const
    {
        return versionCode_;
    }

    virtual bool GetBundleInfo(const std::string& packageName, AceBundleInfo& bundleInfo) = 0;
    virtual double GetLifeTime() const = 0;

    virtual std::string GetJsEngineParam(const std::string& key) const = 0;

    const std::string& GetCountryOrRegion() const
    {
        return countryOrRegion_;
    }

    const std::string& GetLanguage() const
    {
        return language_;
    }

    const std::string& GetScript() const
    {
        return script_;
    }

    std::string GetLocaleTag()
    {
        std::shared_lock<std::shared_mutex> lock(localeTagMutex_);
        return localeTag_;
    }

    std::string GetUnicodeSetting() const;

    bool IsRightToLeft() const
    {
        return isRightToLeft_;
    }

    bool IsDebugVersion() const
    {
        return isDebugVersion_;
    }

    bool IsNeedDebugBreakPoint() const
    {
        return needDebugBreakpoint_;
    }

    void SetNeedDebugBreakPoint(const bool needDebugBreakpoint)
    {
        needDebugBreakpoint_ = needDebugBreakpoint;
    }

    void SetCardType()
    {
        isCardType_ = true;
    }

    bool GetIsCardType() const
    {
        return isCardType_;
    }
    void SetBarrierfreeDuration(int32_t duration)
    {
        barrierfreeDuration_ = duration;
    }
    int32_t GetBarrierfreeDuration() const
    {
        return barrierfreeDuration_;
    }
    void SetAccessibilityEnabled(bool isEnabled)
    {
        isAccessibilityEnabled_ = isEnabled;
    }
    bool IsAccessibilityEnabled() const
    {
        return isAccessibilityEnabled_;
    }
    void SetAccessibilityScreenReadEnabled(bool isEnabled)
    {
        isAccessibilityScreenReadEnabled_ = isEnabled;
    }
    bool IsAccessibilityScreenReadEnabled() const
    {
        return isAccessibilityScreenReadEnabled_;
    }
    void SetPid(int32_t pid)
    {
        pid_ = pid;
    }
    int32_t GetPid() const
    {
        return pid_;
    }

    void SetMissionId(int32_t missionId)
    {
        missionId_ = missionId;
    }
    int32_t GetMissionId() const
    {
        return missionId_;
    }

    bool IsUseNewPipeline()
    {
        return useNewPipeline_.value_or(AceForwardCompatibility::IsUseNG());
    }

    void SetIsUseNewPipeline(bool useNewPipeline)
    {
        useNewPipeline_.emplace(useNewPipeline);
    }

    void SetTouchEventPassMode(TouchPassMode mode)
    {
        std::unique_lock<std::shared_mutex> lock(eventsPassMutex_);
        touchPassMode_ = mode;
    }

    TouchPassMode GetTouchEventPassMode() const
    {
        std::shared_lock<std::shared_mutex> lock(eventsPassMutex_);
        return touchPassMode_;
    }

    void SetReusedNodeSkipMeasure(bool reusedNodeSkipMeasure)
    {
        reusedNodeSkipMeasure_= reusedNodeSkipMeasure;
    }

    bool IsReusedNodeSkipMeasure() const
    {
        return reusedNodeSkipMeasure_;
    }

    void SetMouseTransformEnable(bool mouseTransformEnable)
    {
        mouseTransformEnable_ = mouseTransformEnable;
    }

    bool IsMouseTransformEnable()
    {
        return mouseTransformEnable_;
    }

    void AddTextMenuDisableFlag(uint32_t flag)
    {
        textMenuInfo_.disableFlags |= flag;
    }

    void SetTextMenuDisableFlags(uint32_t flag)
    {
        textMenuInfo_.disableFlags &= flag;
    }

    void SetTextMenuOnChangeCallback(std::function<bool()>&& callback)
    {
        textMenuInfo_.menuOnChangeCallback = std::move(callback);
    }

    const TextMenuInfo& GetTextMenuInfo()
    {
        return textMenuInfo_;
    }

    void SetTouchPadIdChanged(bool touchPadIdChanged)
    {
        touchPadIdChanged_ = touchPadIdChanged;
    }

    bool GetTouchPadIdChanged() const
    {
        return touchPadIdChanged_;
    }

protected:
    std::string countryOrRegion_;
    std::string language_;
    std::string script_;
    std::string localeTag_;
    mutable std::shared_mutex localeTagMutex_;
    std::string keywordsAndValues_;

    std::string packageName_;
    std::string processName_;
    std::string dataDirFilePath_;
    std::string abilityName_;
    int32_t pid_ = 0;
    int32_t uid_ = 0;
    int32_t barrierfreeDuration_ = 0;

    bool isRightToLeft_ = false;
    bool isDebugVersion_ = false;
    bool needDebugBreakpoint_ = false;
    std::optional<bool> useNewPipeline_;

    bool isCardType_ = false;

    int userId_ = 0;
    bool isAccessibilityEnabled_ = false;
    bool isAccessibilityScreenReadEnabled_ = false;

    int32_t apiVersion_ = 0;
    std::string versionName_;
    uint32_t versionCode_ = 0;
    int32_t missionId_ = -1;
    mutable std::shared_mutex eventsPassMutex_;
    TouchPassMode touchPassMode_ = TouchPassMode::DEFAULT;
    bool reusedNodeSkipMeasure_ = false;
    bool mouseTransformEnable_ = false;
    bool touchPadIdChanged_ = false;
    TextMenuInfo textMenuInfo_;
};

} // namespace OHOS::Ace

#endif // FOUNDATION_ACE_FRAMEWORKS_CORE_COMMON_ACE_APPLICATION_INFO_H
