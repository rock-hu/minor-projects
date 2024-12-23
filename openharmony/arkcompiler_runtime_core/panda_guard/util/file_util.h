/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PANDA_GUARD_UTIL_FILE_UTIL_H
#define PANDA_GUARD_UTIL_FILE_UTIL_H

#include <string>
#include <vector>

namespace panda::guard {

class FileUtil {
public:
    static std::string GetFileContent(const std::string &filePath);

    static std::vector<std::string> GetLineDataFromFile(const std::string &filePath);

    static void WriteFile(const std::string &filePath, const std::string &content);
};

}  // namespace panda::guard

#endif  // PANDA_GUARD_UTIL_FILE_UTIL_H
