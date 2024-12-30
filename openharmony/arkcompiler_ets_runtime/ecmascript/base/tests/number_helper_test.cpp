/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ecmascript/base/number_helper.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::base;

namespace panda::test {
class NumberHelperTest : public BaseTestWithScope<false> {
};

/**
 * @tc.name: IsNaN
 * @tc.desc: Check whether number is Nan type data through "IsNaN" function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, IsNaN)
{
    JSTaggedValue number1(1.23);
    EXPECT_FALSE(NumberHelper::IsNaN(number1));

    JSTaggedValue number2(-1.23);
    EXPECT_FALSE(NumberHelper::IsNaN(number2));

    JSTaggedValue number3(0.0f);
    EXPECT_FALSE(NumberHelper::IsNaN(number3));

    JSTaggedValue number4(0.0f / 0.0f);
    EXPECT_TRUE(NumberHelper::IsNaN(number4));

    JSTaggedValue number5(NAN_VALUE);
    EXPECT_TRUE(NumberHelper::IsNaN(number5));
}

/**
 * @tc.name: DoubleToString
 * @tc.desc: This function Convert the double type data into a string.first convert it into the corresponding
 *           hexadecimal number according to the transmitted radix, and convert the hexadecimal number into a
 *           string.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToString_001)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int radix;
    radix = 2;
    JSHandle<EcmaString> resultStr = factory->NewFromASCII("100101");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);

    radix = 3;
    resultStr = factory->NewFromASCII("-1101");
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToString(thread, -37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    radix = 4;
    resultStr = factory->NewFromASCII("211");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);

    radix = 5;
    resultStr = factory->NewFromASCII("122");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);

    radix = 5;
    resultStr = factory->NewFromASCII("-1104332401304422434310320000");
    JSHandle<EcmaString> handleEcmaStr5(thread,
        NumberHelper::DoubleToString(thread, static_cast<double>(-9223372036854775807), radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    radix = 6;
    resultStr = factory->NewFromASCII("101");
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);

    radix = 7;
    resultStr = factory->NewFromASCII("52");
    JSHandle<EcmaString> handleEcmaStr7(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr7, resultStr), 0);

    radix = 36;
    resultStr = factory->NewFromASCII("11");
    JSHandle<EcmaString> handleEcmaStr8(thread, NumberHelper::DoubleToString(thread, 37, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr8, resultStr), 0);
}

HWTEST_F_L0(NumberHelperTest, DoubleToString_002)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int radix = 2;

    JSHandle<EcmaString> resultStr =
        factory->NewFromASCII("10.111111011011000110000101010010001010100110111101");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToString(thread, 2.99099, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);

    resultStr = factory->NewFromASCII("10.000000101001000000000011111011101010001000001001101");
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToString(thread, 2.01001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    resultStr = factory->NewFromASCII("10.100000000000011010001101101110001011101011000111001");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToString(thread, 2.5001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);

    radix = 36;
    resultStr = factory->NewFromASCII("0.i04nym8equ");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToString(thread, 0.5001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);

    resultStr = factory->NewFromASCII("0.wej2d0mt58f");
    JSHandle<EcmaString> handleEcmaStr5(thread, NumberHelper::DoubleToString(thread, 0.9001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    resultStr = factory->NewFromASCII("0.0d384dldb02");
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToString(thread, 0.0101, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);
}

/**
 * @tc.name: DoubleToCString
 * @tc.desc: This function Convert the double type data into a CString.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToCString)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<EcmaString> resultStr1 =
        factory->NewFromASCII("5562684646268003");
    double d1 = 5562684646268003;
    JSHandle<EcmaString> resultJSHandle1 = factory->NewFromASCII(NumberHelper::DoubleToCString(d1));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle1, resultStr1), 0);

    JSHandle<EcmaString> resultStr2 =
        factory->NewFromASCII("0.005431");
    double d2 = 0.005431;
    JSHandle<EcmaString> resultJSHandle2 = factory->NewFromASCII(NumberHelper::DoubleToCString(d2));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle2, resultStr2), 0);

    JSHandle<EcmaString> resultStr3 =
        factory->NewFromASCII("1.9045e-7");
    double d3 = 0.00000019045;
    JSHandle<EcmaString> resultJSHandle3 = factory->NewFromASCII(NumberHelper::DoubleToCString(d3));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle3, resultStr3), 0);

    JSHandle<EcmaString> resultStr4 =
        factory->NewFromASCII("-79.39773355813419");
    double d4 = -79.39773355813419;
    JSHandle<EcmaString> resultJSHandle4 = factory->NewFromASCII(NumberHelper::DoubleToCString(d4));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle4, resultStr4), 0);

    JSHandle<EcmaString> resultStr5 =
        factory->NewFromASCII("1e+21");
    double d5 = 1e21;
    JSHandle<EcmaString> resultJSHandle5 = factory->NewFromASCII(NumberHelper::DoubleToCString(d5));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle5, resultStr5), 0);

    JSHandle<EcmaString> resultStr6 =
        factory->NewFromASCII("340000000000000000");
    double d6 = 340000000000000000;
    JSHandle<EcmaString> resultJSHandle6 = factory->NewFromASCII(NumberHelper::DoubleToCString(d6));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle6, resultStr6), 0);

    JSHandle<EcmaString> resultStr7 =
        factory->NewFromASCII("12.012345678901234");
    double d7 = 12.01234567890123456789;
    JSHandle<EcmaString> resultJSHandle7 = factory->NewFromASCII(NumberHelper::DoubleToCString(d7));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle7, resultStr7), 0);

    JSHandle<EcmaString> resultStr8 =
        factory->NewFromASCII("0.0000012345678901234567");
    double digit8 = 0.000001234567890123456789;
    JSHandle<EcmaString> resultJSHandle8 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit8));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle8, resultStr8), 0);

    JSHandle<EcmaString> resultStr9 =
        factory->NewFromASCII("Infinity");
    double digit9 = std::numeric_limits<double>::infinity();
    JSHandle<EcmaString> resultJSHandle9 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit9));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle9, resultStr9), 0);

    JSHandle<EcmaString> resultStr10 =
        factory->NewFromASCII("-Infinity");
    double digit10 = -std::numeric_limits<double>::infinity();
    JSHandle<EcmaString> resultJSHandle10 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit10));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle10, resultStr10), 0);

    JSHandle<EcmaString> resultStr11 =
        factory->NewFromASCII("1.7976931348623157e+308");
    double digit11 = 1.797693134862315807937e+308;
    JSHandle<EcmaString> resultJSHandle11 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit11));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle11, resultStr11), 0);

    JSHandle<EcmaString> resultStr12 =
        factory->NewFromASCII("-1.7976931348623157e+308");
    double digit12 = -1.797693134862315807937e+308;
    JSHandle<EcmaString> resultJSHandle12 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit12));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle12, resultStr12), 0);

    JSHandle<EcmaString> resultStr13 =
        factory->NewFromASCII("2.22507e-308");
    double digit13 = 2.22507e-308;
    JSHandle<EcmaString> resultJSHandle13 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit13));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle13, resultStr13), 0);

    JSHandle<EcmaString> resultStr14 =
        factory->NewFromASCII("1.2345678901234568e-7");
    double digit14 = 0.0000001234567890123456789;
    JSHandle<EcmaString> resultJSHandle14 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit14));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle14, resultStr14), 0);

    JSHandle<EcmaString> resultStr15 =
        factory->NewFromASCII("3.4e+21");
    double digit15 = 3.4e21;
    JSHandle<EcmaString> resultJSHandle15 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit15));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle15, resultStr15), 0);

    JSHandle<EcmaString> resultStr16 =
        factory->NewFromASCII("120000000000000000000");
    double digit16 = 1.2e20;
    JSHandle<EcmaString> resultJSHandle16 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit16));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle16, resultStr16), 0);

    JSHandle<EcmaString> resultStr17 =
        factory->NewFromASCII("1.2");
    double digit17 = 1.2e0;
    JSHandle<EcmaString> resultJSHandle17 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit17));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle17, resultStr17), 0);

    JSHandle<EcmaString> resultStr18 =
        factory->NewFromASCII("0.0000012");
    double digit18 = 1.2e-6;
    JSHandle<EcmaString> resultJSHandle18 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit18));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle18, resultStr18), 0);

    JSHandle<EcmaString> resultStr19 =
        factory->NewFromASCII("1.2e-7");
    double digit19 = 1.2e-7;
    JSHandle<EcmaString> resultJSHandle19 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit19));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle19, resultStr19), 0);

    JSHandle<EcmaString> resultStr20 =
        factory->NewFromASCII("NaN");
    double digit20 = std::numeric_limits<double>::quiet_NaN();
    JSHandle<EcmaString> resultJSHandle20 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit20));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle20, resultStr20), 0);

    JSHandle<EcmaString> resultStr21 =
        factory->NewFromASCII("-12.012345678901234");
    double digit21 = -12.01234567890123456789;
    JSHandle<EcmaString> resultJSHandle21 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit21));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle21, resultStr21), 0);

    JSHandle<EcmaString> resultStr22 =
        factory->NewFromASCII("-0.0000012345678901234567");
    double digit22 = -0.000001234567890123456789;
    JSHandle<EcmaString> resultJSHandle22 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit22));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle22, resultStr22), 0);

    JSHandle<EcmaString> resultStr23 =
        factory->NewFromASCII("-1.2345678901234568e-7");
    double digit23 = -0.0000001234567890123456789;
    JSHandle<EcmaString> resultJSHandle23 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit23));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle23, resultStr23), 0);

    JSHandle<EcmaString> resultStr24 =
        factory->NewFromASCII("-3.4e+21");
    double digit24 = -3.4e21;
    JSHandle<EcmaString> resultJSHandle24 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit24));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle24, resultStr24), 0);

    JSHandle<EcmaString> resultStr25 =
        factory->NewFromASCII("-120000000000000000000");
    double digit25 = -1.2e20;
    JSHandle<EcmaString> resultJSHandle25 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit25));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle25, resultStr25), 0);

    JSHandle<EcmaString> resultStr26 =
        factory->NewFromASCII("-1.2");
    double digit26 = -1.2e0;
    JSHandle<EcmaString> resultJSHandle26 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit26));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle26, resultStr26), 0);

    JSHandle<EcmaString> resultStr27 =
        factory->NewFromASCII("-0.0000012");
    double digit27 = -1.2e-6;
    JSHandle<EcmaString> resultJSHandle27 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit27));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle27, resultStr27), 0);

    JSHandle<EcmaString> resultStr28 =
        factory->NewFromASCII("-1.2e-7");
    double digit28 = -1.2e-7;
    JSHandle<EcmaString> resultJSHandle28 = factory->NewFromASCII(NumberHelper::DoubleToCString(digit28));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, resultJSHandle28, resultStr28), 0);
}

/**
 * @tc.name: IsEmptyString
 * @tc.desc: Check whether the character is empty string through "IsEmptyString" function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, IsEmptyString_001)
{
    // 9 ~ 13 and 32 belong to empty string
    uint8_t a[] = {0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x20};
    for (int i = 0; i < static_cast<int>(sizeof(a)); i++) {
        EXPECT_TRUE(NumberHelper::IsEmptyString(&a[i], &a[i] + 1));
    }
}

HWTEST_F_L0(NumberHelperTest, IsEmptyString_002)
{
    // 14 ~ 31 not belong to empty string
    uint8_t a[] = {0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                                      0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F};
    for (int i = 0; i < static_cast<int>(sizeof(a)); i++) {
        EXPECT_FALSE(NumberHelper::IsEmptyString(&a[i], &a[i] + 1));
    }
}

HWTEST_F_L0(NumberHelperTest, IsEmptyString_003)
{
    // 0 ~ 8 not belong to empty string
    uint8_t a[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    for (int i = 0; i < static_cast<int>(sizeof(a)); i++) {
        EXPECT_FALSE(NumberHelper::IsEmptyString(&a[i], &a[i] + 1));
    }
}

HWTEST_F_L0(NumberHelperTest, IsEmptyString_004)
{
    // 160 belong to empty string
    uint16_t c = 160;
    utf_helper::Utf8Char d = utf_helper::ConvertUtf16ToUtf8(c, 0, true);
    EXPECT_EQ(d.ch.at(1), 160);
    uint8_t b = d.ch.at(1);
    EXPECT_TRUE(NumberHelper::IsEmptyString(&b, &b + 1));
}

/**
 * @tc.name: TruncateDouble
 * @tc.desc:This function takes the integer part of double type.When it is positive,it is rounded down
 *          When it is negative,it is rounded up.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, TruncateDouble)
{
    EXPECT_EQ(NumberHelper::TruncateDouble(NAN_VALUE), 0);
    EXPECT_EQ(NumberHelper::TruncateDouble(POSITIVE_INFINITY), POSITIVE_INFINITY);
    // round down
    EXPECT_EQ(NumberHelper::TruncateDouble(4.1), 4);
    EXPECT_EQ(NumberHelper::TruncateDouble(4.9), 4);
    EXPECT_EQ(NumberHelper::TruncateDouble(101.111), 101);
    EXPECT_EQ(NumberHelper::TruncateDouble(101.999), 101);
    // round up
    EXPECT_EQ(NumberHelper::TruncateDouble(-4.1), -4);
    EXPECT_EQ(NumberHelper::TruncateDouble(-4.9), -4);
    EXPECT_EQ(NumberHelper::TruncateDouble(-101.111), -101);
    EXPECT_EQ(NumberHelper::TruncateDouble(-101.999), -101);
}

/**
 * @tc.name: DoubleToInt
 * @tc.desc: This function takes the double of integer type.When the decimal part is eight and the number of digits
 *           is 15 ~ 16, add one to convert to integer.According to the binary digits of the integer part,it is divided
 *           into 8, 16 and 64 hexadecimal conversion,for example, 8-hexadecimal conversion.The maximum value of the
 *           integer part is 255. If it exceeds 255,the conversion fails.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToInt_001)
{
    EXPECT_EQ(NumberHelper::DoubleToInt(9.0, INT8_BITS), 9);
    EXPECT_EQ(NumberHelper::DoubleToInt(9.5555555555555559, INT8_BITS), 9);
    EXPECT_EQ(NumberHelper::DoubleToInt(9.9999999999999999, INT8_BITS), 10);
    EXPECT_EQ(NumberHelper::DoubleToInt(128.123456, INT8_BITS), 128);
    EXPECT_EQ(NumberHelper::DoubleToInt(-128.987654321, INT8_BITS), -128);

    // the exponential bit digits exceeds 7
    EXPECT_EQ(NumberHelper::DoubleToInt(256.0, INT8_BITS), 0);
    EXPECT_EQ(NumberHelper::DoubleToInt(-256.0, INT8_BITS), 0);

    double nanDouble = 0.0f / 0.0f;
    EXPECT_EQ(NumberHelper::DoubleToInt(nanDouble, INT8_BITS), 0);
    double infDouble = POSITIVE_INFINITY;
    EXPECT_EQ(NumberHelper::DoubleToInt(infDouble, INT8_BITS), 0);
}

HWTEST_F_L0(NumberHelperTest, DoubleToInt_002)
{
    EXPECT_EQ(NumberHelper::DoubleToInt(256.0, INT16_BITS), 256);
    EXPECT_EQ(NumberHelper::DoubleToInt(256.555555555555556, INT16_BITS), 256);
    EXPECT_EQ(NumberHelper::DoubleToInt(256.999999999999999, INT16_BITS), 257);
    EXPECT_EQ(NumberHelper::DoubleToInt(1024.56789, INT16_BITS), 1024);
    EXPECT_EQ(NumberHelper::DoubleToInt(-1024.987654, INT16_BITS), -1024);

    // the exponential bit digits exceeds 15
    EXPECT_EQ(NumberHelper::DoubleToInt(65536.0, INT16_BITS), 0);
    EXPECT_EQ(NumberHelper::DoubleToInt(-65536.0, INT16_BITS), 0);

    double nanDouble = 0.0f / 0.0f;
    EXPECT_EQ(NumberHelper::DoubleToInt(nanDouble, INT16_BITS), 0);
    double infDouble = POSITIVE_INFINITY;
    EXPECT_EQ(NumberHelper::DoubleToInt(infDouble, INT16_BITS), 0);
}

HWTEST_F_L0(NumberHelperTest, DoubleToInt_003)
{
    EXPECT_EQ(NumberHelper::DoubleToInt(256.0, INT64_BITS), 256);
    EXPECT_EQ(NumberHelper::DoubleToInt(256.555555555555556, INT64_BITS), 256);
    EXPECT_EQ(NumberHelper::DoubleToInt(256.999999999999999, INT64_BITS), 257);
    EXPECT_EQ(NumberHelper::DoubleToInt(65536.55555, INT64_BITS), 65536);
    EXPECT_EQ(NumberHelper::DoubleToInt(-65536.99999, INT64_BITS), -65536);

    EXPECT_EQ(NumberHelper::DoubleToInt(2147483647.0, INT64_BITS), 2147483647);
    EXPECT_EQ(NumberHelper::DoubleToInt(-2147483647.0, INT64_BITS), -2147483647);

    double nanDouble = 0.0f / 0.0f;
    EXPECT_EQ(NumberHelper::DoubleToInt(nanDouble, INT64_BITS), 0);
    double infDouble = POSITIVE_INFINITY;
    EXPECT_EQ(NumberHelper::DoubleToInt(infDouble, INT64_BITS), 0);
}

/**
 * @tc.name: DoubleInRangeInt32
 * @tc.desc: The function is to convert double type to int type,The maximum value of integer part of double type
 *           cannot exceed the maximum value of int, and the minimum value of integer part cannot exceed the
 *           minimum value of int.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleInRangeInt32)
{
    // more than INT_MAX
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(2147483649.0), 2147483647);
    // less than INT_MIN
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(-2147483649.0), -2147483648);

    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(128.0), 128);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(-128.999999999999999), -129);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(256.0), 256);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(-256.0), -256);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(12345.6789), 12345);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(-12345.6789), -12345);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(65535.999999999999999), 65536);
    EXPECT_EQ(NumberHelper::DoubleInRangeInt32(-65535), -65535);
}

/**
 * @tc.name: DoubleToExponential
 * @tc.desc: Convert integer type to string type through "DoubleToExponential" function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToExponential01)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int radix;

    radix = 0;
    JSHandle<EcmaString> resultStr = factory->NewFromASCII("1.12356e-4");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToExponential(thread,
                                        0.00011235600000000001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);

    radix = 1;
    resultStr = factory->NewFromASCII("1e-4");
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToExponential(thread,
                                        0.00011235600000000001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    radix = 2;
    resultStr = factory->NewFromASCII("1.1e-4");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToExponential(thread,
                                        0.00011235600000000001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);

    radix = 16;
    resultStr = factory->NewFromASCII("1.123560000000000e-4");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToExponential(thread,
                                        0.00011235600000000001, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);

    radix = 0;
    resultStr = factory->NewFromASCII("1.23456e+2");
    JSHandle<EcmaString> handleEcmaStr5(thread, NumberHelper::DoubleToExponential(thread, 123.456, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    radix = 1;
    resultStr = factory->NewFromASCII("1e+2");
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToExponential(thread, 123.456, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);
    
    radix = 2;
    resultStr = factory->NewFromASCII("1.2e+2");
    JSHandle<EcmaString> handleEcmaStr7(thread, NumberHelper::DoubleToExponential(thread, 123.456, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr7, resultStr), 0);

    radix = 16;
    resultStr = factory->NewFromASCII("1.234560000000000e+2");
    JSHandle<EcmaString> handleEcmaStr8(thread, NumberHelper::DoubleToExponential(thread, 123.456, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr8, resultStr), 0);
}

HWTEST_F_L0(NumberHelperTest, DoubleToExponential02)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int radix;

    radix = -4;
    JSHandle<EcmaString> resultStr = factory->NewFromASCII("1.239876e+2");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToExponential(thread, 123.9876, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);

    radix = -6;
    resultStr = factory->NewFromASCII("1.239876e+2");
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToExponential(thread, 123.9876, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    radix = 3;
    resultStr = factory->NewFromASCII("1.24e+2");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToExponential(thread, 123.567, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);

    radix = 7;
    resultStr = factory->NewFromASCII("1.234567e+2");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToExponential(thread, 123.4567, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);

    radix = 8;
    resultStr = factory->NewFromASCII("1.2345670e+2");
    JSHandle<EcmaString> handleEcmaStr5(thread, NumberHelper::DoubleToExponential(thread, 123.45670, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    radix = 4;
    resultStr = factory->NewFromASCII("1.230e+2");
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToExponential(thread, 123.0123, radix));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);
}

/**
 * @tc.name: StringToDoubleWithRadix
 * @tc.desc: Convert double decimal type to Precision type through "DoubleToPrecision" function.If the logarithm
 *           of number based on ten is less than zero and radix Digit is more than zero or it is greater than zero
 *           and radix Digit is less than MAX_EXPONENT_DIGIT call the DoubleToFixed function.other call
 *           DoubleToExponential function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, StringToDoubleWithRadix)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int radix;
    Span<const uint8_t> sp;
    CVector<uint8_t> buf;
    JSHandle<EcmaString> resultStr;

    radix = 3;
    resultStr = factory->NewFromASCII("-12");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    bool negative = false;
    // 5 = 1 * 3 + 2
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), -5);

    radix = 4;
    resultStr = factory->NewFromASCII("1234567");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    // 27 = (1 * 4 + 2) * 4 + 3
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 27);
    // string has space
    resultStr = factory->NewFromASCII(" 12345 ");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 27);

    radix = 16;
    resultStr = factory->NewFromASCII("0x00ff");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 255);

    resultStr = factory->NewFromASCII("0x0010");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 16);

    resultStr = factory->NewFromASCII("0x1234");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 4660);
    // string has space
    resultStr = factory->NewFromASCII(" 0x12  ");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 18);

    resultStr = factory->NewFromASCII("0x1234XX");
    sp = EcmaStringAccessor(resultStr).ToUtf8Span(buf);
    negative = false;
    EXPECT_EQ(NumberHelper::StringToDoubleWithRadix(sp.begin(), sp.end(), radix, &negative).GetDouble(), 4660);
}

/**
 * @tc.name: IntToString
 * @tc.desc: Convert integer type to string type through "IntToString" function.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, IntToString)
{
    EXPECT_STREQ(NumberHelper::IntToString(0).c_str(), "0");
    EXPECT_STREQ(NumberHelper::IntToString(-100).c_str(), "-100");
    EXPECT_STREQ(NumberHelper::IntToString(123).c_str(), "123");
    EXPECT_STREQ(NumberHelper::IntToString(1234).c_str(), "1234");
}

/**
 * @tc.name: IntegerToString
 * @tc.desc: Convert the decimal number into the hexadecimal number corresponding to Radix and convert it into a string
 *           Check whether the returned string result is the same as the actual conversion result.
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, IntegerToString)
{
    int radix = 2;
    // number = (radix + 1) * MAX_MANTISSA
    CString integerStr = NumberHelper::IntegerToString(static_cast<double>((radix + 1) * (0x1ULL << 52U)), radix);
    EXPECT_STREQ(integerStr.c_str(), "110000000000000000000000000000000000000000000000000000");

    integerStr = NumberHelper::IntegerToString(static_cast<double>(10), radix);
    EXPECT_STREQ(integerStr.c_str(), "1010");

    radix = 3;
    integerStr = NumberHelper::IntegerToString(static_cast<double>(33), radix);
    EXPECT_STREQ(integerStr.c_str(), "1020");

    radix = 8;
    integerStr = NumberHelper::IntegerToString(static_cast<double>(128), radix);
    EXPECT_STREQ(integerStr.c_str(), "200");

    integerStr = NumberHelper::IntegerToString(static_cast<double>(128.985), radix);
    EXPECT_STREQ(integerStr.c_str(), "200");

    radix = 16;
    integerStr = NumberHelper::IntegerToString(static_cast<double>(256), radix);
    EXPECT_STREQ(integerStr.c_str(), "100");

    radix = 24;
    integerStr = NumberHelper::IntegerToString(static_cast<double>(987654), radix);
    EXPECT_STREQ(integerStr.c_str(), "2nag6");

    integerStr = NumberHelper::IntegerToString(static_cast<double>(23), radix);
    EXPECT_STREQ(integerStr.c_str(), "n");
}

/**
 * @tc.name: NumberToString
 * @tc.desc: The abstract operation NumberToString converts a Number m to String format
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, NumberToString)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    double d = 8.1999999999999993;
    JSHandle<EcmaString> result = NumberHelper::NumberToString(thread, JSTaggedValue(d));
    JSHandle<EcmaString> target = factory->NewFromASCII("8.2");
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, result, target), 0);

    double d2 = 0.30000000000000004;
    JSHandle<EcmaString> result1 = NumberHelper::NumberToString(thread, JSTaggedValue(d2));
    JSHandle<EcmaString> target1 = factory->NewFromASCII("0.30000000000000004");
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, result1, target1), 0);
}

/**
 * @tc.name: DoubleToInt64
 * @tc.desc: The abstract operation DoubleToInt64 converts a double value to int64_t
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToInt64)
{
    int64_t d = NumberHelper::DoubleToInt64(std::numeric_limits<double>::max());
    EXPECT_EQ(d, std::numeric_limits<int64_t>::max());

    int64_t d1 = NumberHelper::DoubleToInt64(static_cast<double>(std::numeric_limits<int64_t>::max()));
    EXPECT_EQ(d1, std::numeric_limits<int64_t>::max());
    int64_t d2 = NumberHelper::DoubleToInt64(static_cast<double>(std::numeric_limits<int64_t>::min()));
    EXPECT_EQ(d2, std::numeric_limits<int64_t>::min());

    int64_t d3 = NumberHelper::DoubleToInt64(static_cast<double>(std::numeric_limits<int64_t>::max()) + 1);
    EXPECT_EQ(d3, std::numeric_limits<int64_t>::max());
    int64_t d4 = NumberHelper::DoubleToInt64(static_cast<double>(std::numeric_limits<int64_t>::min()) - 1);
    EXPECT_EQ(d4, std::numeric_limits<int64_t>::min());

    int64_t d5 = NumberHelper::DoubleToInt64(0);
    EXPECT_EQ(d5, 0);
}
/**
 * @tc.name: DoubleToASCII
 * @tc.desc: When flag is equal to 2, the calculation result of DoubleToASCII is number.toFixed()
 * @tc.type: FUNC
 * @tc.require:
 */

HWTEST_F_L0(NumberHelperTest, DoubleToASCII_001)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int flags = 2;
    int digit = 2;
    JSHandle<EcmaString> resultStr = factory->NewFromASCII("0.10");
    JSHandle<EcmaString> handleEcmaStr(thread, NumberHelper::DoubleToASCII(thread,
                                       0.10000000000000001, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr, resultStr), 0);

