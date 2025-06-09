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

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
namespace ark::ets::ani::testing {

class CallStaticMethodTest : public AniTest {
public:
    static constexpr size_t ARG_COUNT = 2U;
    void GetMethodData(ani_class *clsResult, ani_static_method *methodResult)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass("Lcall_static_method_char_test/Operations;", &cls), ANI_OK);
        ASSERT_NE(cls, nullptr);

        ani_static_method method;
        ASSERT_EQ(env_->Class_FindStaticMethod(cls, "sub", "CC:C", &method), ANI_OK);
        ASSERT_NE(method, nullptr);

        *clsResult = cls;
        *methodResult = method;
    }
};

TEST_F(CallStaticMethodTest, call_static_method_char)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    char c = 'C' - 'A';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethod_Char(env_, cls, method, &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, c);
}

TEST_F(CallStaticMethodTest, call_static_method_char_v)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    char c = 'C' - 'A';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(cls, method, &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, c);
}

TEST_F(CallStaticMethodTest, call_static_method_char_A)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, method, &value, args), ANI_OK);
    ASSERT_EQ(value, args[1U].c - args[0U].c);
}

TEST_F(CallStaticMethodTest, call_static_method_char_null_class)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethod_Char(env_, nullptr, method, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_null_method)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethod_Char(env_, cls, nullptr, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_null_result)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ASSERT_EQ(env_->c_api->Class_CallStaticMethod_Char(env_, cls, method, nullptr, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_V_null_class)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(nullptr, method, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_V_null_method)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(cls, nullptr, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_v_null_result)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ASSERT_EQ(env_->Class_CallStaticMethod_Char(cls, method, nullptr, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_A_null_class)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(nullptr, method, &value, args), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_A_null_method)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, nullptr, &value, args), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_A_null_result)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, method, nullptr, args), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_A_null_args)
{
    ani_class cls {};
    ani_static_method method;
    GetMethodData(&cls, &method);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, method, &value, nullptr), ANI_INVALID_ARGS);
}

TEST_F(CallStaticMethodTest, call_static_method_char_combine_scenes_1)
{
    ani_class clsA {};
    ASSERT_EQ(env_->FindClass("Lcall_static_method_char_test/A;", &clsA), ANI_OK);
    ani_static_method methodA;
    ASSERT_EQ(env_->Class_FindStaticMethod(clsA, "funcA", "CC:C", &methodA), ANI_OK);

    ani_class clsB {};
    ASSERT_EQ(env_->FindClass("Lcall_static_method_char_test/B;", &clsB), ANI_OK);
    ani_static_method methodB;
    ASSERT_EQ(env_->Class_FindStaticMethod(clsB, "funcB", "CC:C", &methodB), ANI_OK);

    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(clsA, methodA, &valueA, 'A', 'C'), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_char valueB = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(clsB, methodB, &valueB, 'A', 'C'), ANI_OK);
    ASSERT_EQ(valueB, 'A' + 'C');

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueAA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(clsA, methodA, &valueAA, args), ANI_OK);
    ASSERT_EQ(valueAA, 'C' - 'A');

    ani_char valueBA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(clsB, methodB, &valueBA, args), ANI_OK);
    ASSERT_EQ(valueBA, 'A' + 'C');
}

TEST_F(CallStaticMethodTest, call_static_method_char_combine_scenes_2)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lcall_static_method_char_test/A;", &cls), ANI_OK);
    ani_static_method methodA;
    ASSERT_EQ(env_->Class_FindStaticMethod(cls, "funcA", "CC:C", &methodA), ANI_OK);
    ani_static_method methodB;
    ASSERT_EQ(env_->Class_FindStaticMethod(cls, "funcA", "II:I", &methodB), ANI_OK);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(cls, methodA, &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, methodA, &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_int value2 = 0;
    ASSERT_EQ(env_->Class_CallStaticMethod_Int(cls, methodB, &value2, 6U, 6U), ANI_OK);
    ASSERT_EQ(value2, 6U + 6U);
}

TEST_F(CallStaticMethodTest, call_static_method_char_combine_scenes_3)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lcall_static_method_char_test/A;", &cls), ANI_OK);
    ani_static_method method;
    ASSERT_EQ(env_->Class_FindStaticMethod(cls, "funcB", "CC:C", &method), ANI_OK);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char(cls, method, &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[ARG_COUNT];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethod_Char_A(cls, method, &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');
}
}  // namespace ark::ets::ani::testing
   // NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)