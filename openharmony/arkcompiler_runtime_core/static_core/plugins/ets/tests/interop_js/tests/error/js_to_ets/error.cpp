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

#include <gtest/gtest.h>
#include "ets_interop_js_gtest.h"

namespace ark::ets::interop::js::testing {

class EtsErrorJsToEtsTest : public EtsInteropTest {};

TEST_F(EtsErrorJsToEtsTest, test_error_catch_num_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkNumError"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_str_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkStrError"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_obj_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkObjError"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_subclass_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkSubClassError"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_error_data)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkErrorData"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_error_msg)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkErrorMsg"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_error_cause)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkErrorCause"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkError"));
}

TEST_F(EtsErrorJsToEtsTest, test_error_catch_ets_error)
{
    ASSERT_EQ(true, CallEtsFunction<bool>(GetPackageName(), "checkEtsError"));
}

}  // namespace ark::ets::interop::js::testing
