/*
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

#include "ani_gtest_tuple_ops.h"

namespace ark::ets::ani::testing {

class TupleValueSetItemShortTest : public AniGTestTupleOps {};

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShort)
{
    auto tuple = GetTupleWithCheck("tuplevalue_setitem_short_test", "getShortTuple");
    ani_short value = 1;
    ASSERT_EQ(env_->TupleValue_SetItem_Short(tuple, 0U, value), ANI_OK);
    ani_short result = 0;
    ASSERT_EQ(env_->TupleValue_GetItem_Short(tuple, 0U, &result), ANI_OK);
    ASSERT_EQ(result, value);
}

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShortNullEnv)
{
    auto tuple = GetTupleWithCheck("tuplevalue_setitem_short_test", "getShortTuple");
    ani_short value = 1;
    ASSERT_EQ(env_->c_api->TupleValue_SetItem_Short(nullptr, tuple, 0U, value), ANI_INVALID_ARGS);
}

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShortNullTuple)
{
    ani_short value = 1;
    ASSERT_EQ(env_->TupleValue_SetItem_Short(nullptr, 0U, value), ANI_INVALID_ARGS);
}

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShortIndexOutOfRange1)
{
    auto tuple = GetTupleWithCheck("tuplevalue_setitem_short_test", "getShortTuple");
    ani_short value = 1;
    ASSERT_EQ(env_->TupleValue_SetItem_Short(tuple, 6U, value), ANI_OUT_OF_RANGE);
}

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShortIndexOutOfRange2)
{
    auto tuple = GetTupleWithCheck("tuplevalue_setitem_short_test", "getShortTuple");
    ani_short value = 1;
    ASSERT_EQ(env_->TupleValue_SetItem_Short(tuple, -1U, value), ANI_OUT_OF_RANGE);
}

TEST_F(TupleValueSetItemShortTest, tupleValueSetItemShortRepeat)
{
    auto tuple = GetTupleWithCheck("tuplevalue_setitem_short_test", "getShortTuple");
    for (int32_t i = 0; i < LOOP_COUNT; i++) {
        ani_short value = 1;
        ASSERT_EQ(env_->TupleValue_SetItem_Short(tuple, 0U, value), ANI_OK);
        ani_short result = 0;
        ASSERT_EQ(env_->TupleValue_GetItem_Short(tuple, 0U, &result), ANI_OK);
        ASSERT_EQ(result, value);
    }
}
}  // namespace ark::ets::ani::testing