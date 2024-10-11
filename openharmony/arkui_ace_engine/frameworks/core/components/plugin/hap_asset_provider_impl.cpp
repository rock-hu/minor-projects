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

#include "core/components/plugin/hap_asset_provider_impl.h"

#include "base/log/ace_trace.h"
#include "base/utils/utils.h"

namespace OHOS::Ace::Plugin {
bool HapAssetProviderImpl::Initialize(const std::string& hapPath, const std::vector<std::string>& assetBasePaths)
{
    ACE_SCOPED_TRACE("Initialize");
    if (hapPath.empty() || assetBasePaths.empty()) {
        LOGE("the packagePath or assetBasePath is empty");
        return false;
    }

    bool newCreate = false;
    runtimeExtractor_ = AbilityBase::ExtractorUtil::GetExtractor(hapPath, newCreate);
    CHECK_NULL_RETURN(runtimeExtractor_, false);
    assetBasePaths_ = assetBasePaths;
    hapPath_ = hapPath;
    return true;
}

bool HapAssetProviderImpl::IsValid() const
{
    return true;
}

std::unique_ptr<AssetMapping> HapAssetProviderImpl::GetAsMapping(const std::string& assetName) const
{
    ACE_SCOPED_TRACE("GetAsMapping");
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_NULL_RETURN(runtimeExtractor_, nullptr);
    for (const auto& basePath : assetBasePaths_) {
        std::string fileName = basePath + assetName;
        bool hasFile = runtimeExtractor_->HasEntry(fileName);
        if (!hasFile) {
            continue;
        }
        std::ostringstream osStream;
        hasFile = runtimeExtractor_->GetFileBuffer(fileName, osStream);
        if (!hasFile) {
            continue;
        }
        return std::make_unique<HapAssetImplMapping>(osStream);
    }
    return nullptr;
}

std::string HapAssetProviderImpl::GetAssetPath(const std::string& assetName, bool isAddHapPath)
{
    std::lock_guard<std::mutex> lock(mutex_);
    CHECK_NULL_RETURN(runtimeExtractor_, "");
    for (const auto& basePath : assetBasePaths_) {
        std::string fileName = basePath + assetName;
        bool hasFile = runtimeExtractor_->HasEntry(fileName);
        if (!hasFile) {
            continue;
        }
        return isAddHapPath ? (hapPath_ + "/" + basePath) : fileName;
    }
    return {};
}

void HapAssetProviderImpl::GetAssetList(const std::string& path, std::vector<std::string>& assetList)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (!runtimeExtractor_) {
        LOGW("RuntimeExtractor null:%{public}s", hapPath_.c_str());
        return;
    }
    for (const auto& basePath : assetBasePaths_) {
        std::string assetPath = basePath + path;
        bool res = runtimeExtractor_->IsDirExist(assetPath);
        if (!res) {
            continue;
        }
        res = runtimeExtractor_->GetFileList(assetPath, assetList);
        if (!res) {
            continue;
        }
    }
}

bool HapAssetProviderImpl::GetFileInfo(const std::string& /* fileName */, MediaFileInfo& /* fileInfo */) const
{
    return false;
}
} // namespace OHOS::Ace::Plugin
