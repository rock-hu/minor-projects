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

#include <gtest/gtest.h>
#include "ets_interop_js_gtest.h"

namespace ark::ets::interop::js::testing {

class EtsAgeNameArgGenericNegativeResultTsToEtsTest : public EtsInteropTest {};

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkArgFuFromSts)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkArgFuFromSts"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkTypeClass)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkTypeClass"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkClassMethod)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkClassMethod"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkInstanceClass)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkInstanceClass"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkChildClass)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkChildClass"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkChildClassMethod)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkChildClassMethod"));
}

TEST_F(EtsAgeNameArgGenericNegativeResultTsToEtsTest, checkInstanceChildClassMethod)
{
    ASSERT_EQ(true, CallEtsMethod<bool>("checkInstanceChildClassMethod"));
}

}  // namespace ark::ets::interop::js::testing
