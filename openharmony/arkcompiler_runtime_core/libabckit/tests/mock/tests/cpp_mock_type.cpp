/*
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

#include "../check_mock.h"
#include "../../../src/mock/mock_values.h"
#include "../cpp_helpers_mock.h"

#include <gtest/gtest.h>

namespace libabckit::test {

class LibAbcKitCppMockTestType : public ::testing::Test {};

// Test: test-kind=mock, api=Type::GetTypeId, abc-kind=ArkTS1, category=internal, extension=cpp
TEST_F(LibAbcKitCppMockTestType, Type_GetTypeId)
{
    ASSERT_TRUE(CheckMockedStackEmpty());
    {
        abckit::File f(DEFAULT_PATH);
        ASSERT_TRUE(CheckMockedApi("OpenAbc"));
        abckit::mock::helpers::GetMockType(f).GetTypeId();
        ASSERT_TRUE(CheckMockedApi("TypeGetTypeId"));
    }
    ASSERT_TRUE(CheckMockedApi("CloseFile"));
    ASSERT_TRUE(CheckMockedStackEmpty());
}

// Test: test-kind=mock, api=Type::GetReferenceClass, abc-kind=ArkTS1, category=internal, extension=cpp
TEST_F(LibAbcKitCppMockTestType, Type_GetReferenceClass)
{
    ASSERT_TRUE(CheckMockedStackEmpty());
    {
        abckit::File f(DEFAULT_PATH);
        ASSERT_TRUE(CheckMockedApi("OpenAbc"));
        abckit::mock::helpers::GetMockType(f).GetReferenceClass();
        ASSERT_TRUE(CheckMockedApi("TypeGetReferenceClass"));
    }
    ASSERT_TRUE(CheckMockedApi("CloseFile"));
    ASSERT_TRUE(CheckMockedStackEmpty());
}

}  // namespace libabckit::test