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

class DateTsToEtsTest : public EtsInteropTest {};

TEST_F(DateTsToEtsTest, check_date)
{
    ASSERT_TRUE(RunJsTestSuite("date.ts"));
}

TEST_F(DateTsToEtsTest, DISABLED_check_disable_date)
{
    ASSERT_TRUE(RunJsTestSuite("disable_date.ts"));
}

}  // namespace ark::ets::interop::js::testing
