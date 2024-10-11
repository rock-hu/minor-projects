/*
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
#include <windows.h>
#include <cstdlib>

namespace panda::os {

std::string GetAbsolutePath(std::string_view path)
{
    constexpr size_t MAX_PATH_LEN = 2048;
    char full_path[MAX_PATH_LEN] = {0};
    _fullpath(full_path, path.data(), MAX_PATH_LEN);
    return full_path;
}

}  // namespace panda::os
