/**
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "os/filesystem.h"

#include <ostream>
#include <string>
#if defined PANDA_TARGET_MOBILE || defined PANDA_TARGET_UNIX || defined PANDA_TARGET_ARM32 || \
    defined PANDA_TARGET_ARM64
#include <sys/stat.h>
#endif
#if defined(PANDA_TARGET_WINDOWS)
#include <fileapi.h>
#endif

#include "utils/logger.h"

namespace panda::os {

void CreateDirectories(const std::string &folder_name)
{
#ifdef PANDA_TARGET_MOBILE
    constexpr auto DIR_PERMISSIONS = 0775;
    mkdir(folder_name.c_str(), DIR_PERMISSIONS);
#elif PANDA_TARGET_MACOS || PANDA_TARGET_OHOS
    std::filesystem::create_directories(std::filesystem::path(folder_name));
#elif PANDA_TARGET_WINDOWS
    CreateDirectory(folder_name.c_str(), NULL);
#else
    constexpr auto DIR_PERMISSIONS = 0775;
    auto status = mkdir(folder_name.c_str(), DIR_PERMISSIONS);
    if (status != 0) {
        LOG(WARNING, COMMON) << "Failed to create directory \"" << folder_name.c_str() << "\"\n";
        LOG(WARNING, COMMON) << "Return status :" << status << "\n";
    }
#endif
}

}  // namespace panda::os
