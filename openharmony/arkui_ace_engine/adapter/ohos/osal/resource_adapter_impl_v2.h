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

#ifndef FOUNDATION_ACE_ADAPTER_OHOS_OSAL_RESOURCE_ADAPTER_IMPL_V2_H
#define FOUNDATION_ACE_ADAPTER_OHOS_OSAL_RESOURCE_ADAPTER_IMPL_V2_H

#include <mutex>

#include "resource_manager.h"
#include "base/thread/task_executor.h"
#include "base/image/pixel_map.h"
#include "base/utils/device_config.h"
#include "base/utils/string_utils.h"
#include "core/components/theme/resource_adapter.h"
#include "adapter/ohos/osal/resource_theme_style.h"

namespace OHOS::Ace {
class ResourceAdapterImplV2 : public ResourceAdapter {
    DECLARE_ACE_TYPE(ResourceAdapterImplV2, ResourceAdapter);

public:
    ResourceAdapterImplV2() = default;
    explicit ResourceAdapterImplV2(std::shared_ptr<Global::Resource::ResourceManager> resourceManager);
    ResourceAdapterImplV2(std::shared_ptr<Global::Resource::ResourceManager> resourceManager, int32_t instanceId);
    ResourceAdapterImplV2(
        std::shared_ptr<Global::Resource::ResourceManager> resourceManager, const ResourceInfo& resourceInfo);
    ~ResourceAdapterImplV2() override = default;

    void Init(const ResourceInfo& resourceInfo) override;
    void UpdateConfig(const ResourceConfiguration& config, bool themeFlag = false) override;

    RefPtr<ThemeStyle> GetTheme(int32_t themeId) override;

    Color GetColor(uint32_t resId) override;
    Color GetColorByName(const std::string& resName) override;
    Dimension GetDimension(uint32_t resId) override;
    Dimension GetDimensionByName(const std::string& resName) override;
    std::string GetString(uint32_t resId) override;
    std::string GetStringByName(const std::string& resName) override;
    std::string GetPluralString(uint32_t resId, int quantity) override;
    std::string GetPluralStringByName(const std::string& resName, int quantity) override;
    std::vector<std::string> GetStringArray(uint32_t resId) const override;
    std::vector<std::string> GetStringArrayByName(const std::string& resName) const override;
    double GetDouble(uint32_t resId) override;
    double GetDoubleByName(const std::string& resName) override;
    int32_t GetInt(uint32_t resId) override;
    int32_t GetIntByName(const std::string& resName) override;
    std::vector<uint32_t> GetIntArray(uint32_t resId) const override;
    std::vector<uint32_t> GetIntArrayByName(const std::string& resName) const override;
    bool GetBoolean(uint32_t resId) const override;
    bool GetBooleanByName(const std::string& resName) const override;
    std::shared_ptr<Media::PixelMap> GetPixelMap(uint32_t resId) override;
    std::string GetMediaPath(uint32_t resId) override;
    std::string GetMediaPathByName(const std::string& resName) override;
    std::string GetRawfile(const std::string& fileName) override;
    bool GetRawFileData(const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest) override;
    bool GetRawFileData(const std::string& rawFile, size_t& len, std::unique_ptr<uint8_t[]>& dest,
        const std::string& bundleName, const std::string& moduleName) override;
    bool GetMediaData(uint32_t resId, size_t& len, std::unique_ptr<uint8_t[]>& dest) override;
    bool GetMediaData(uint32_t resId, size_t& len, std::unique_ptr<uint8_t[]>& dest, const std::string& bundleName,
        const std::string& moduleName) override;
    bool GetMediaData(const std::string& resName, size_t& len, std::unique_ptr<uint8_t[]>& dest) override;
    bool GetMediaData(const std::string& resName, size_t& len, std::unique_ptr<uint8_t[]>& dest,
        const std::string& bundleName, const std::string& moduleName) override;
    void UpdateResourceManager(const std::string& bundleName, const std::string& moduleName) override {};
    bool GetRawFileDescription(const std::string& rawfileName, RawfileDescription& rawfileDescription) const override;
    bool CloseRawFileDescription(const std::string &rawfileName) const override;
    bool GetRawFD(const std::string& rawfileName, RawfileDescription& rawfileDescription) const override;
    bool GetMediaById(const int32_t& resId, std::string& mediaPath) const override;
    uint32_t GetResourceLimitKeys() const override;
    uint32_t GetSymbolByName(const char* resName) const override;
    uint32_t GetSymbolById(uint32_t resId) const override;
    RefPtr<ThemeStyle> GetPatternByName(const std::string& patternName) override;
    void UpdateColorMode(ColorMode colorMode) override;
    ColorMode GetResourceColorMode() const override;
    void SetAppHasDarkRes(bool hasDarkRes);
    RefPtr<ResourceAdapter> GetOverrideResourceAdapter(
        const ResourceConfiguration& config, const ConfigurationChange& configurationChange) override;
    bool ExistDarkResById(const std::string& resourceId) override;
    bool ExistDarkResByName(const std::string& resourceName, const std::string& resourceType) override;

private:
    std::unordered_map<std::string, uint32_t> patternNameMap_;
    std::string GetActualResourceName(const std::string& resName) const;
    bool NeedUpdateResConfig(const std::shared_ptr<Global::Resource::ResConfig>& oldResConfig,
        const std::shared_ptr<Global::Resource::ResConfig>& newResConfig);

    inline std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager() const
    {
        return sysResourceManager_;
    }
    RefPtr<TaskExecutor> GetTaskExecutor();

    std::shared_ptr<Global::Resource::ResourceManager> sysResourceManager_;
    std::string packagePathStr_;
    std::shared_ptr<Global::Resource::ResConfig> resConfig_;
    bool appHasDarkRes_ = false;
    std::mutex updateResConfigMutex_;
    ACE_DISALLOW_COPY_AND_MOVE(ResourceAdapterImplV2);
    void PreloadTheme(int32_t themeId, RefPtr<ResourceThemeStyle> theme);
};
} // namespace OHOS::Ace

#endif
