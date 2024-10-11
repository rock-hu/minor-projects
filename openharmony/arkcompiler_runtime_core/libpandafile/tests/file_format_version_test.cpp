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

#include "file_format_version.h"

#include <gtest/gtest.h>

namespace panda::panda_file::test {
HWTEST(File, TestGetVersion, testing::ext::TestSize.Level0)
{
    std::string versionstr;
    for (size_t i = 0; i < File::VERSION_SIZE; i++) {
        versionstr += std::to_string(version[i]);
        if (i == (File::VERSION_SIZE - 1)) {
            break;
        }
        versionstr += ".";
    }
    EXPECT_EQ(GetVersion(version), versionstr);
}

HWTEST(File, GetMinVersion, testing::ext::TestSize.Level0)
{
    std::string versionstr;
    for (size_t i = 0; i < File::VERSION_SIZE; i++) {
        versionstr += std::to_string(minVersion[i]);
        if (i == (File::VERSION_SIZE - 1)) {
            break;
        }
        versionstr += ".";
    }
    EXPECT_EQ(GetVersion(minVersion), versionstr);
}

}  // namespace panda::panda_file::test
