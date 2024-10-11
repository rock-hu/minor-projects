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

class EtsInteropScenariosJsToEtsIndirectCallUnion : public EtsInteropTest {};

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_call_arg_numeric)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_call_arg_numeric");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_call_arg_string)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_call_arg_string");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_apply_arg_numeric)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_apply_arg_numeric");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_apply_arg_string)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_apply_arg_string");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_bind_with_arg_numeric)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_bind_with_arg_numeric");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_bind_with_arg_string)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_bind_with_arg_string");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_bind_without_arg_numeric)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_bind_without_arg_numeric");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropScenariosJsToEtsIndirectCallUnion, Test_indirect_call_type_union_bind_without_arg_string)
{
    auto ret = CallEtsMethod<bool>("Test_indirect_call_type_union_bind_without_arg_string");
    ASSERT_EQ(ret, true);
}

}  // namespace ark::ets::interop::js::testing
