/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
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

class EtsInteropScenariosJsToEtsIndirectCallAny : public EtsInteropTest {};

// NOTE #18409 enable this test after interop will return valid value from test call.
TEST_F(EtsInteropScenariosJsToEtsIndirectCallAny, DISABLED_Test_indirect_call_test_return_value)
{
    auto ret = CallEtsFunction<bool>(GetPackageName(), "Test_indirect_call_test_return_value");
    ASSERT_EQ(ret, false);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallAny, Test_indirect_call_type_any_call)
{
    auto ret = CallEtsFunction<bool>(GetPackageName(), "Test_indirect_call_type_any_call");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallAny, Test_indirect_call_type_any_apply)
{
    auto ret = CallEtsFunction<bool>(GetPackageName(), "Test_indirect_call_type_any_apply");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallAny, Test_indirect_call_type_any_bind_with_arg)
{
    auto ret = CallEtsFunction<bool>(GetPackageName(), "Test_indirect_call_type_any_bind_with_arg");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallAny, Test_indirect_call_type_any_bind_without_arg)
{
    auto ret = CallEtsFunction<bool>(GetPackageName(), "Test_indirect_call_type_any_bind_without_arg");
    ASSERT_EQ(ret, true);
}

}  // namespace ark::ets::interop::js::testing
