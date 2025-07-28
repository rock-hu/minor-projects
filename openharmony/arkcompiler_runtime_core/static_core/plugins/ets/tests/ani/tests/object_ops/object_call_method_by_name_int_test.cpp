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

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays, readability-magic-numbers)
namespace ark::ets::ani::testing {

class CallObjectMethodIntByNameTest : public AniTest {
public:
    static constexpr ani_int VAL1 = 5;
    static constexpr ani_int VAL2 = 6;

    void GetMethodData(ani_object *objectResult)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/A;", &cls), ANI_OK);
        ASSERT_NE(cls, nullptr);

        ani_static_method newMethod {};
        ASSERT_EQ(env_->Class_FindStaticMethod(cls, "new_A", ":Lobject_call_method_by_name_int_test/A;", &newMethod),
                  ANI_OK);
        ani_ref ref {};
        ASSERT_EQ(env_->Class_CallStaticMethod_Ref(cls, newMethod, &ref), ANI_OK);
        *objectResult = static_cast<ani_object>(ref);
    }
};

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_a)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, "intByNameMethod", "II:I", &res, args), ANI_OK);
    ASSERT_EQ(res, VAL1 + VAL2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_v)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", "II:I", &res, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(res, VAL1 + VAL2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->c_api->Object_CallMethodByName_Int(env_, object, "intByNameMethod", "II:I", &res, VAL1, VAL2),
              ANI_OK);
    ASSERT_EQ(res, VAL1 + VAL2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_v_abnormal)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", "II:X", &res, VAL1, VAL2), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "unknown_function", "II:I", &res, VAL1, VAL2), ANI_NOT_FOUND);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_v_invalid_method)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, nullptr, "II:I", &res, VAL1, VAL2), ANI_INVALID_ARGS);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_v_invalid_result)
{
    ani_object object {};
    GetMethodData(&object);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", "II:I", nullptr, VAL1, VAL2),
              ANI_INVALID_ARGS);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_v_invalid_object)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(nullptr, "intByNameMethod", "II:I", &res, VAL1, VAL2),
              ANI_INVALID_ARGS);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_int_a_invalid_args)
{
    ani_object object {};
    GetMethodData(&object);

    ani_int res = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(nullptr, "intByNameMethod", "II:I", &res, nullptr), ANI_INVALID_ARGS);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_001)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 100;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(sum, arg);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intMethod", "II:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_002)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 100;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value = 5;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "I:I", &sum, value), ANI_OK);
    ASSERT_EQ(sum, arg);

    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intMethod", "I:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_003)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 100;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intAddMethod", "II:I", &sum, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(sum, arg + VAL1 + VAL2);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intAddMethod", "II:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg + VAL1 + VAL2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_004)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 100;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(sum, arg - VAL1 - VAL2);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intMethod", "II:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg - VAL1 - VAL2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_005)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 100;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int num = 0;
    const ani_int value = 5;
    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "protectedMethod", "I:I", &num, value), ANI_OK);
    ASSERT_EQ(num, arg + value);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "protectedMethod", "I:I", &num, args), ANI_OK);
    ASSERT_EQ(num, arg + value);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "privateMethod", "I:I", &num, value), ANI_OK);
    ASSERT_EQ(num, arg - value);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "privateMethod", "I:I", &num, args), ANI_OK);
    ASSERT_EQ(num, arg - value);

    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "callProtected", "I:I", &num, value), ANI_OK);
    ASSERT_EQ(num, arg + value);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "callProtected", "I:I", &num, args), ANI_OK);
    ASSERT_EQ(num, arg + value);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_006)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value = 5;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "nestedMethod", "I:I", &sum, value), ANI_OK);
    ASSERT_EQ(sum, arg + value);

    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "nestedMethod", "I:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg + value);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_007)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value1 = 5;
    const ani_int result = 120;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "recursiveMethod", "I:I", &sum, value1), ANI_OK);
    ASSERT_EQ(sum, result);

    ani_value args[1U];
    args[0U].i = value1;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "recursiveMethod", "I:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, result);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_008)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value1 = 1;
    const ani_char value2 = 'A';
    const ani_double value3 = 1.0;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "calculateSum", "ICD:I", &sum, value1, value2, value3), ANI_OK);
    ASSERT_EQ(sum, arg - value1);

    const ani_char value4 = 'B';
    ani_value args[3U];
    args[0U].i = value1;
    args[1U].c = value4;
    args[2U].d = value3;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "calculateSum", "ICD:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, value1);

    const ani_double value5 = 2U;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "calculateSum", "ICD:I", &sum, value1, value4, value5), ANI_OK);
    ASSERT_EQ(sum, arg + value1);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_009)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 15;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value1 = 5;
    const ani_int value2 = 6;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, value1, value2), ANI_OK);
    ASSERT_EQ(sum, arg);

    const ani_int value3 = 7;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, value1, value3), ANI_OK);
    ASSERT_EQ(sum, arg);

    const ani_int value4 = 3;
    ani_value args[2U];
    args[0U].i = value1;
    args[1U].i = value4;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intMethod", "II:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg);

    const ani_int value5 = 10;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, value1, value5), ANI_OK);
    ASSERT_EQ(sum, value1 + value5);

    const ani_int value6 = 12;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intMethod", "II:I", &sum, value1, value6), ANI_OK);
    ASSERT_EQ(sum, value1 + value6);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_010)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_int_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 10;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    ani_int sum = 0;
    const ani_int value = 2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "jf", "I:I", &sum, value), ANI_OK);
    ASSERT_EQ(sum, arg + value);

    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "jf", "I:I", &sum, args), ANI_OK);
    ASSERT_EQ(sum, arg + value);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_011)
{
    ani_object obj {};
    GetMethodData(&obj);

    ani_int sum = 0;
    const ani_int value1 = std::numeric_limits<ani_int>::max();
    const ani_int value2 = 0;
    ani_value args1[2U];
    args1[0U].i = value1;
    args1[1U].i = value2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intByNameMethod", "II:I", &sum, value1, value2), ANI_OK);
    ASSERT_EQ(sum, value1 + value2);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intByNameMethod", "II:I", &sum, args1), ANI_OK);
    ASSERT_EQ(sum, value1 + value2);

    const ani_int value3 = std::numeric_limits<ani_int>::min();
    ani_value args2[2U];
    args2[0U].i = value3;
    args2[1U].i = value2;
    ASSERT_EQ(env_->Object_CallMethodByName_Int(obj, "intByNameMethod", "II:I", &sum, value3, value2), ANI_OK);
    ASSERT_EQ(sum, value3 + value2);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(obj, "intByNameMethod", "II:I", &sum, args2), ANI_OK);
    ASSERT_EQ(sum, value3 + value2);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_012)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;

    ani_int res = 0;
    ASSERT_EQ(env_->c_api->Object_CallMethodByName_Int(nullptr, object, "intByNameMethod", "II:I", &res, VAL1, VAL2),
              ANI_INVALID_ARGS);
    ASSERT_EQ(env_->c_api->Object_CallMethodByName_Int_A(nullptr, object, "intByNameMethod", "II:I", &res, args),
              ANI_INVALID_ARGS);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(nullptr, "intByNameMethod", "II:I", &res, VAL1, VAL2),
              ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(nullptr, "intByNameMethod", "II:I", &res, args), ANI_INVALID_ARGS);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, nullptr, "II:I", &res, VAL1, VAL2), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, nullptr, "II:I", &res, args), ANI_INVALID_ARGS);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", nullptr, &res, VAL1, VAL2), ANI_OK);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, "intByNameMethod", nullptr, &res, args), ANI_OK);

    ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", "II:I", nullptr, VAL1, VAL2),
              ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, "intByNameMethod", "II:I", nullptr, args), ANI_INVALID_ARGS);
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_013)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;

    ani_int res = 0;
    const std::array<std::string_view, 4U> invalidMethodNames = {{"", "测试emoji🙂🙂", "\n\r\t", "\x01\x02\x03"}};

    for (const auto &methodName : invalidMethodNames) {
        ASSERT_EQ(env_->Object_CallMethodByName_Int(object, methodName.data(), "II:I", &res, VAL1, VAL2),
                  ANI_NOT_FOUND);
        ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, methodName.data(), "II:I", &res, args), ANI_NOT_FOUND);
    }
}

TEST_F(CallObjectMethodIntByNameTest, object_call_method_by_name_int_014)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;

    ani_int res = 0;
    const std::array<std::string_view, 4U> invalidMethodNames = {{"", "测试emoji🙂🙂", "\n\r\t", "\x01\x02\x03"}};

    for (const auto &methodName : invalidMethodNames) {
        ASSERT_EQ(env_->Object_CallMethodByName_Int(object, "intByNameMethod", methodName.data(), &res, VAL1, VAL2),
                  ANI_NOT_FOUND);
        ASSERT_EQ(env_->Object_CallMethodByName_Int_A(object, "intByNameMethod", methodName.data(), &res, args),
                  ANI_NOT_FOUND);
    }
}
}  // namespace ark::ets::ani::testing

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays, readability-magic-numbers)