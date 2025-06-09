/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "core/common/resource/pattern_resource_manager.h"

#include "base/utils/device_config.h"
#include "core/common/container.h"
#include "core/common/resource/resource_parse_utils.h"

namespace OHOS::Ace {
void PatternResourceManager::AddResource(
    const std::string& key,
    const RefPtr<ResourceObject>& resObj,
    std::function<void(const RefPtr<ResourceObject>&)>&& updateFunc)
{
    if (resObj == nullptr || !updateFunc) {
        return;
    }
    if (resMap_.count(key) > 0) {
        resCacheMap_.clear();
    }
    resMap_[key] = { resObj, std::move(updateFunc) };
    resKeyArray_.emplace_back(key);
}

void PatternResourceManager::AddResCache(const std::string& key, const std::string& value)
{
    if (key.empty() || value.empty()) {
        return;
    }
    bool darkMode = Container::CurrentColorMode() == ColorMode::DARK;
    resCacheMap_[key][darkMode] = value;
}

std::string PatternResourceManager::GetResCacheMapByKey(const std::string& key)
{
    bool darkMode = Container::CurrentColorMode() == ColorMode::DARK;
    if (!resCacheMap_.count(key) || !resCacheMap_[key].count(darkMode)) {
        return "";
    }
    return resCacheMap_[key][darkMode];
}

void PatternResourceManager::RemoveResource(const std::string& key)
{
    auto iter = resMap_.find(key);
    if (iter != resMap_.end()) {
        resMap_.erase(iter);
    }
    auto it = std::find(resKeyArray_.begin(), resKeyArray_.end(), key);
    if (it != resKeyArray_.end()) {
        resKeyArray_.erase(it);
    }
    resCacheMap_.clear();
}

void PatternResourceManager::ReloadResources()
{
    for (const auto& key : resKeyArray_) {
        if (!resMap_.count(key)) {
            continue;
        }
        auto resourceUpdater = resMap_[key];
        resourceUpdater.updateFunc(resourceUpdater.obj);
    }
}

bool PatternResourceManager::Empty()
{
    return resMap_.empty();
}

void PatternResourceManager::ParsePropertyValue(
    const RefPtr<ResourceObject>& resObj, RefPtr<PropertyValueBase> valueBase)
{
    if (auto castdVal = AceType::DynamicCast<PropertyValue<std::string>>(valueBase)) {
        std::string value;
        ResourceParseUtils::ParseResString(resObj, value);
        castdVal->SetValue(value);
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<std::u16string>>(valueBase)) {
        std::u16string value;
        ResourceParseUtils::ParseResString(resObj, value);
        castdVal->SetValue(value);
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<Color>>(valueBase)) {
        Color value;
        ResourceParseUtils::ParseResColor(resObj, value);
        castdVal->SetValue(value);
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<double>>(valueBase)) {
        double value;
        ResourceParseUtils::ParseResDouble(resObj, value);
        castdVal->SetValue(value);
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<CalcDimension>>(valueBase)) {
        CalcDimension value;
        ResourceParseUtils::ParseResDimensionFpNG(resObj, value);
        castdVal->SetValue(value);
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<float>>(valueBase)) {
        double value;
        ResourceParseUtils::ParseResDouble(resObj, value);
        castdVal->SetValue(static_cast<float>(value));
    } else if (auto castdVal = AceType::DynamicCast<PropertyValue<std::vector<std::string>>>(valueBase)) {
        std::vector<std::string> value;
        ResourceParseUtils::ParseResFontFamilies(resObj, value);
        castdVal->SetValue(value);
    }
}
}
