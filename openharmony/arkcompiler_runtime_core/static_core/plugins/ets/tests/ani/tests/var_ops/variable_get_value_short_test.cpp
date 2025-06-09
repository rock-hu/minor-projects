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

// NOLINTBEGIN(readability-identifier-naming, misc-non-private-member-variables-in-classes)
namespace ark::ets::ani::testing {

class VariableGetValueShortTest : public AniTest {
public:
    void SetUp() override
    {
        AniTest::SetUp();
        ASSERT_EQ(env_->FindNamespace("Lvariable_get_value_short_test/anyns;", &ns_), ANI_OK);
        ASSERT_NE(ns_, nullptr);
    }

    ani_namespace ns_ {};
};

TEST_F(VariableGetValueShortTest, get_short_value_normal_1)
{
    ani_variable variable {};
    ASSERT_EQ(env_->Namespace_FindVariable(ns_, "x", &variable), ANI_OK);
    ASSERT_NE(variable, nullptr);

    ani_short x = 0;
    ASSERT_EQ(env_->Variable_GetValue_Short(variable, &x), ANI_OK);
    ASSERT_EQ(x, 3U);
}

TEST_F(VariableGetValueShortTest, invalid_env)
{
    ani_variable variable {};
    ASSERT_EQ(env_->Namespace_FindVariable(ns_, "x", &variable), ANI_OK);
    ASSERT_NE(variable, nullptr);

    ani_short x = 0;
    ASSERT_EQ(env_->c_api->Variable_GetValue_Short(nullptr, variable, &x), ANI_INVALID_ARGS);
}

TEST_F(VariableGetValueShortTest, invalid_variable_type)
{
    ani_variable variable {};
    ASSERT_EQ(env_->Namespace_FindVariable(ns_, "z", &variable), ANI_OK);
    ASSERT_NE(variable, nullptr);

    ani_short x = 0;
    ASSERT_EQ(env_->Variable_GetValue_Short(variable, &x), ANI_INVALID_TYPE);
}

TEST_F(VariableGetValueShortTest, invalid_args_variable)
{
    ani_short x = 0;
    ASSERT_EQ(env_->Variable_GetValue_Short(nullptr, &x), ANI_INVALID_ARGS);
}

TEST_F(VariableGetValueShortTest, invalid_args_value)
{
    ani_variable variable {};
    ASSERT_EQ(env_->Namespace_FindVariable(ns_, "x", &variable), ANI_OK);
    ASSERT_NE(variable, nullptr);

    ASSERT_EQ(env_->Variable_GetValue_Short(variable, nullptr), ANI_INVALID_ARGS);
}

}  // namespace ark::ets::ani::testing

// NOLINTEND(readability-identifier-naming, misc-non-private-member-variables-in-classes)