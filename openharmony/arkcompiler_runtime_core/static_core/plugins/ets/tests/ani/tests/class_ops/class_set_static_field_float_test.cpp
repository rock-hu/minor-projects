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
#include <cmath>

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
namespace ark::ets::ani::testing {

class ClassSetStaticFieldFloatTest : public AniTest {
public:
    void CheckFieldValue(const char *className, const char *fieldName)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass(className, &cls), ANI_OK);
        ani_static_field field {};
        ASSERT_EQ(env_->Class_FindStaticField(cls, fieldName, &field), ANI_OK);
        ASSERT_NE(field, nullptr);
        ani_float result = 0U;
        const ani_float target = 18.0F;
        ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
        ASSERT_EQ(result, target);
        const ani_float setTar = 20.0F;
        ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, setTar), ANI_OK);
        ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
        ASSERT_EQ(result, setTar);
    }
};

TEST_F(ClassSetStaticFieldFloatTest, set_float)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    ani_float result = 0.0F;
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
    const ani_float target = 18.0F;
    ASSERT_EQ(result, target);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, setTar), ANI_OK);
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
    ASSERT_EQ(result, setTar);
}

TEST_F(ClassSetStaticFieldFloatTest, set_float_c_api)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    ani_float result = 0.0F;
    ASSERT_EQ(env_->c_api->Class_GetStaticField_Float(env_, cls, field, &result), ANI_OK);
    const ani_float target = 18.0F;
    ASSERT_EQ(result, target);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->c_api->Class_SetStaticField_Float(env_, cls, field, setTar), ANI_OK);
    ASSERT_EQ(env_->c_api->Class_GetStaticField_Float(env_, cls, field, &result), ANI_OK);
    ASSERT_EQ(result, setTar);
}

TEST_F(ClassSetStaticFieldFloatTest, set_invalid_field_type)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    ASSERT_EQ(env_->Class_FindStaticField(cls, "string_value", &field), ANI_OK);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, setTar), ANI_INVALID_TYPE);
}

TEST_F(ClassSetStaticFieldFloatTest, invalid_argument2)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    ani_float result = 0.0F;
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
    const ani_float target = 18.0F;
    ASSERT_EQ(result, target);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->Class_SetStaticField_Float(nullptr, field, setTar), ANI_INVALID_ARGS);
}

TEST_F(ClassSetStaticFieldFloatTest, invalid_argument3)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, nullptr, setTar), ANI_INVALID_ARGS);
}

TEST_F(ClassSetStaticFieldFloatTest, invalid_argument4)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field = nullptr;
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    const ani_float setTar = 28.0F;
    ASSERT_EQ(env_->c_api->Class_SetStaticField_Float(nullptr, cls, field, setTar), ANI_INVALID_ARGS);
}

TEST_F(ClassSetStaticFieldFloatTest, special_values)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    ani_float single = 0.0F;
    ani_float max = std::numeric_limits<ani_float>::max();
    ani_float minpositive = std::numeric_limits<ani_float>::min();
    ani_float min = -std::numeric_limits<ani_float>::max();

    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, max), ANI_OK);
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &single), ANI_OK);
    ASSERT_EQ(single, max);

    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, minpositive), ANI_OK);
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &single), ANI_OK);
    ASSERT_EQ(single, minpositive);

    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, min), ANI_OK);
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &single), ANI_OK);
    ASSERT_EQ(single, min);
}

TEST_F(ClassSetStaticFieldFloatTest, combination_test1)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_set_static_field_float_test/TestSetFloat;", &cls), ANI_OK);
    ani_static_field field {};
    const ani_float setTar = 28.0F;
    const ani_float setTar2 = 18.0F;
    ASSERT_EQ(env_->Class_FindStaticField(cls, "float_value", &field), ANI_OK);
    ASSERT_NE(field, nullptr);
    ani_float result = 0.0F;
    const int32_t loopCount = 3;
    for (int32_t i = 0; i < loopCount; i++) {
        ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, setTar2), ANI_OK);
        ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
        ASSERT_EQ(result, setTar2);
    }
    ASSERT_EQ(env_->Class_SetStaticField_Float(cls, field, setTar), ANI_OK);
    ASSERT_EQ(env_->Class_GetStaticField_Float(cls, field, &result), ANI_OK);
    ASSERT_EQ(result, setTar);
}

TEST_F(ClassSetStaticFieldFloatTest, combination_test2)
{
    CheckFieldValue("Lclass_set_static_field_float_test/TestSetFloatA;", "float_value");
}

TEST_F(ClassSetStaticFieldFloatTest, combination_test3)
{
    CheckFieldValue("Lclass_set_static_field_float_test/TestSetFloatFinal;", "float_value");
}
}  // namespace ark::ets::ani::testing

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)