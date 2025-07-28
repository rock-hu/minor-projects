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

class ObjectSetFieldByNameByteTest : public AniTest {
public:
    ani_object NewAnimal()
    {
        auto animalRef = CallEtsFunction<ani_ref>("object_set_field_by_name_byte_test", "newAnimalObject");
        return static_cast<ani_object>(animalRef);
    }
};

constexpr ani_byte CMP_VALUE = -128;
constexpr ani_byte SET_VALUE = 127;

TEST_F(ObjectSetFieldByNameByteTest, set_field01)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(CallEtsFunction<ani_boolean>("object_set_field_by_name_byte_test", "checkObjectField", animal,
                                           static_cast<ani_byte>(SET_VALUE)),
              ANI_TRUE);

    ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "value", static_cast<ani_byte>(0)), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>("object_set_field_by_name_byte_test", "checkObjectField", animal,
                                           static_cast<ani_byte>(0)),
              ANI_TRUE);

    ani_byte value = 0;
    ASSERT_EQ(env_->Object_GetFieldByName_Byte(animal, "value", &value), ANI_OK);
    ASSERT_EQ(value, 0);
}
TEST_F(ObjectSetFieldByNameByteTest, set_field02)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(CallEtsFunction<ani_boolean>("object_set_field_by_name_byte_test", "checkObjectField", animal,
                                           static_cast<ani_byte>(SET_VALUE)),
              ANI_TRUE);

    const int32_t loopCount = 3;
    for (int32_t i = 1; i <= loopCount; i++) {
        ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "value", static_cast<ani_byte>(SET_VALUE)), ANI_OK);
        ASSERT_EQ(CallEtsFunction<ani_boolean>("object_set_field_by_name_byte_test", "checkObjectField", animal,
                                               static_cast<ani_byte>(SET_VALUE)),
                  ANI_TRUE);

        ani_byte value = 0;
        ASSERT_EQ(env_->Object_GetFieldByName_Byte(animal, "value", &value), ANI_OK);
        ASSERT_EQ(value, SET_VALUE);

        ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "value", static_cast<ani_byte>(CMP_VALUE)), ANI_OK);
        ASSERT_EQ(CallEtsFunction<ani_boolean>("object_set_field_by_name_byte_test", "checkObjectField", animal,
                                               static_cast<ani_byte>(CMP_VALUE)),
                  ANI_TRUE);

        ASSERT_EQ(env_->Object_GetFieldByName_Byte(animal, "value", &value), ANI_OK);
        ASSERT_EQ(value, CMP_VALUE);
    }
}

TEST_F(ObjectSetFieldByNameByteTest, invalid_env)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(env_->c_api->Object_SetFieldByName_Byte(nullptr, animal, "value", static_cast<ani_byte>(SET_VALUE)),
              ANI_INVALID_ARGS);
}

TEST_F(ObjectSetFieldByNameByteTest, not_found_name)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "x", static_cast<ani_byte>(SET_VALUE)), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "", static_cast<ani_byte>(SET_VALUE)), ANI_NOT_FOUND);
}

TEST_F(ObjectSetFieldByNameByteTest, invalid_type)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, "name", static_cast<ani_byte>(SET_VALUE)), ANI_INVALID_TYPE);
}

TEST_F(ObjectSetFieldByNameByteTest, invalid_object)
{
    ASSERT_EQ(env_->Object_SetFieldByName_Byte(nullptr, "x", static_cast<ani_byte>(SET_VALUE)), ANI_INVALID_ARGS);
}

TEST_F(ObjectSetFieldByNameByteTest, invalid_name)
{
    ani_object animal = NewAnimal();
    ASSERT_EQ(env_->Object_SetFieldByName_Byte(animal, nullptr, static_cast<ani_byte>(SET_VALUE)), ANI_INVALID_ARGS);
}

}  // namespace ark::ets::ani::testing
