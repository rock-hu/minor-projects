/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

class EtsInteropClassCompositeTest : public EtsInteropTest {};

TEST_F(EtsInteropClassCompositeTest, Test1)
{
    auto ret = CallEtsFunction<int64_t>(GetPackageName(), "Test1");
    ASSERT_EQ(ret, 0);
}

TEST_F(EtsInteropClassCompositeTest, Test2)
{
    auto ret = CallEtsFunction<int64_t>(GetPackageName(), "Test2");
    ASSERT_EQ(ret, 0);
}

}  // namespace ark::ets::interop::js::testing
