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

class ClassCallStaticMethodByNameIntTest : public AniTest {
public:
    static constexpr ani_double VAL1 = 1.5;
    static constexpr ani_double VAL2 = 2.5;
    static constexpr ani_int VAL3 = 5;
    static constexpr ani_int VAL4 = 6;
    void GetMethodData(ani_class *clsResult)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/Operations;", &cls), ANI_OK);
        ASSERT_NE(cls, nullptr);
        *clsResult = cls;
    }
    void TestFuncV(ani_class cls, const char *name, ani_int *value, ...)
    {
        va_list args {};
        va_start(args, value);
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_V(cls, name, "II:I", value, args), ANI_OK);
        va_end(args);
    }

    void TestCombineScene(const char *className, const char *methodName, ani_int expectedValue)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass(className, &cls), ANI_OK);

        ani_int value = 0;
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, methodName, "II:I", &value, VAL3, VAL4), ANI_OK);
        ASSERT_EQ(value, expectedValue);

        ani_value args[2U];
        args[0U].i = VAL3;
        args[1U].i = VAL4;
        ani_int valueA = 0;
        ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, methodName, "II:I", &valueA, args), ANI_OK);
        ASSERT_EQ(valueA, expectedValue);
    }
};

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int sum = 0;
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int(env_, cls, "sum", nullptr, &sum, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(sum, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_v)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "sum", nullptr, &sum, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(sum, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_A)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;

    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "sum", nullptr, &sum, args), ANI_OK);
    ASSERT_EQ(sum, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_invalid_cls)
{
    ani_int sum = 0;
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int(env_, nullptr, "sum", nullptr, &sum, VAL3, VAL4),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_invalid_name)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int sum = 0;
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int(env_, cls, nullptr, nullptr, &sum, VAL3, VAL4),
              ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "", nullptr, &sum, VAL3, VAL4), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "\n", nullptr, &sum, VAL3, VAL4), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_invalid_result)
{
    ani_class cls {};
    GetMethodData(&cls);

    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int(env_, cls, "sum", nullptr, nullptr, VAL3, VAL4),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_v_invalid_cls)
{
    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(nullptr, "sum", nullptr, &sum, VAL3, VAL4), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_v_invalid_name)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, nullptr, nullptr, &sum, VAL3, VAL4), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "sum_not_exist", nullptr, &sum, VAL3, VAL4), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_v_invalid_result)
{
    ani_class cls {};
    GetMethodData(&cls);

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "sum", nullptr, nullptr, VAL3, VAL4), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_a_invalid_cls)
{
    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(nullptr, "sum", nullptr, &sum, args), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_a_invalid_name)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, nullptr, nullptr, &sum, args), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "sum_not_exist", nullptr, &sum, args), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "", nullptr, &sum, args), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "\n", nullptr, &sum, args), ANI_NOT_FOUND);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_a_invalid_result)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "sum", nullptr, nullptr, args), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_a_invalid_args)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int sum = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "sum", nullptr, &sum, nullptr), ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_1)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_int_test/na;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LA;", &cls), ANI_OK);

    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "funcA", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);

    ani_int valueV = 0;
    TestFuncV(cls, "funcA", &valueV, VAL3, VAL4);
    ASSERT_EQ(valueV, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_2)
{
    ani_namespace nb {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_int_test/nb;", &nb), ANI_OK);
    ani_namespace nc {};
    ASSERT_EQ(env_->Namespace_FindNamespace(nb, "Lnc;", &nc), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(nc, "LA;", &cls), ANI_OK);

    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "funcA", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);

    ani_int valueV = 0;
    TestFuncV(cls, "funcA", &valueV, VAL3, VAL4);
    ASSERT_EQ(valueV, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_3)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_int_test/na;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LA;", &cls), ANI_OK);
    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "funcA", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);

    ani_int valueV = 0;
    TestFuncV(cls, "funcA", &valueV, VAL3, VAL4);
    ASSERT_EQ(valueV, VAL3 + VAL4);

    ani_double value2 = 0.0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Double(cls, "funcA", "DD:D", &value2, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(value2, VAL2 - VAL1);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_4)
{
    ani_namespace ns {};
    ASSERT_EQ(env_->FindNamespace("Lclass_call_static_method_by_name_int_test/nd;", &ns), ANI_OK);
    ani_class cls {};
    ASSERT_EQ(env_->Namespace_FindClass(ns, "LB;", &cls), ANI_OK);

    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL4 - VAL3);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "funcA", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL4 - VAL3);

    ani_int valueV = 0;
    TestFuncV(cls, "funcA", &valueV, VAL3, VAL4);
    ASSERT_EQ(valueV, VAL4 - VAL3);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_null_env)
{
    ani_class cls {};
    GetMethodData(&cls);

    ani_int value = 0;
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int(nullptr, cls, "or", nullptr, &value, VAL3, VAL4),
              ANI_INVALID_ARGS);
    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ASSERT_EQ(env_->c_api->Class_CallStaticMethodByName_Int_A(nullptr, cls, "or", nullptr, &value, args),
              ANI_INVALID_ARGS);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_5)
{
    ani_class clsA {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/A;", &clsA), ANI_OK);
    ani_class clsB {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/B;", &clsB), ANI_OK);

    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(clsA, "funcA", "II:I", &valueA, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);
    ani_int valueB = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(clsB, "funcB", "II:I", &valueB, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(valueB, VAL4 - VAL3);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueAA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(clsA, "funcA", "II:I", &valueAA, args), ANI_OK);
    ASSERT_EQ(valueAA, VAL3 + VAL4);
    ani_int valueBA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(clsB, "funcB", "II:I", &valueBA, args), ANI_OK);
    ASSERT_EQ(valueBA, VAL4 - VAL3);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_6)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/A;", &cls), ANI_OK);
    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "funcA", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "funcA", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);

    ani_double value2 = 0.0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Double(cls, "funcA", "DD:D", &value2, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(value2, VAL1 + VAL2);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_7)
{
    TestCombineScene("Lclass_call_static_method_by_name_int_test/A;", "funcB", VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_8)
{
    TestCombineScene("Lclass_call_static_method_by_name_int_test/C;", "funcA", VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_9)
{
    TestCombineScene("Lclass_call_static_method_by_name_int_test/D;", "funcA", VAL4 - VAL3);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_10)
{
    TestCombineScene("Lclass_call_static_method_by_name_int_test/E;", "funcA", VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_11)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/F;", &cls), ANI_OK);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Void(cls, "increment", nullptr, VAL3, VAL4), ANI_OK);
    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "getCount", nullptr, &value), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "getCount", nullptr, &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);
}

