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

class CallObjectMethodByNameRefTest : public AniTest {
public:
    static constexpr ani_int VAL0 = 100;
    static constexpr ani_int VAL1 = 10;
    static constexpr ani_int VAL2 = 20;

    void GetMethodData(ani_object *objectResult)
    {
        ani_class cls {};
        ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/A;", &cls), ANI_OK);
        ASSERT_NE(cls, nullptr);

        ani_static_method newMethod {};
        ASSERT_EQ(env_->Class_FindStaticMethod(cls, "new_A", ":Lobject_call_method_by_name_ref_test/A;", &newMethod),
                  ANI_OK);
        ani_ref ref {};
        ASSERT_EQ(env_->Class_CallStaticMethod_Ref(cls, newMethod, &ref), ANI_OK);

        *objectResult = static_cast<ani_object>(ref);
    }
};

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_normal)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, "getName", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto string = reinterpret_cast<ani_string>(ref);
    ani_size result = 0U;
    ASSERT_EQ(env_->String_GetUTF8Size(string, &result), ANI_OK);
    ASSERT_EQ(result, 8U);

    ani_size substrOffset = 0U;
    ani_size substrSize = result;
    const uint32_t bufferSize = 11U;
    char utfBuffer[bufferSize] = {};
    result = 0U;

    auto status =
        env_->String_GetUTF8SubString(string, substrOffset, substrSize, utfBuffer, sizeof(utfBuffer), &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_STREQ(utfBuffer, "Equality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_normal_1)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    const ani_int arg1 = 11;
    const ani_int arg2 = 21;
    args[0U].i = arg1;
    args[1U].i = arg2;
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, "getName", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto string = reinterpret_cast<ani_string>(ref);
    ani_size result = 0U;
    ASSERT_EQ(env_->String_GetUTF8Size(string, &result), ANI_OK);
    ASSERT_EQ(result, 10U);

    ani_size substrOffset = 0U;
    ani_size substrSize = result;
    const uint32_t bufferSize = 11U;
    char utfBuffer[bufferSize] = {};
    result = 0U;

    auto status =
        env_->String_GetUTF8SubString(string, substrOffset, substrSize, utfBuffer, sizeof(utfBuffer), &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_STREQ(utfBuffer, "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_abnormal)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, "xxxxxx", nullptr, &ref, args), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, "getName", "II:Z", &ref, args), ANI_NOT_FOUND);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_invalid_object)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ani_ref ref = nullptr;

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(nullptr, "getName", nullptr, &ref, args), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_invalid_method)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ani_ref ref = nullptr;

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, nullptr, nullptr, &ref, args), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_a_invalid_result)
{
    ani_object object {};
    GetMethodData(&object);

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ani_ref ref = nullptr;

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(object, "getName", nullptr, nullptr, args), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_normal)
{
    ani_object object {};
    GetMethodData(&object);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, "getName", nullptr, &ref, VAL1, VAL2), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto string = reinterpret_cast<ani_string>(ref);
    ani_size result = 0U;
    ASSERT_EQ(env_->String_GetUTF8Size(string, &result), ANI_OK);
    ASSERT_EQ(result, 8U);

    ani_size substrOffset = 0U;
    ani_size substrSize = result;
    const uint32_t bufferSize = 11U;
    char utfBuffer[bufferSize] = {};
    result = 0U;

    auto status =
        env_->String_GetUTF8SubString(string, substrOffset, substrSize, utfBuffer, sizeof(utfBuffer), &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_STREQ(utfBuffer, "Equality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_normal_1)
{
    ani_object object {};
    GetMethodData(&object);

    const ani_int arg1 = 11;
    const ani_int arg2 = 20;
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, "getName", nullptr, &ref, arg1, arg2), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto string = reinterpret_cast<ani_string>(ref);
    ani_size result = 0U;
    ASSERT_EQ(env_->String_GetUTF8Size(string, &result), ANI_OK);
    ASSERT_EQ(result, 10U);

    ani_size substrOffset = 0U;
    ani_size substrSize = result;
    const uint32_t bufferSize = 11U;
    char utfBuffer[bufferSize] = {};
    result = 0U;

    auto status =
        env_->String_GetUTF8SubString(string, substrOffset, substrSize, utfBuffer, sizeof(utfBuffer), &result);
    ASSERT_EQ(status, ANI_OK);
    ASSERT_STREQ(utfBuffer, "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_abnormal)
{
    ani_object object {};
    GetMethodData(&object);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, "xxxxxxxx", nullptr, &ref, VAL1, VAL2), ANI_NOT_FOUND);
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, "getName", "II:J", &ref, VAL1, VAL2), ANI_NOT_FOUND);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_invalid_object)
{
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(nullptr, "getName", nullptr, &ref, VAL1, VAL2), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_invalid_method)
{
    ani_object object {};
    GetMethodData(&object);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, nullptr, nullptr, &ref, VAL1, VAL2), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_invalid_result)
{
    ani_object object {};
    GetMethodData(&object);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(object, "getName", nullptr, nullptr), ANI_INVALID_ARGS);
    ASSERT_EQ(ref, nullptr);
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_001)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, VAL1, VAL2), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");

    ani_value args[2U];
    args[0U].i = VAL1;
    args[1U].i = VAL2;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "refMethod", "II:Lstd/core/String;", &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_002)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "I:Lstd/core/String;", &ref, VAL1), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ani_value args[1U];
    args[0U].i = VAL1;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "refMethod", "I:Lstd/core/String;", &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_003)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value1 = 5;
    const ani_int value2 = 8;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refCompareMethod", nullptr, &ref, value1, value2), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Less");

    ani_value args[2U];
    args[0U].i = value1;
    args[1U].i = value2;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "refCompareMethod", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Less");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_004)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value1 = 5;
    const ani_int value2 = 8;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, value1, value2),
              ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");

    ani_value args[2U];
    args[0U].i = value1;
    args[1U].i = value2;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "refMethod", "II:Lstd/core/String;", &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_005)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value = 5;
    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "protectedMethod", nullptr, &ref, value), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "protectedMethod", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "privateMethod", nullptr, &ref, value), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "privateMethod", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", nullptr, &method), ANI_OK);
    ASSERT_EQ(env_->Object_New(cls, method, &obj, VAL0), ANI_OK);
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "callProtected", nullptr, &ref, value), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "callProtected", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_006)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value = 10;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "nestedMethod", nullptr, &ref, value), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Less");

    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "nestedMethod", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Less");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_007)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value1 = 5;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "recursiveMethod", nullptr, &ref, value1), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Greater");

    const ani_int value2 = -5;
    ani_value args[1U];
    args[0U].i = value2;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "recursiveMethod", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Less");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_008)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 6;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value1 = 1;
    const ani_char value2 = 'A';
    const ani_double value3 = 1.0;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "calculateSum", nullptr, &ref, value1, value2, value3), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "CharEquality");

    const ani_char value4 = 'B';
    ani_value args[3U];
    args[0U].i = value1;
    args[1U].c = value4;
    args[2U].d = value3;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "calculateSum", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "NumEquality");

    const ani_double value5 = 2U;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "calculateSum", nullptr, &ref, value1, value4, value5), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_009)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/B;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 15;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value1 = 5;
    const ani_int value2 = 6;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, value1, value2),
              ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");

    const ani_int value3 = 7;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, value1, value3),
              ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");

    const ani_int value4 = 3;
    ani_value args[3U];
    args[0U].i = value1;
    args[1U].i = value4;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "refMethod", "II:Lstd/core/String;", &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");

    const ani_int value5 = 5;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, value1, value5),
              ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Equality");

    const ani_int value6 = 12;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "refMethod", "II:Lstd/core/String;", &ref, value1, value6),
              ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Inequality");
}

TEST_F(CallObjectMethodByNameRefTest, object_call_method_by_name_ref_010)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Lobject_call_method_by_name_ref_test/C;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_method method {};
    ASSERT_EQ(env_->Class_FindMethod(cls, "<ctor>", "I:V", &method), ANI_OK);

    ani_object obj {};
    const ani_int arg = 10;
    ASSERT_EQ(env_->Object_New(cls, method, &obj, arg), ANI_OK);

    std::string result {};
    ani_ref ref = nullptr;
    const ani_int value = 10;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref(obj, "jf", nullptr, &ref, value), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Equality");

    ani_value args[1U];
    args[0U].i = value;
    ASSERT_EQ(env_->Object_CallMethodByName_Ref_A(obj, "jf", nullptr, &ref, args), ANI_OK);
    ASSERT_NE(ref, nullptr);
    GetStdString(static_cast<ani_string>(ref), result);
    ASSERT_STREQ(result.c_str(), "Equality");
}
}  // namespace ark::ets::ani::testing

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays, readability-magic-numbers)
