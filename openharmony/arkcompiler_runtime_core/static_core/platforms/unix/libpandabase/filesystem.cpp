/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <array>
#include <climits>
#include <cstdlib>

namespace ark::os {

std::string GetAbsolutePath(std::string_view path)
{
    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-member-init)
    std::array<char, PATH_MAX> resolvedPath;
    auto res = realpath(path.data(), resolvedPath.data());
    return res == nullptr ? std::string() : res;
}

}  // namespace ark::os
