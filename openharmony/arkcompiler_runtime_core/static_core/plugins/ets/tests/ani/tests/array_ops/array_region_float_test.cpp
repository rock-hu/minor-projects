/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"
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

#include "ani_gtest_array_ops.h"
#include <iostream>

// NOLINTBEGIN(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
namespace ark::ets::ani::testing {

class ArraySetGetRegionFloatTest : public AniGTestArrayOps {
protected:
    static constexpr float TEST_VALUE_1 = 1.0F;
    static constexpr float TEST_VALUE_2 = 2.0F;
    static constexpr float TEST_VALUE_3 = 3.0F;
    static constexpr float TEST_VALUE_4 = 4.0F;
    static constexpr float TEST_VALUE_5 = 5.0F;
    static constexpr float TEST_UPDATE_1 = 30.0F;
    static constexpr float TEST_UPDATE_2 = 40.0F;
    static constexpr float TEST_UPDATE_3 = 50.0F;

    static constexpr float TEST_UPDATE_4 = 22.0F;
    static constexpr float TEST_UPDATE_5 = 44.0F;
    static constexpr float TEST_UPDATE_6 = 33.0F;
};

TEST_F(ArraySetGetRegionFloatTest, SetFloatArrayRegionErrorTests)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->Array_New_Float(LENGTH_5, &array), ANI_OK);
    ani_float nativeBuffer[LENGTH_5] = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4, TEST_VALUE_5};
    const ani_size offset1 = -1;
    ASSERT_EQ(env_->Array_SetRegion_Float(array, offset1, LENGTH_2, nativeBuffer), ANI_OUT_OF_RANGE);
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_5, LENGTH_10, nativeBuffer), ANI_OUT_OF_RANGE);
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer), ANI_OK);
}

TEST_F(ArraySetGetRegionFloatTest, GetFloatArrayRegionErrorTests)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->Array_New_Float(LENGTH_5, &array), ANI_OK);
    ani_float nativeBuffer[LENGTH_5] = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4, TEST_VALUE_5};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_1, nullptr), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_5, LENGTH_10, nativeBuffer), ANI_OUT_OF_RANGE);
    // Should change to ANI_OK when std lib array will work according to spec
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_1, nativeBuffer), ANI_ERROR);
}

TEST_F(ArraySetGetRegionFloatTest, SetFloatFixedArrayRegionErrorTests)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->FixedArray_New_Float(LENGTH_5, &array), ANI_OK);
    ani_float nativeBuffer[LENGTH_5] = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4, TEST_VALUE_5};
    const ani_size offset1 = -1;
    ASSERT_EQ(env_->Array_SetRegion_Float(array, offset1, LENGTH_2, nativeBuffer), ANI_OUT_OF_RANGE);
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_5, LENGTH_10, nativeBuffer), ANI_OUT_OF_RANGE);
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer), ANI_OK);
}

TEST_F(ArraySetGetRegionFloatTest, GetFloatFixedArrayRegionErrorTests)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->FixedArray_New_Float(LENGTH_5, &array), ANI_OK);
    ani_float nativeBuffer[LENGTH_5] = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4, TEST_VALUE_5};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_1, nullptr), ANI_INVALID_ARGS);
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_5, LENGTH_10, nativeBuffer), ANI_OUT_OF_RANGE);
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_1, nativeBuffer), ANI_OK);
}

TEST_F(ArraySetGetRegionFloatTest, GetRegionFloatTest)
{
    const auto array = static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetArray"));

    ani_float nativeBuffer[LENGTH_5] = {0.0F};
    const float epsilon = 1e-6;  // Define acceptable tolerance
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer), ANI_OK);
    ASSERT_NEAR(nativeBuffer[0U], TEST_VALUE_1, epsilon);
    ASSERT_NEAR(nativeBuffer[1U], TEST_VALUE_2, epsilon);
    ASSERT_NEAR(nativeBuffer[2U], TEST_VALUE_3, epsilon);
    ASSERT_NEAR(nativeBuffer[3U], TEST_VALUE_4, epsilon);
    ASSERT_NEAR(nativeBuffer[4U], TEST_VALUE_5, epsilon);
}

TEST_F(ArraySetGetRegionFloatTest, SetRegionFloatTest)
{
    const auto array = static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetArray"));
    ani_float nativeBuffer1[LENGTH_5] = {TEST_UPDATE_1, TEST_UPDATE_2, TEST_UPDATE_3};
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_2, LENGTH_3, nativeBuffer1), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>("array_region_float_test", "CheckArray", array), ANI_TRUE);
}

