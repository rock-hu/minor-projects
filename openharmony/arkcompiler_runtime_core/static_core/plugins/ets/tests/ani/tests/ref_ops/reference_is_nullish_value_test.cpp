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

#include "ani_gtest.h"

namespace ark::ets::ani::testing {

class ReferenceIsNullishValueTest : public AniTest {
public:
    static constexpr int32_t LOOP_COUNT = 3;
};

TEST_F(ReferenceIsNullishValueTest, check_null)
{
    auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetNull");
    ani_boolean isUndefined = ANI_FALSE;
    ASSERT_EQ(env_->Reference_IsNullishValue(ref, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_TRUE);
}

TEST_F(ReferenceIsNullishValueTest, check_undefined)
{
    auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetUndefined");
    ani_boolean isUndefined = ANI_FALSE;
    ASSERT_EQ(env_->Reference_IsNullishValue(ref, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_TRUE);
}

TEST_F(ReferenceIsNullishValueTest, check_object)
{
    auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetObject");
    ani_boolean isUndefined = ANI_TRUE;
    ASSERT_EQ(env_->Reference_IsNullishValue(ref, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_FALSE);
}

TEST_F(ReferenceIsNullishValueTest, invalid_argument)
{
    auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetNull");
    ASSERT_EQ(env_->Reference_IsNullishValue(ref, nullptr), ANI_INVALID_ARGS);
    ani_boolean isUndefined = ANI_FALSE;
    ASSERT_EQ(env_->c_api->Reference_IsNullishValue(nullptr, ref, &isUndefined), ANI_INVALID_ARGS);
}

TEST_F(ReferenceIsNullishValueTest, check_null_loop)
{
    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetNull");
        ani_boolean isUndefined = ANI_FALSE;
        ASSERT_EQ(env_->Reference_IsNullishValue(ref, &isUndefined), ANI_OK);
        ASSERT_EQ(isUndefined, ANI_TRUE);
    }
}

TEST_F(ReferenceIsNullishValueTest, mix_test)
{
    auto ref = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetNull");
    ani_boolean isUndefined = ANI_FALSE;
    ASSERT_EQ(env_->Reference_IsNullishValue(ref, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_TRUE);

    auto undefineRef = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetUndefined");
    ASSERT_EQ(env_->Reference_IsNullishValue(undefineRef, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_TRUE);

    auto objectRef = CallEtsFunction<ani_ref>("reference_is_nullish_value_test", "GetObject");
    ASSERT_EQ(env_->Reference_IsNullishValue(objectRef, &isUndefined), ANI_OK);
    ASSERT_EQ(isUndefined, ANI_FALSE);

    ani_ref nullRef = nullptr;

    ani_boolean isEquals = ANI_FALSE;
    ASSERT_EQ(env_->Reference_Equals(ref, undefineRef, &isEquals), ANI_OK);
    ASSERT_EQ(isEquals, ANI_TRUE);

    ASSERT_EQ(env_->Reference_Equals(undefineRef, objectRef, &isEquals), ANI_OK);
    ASSERT_EQ(isEquals, ANI_FALSE);

    ASSERT_EQ(env_->Reference_Equals(ref, objectRef, &isEquals), ANI_OK);
    ASSERT_EQ(isEquals, ANI_FALSE);

    ASSERT_EQ(env_->Reference_Equals(ref, nullRef, &isEquals), ANI_OK);
    ASSERT_EQ(isEquals, ANI_TRUE);
}
}  // namespace ark::ets::ani::testing
