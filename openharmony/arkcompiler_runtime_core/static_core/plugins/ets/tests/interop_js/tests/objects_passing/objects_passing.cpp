/**
 * Copyright (c) 2023-2024 Huawei Device Co., Ltd.
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

#include <gtest/gtest.h>
#include "ets_interop_js_gtest.h"

namespace ark::ets::interop::js::testing {

class EtsInteropObjectsPassing : public EtsInteropTest {};

TEST_F(EtsInteropObjectsPassing, getPublicProperty)
{
    ASSERT_EQ(CallEtsMethod<std::string>("getPublicProperty"), "TestName");
}

TEST_F(EtsInteropObjectsPassing, usePublicMethod)
{
    ASSERT_EQ(CallEtsMethod<std::string>("usePublicMethod"),
              "Name: TestName, Age: 30, ID: 456, Description: testDescription");
}

TEST_F(EtsInteropObjectsPassing, changePublicProperty)
{
    ASSERT_EQ(CallEtsMethod<std::string>("changePublicProperty"), "NewTestName");
}

TEST_F(EtsInteropObjectsPassing, getReadonlyProperty)
{
    ASSERT_EQ(CallEtsMethod<std::string>("getReadonlyProperty"), "testEdu");
}

TEST_F(EtsInteropObjectsPassing, getProtectedProperty)
{
    auto ret = CallEtsMethod<int64_t>("getProtectedProperty");
    constexpr int EXPECTED_VALUE = 789;
    ASSERT_EQ(ret, EXPECTED_VALUE);
}

TEST_F(EtsInteropObjectsPassing, useProtectedMethod)
{
    auto ret = CallEtsMethod<int64_t>("useProtectedMethod");
    constexpr int EXPECTED_VALUE = 789;
    ASSERT_EQ(ret, EXPECTED_VALUE);
}

TEST_F(EtsInteropObjectsPassing, getPropertyFromObject)
{
    ASSERT_EQ(CallEtsMethod<std::string>("getPropertyFromObject"), "TestName");
}

TEST_F(EtsInteropObjectsPassing, getOuterObject)
{
    auto ret = CallEtsMethod<int64_t>("getOuterObject");
    constexpr int EXPECTED_VALUE = 123;
    ASSERT_EQ(ret, EXPECTED_VALUE);
}

TEST_F(EtsInteropObjectsPassing, updateObjectValue)
{
    auto ret = CallEtsMethod<int64_t>("updateObjectValue");
    constexpr int EXPECTED_VALUE = 333;
    ASSERT_EQ(ret, EXPECTED_VALUE);
}

}  // namespace ark::ets::interop::js::testing
