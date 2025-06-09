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

class EtsInteropCallableTest : public EtsInteropTest {};

TEST_F(EtsInteropCallableTest, TestFuncCallArg_Pos)
{
    // NOLINTNEXTLINE(modernize-use-auto)
    auto ret = CallEtsFunction<bool>(GetPackageName(), "TestFuncCallArg_Pos");
    ASSERT_EQ(ret, true);
}

TEST_F(EtsInteropCallableTest, TestFuncCallArg_Neg)
{
    // NOLINTNEXTLINE(modernize-use-auto)
    auto ret = CallEtsFunction<bool>(GetPackageName(), "TestFuncCallArg_Neg");
    ASSERT_EQ(ret, false);
}

}  // namespace ark::ets::interop::js::testing
