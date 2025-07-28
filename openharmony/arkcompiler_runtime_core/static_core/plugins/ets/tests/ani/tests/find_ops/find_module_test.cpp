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

// NOLINTBEGIN(modernize-avoid-c-arrays)
namespace ark::ets::ani::testing {

class FindModuleTest : public AniTest {};

TEST_F(FindModuleTest, find_module_001)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule("L@abcModule/find_module_test;", &module), ANI_OK);
    ASSERT_NE(module, nullptr);

    ani_function fn {};
    ASSERT_EQ(env_->Module_FindFunction(module, "sum", "II:I", &fn), ANI_OK);
    ASSERT_NE(fn, nullptr);

    ani_value args[2U];
    const ani_int value1 = 5;
    const ani_int value2 = 6;
    args[0].i = value1;
    args[1].i = value2;
    ani_int value;
    ASSERT_EQ(env_->Function_Call_Int_A(fn, &value, args), ANI_OK);
    ASSERT_EQ(value, value1 + value2);
}

TEST_F(FindModuleTest, invalid_argument_env)
{
    ani_module module {};
    ASSERT_EQ(env_->FindModule(nullptr, &module), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->FindModule("", &module), ANI_INVALID_DESCRIPTOR);
    ASSERT_EQ(env_->FindModule("\t", &module), ANI_NOT_FOUND);
    ASSERT_EQ(env_->FindModule("L@abcModule/find_module_test;", nullptr), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->c_api->FindModule(nullptr, "L@abcModule/find_module_test;", &module), ANI_INVALID_ARGS);
}

}  // namespace ark::ets::ani::testing

// NOLINTEND(modernize-avoid-c-arrays)
