
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

class EtsCallableSignatureEtsToJsTest : public EtsInteropTest {};

TEST_F(EtsCallableSignatureEtsToJsTest, check_callable_signature)
{
    ASSERT_TRUE(RunJsTestSuite("check_callable_signature.js"));
}
// NOTE (alexanderpolenov) (18506) js call static invoke and instantiate
TEST_F(EtsCallableSignatureEtsToJsTest, DISABLED_check_call_invoke_instantiate)
{
    ASSERT_TRUE(RunJsTestSuite("check_call_invoke_instantiate.js"));
}

}  // namespace ark::ets::interop::js::testing
