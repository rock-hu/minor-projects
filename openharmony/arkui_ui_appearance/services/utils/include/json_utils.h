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

#ifndef UI_APPEARANCE_JSON_UTILS_H
#define UI_APPEARANCE_JSON_UTILS_H

#include <string>

#include "nlohmann/json.hpp"

namespace OHOS::ArkUi::UiAppearance {
/**
 * @class JsonUtils
 * provides json utilities.
 */
class JsonUtils {
public:
    static bool LoadConfiguration(const std::string& path, nlohmann::json& jsonBuf,
        const std::string& defaultPath = "");
    static std::string GetConfigPath(const std::string& path, const std::string& defaultPath);
    static bool ReadFileInfoJson(const std::string &filePath, nlohmann::json &jsonBuf);
    static bool ReadFileToBuffer(const std::string &filePath, std::string &dataBuffer);
};
} // namespace OHOS::ArkUi::UiAppearance
#endif // UI_APPEARANCE_JSON_UTILS_H
