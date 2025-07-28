/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "ets_interop_js_gtest.h"
#include <gtest/gtest.h>

namespace ark::ets::interop::js::testing {

class FunctionoverloadstsToEtsTest : public EtsInteropTest {};

TEST_F(FunctionoverloadstsToEtsTest, testProcessInput)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "testProcessInput"));
}

TEST_F(FunctionoverloadstsToEtsTest, testCreateArray)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "testCreateArray"));
}

TEST_F(FunctionoverloadstsToEtsTest, testGetUser)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "testGetUser"));
}

}  // namespace ark::ets::interop::js::testing