TEST_F(ClassCallStaticMethodByNameIntTest, call_static_method_by_name_int_combine_scenes_12)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lclass_call_static_method_by_name_int_test/G;", &cls), ANI_OK);
    ani_int value = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "publicMethod", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL3 + VAL4);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "callPrivateMethod", "II:I", &value, VAL3, VAL4), ANI_OK);
    ASSERT_EQ(value, VAL4 - VAL3);

    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;
    ani_int valueA = 0;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "publicMethod", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL3 + VAL4);
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "callPrivateMethod", "II:I", &valueA, args), ANI_OK);
    ASSERT_EQ(valueA, VAL4 - VAL3);
}

TEST_F(ClassCallStaticMethodByNameIntTest, check_initialization_int)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("class_call_static_method_by_name_int_test.G", &cls), ANI_OK);

    ASSERT_FALSE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));
    ani_int value {};

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "publicMethodx", "ii:i", &value, VAL3, VAL4), ANI_NOT_FOUND);
    ASSERT_FALSE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int(cls, "publicMethod", "ii:i", &value, VAL3, VAL4), ANI_OK);
    ASSERT_TRUE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));
}

TEST_F(ClassCallStaticMethodByNameIntTest, check_initialization_int_a)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("class_call_static_method_by_name_int_test.G", &cls), ANI_OK);

    ASSERT_FALSE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));
    ani_int value {};
    ani_value args[2U];
    args[0U].i = VAL3;
    args[1U].i = VAL4;

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "publicMethodx", "ii:i", &value, args), ANI_NOT_FOUND);
    ASSERT_FALSE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Int_A(cls, "publicMethod", "ii:i", &value, args), ANI_OK);
    ASSERT_TRUE(IsRuntimeClassInitialized("class_call_static_method_by_name_int_test.G"));
}

}  // namespace ark::ets::ani::testing
// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