TEST_F(ArraySetGetRegionFloatTest, CheckChangeFromManagedRegionFloatTest)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Larray_region_float_test/ArrayClass;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Class_GetStaticFieldByName_Ref(cls, "array", &ref), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto array = reinterpret_cast<ani_array_float>(ref);
    ani_float nativeBuffer[LENGTH_5] = {0.0};
    const double epsilon = 1e-6;  // Define acceptable tolerance

    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer), ANI_OK);
    ASSERT_NEAR(nativeBuffer[0U], TEST_VALUE_1, epsilon);
    ASSERT_NEAR(nativeBuffer[1U], TEST_VALUE_2, epsilon);
    ASSERT_NEAR(nativeBuffer[2U], TEST_VALUE_3, epsilon);
    ASSERT_NEAR(nativeBuffer[3U], TEST_VALUE_4, epsilon);
    ASSERT_NEAR(nativeBuffer[4U], TEST_VALUE_5, epsilon);

    ASSERT_EQ(env_->Class_CallStaticMethodByName_Void(cls, "ChangeStaticArray", nullptr), ANI_OK);
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer), ANI_OK);
    ASSERT_NEAR(nativeBuffer[0U], TEST_VALUE_1, epsilon);
    ASSERT_NEAR(nativeBuffer[1U], TEST_VALUE_2, epsilon);
    ASSERT_NEAR(nativeBuffer[2U], TEST_UPDATE_4, epsilon);
    ASSERT_NEAR(nativeBuffer[3U], TEST_VALUE_4, epsilon);
    ASSERT_NEAR(nativeBuffer[4U], TEST_UPDATE_5, epsilon);
}

TEST_F(ArraySetGetRegionFloatTest, CheckChangeFromApiRegionFloatTest)
{
    ani_class cls {};
    ASSERT_EQ(env_->FindClass("Larray_region_float_test/ArrayClass;", &cls), ANI_OK);
    ASSERT_NE(cls, nullptr);

    ani_ref ref = nullptr;
    ASSERT_EQ(env_->Class_GetStaticFieldByName_Ref(cls, "array", &ref), ANI_OK);
    ASSERT_NE(ref, nullptr);

    auto array = reinterpret_cast<ani_array_float>(ref);
    ani_float nativeBuffer[LENGTH_3] = {TEST_UPDATE_4, TEST_UPDATE_6, TEST_UPDATE_5};

    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_2, LENGTH_3, nativeBuffer), ANI_OK);

    ani_boolean result = ANI_FALSE;
    ASSERT_EQ(env_->Class_CallStaticMethodByName_Boolean(cls, "CheckStaticArray", nullptr, &result), ANI_OK);
    ASSERT_EQ(result, ANI_TRUE);
}

TEST_F(ArraySetGetRegionFloatTest, GetSpecialValueToArrayTest)
{
    const auto array =
        static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetSpecialArray"));

    std::array<ani_float, LENGTH_6> nativeBuffer = {};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_6, nativeBuffer.data()), ANI_OK);
    const ani_float minFloatValue = 1.40129846432481707e-45;
    const ani_float maxFloatValue = 3.40282346638528860e+38;
    const ani_float minusFloatValue = -3.40282346638528860e+38;
    ASSERT_EQ(nativeBuffer[0U], minFloatValue);
    ASSERT_EQ(nativeBuffer[1U], maxFloatValue);
    ASSERT_EQ(nativeBuffer[2U], minusFloatValue);
    ASSERT_EQ(nativeBuffer[3U], 0.0F);
}

TEST_F(ArraySetGetRegionFloatTest, SetSpecialValueToArrayTest)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->FixedArray_New_Float(LENGTH_6, &array), ANI_OK);
    ani_float max = std::numeric_limits<float>::max();
    ani_float minPositive = std::numeric_limits<float>::min();
    ani_float min = -std::numeric_limits<float>::max();
    const std::array<ani_float, LENGTH_6> nativeBuffer = {minPositive, max, min, 0.0F, -1.0F, 1.0F};
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_6, nativeBuffer.data()), ANI_OK);

    std::array<ani_float, LENGTH_6> nativeBuffer2 = {};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_6, nativeBuffer2.data()), ANI_OK);
    ASSERT_EQ(nativeBuffer2[0U], minPositive);
    ASSERT_EQ(nativeBuffer2[1U], max);
    ASSERT_EQ(nativeBuffer2[2U], min);
    ASSERT_EQ(nativeBuffer2[3U], 0.0F);
    ASSERT_EQ(nativeBuffer2[4U], -1.0F);
}

