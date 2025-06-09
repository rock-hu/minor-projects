/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"
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

#include "ani_gtest.h"

namespace ark::ets::ani::testing {

class GetUndefinedTest : public AniTest {};

TEST_F(GetUndefinedTest, get_undefined)
{
    ani_ref ref;
    ASSERT_EQ(env_->GetUndefined(&ref), ANI_OK);

    auto isUndefined = CallEtsFunction<ani_boolean>("get_undefined_test", "isUndefined", ref);
    ASSERT_EQ(isUndefined, ANI_TRUE);
}

TEST_F(GetUndefinedTest, invalid_argument)
{
    ASSERT_EQ(env_->GetUndefined(nullptr), ANI_INVALID_ARGS);
}

}  // namespace ark::ets::ani::testing
