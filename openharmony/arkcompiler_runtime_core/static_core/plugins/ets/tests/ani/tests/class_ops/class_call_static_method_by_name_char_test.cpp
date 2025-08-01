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

class ClassCallStaticMethodByNameTest : public AniTest {
public:
    void GetClassData(ani_class *clsResult)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/Operations;", &cls), ANI_OK);
        ASSERT_NE(cls, nullptr);
        *clsResult = cls;
    }
    void TestFuncV(ani_class cls, const char *name, ani_char *value, ...)
    {
        va_list args {};
        va_start(args, value);
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_V(cls, name, "CC:C", value, args), ANI_OK);
        va_end(args);
    }

    void TestCombineScene(const char *className, const char *methodName, ani_char val1, ani_char val2,
                          ani_char expectedValue)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass(className, &cls), ANI_OK);

        ani_char value = '\0';
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, methodName, "CC:C", &value, val1, val2), ANI_OK);
        ASSERT_EQ(value, expectedValue);

        ani_value args[2U];
        args[0U].c = val1;
        args[1U].c = val2;
        ani_char valueA = '\0';
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, methodName, "CC:C", &valueA, args), ANI_OK);
        ASSERT_EQ(valueA, expectedValue);
    }
};

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    char c = 'C' - 'A';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, cls, "sub", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, c);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    char c = 'C' - 'A';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "sub", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, c);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "sub", "CC:C", &value, args), ANI_OK);
    ASSERT_EQ(value, args[1U].c - args[0U].c);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_error_signature)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, cls, "sub", "CC:I", &value, 'A', 'C'),
              ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_null_class)
{
    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, nullptr, "sub", nullptr, &value, 'A', 'C'),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_null_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, cls, nullptr, nullptr, &value, 'A', 'C'),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_error_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, cls, "aa", nullptr, &value, 'A', 'C'),
              ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "", nullptr, &value, 'A', 'C'), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "\n", nullptr, &value, 'A', 'C'), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_null_result)
{
    ani_class cls {};
    GetClassData(&cls);

    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(env_, cls, "sub", nullptr, nullptr, 'A', 'C'),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v_error_signature)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "sub", "CC:I", &value, 'A', 'C'), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v_null_class)
{
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(nullptr, "sub", nullptr, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v_null_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, nullptr, nullptr, &value, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v_error_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "aa", nullptr, &value, 'A', 'C'), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_v_null_result)
{
    ani_class cls {};
    GetClassData(&cls);

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "sub", nullptr, nullptr, 'A', 'C'), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_error_signature)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "sub", "CC:I", &value, args), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_null_class)
{
    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(nullptr, "sub", nullptr, &value, args), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_null_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, nullptr, nullptr, &value, args), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_error_name)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "aa", nullptr, &value, args), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "", nullptr, &value, args), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "\n", nullptr, &value, args), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_null_result)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "sub", nullptr, nullptr, args), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_A_null_args)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "sub", nullptr, &value, nullptr), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_1)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_char_test/na;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LA;", &cls), ANI_OK);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "funcA", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "funcA", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_char valueV = '\0';
    TestFuncV(cls, "funcA", &valueV, 'A', 'C');
    ASSERT_EQ(valueV, 'C' - 'A');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_2)
{
    ani_namespace nb {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_char_test/nb;", &nb), ANI_OK);
    ani_namespace nc {};
    ASSERT_EQ(env_->Namespace_FindNamespace(nb, "Lnc;", &nc), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(nc, "LA;", &cls), ANI_OK);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "funcA", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "funcA", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_char valueV = '\0';
    TestFuncV(cls, "funcA", &valueV, 'A', 'C');
    ASSERT_EQ(valueV, 'C' - 'A');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_3)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_char_test/na;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LA;", &cls), ANI_OK);

    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "funcA", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "funcA", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_char valueV = '\0';
    TestFuncV(cls, "funcA", &valueV, 'A', 'C');
    ASSERT_EQ(valueV, 'C' - 'A');

    ani_int value2 = 0;
    const ani_int value3 = 4;
    const ani_int value4 = 7;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value2, value3, value4), ANI_OK);
    ASSERT_EQ(value2, value4 - value3);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_4)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_char_test/nd;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LB;", &cls), ANI_OK);
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "funcA", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'A' + 'C');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "funcA", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'A' + 'C');

    ani_char valueV = '\0';
    TestFuncV(cls, "funcA", &valueV, 'A', 'C');
    ASSERT_EQ(valueV, 'A' + 'C');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_null_env)
{
    ani_class cls {};
    GetClassData(&cls);

    ani_char value = '\0';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char(nullptr, cls, "or", nullptr, &value, 'A', 'C'),
              ANI_INVALID_ARGS);
    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Char_A(nullptr, cls, "or", nullptr, &value, args),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_5)
{
    ani_class clsA {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/A;", &clsA), ANI_OK);
    ani_class clsB {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/B;", &clsB), ANI_OK);

    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(clsA, "funcA", "CC:C", &valueA, 'A', 'C'), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');
    ani_char valueB = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(clsB, "funcB", "CC:C", &valueB, 'A', 'C'), ANI_OK);
    ASSERT_EQ(valueB, 'A' + 'C');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueAA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(clsA, "funcA", "CC:C", &valueAA, args), ANI_OK);
    ASSERT_EQ(valueAA, 'C' - 'A');
    ani_char valueBA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(clsB, "funcB", "CC:C", &valueBA, args), ANI_OK);
    ASSERT_EQ(valueBA, 'A' + 'C');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_6)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/A;", &cls), ANI_OK);
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "funcA", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "funcA", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');

    ani_int value2 = 0;
    const ani_int value3 = 4;
    const ani_int value4 = 7;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value2, value3, value4), ANI_OK);
    ASSERT_EQ(value2, value3 + value4);
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_7)
{
    TestCombineScene("Lclass_call_static_method_by_name_char_test/A;", "funcB", 'A', 'C', 'C' - 'A');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_8)
{
    TestCombineScene("Lclass_call_static_method_by_name_char_test/C;", "funcA", 'A', 'C', 'C' - 'A');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_9)
{
    TestCombineScene("Lclass_call_static_method_by_name_char_test/D;", "funcA", 'A', 'C', 'A' + 'C');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_10)
{
    TestCombineScene("Lclass_call_static_method_by_name_char_test/E;", "funcA", 'A', 'C', 'C' - 'A');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_11)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/F;", &cls), ANI_OK);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Void(cls, "increment", nullptr, 'A', 'C'), ANI_OK);
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "getCount", nullptr, &value), ANI_OK);
    ASSERT_EQ(value, 'A' + 'C');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "getCount", nullptr, &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'A' + 'C');
}

TEST_F(ClassCallStaticMethodByNameTest, call_static_method_by_name_char_combine_scenes_12)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_char_test/G;", &cls), ANI_OK);
    ani_char value = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "publicMethod", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'A' + 'C');
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char(cls, "callPrivateMethod", "CC:C", &value, 'A', 'C'), ANI_OK);
    ASSERT_EQ(value, 'C' - 'A');

    ani_value args[2U];
    args[0U].c = 'A';
    args[1U].c = 'C';
    ani_char valueA = '\0';
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "publicMethod", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'A' + 'C');
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Char_A(cls, "callPrivateMethod", "CC:C", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, 'C' - 'A');
}
}  // namespace ark::ets::ani::testing
   // NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