TEST_F(ArraySetGetRegionFloatTest, SetGetUnionToArrayTest)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->FixedArray_New_Float(LENGTH_5, &array), ANI_OK);

    std::array<ani_float, LENGTH_5> nativeBuffer = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4,
                                                    TEST_VALUE_5};
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);

    std::array<ani_float, LENGTH_5> nativeBuffer2 = {};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
    CompareArray(nativeBuffer, nativeBuffer2);

    for (ani_size i = 0; i < LENGTH_5; i++) {
        ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
    }
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
    CompareArray(nativeBuffer, nativeBuffer2);

    std::array<ani_float, LENGTH_5> nativeBuffer3 = {TEST_VALUE_1, TEST_VALUE_3, TEST_VALUE_5, TEST_VALUE_2,
                                                     TEST_VALUE_4};
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer3.data()), ANI_OK);
    for (ani_int i = 0; i < LOOP_COUNT; i++) {
        ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
    }
    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer3.data()), ANI_OK);
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
    CompareArray(nativeBuffer2, nativeBuffer3);

    ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
    for (ani_size i = 0; i < LENGTH_5; i++) {
        ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
        CompareArray(nativeBuffer, nativeBuffer2);
    }
}

TEST_F(ArraySetGetRegionFloatTest, SetGetStabilityToArrayTest)
{
    ani_array_float array = nullptr;
    ASSERT_EQ(env_->FixedArray_New_Float(LENGTH_5, &array), ANI_OK);

    std::array<ani_float, LENGTH_5> nativeBuffer = {TEST_VALUE_1, TEST_VALUE_2, TEST_VALUE_3, TEST_VALUE_4,
                                                    TEST_VALUE_5};
    std::array<ani_float, LENGTH_5> nativeBuffer2 = {};
    for (ani_int i = 0; i < LOOP_COUNT; i++) {
        ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
    }
    ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
    CompareArray(nativeBuffer, nativeBuffer2);

    for (ani_int i = 0; i < LOOP_COUNT; i++) {
        ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
    }
    for (ani_int i = 0; i < LOOP_COUNT; i++) {
        ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
        CompareArray(nativeBuffer, nativeBuffer2);
    }

    for (ani_int i = 0; i < LOOP_COUNT; i++) {
        ASSERT_EQ(env_->Array_SetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer.data()), ANI_OK);
        ASSERT_EQ(env_->Array_GetRegion_Float(array, OFFSET_0, LENGTH_5, nativeBuffer2.data()), ANI_OK);
        CompareArray(nativeBuffer, nativeBuffer2);
    }
}

TEST_F(ArraySetGetRegionFloatTest, EscompatGetRegionFloatTest)
{
    const auto array =
        static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetEscompatArray"));

    ani_float nativeBuffer[5U] = {0.0F};
    const ani_size offset3 = 0;
    const ani_size len3 = 5;
    const float epsilon = 1e-6;  // Define acceptable tolerance
    ASSERT_EQ(env_->Array_GetRegion_Float(array, offset3, len3, nativeBuffer), ANI_OK);
    ASSERT_NEAR(nativeBuffer[0U], TEST_VALUE_1, epsilon);
    ASSERT_NEAR(nativeBuffer[1U], TEST_VALUE_2, epsilon);
    ASSERT_NEAR(nativeBuffer[2U], TEST_VALUE_3, epsilon);
    ASSERT_NEAR(nativeBuffer[3U], TEST_VALUE_4, epsilon);
    ASSERT_NEAR(nativeBuffer[4U], TEST_VALUE_5, epsilon);
}

TEST_F(ArraySetGetRegionFloatTest, EscompatSetRegionFloatTest)
{
    const auto array =
        static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetEscompatArray"));
    ani_float nativeBuffer1[5U] = {TEST_UPDATE_1, TEST_UPDATE_2, TEST_UPDATE_3};
    const ani_size offset4 = 2;
    const ani_size len4 = 3;
    ASSERT_EQ(env_->Array_SetRegion_Float(array, offset4, len4, nativeBuffer1), ANI_OK);
    ASSERT_EQ(CallEtsFunction<ani_boolean>("array_region_float_test", "CheckEscompatArray", array), ANI_TRUE);
}

TEST_F(ArraySetGetRegionFloatTest, EscompatInvalidFloatTest)
{
    const auto array =
        static_cast<ani_array_float>(CallEtsFunction<ani_ref>("array_region_float_test", "GetEscompatArray"));
    ani_float nativeBuffer1[5U] = {TEST_UPDATE_1, TEST_UPDATE_2, TEST_UPDATE_3};
    const ani_size offset4 = 3;
    const ani_size len4 = 3;
    ASSERT_EQ(env_->Array_SetRegion_Float(array, offset4, len4, nativeBuffer1), ANI_OUT_OF_RANGE);
    ani_float nativeBuffer[5U] = {0};
    ASSERT_EQ(env_->Array_GetRegion_Float(array, offset4, len4, nativeBuffer), ANI_OUT_OF_RANGE);
}

}  // namespace ark::ets::ani::testing

// NOLINTEND(cppcoreguidelines-pro-type-vararg, modernize-avoid-c-arrays)