    digit = 3;
    resultStr = factory->NewFromASCII("0.100");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToASCII(thread,
                                        0.10000000000000001, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);
    
    digit = 2;
    resultStr = factory->NewFromASCII("0.01");
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToASCII(thread,
                                        0.01, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    digit = 3;
    resultStr = factory->NewFromASCII("0.010");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToASCII(thread,
                                        0.01, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);

    digit = 7;
    resultStr = factory->NewFromASCII("0.0000006");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToASCII(thread,
                                        5.9999999999999997e-07, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);
    
    digit = 8;
    resultStr = factory->NewFromASCII("0.00000006");
    JSHandle<EcmaString> handleEcmaStr5(thread, NumberHelper::DoubleToASCII(thread,
                                        5.9999999999999997e-08, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    digit = 1;
#ifdef PANDA_TARGET_ARM32
    resultStr = factory->NewFromASCII("1.2");
#else
    resultStr = factory->NewFromASCII("1.3");
#endif
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToASCII(thread, 1.25, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);
    
    digit = 1;
#ifdef PANDA_TARGET_ARM32
    resultStr = factory->NewFromASCII("-1.2");
#else
    resultStr = factory->NewFromASCII("-1.3");
#endif
    JSHandle<EcmaString> handleEcmaStr7(thread, NumberHelper::DoubleToASCII(thread, -1.25, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr7, resultStr), 0);
}

/**
 * @tc.name: DoubleToASCII
 * @tc.desc: When flag is equal to 1, the calculation result of DoubleToASCII is number.toPrecision()
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(NumberHelperTest, DoubleToASCII_002)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    int flags = 1;
    int digit = 2;
    JSHandle<EcmaString> resultStr = factory->NewFromASCII("-1.2e-9");
    JSHandle<EcmaString> handleEcmaStr(thread, NumberHelper::DoubleToASCII(thread, -1.2345e-09, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr, resultStr), 0);

    digit = 2;
    flags = 1;
    resultStr = factory->NewFromASCII("1.2e-8");
    JSHandle<EcmaString> handleEcmaStr1(thread, NumberHelper::DoubleToASCII(thread,
                                        1.2345000000000001e-08, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr1, resultStr), 0);
    
    digit = 2;
    flags = 1;
#ifdef PANDA_TARGET_ARM32
    resultStr = factory->NewFromASCII("1.2");
#else
    resultStr = factory->NewFromASCII("1.3");
#endif
    JSHandle<EcmaString> handleEcmaStr2(thread, NumberHelper::DoubleToASCII(thread, 1.25, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr2, resultStr), 0);

    digit = 2;
    flags = 1;
    resultStr = factory->NewFromASCII("1.4");
    JSHandle<EcmaString> handleEcmaStr3(thread, NumberHelper::DoubleToASCII(thread, 1.35, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr3, resultStr), 0);


    digit = 15;
    flags = 1;
    resultStr = factory->NewFromASCII("0.000555000000000000");
    JSHandle<EcmaString> handleEcmaStr4(thread, NumberHelper::DoubleToASCII(thread,
                                        0.00055500000000000005, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr4, resultStr), 0);
    
    digit = 15;
    flags = 1;
    resultStr = factory->NewFromASCII("5.55000000000000e-7");
    JSHandle<EcmaString> handleEcmaStr5(thread, NumberHelper::DoubleToASCII(thread,
                                        5.5499999999999998e-07, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr5, resultStr), 0);

    digit = 15;
    flags = 1;
    resultStr = factory->NewFromASCII("-5.55000000000000e-7");
    JSHandle<EcmaString> handleEcmaStr6(thread, NumberHelper::DoubleToASCII(thread,
                                        -5.5499999999999998e-07, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr6, resultStr), 0);
    
    digit = 2;
    flags = 1;
    resultStr = factory->NewFromASCII("-12");
    JSHandle<EcmaString> handleEcmaStr7(thread, NumberHelper::DoubleToASCII(thread,
                                        -12.345000000000001, digit, flags));
    EXPECT_EQ(EcmaStringAccessor::Compare(instance, handleEcmaStr7, resultStr), 0);
}

} // namespace panda::ecmascript
