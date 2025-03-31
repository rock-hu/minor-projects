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
#include "grid_test_ng.h"

#include "core/components_ng/pattern/grid/grid_pattern.h"
#include "core/components_ng/pattern/grid/grid_utils.h"

namespace OHOS::Ace::NG {
class GridPatternTestNg : public GridTestNg {};

/**
 * @tc.name: ParseArgsTest001
 * @tc.desc: Test Empty String
 * @tc.type: FUNC
 */
HWTEST_F(GridPatternTestNg, ParseArgsTest001, TestSize.Level1)
{
    // Arrange
    std::string args = "";

    // Act
    std::string result = GridUtils::ParseArgs(args);

    // Assert
    EXPECT_EQ(args, result);
}

/**
 * @tc.name: ParseArgsTest002
 * @tc.desc: Test No Auto Keyword
 * @tc.type: FUNC
 */
HWTEST_F(GridPatternTestNg, ParseArgsTest002, TestSize.Level1)
{
    // Arrange
    std::string args = "1 2 3";

    // Act
    std::string result = GridUtils::ParseArgs(args);

    // Assert
    EXPECT_EQ(args, result);
}

/**
 * @tc.name: ParseArgsTest003
 * @tc.desc: Test Single Auto Keyword
 * @tc.type: FUNC
 */
HWTEST_F(GridPatternTestNg, ParseArgsTest003, TestSize.Level1)
{
    // Arrange
    std::string args = "auto";

    // Act
    std::string result = GridUtils::ParseArgs(args);

    // Assert
    EXPECT_EQ(" 1fr", result);
}

/**
 * @tc.name: ParseArgsTest004
 * @tc.desc: Test Multiple Auto Keywords
 * @tc.type: FUNC
 */
HWTEST_F(GridPatternTestNg, ParseArgsTest004, TestSize.Level1)
{
    // Arrange
    std::string args = "auto 2 auto";

    // Act
    std::string result = GridUtils::ParseArgs(args);

    // Assert
    EXPECT_EQ(" 1fr 2 1fr", result);
}

/**
 * @tc.name: ParseArgsTest005
 * @tc.desc: Test Mixed Args
 * @tc.type: FUNC
 */
HWTEST_F(GridPatternTestNg, ParseArgsTest005, TestSize.Level1)
{
    // Arrange
    std::string args = "1fr 2 3 auto";

    // Act
    std::string result = GridUtils::ParseArgs(args);

    // Assert
    EXPECT_EQ(" 1fr 2 3 1fr", result);
}
} // namespace OHOS::Ace::NG
