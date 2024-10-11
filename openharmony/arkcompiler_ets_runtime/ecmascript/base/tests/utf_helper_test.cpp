/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ecmascript/base/utf_helper.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::base;
using namespace panda::ecmascript::base::utf_helper;

namespace panda::test {
class UtfHelperTest : public BaseTestWithScope<false> {
};

/*
* @tc.name: CombineTwoU16
* @tc.desc: Enter a pair of UTF16-encoded surrogate pair corresponding to the lead surrogates and trail surrogates,
*           and return the corresponding Unicode codepoint value.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, CombineTwoU16)
{
    uint16_t leadSur = 0xD800;
    uint16_t trailSur = 0xDC00;
    uint32_t codePoint = static_cast<uint32_t>(((leadSur - 0xD800U) << 10)|(trailSur - 0xDc00U)) + 0x10000U;
    uint32_t utfHelperCodePoint = CombineTwoU16(leadSur, trailSur);
    EXPECT_EQ(codePoint, utfHelperCodePoint);
    EXPECT_EQ(codePoint, static_cast<uint32_t>(0x10000));
    trailSur = 0xDFFF;
    codePoint = static_cast<uint32_t>(((leadSur - 0xD800U) << 10) | (trailSur - 0xDC00U))+ 0x10000U;
    utfHelperCodePoint = CombineTwoU16(leadSur, trailSur);
    EXPECT_EQ(codePoint, utfHelperCodePoint);
    EXPECT_EQ(codePoint, static_cast<uint32_t>(0x103FF));
    leadSur = 0xDBFF;
    codePoint = static_cast<uint32_t>(((leadSur - 0xD800U) << 10) | (trailSur - 0xDC00U)) + 0x10000U;
    utfHelperCodePoint = CombineTwoU16(leadSur, trailSur);
    EXPECT_EQ(codePoint, utfHelperCodePoint);
    EXPECT_EQ(codePoint, static_cast<uint32_t>(0x10FFFF));
    trailSur = 0xDC00;
    codePoint = static_cast<uint32_t>(((leadSur - 0xD800U) << 10) | (trailSur - 0xDC00U)) + 0x10000U;
    utfHelperCodePoint = CombineTwoU16(leadSur, trailSur);
    EXPECT_EQ(codePoint, utfHelperCodePoint);
    EXPECT_EQ(codePoint, static_cast<uint32_t>(0x10FC00));
    leadSur = 0xD950;
    trailSur = 0xDF21;
    codePoint = static_cast<uint32_t>(((leadSur - 0xD800U)<< 10) | (trailSur - 0xDC00U)) + 0x10000U;
    utfHelperCodePoint = CombineTwoU16(leadSur, trailSur);
    EXPECT_EQ(codePoint, utfHelperCodePoint);
    EXPECT_EQ(codePoint, static_cast<uint32_t>(0x64321));
}

/*
* @tc.name: UTF16Decode
* @tc.desc: Enter a pair of UTF16-encoded surrogate pair corresponding to the lead surrogates and trail surrogates,
            Decodes them into corresponding Unicode codepoint values and returns.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, UTF16Decode)
{
    uint16_t lead = 0xD950;
    uint16_t trail = 0xDF21;
    EXPECT_TRUE(lead >= DECODE_LEAD_LOW && lead <= DECODE_LEAD_HIGH);
    EXPECT_TRUE(trail >= DECODE_TRAIL_LOW && trail <= DECODE_TRAIL_HIGH);
    uint32_t codePoint = utf_helper::UTF16Decode(lead, trail);
    EXPECT_EQ(codePoint, 0x64321U);
    lead = 0xD85D;
    trail = 0xDFCC;
    EXPECT_TRUE(lead >= DECODE_LEAD_LOW && lead <= DECODE_LEAD_HIGH);
    EXPECT_TRUE(trail >= DECODE_TRAIL_LOW && trail <= DECODE_TRAIL_HIGH);
    codePoint = utf_helper::UTF16Decode(lead, trail);
    EXPECT_EQ(codePoint, 0x277CCU);
}

/*
 * @tc.name: IsValidUTF8
 * @tc.desc: Judge whether an input group of symbols is a valid UTF8 coding sequence.
 * @tc.type: FUNC
 */
HWTEST_F_L0(UtfHelperTest, IsValidUTF8)
{
    // 0xxxxxxx, min:0, max:127
    const std::vector<uint8_t> utfDataOneBitVaild1 = {0x00};
    const std::vector<uint8_t> utfDataOneBitVaild2 = {BIT_MASK_1 - 0x01};
    const std::vector<uint8_t> utfDataOneBitInvaild = {BIT_MASK_1};
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataOneBitVaild1));
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataOneBitVaild2));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataOneBitInvaild));
    // 110xxxxx 10xxxxxx, min:128, max:2047
    const std::vector<uint8_t> utfDataTwoBitVaild1 = {BIT_MASK_2 + 0x02, BIT_MASK_1};
    const std::vector<uint8_t> utfDataTwoBitVaild2 = {BIT_MASK_3 - 0x01, BIT_MASK_2 - 0x01};
    const std::vector<uint8_t> utfDataTwoBitInvaild1 = {BIT_MASK_2, BIT_MASK_2};
    const std::vector<uint8_t> utfDataTwoBitInvaild2 = {BIT_MASK_3, BIT_MASK_1};
    const std::vector<uint8_t> utfDataTwoBitInvaild3 = {BIT_MASK_2, BIT_MASK_1};
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataTwoBitVaild1));
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataTwoBitVaild2));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataTwoBitInvaild1));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataTwoBitInvaild2));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataTwoBitInvaild3));
    // 1110xxxx 10xxxxxx 10xxxxxx, min:2048, max:65535
    const std::vector<uint8_t> utfDataThreeBitVaild1 = {BIT_MASK_3, BIT_MASK_1 + 0x20, BIT_MASK_1};
    const std::vector<uint8_t> utfDataThreeBitVaild2 = {BIT_MASK_4 - 0x01, BIT_MASK_2 - 0x01, BIT_MASK_2 - 0x01};
    const std::vector<uint8_t> utfDataThreeBitVaild3 = {BIT_MASK_3 + 0x01, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataThreeBitInvaild1 = {BIT_MASK_3, BIT_MASK_1, BIT_MASK_2};
    const std::vector<uint8_t> utfDataThreeBitInvaild2 = {BIT_MASK_3, BIT_MASK_2, BIT_MASK_1};
    const std::vector<uint8_t> utfDataThreeBitInvaild3 = {BIT_MASK_4, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataThreeBitInvaild4 = {BIT_MASK_4, BIT_MASK_2, BIT_MASK_2};
    const std::vector<uint8_t> utfDataThreeBitInvaild5 = {BIT_MASK_3, BIT_MASK_1, BIT_MASK_1};
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataThreeBitVaild1));
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataThreeBitVaild2));
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataThreeBitVaild3));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataThreeBitInvaild1));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataThreeBitInvaild2));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataThreeBitInvaild3));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataThreeBitInvaild4));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataThreeBitInvaild5));
    // 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx, min:65536, max:1114111(0x10FFFF)
    const std::vector<uint8_t> utfDataFourBitVaild1 = {BIT_MASK_4, BIT_MASK_1 + 0x10, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitVaild3 = {BIT_MASK_4 + 0x01, BIT_MASK_1, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitInvaild1 = {BIT_MASK_4, BIT_MASK_1, BIT_MASK_1, BIT_MASK_2};
    const std::vector<uint8_t> utfDataFourBitInvaild2 = {BIT_MASK_4, BIT_MASK_1, BIT_MASK_2, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitInvaild3 = {BIT_MASK_4, BIT_MASK_2, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitInvaild4 = {BIT_MASK_5, BIT_MASK_1, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitInvaild5 = {BIT_MASK_5, BIT_MASK_2, BIT_MASK_2, BIT_MASK_2};
    const std::vector<uint8_t> utfDataFourBitInvaild6 = {BIT_MASK_4, BIT_MASK_1, BIT_MASK_1, BIT_MASK_1};
    const std::vector<uint8_t> utfDataFourBitInvaild7 =
        {BIT_MASK_5 - 0x01, BIT_MASK_2 - 0x01, BIT_MASK_2 - 0x01, BIT_MASK_2 - 0x01};
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataFourBitVaild1));
    EXPECT_TRUE(utf_helper::IsValidUTF8(utfDataFourBitVaild3));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild1));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild2));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild3));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild4));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild5));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild6));
    EXPECT_FALSE(utf_helper::IsValidUTF8(utfDataFourBitInvaild7));
}

/*
* @tc.name: ConvertUtf16ToUtf8
* @tc.desc: Converts a UTF16 encoding sequence encoding a character into a UTF8 encoding sequence,
*           and returns the sequence and the byte length of the sequence. The parameter "modify"
*           indicates whether to perform special conversion for 0.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertUtf16ToUtf8_001)
{
    // codePoint lie in [0,0x7F]--->UTF-8(length:1)
    {
        uint16_t utf16Data0 = 0x00;
        uint16_t utf16Data1 = 0x00;
        Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        Utf8Char utf8CharTemp = {0, {0x00U}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    }

    // special case for \u0000 ==> Co80- 1100'0000 1000'0000
    {
        uint16_t utf16Data0 = 0x00;
        uint16_t utf16Data1 = 0x00;
        Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, true);
        Utf8Char utf8CharTemp = {2, {UTF8_2B_FIRST, UTF8_2B_SECOND}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
        utf16Data0 = 0x7F;
        utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        utf8CharTemp = {1, {0x7F}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);

        // codePoint lie in [0x80,0x7FF]--> UTF-8(length:2)
        utf16Data0 = 0x80;
        utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        utf8CharTemp = {2, {UTF8_2B_FIRST + 0x02U, UTF8_2B_SECOND}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
        utf16Data0 = 0x7FF;
        utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        utf8CharTemp = {2, {BIT_MASK_3 - 0x01, BIT_MASK_2 - 0x01}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    }

    // codePoint lie in [0xD800,0xDFFF]--> UTF-8(length:3)
    {
        uint16_t utf16Data0 = 0xD800;
        uint16_t utf16Data1 = 0x00;
        Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        Utf8Char utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0xD800 >> 12),
                            UTF8_3B_SECOND | (static_cast<uint8_t>(0xD800 >> 6) & utf::MASK_6BIT),
                            UTF8_3B_THIRD | (static_cast<uint8_t>(0xD800) & utf::MASK_6BIT)}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
        utf16Data0 = 0xDFFF;
        utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0xDFFF >> 12),
                            UTF8_3B_SECOND | (static_cast<uint8_t>(0xDFFF >> 6) & utf::MASK_6BIT),
                            UTF8_3B_THIRD | (static_cast<uint8_t>(0xDFFF) & utf::MASK_6BIT)}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    }
}

HWTEST_F_L0(UtfHelperTest, ConvertUtf16ToUtf8_002)
{
    // codePoint lie in [0x800,0xD7FF]&&[0xE000,0xFFFF]-->UTF-8(length:3)
    uint16_t utf16Data0 = 0x800;
    uint16_t utf16Data1 = 0x00;
    Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
    Utf8Char utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0x800 >> 12),
                                 UTF8_3B_SECOND | (static_cast<uint8_t>(0x800 >> 6) & utf::MASK_6BIT),
                                 UTF8_3B_THIRD | (static_cast<uint8_t>(0x800) & utf::MASK_6BIT)}};
    EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
    EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    utf16Data0 = 0xD7FF;
    utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
    utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0xD7FF>>12),
                        UTF8_3B_SECOND | (static_cast<uint8_t>(0xD7FF >> 6) & utf::MASK_6BIT),
                        UTF8_3B_THIRD | (static_cast<uint8_t>(0xD7FF) & utf::MASK_6BIT)}};
    EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
    EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    utf16Data0 = 0xE000;
    utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
    utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0xE000 >> 12),
                        UTF8_3B_SECOND | (static_cast<uint8_t>(0xE000 >> 6)& utf::MASK_6BIT),
                        UTF8_3B_THIRD | (static_cast<uint8_t>(0xE000) & utf::MASK_6BIT)}};
    EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
    EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    utf16Data0 = 0xFFFF;
    utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
    utf8CharTemp = {3, {UTF8_3B_FIRST | static_cast<uint8_t>(0xFFFF >> 12),
                        UTF8_3B_SECOND | (static_cast<uint8_t>(0xFFFF >> 6)& utf::MASK_6BIT),
                        UTF8_3B_THIRD | (static_cast<uint8_t>(0xFFFF) & utf::MASK_6BIT)}};
    EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
    EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
}

HWTEST_F_L0(UtfHelperTest, ConvertUtf16ToUtf8_003)
{
    // codePoint lie in [0x10000,0x10FFFF] --> UTF-8(length:4)
    {
        uint16_t utf16Data0 = 0xD800;
        uint16_t utf16Data1 = 0xDC00;
        Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        uint32_t codePoint = CombineTwoU16(utf16Data0, utf16Data1);
        Utf8Char utf8CharTemp = {4, {static_cast<uint8_t>((codePoint >> 18) | UTF8_4B_FIRST),
                            static_cast<uint8_t>(((codePoint >> 12) & utf::MASK_6BIT) | utf::MASK1),
                            static_cast<uint8_t>(((codePoint >> 6) & utf::MASK_6BIT) | utf::MASK1),
                            static_cast<uint8_t>((codePoint & utf::MASK_6BIT) | utf::MASK1)}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    }

    // 0xD950 0xDF21 --> 0x64321 --> 0xf1 0xa4 0x8c 0xa1
    {
        uint16_t utf16Data0 = 0xD950;
        uint16_t utf16Data1 = 0xDF21;
        Utf8Char utf8Char = ConvertUtf16ToUtf8(utf16Data0, utf16Data1, false);
        uint32_t codePoint = CombineTwoU16(utf16Data0, utf16Data1);
        Utf8Char utf8CharTemp = {4, {static_cast<uint8_t>((codePoint >> 18) | UTF8_4B_FIRST),
                            static_cast<uint8_t>(((codePoint >> 12)& utf::MASK_6BIT)| utf::MASK1),
                            static_cast<uint8_t>(((codePoint >> 6)& utf::MASK_6BIT) | utf::MASK1),
        static_cast<uint8_t>((codePoint & utf::MASK_6BIT) | utf::MASK1)}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
        utf8CharTemp = {4, {0xf1, 0xa4, 0x8c, 0xa1}};
        EXPECT_EQ(utf8Char.n, utf8CharTemp.n);
        EXPECT_EQ(utf8Char.ch, utf8CharTemp.ch);
    }
}

/*
* @tc.name: Utf16ToUtf8Size
* @tc.desc: Enter a string of UTF16 coded sequences and return the length of the sequence converted into UTF8 coded
*           sequences. "length" indicates the length of the input UTF16 sequence, and "modify" indicates whether
*           to perform special conversion for.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf16ToUtf8Size_001)
{
    // when utf16 data length is only 1 and code in 0xd800-0xdfff, means that is a single code point, it needs to be
    // represented by three UTF8 code.
    uint32_t length = 0;
    uint16_t utf16Value1[1] = {0xD800};
    const uint16_t *utf16ValuePtr1 = utf16Value1;
    length = Utf16ToUtf8Size(utf16ValuePtr1, 1, false);
    EXPECT_EQ(length - 1, UtfLength::THREE);
    length = 1;
    uint16_t utf16Value2[1] = {0xDFFF};
    const uint16_t *utf16ValuePtr2 = utf16Value2;
    length = Utf16ToUtf8Size(utf16ValuePtr2, 1, false);
    EXPECT_EQ(length - 1, UtfLength::THREE);

    // special case for U+0000 => c0 80
    uint16_t utf16Value3[1] = {0x00};
    const uint16_t *utf16ValuePtr3 = utf16Value3;
    length = Utf16ToUtf8Size(utf16ValuePtr3, 1, false);
    EXPECT_EQ(length - 1, 0U);
    length = Utf16ToUtf8Size(utf16ValuePtr3, 1, true);
    EXPECT_EQ(length - 1, 2U);

    // if isGetBufferSize is true, special case for U+0000 => 00
    uint16_t utf16Value12[1] = {0x00};
    const uint16_t *utf16ValuePtr12 = utf16Value12;
    length = Utf16ToUtf8Size(utf16ValuePtr12, 1, false, true);
    EXPECT_EQ(length - 1, 1U);
    length = Utf16ToUtf8Size(utf16ValuePtr12, 1, true, true);
    EXPECT_EQ(length - 1, 1U);

    // code point lie in [0x00, 0x7F], it needs to be represented by one UTF8 code.
    uint16_t utf16Value4[1] = {0x00};
    uint16_t utf16Value5[1] = {0x7F};
    const uint16_t *utf16ValuePtr4 = utf16Value4;
    const uint16_t *utf16ValuePtr5 = utf16Value5;
    length = Utf16ToUtf8Size(utf16ValuePtr4, 1, false);
    EXPECT_EQ(length - 1, 0U);
    length = Utf16ToUtf8Size(utf16ValuePtr5, 1, false);
    EXPECT_EQ(length - 1, 1U);

    // code point lie in [0x80, 0x7FF], it needs to be represented by two UTF8 code,
    uint16_t utf16Value6[1] = {0x80};
    uint16_t utf16Value7[1] = {0x7FF};
    const uint16_t *utf16ValuePtr6 = utf16Value6;
    const uint16_t *utf16ValuePtr7 = utf16Value7;
    length = Utf16ToUtf8Size(utf16ValuePtr6, 1, false);
    EXPECT_EQ(length - 1, 2U);
    length = Utf16ToUtf8Size(utf16ValuePtr7, 1, false);
    EXPECT_EQ(length - 1, 2U);
    // code point lie in [0x800, 0xD7FF] or [0xDCoo, 0xFFFF], it needs to be represented by three UTF8 code.
    uint16_t utf16Value8[1] = {0x800};
    uint16_t utf16Value9[1] = {0xD7FF};
    uint16_t utf16Value10[1] = {0xDC00};
    uint16_t utf16Value11[1] = {0xFFFF};
    const uint16_t *utf16ValuePtr8 = utf16Value8;
    const uint16_t *utf16ValuePtr9 = utf16Value9;
    const uint16_t *utf16ValuePtr10 = utf16Value10;
    const uint16_t *utf16ValuePtr11 = utf16Value11;
    length = Utf16ToUtf8Size(utf16ValuePtr8, 1, false);
    EXPECT_EQ(length - 1, 3U);
    length = Utf16ToUtf8Size(utf16ValuePtr9, 1, false);
    EXPECT_EQ(length - 1, 3U);
    length = Utf16ToUtf8Size(utf16ValuePtr10, 1, false);
    EXPECT_EQ(length-1, 3U);
    length = Utf16ToUtf8Size(utf16ValuePtr11, 1, false);
    EXPECT_EQ(length - 1, 3U);
}

HWTEST_F_L0(UtfHelperTest, Utf16ToUtf8Size_002)
{
    // The trail value is valid, located in [0xDc00, 0xDFFF].It needs to be represented by four UTF8 code.
    uint16_t utf16Value12[2] = {0xD800, 0xDc00};
    uint16_t utf16Value13[2] = {0xD800, 0xDFFF};
    uint16_t utf16Value14[2] = {0xDBFF, 0xDC00};
    uint16_t utf16Value15[2] = {0xDBFF, 0xDFFF};
    const uint16_t *utf16ValuePtr12 = utf16Value12;
    const uint16_t *utf16ValuePtr13 = utf16Value13;
    const uint16_t *utf16ValuePtr14 = utf16Value14;
    const uint16_t *utf16ValuePtr15 = utf16Value15;
    uint32_t length = Utf16ToUtf8Size(utf16ValuePtr12, 2, false);
    EXPECT_EQ(length - 1, 4U);
    length = Utf16ToUtf8Size(utf16ValuePtr13, 2, false);
    EXPECT_EQ(length- 1, 4U);
    length = Utf16ToUtf8Size(utf16ValuePtr14, 2, false);
    EXPECT_EQ(length - 1, 4U);
    length = Utf16ToUtf8Size(utf16ValuePtr15, 2, false);
    EXPECT_EQ(length - 1, 4U);

    // The trail value of Bad sequence is invalid, not located in [0xDC00, 0xDFFF].
    // Need to return 6 bytes length
    uint16_t utf16Value16[2] = {0xD800, 0xDBFF};
    uint16_t utf16Value17[2] = {0xDC00, 0xDFFF};
    const uint16_t *utf16ValuePtr16 = utf16Value16;
    const uint16_t *utf16ValuePtr17 = utf16Value17;
    length = Utf16ToUtf8Size(utf16ValuePtr16, 2, false);
    EXPECT_EQ(length- 1, 6U);
    length = Utf16ToUtf8Size(utf16ValuePtr17, 2, false);
    EXPECT_EQ(length-1, 6U);

    // 0(or 2)+ 1+ 2 + 3 + 4 = 10(or 12)
    uint16_t utf16Value18[6] = {0x00, 0x7F, 0x80, 0x800, 0xD800, 0xDC00};
    const uint16_t *utf16ValuePtr18 = utf16Value18;
    length = Utf16ToUtf8Size(utf16ValuePtr18, 6, false);
    EXPECT_EQ(length - 1, 10U);
    length = Utf16ToUtf8Size(utf16ValuePtr18, 6, true);
    EXPECT_EQ(length - 1, 12U);
}

/*
* @tc.name: ConvertUtf8ToUtf16Pair
* @tc.desc: Converts a UTF8 encoding sequence encoding a character into a UTF16 encoding sequence, and returns the
*           sequence and the byte length of the UTF16 encoding sequence. The parameter "combine" identifies whether
*           to return a pr0xy pair of Unicode values in the secondary plane, or the Unicode value itself.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertUtf8ToUtf16Pair)
{
    // code point lie in [0x00, 0x7F], the length of utf8 code element byte is 1
    uint8_t utf8Value1[1] = {0x00};
    uint8_t utf8Value2[1] = {UTF8_1B_MAX};
    const uint8_t *utf8ValuePtr1 = utf8Value1;
    const uint8_t *utf8ValuePtr2 = utf8Value2;
    std::pair<uint32_t, size_t> utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr1);
    std::pair<uint32_t, size_t> utf16Value = {utf8Value1[0], 1};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr2);
    utf16Value = {utf8Value2[0], 1};
    EXPECT_EQ(utf16Res, utf16Value);
    // code point lie in [0x80, 0x7FF], the length of utf8 code element byte is 2
    uint8_t utf8Value3[2] = {0xc2, 0x80}; // 0x80
    uint8_t utf8Value4[2] = {0xDF, 0xBF}; // 0x7FF
    const uint8_t *utf8ValuePtr3 = utf8Value3;
    const uint8_t *utf8ValuePtr4 = utf8Value4;
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr3);
    utf16Value = {0x80, 2};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr4);
    utf16Value = {0x7FF, 2};
    EXPECT_EQ(utf16Res, utf16Value);

    // code point lie in [0x800, 0xD7FF] or [0xDC00,0xFFFF], the length of utf8 code element byte is 3.
    // when code point lie in [0xD800, 0xDBFF], due to the use of UCS-2, it corresponds to 3 utf8 symbols.
    uint8_t utf8Value5[3] = {0xE0, 0xA0, 0x80}; // 0x800
    uint8_t utf8Value6[3] = {0xEF, 0xBF, 0xBF}; // 0xFFFF
    const uint8_t *utf8ValuePtr5 = utf8Value5;
    const uint8_t *utf8ValuePtr6 = utf8Value6;
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr5);
    utf16Value = {0x800, 3};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr6);
    utf16Value = {0xFFFF, 3};
    EXPECT_EQ(utf16Res, utf16Value);
    // code point lie in [0x10000, 0x10FFFF], the length of utf8 code element byte is 4.
    uint8_t utf8Value9[4] = {0xF0, 0x90, 0x80, 0x80}; // 0x10000
    uint8_t utf8Value10[4] = {0xF4, 0x8F, 0xBF, 0xBF}; // 0x10FFFF
    const uint8_t *utf8ValuePtr9 = utf8Value9;
    const uint8_t *utf8ValuePtr10 = utf8Value10;
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr9);
    utf16Value = {0xD800 << 16 | 0xDC00U, 4};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr10);
    utf16Value = {0xDBFF << 16 | 0xDFFF, 4};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr9, true);
    utf16Value = {0x10000, 4};
    EXPECT_EQ(utf16Res, utf16Value);
    utf16Res = ConvertUtf8ToUtf16Pair(utf8ValuePtr10, true);
    utf16Value = {0x10FFFF, 4};
    EXPECT_EQ(utf16Res, utf16Value);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Enter a string of UTF8 coded sequences and return the length of the sequence converted into UTF16 coded
*           sequences.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size)
{
    // when code point lie in (0x00, 0xFFFF], the required utf16 code element length is 1.
    uint8_t utf8ValueOneByteMin[1] = {0x00};
    uint8_t utf8ValueOneByteMax[4] = {0xEF, 0xBF, 0xBF, 0x00}; // 0xFFFF
    const uint8_t *utf8ValueOneByteMinPtr = utf8ValueOneByteMin;
    const uint8_t *utf8ValueOneByteMaxPtr = utf8ValueOneByteMax;
    size_t length = Utf8ToUtf16Size(utf8ValueOneByteMinPtr, sizeof(utf8ValueOneByteMin));
    EXPECT_EQ(length, 1U);
    length = Utf8ToUtf16Size(utf8ValueOneByteMaxPtr, sizeof(utf8ValueOneByteMax));
    EXPECT_EQ(length, 2U);
    // when code point lie in [0x10000, 0x10FFFF], the required utf16 code element length is 2.
    const uint8_t utf8ValueTwoBytesMin[5] = {0xF0, 0x90, 0x80, 0x80, 0x00}; // 0x10000
    const uint8_t utf8ValueTwoBytesMax[5] = {0xF4, 0x8F, 0xBF, 0xBF, 0x00}; // 0x10FFFF
    const uint8_t *utf8ValueTwoBytesMinPtr = utf8ValueTwoBytesMin;
    const uint8_t *utf8ValueTwoBytesMaxPtr = utf8ValueTwoBytesMax;
    length = Utf8ToUtf16Size(utf8ValueTwoBytesMinPtr, sizeof(utf8ValueTwoBytesMin));
    EXPECT_EQ(length, 3U);
    length = Utf8ToUtf16Size(utf8ValueTwoBytesMaxPtr, sizeof(utf8ValueTwoBytesMax));
    EXPECT_EQ(length, 3U);
    uint8_t utf8Value[12] = {
        0xEF, 0xBF, 0xBF, 0xF0,
        0x90, 0x80, 0x80, 0xF4,
        0x8F, 0xBF, 0xBF, 0x00};
    const uint8_t *utf8ValuePtr = utf8Value;
    length = Utf8ToUtf16Size(utf8ValuePtr, sizeof(utf8Value));
    EXPECT_EQ(length, 6U);
}

static void ConvertRegionUtf16ToUtf8Test(bool isDebugger = false)
{
    size_t utf16Len = 8;
    size_t utf8Len = 100;
    size_t start = 0;
    size_t utf8Pos = 0;
    bool modify = false;
    uint16_t utf16Value[8] = {
        0x00, // 0 or 2 (special case for \u0000 ==> C080 - 1100'0000 1000'0000）
        0x7F, // 1(0x00, 0x7F]
        0x7FF, // 2 [0x80, 0x7FF]
        0x800, // 3 [0x800, 0xD7FF]
        0xD800, // 3 [0xD800, 0xDFFF]
        0xFFFF, // 3 [0xE000, 0xFFFF]
        0xD800, 0xDFFF}; // 4 [0x10000, 0x10FFFF]
    const uint16_t *utf16ValuePtr = utf16Value;
    uint8_t *utf8Out = (uint8_t*)malloc(utf8Len);
    if (isDebugger) {
        utf8Pos = DebuggerConvertRegionUtf16ToUtf8(utf16ValuePtr, utf8Out, utf16Len, utf8Len, start, modify);
    } else {
        utf8Pos = ConvertRegionUtf16ToUtf8(utf16ValuePtr, utf8Out, utf16Len, utf8Len, start, modify);
    }
    // 0 + 1 + 2 +(3 *3)+ 4= 16
    EXPECT_EQ(utf8Pos, 16U);
    // 2 + 1 + 2 +(3 * 3)+ 4 = 18
    modify = true;
    if (isDebugger) {
        utf8Pos = DebuggerConvertRegionUtf16ToUtf8(utf16ValuePtr, utf8Out, utf16Len, utf8Len, start, modify);
    } else {
        utf8Pos = ConvertRegionUtf16ToUtf8(utf16ValuePtr, utf8Out, utf16Len, utf8Len, start, modify);
    }
    EXPECT_EQ(utf8Pos, 18U);
    free(utf8Out);
}

/*
* @tc.name: ConvertRegionUtf16ToUtf8
* @tc.desc: Input aUTF16-encoded sequence (thelength is "utf16Len"), convert part of the sequence into a UTF8-encoded
*           sequence, and save it to "utf8Out"(the maximum length is "utf8Len"). The start parameter indicates the
*           start position of the conversion. Whether to perform special processing for O in the "modify" parameter.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf16ToUtf8)
{
    ConvertRegionUtf16ToUtf8Test();
}

HWTEST_F_L0(UtfHelperTest, DebuggerConvertRegionUtf16ToUtf8)
{
    ConvertRegionUtf16ToUtf8Test(true);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Input a UTF8-encoded sequence, convert part of the sequence into a UTF8-encoded sequence, and save it to
*           "utf16Out"(the maximum length is "utf16Len"), The start parameter indicates the start position of the
*           conversion.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16)
{
    size_t utf16Len = 100;
    uint8_t utf8Value[10] = {
        0x7F, // 1-length UTF16 encoding
        0xDF, 0xBF, // 1-length UTF16 encoding
        0xEF, 0xBF, 0xBF, // 1-length UTF16 encoding
        0xF4, 0x8F, 0xBF, 0xBF}; // 2-length UTF16 encoding
    const uint8_t *utf8ValuePtr = utf8Value;
    uint16_t *utf16Out = (uint16_t*)malloc(utf16Len);
    size_t outPos = ConvertRegionUtf8ToUtf16(utf8ValuePtr, utf16Out, sizeof(utf8Value), utf16Len);
    // 1 + 1 + 1 + 2 = 5s
    EXPECT_EQ(outPos, 5U);
    // 1 + 2 = 3
    utf8ValuePtr = utf8Value + 3;
    outPos = ConvertRegionUtf8ToUtf16(utf8ValuePtr, utf16Out, sizeof(utf8Value) - 3, utf16Len);
    EXPECT_EQ(outPos, 3U);
}

/*
* @tc.name: ConvertUtf8ToUnicodeChar
* @tc.desc: Converts a UTF8 encoding sequence encoding a character into a unicode point, and returns the
*           unicode point and the byte length of the utf8 encoding sequence.
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertUtf8ToUnicodeChar)
{
    std::pair<int32_t, size_t> invalidValue = {INVALID_UTF8, 0};
    // utf-8 is one byte, code point lie in [0x00, 0x7F]
    uint8_t utf8Value1[1] = {0x00}; // 0x00
    uint8_t utf8Value2[1] = {0x7F}; // 0x7F
    const uint8_t *utf8ValuePtr1 = utf8Value1;
    const uint8_t *utf8ValuePtr2 = utf8Value2;
    std::pair<int32_t, size_t> unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr1, UtfLength::ONE);
    std::pair<int32_t, size_t> unicodeValue = {0x00, 1};
    EXPECT_EQ(unicodeRes, unicodeValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr2, UtfLength::ONE);
    unicodeValue = {0x7F, 1};
    EXPECT_EQ(unicodeRes, unicodeValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr2, 0);
    EXPECT_EQ(unicodeRes, invalidValue);

    // utf-8 is two bytes, code point lie in [0x80, 0x7FF]
    uint8_t utf8Value3[2] = {0xC2, 0x80}; // 0x80
    uint8_t utf8Value4[2] = {0xDF, 0xBF}; // 0x7FF
    const uint8_t *utf8ValuePtr3 = utf8Value3;
    const uint8_t *utf8ValuePtr4 = utf8Value4;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr3, UtfLength::TWO);
    unicodeValue = {0x80, 2};
    EXPECT_EQ(unicodeRes, unicodeValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr4, UtfLength::TWO);
    unicodeValue = {0x7FF, 2};
    EXPECT_EQ(unicodeRes, unicodeValue);
    uint8_t utf8Value5[2] = {0xD0, 0x00}; // invalid
    const uint8_t *utf8ValuePtr5 = utf8Value5;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr5, UtfLength::TWO);
    EXPECT_EQ(unicodeRes, invalidValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr4, UtfLength::ONE);
    EXPECT_EQ(unicodeRes, invalidValue);

    // utf-8 is three bytes, code point lie in [0x800, 0xFFFF]
    uint8_t utf8Value6[3] = {0xE0, 0xA0, 0x80}; // 0x800
    uint8_t utf8Value7[3] = {0xED, 0x9F, 0xBF}; // 0xD7FF
    const uint8_t *utf8ValuePtr6 = utf8Value6;
    const uint8_t *utf8ValuePtr7 = utf8Value7;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr6, UtfLength::THREE);
    unicodeValue = {0x800, 3};
    EXPECT_EQ(unicodeRes, unicodeValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr7, UtfLength::THREE);
    unicodeValue = {0xD7FF, 3};
    EXPECT_EQ(unicodeRes, unicodeValue);
    uint8_t utf8Value8[3] = {0xEB, 0x80, 0x40}; // invalid
    const uint8_t *utf8ValuePtr8 = utf8Value8;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr8, UtfLength::THREE);
    EXPECT_EQ(unicodeRes, invalidValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr7, UtfLength::TWO);
    EXPECT_EQ(unicodeRes, invalidValue);

    // utf-8 is four bytes, code point lie in [0x10000, 0x10FFFF].
    uint8_t utf8Value9[4] = {0xF0, 0x90, 0x80, 0x80}; // 0x10000
    uint8_t utf8Value10[4] = {0xF4, 0x8F, 0xBF, 0xBF}; // 0x10FFFF
    const uint8_t *utf8ValuePtr9 = utf8Value9;
    const uint8_t *utf8ValuePtr10 = utf8Value10;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr9, UtfLength::FOUR);
    unicodeValue = {0x10000, 4};
    EXPECT_EQ(unicodeRes, unicodeValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr10, UtfLength::FOUR);
    unicodeValue = {0x10FFFF, 4};
    EXPECT_EQ(unicodeRes, unicodeValue);
    uint8_t utf8Value11[4] = {0xF4, 0x80, 0x80, 0x40}; // invalid
    const uint8_t *utf8ValuePtr11 = utf8Value11;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr11, UtfLength::FOUR);
    EXPECT_EQ(unicodeRes, invalidValue);
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr10, UtfLength::THREE);
    EXPECT_EQ(unicodeRes, invalidValue);

    // other exception
    uint8_t utf8Value12[2] = {0x90, 0x00}; // invalid
    const uint8_t *utf8ValuePtr12 = utf8Value12;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr12, UtfLength::FOUR);
    EXPECT_EQ(unicodeRes, invalidValue);
    uint8_t utf8Value13[2] = {0xF8, 0x00}; // invalid
    const uint8_t *utf8ValuePtr13 = utf8Value13;
    unicodeRes = ConvertUtf8ToUnicodeChar(utf8ValuePtr13, UtfLength::FOUR);
    EXPECT_EQ(unicodeRes, invalidValue);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test single byte characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_001) {
    std::string utf8 = "Hello";
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0, 0x0, 0x0}; // "Hello"
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test includes Chinese characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_002) {
    std::string utf8 = "你好，世界！";
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; // "你好，世界！"
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test empty string
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_003) {
    std::string utf8 = "";
    std::vector<uint16_t> expected_utf16 = {}; // empty
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test section conversion
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_004) {
    std::string utf8 = "Hello, 你好";
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; // "Hello, 你"
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test buffer length limit
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_005) {
    std::string utf8 = "你好，世界！";
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0}; // "你好"
    std::vector<uint16_t> utf16(2); // Limit buffer length
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test for incorrect UTF-8 data
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_006) {
    std::string utf8 = "\xF0\x28\x8C\x28";
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0};
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test single byte UTF-8 characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_007) {
    std::string utf8 = "ABC"; // All are single byte characters
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0}; // ASCII characters: A, B, C
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Testing Double Byte UTF-8 Characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_008) {
    std::string utf8 = "\xC2\xA2\xC3\xBC"; // They are ¢ and ü, respectively
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0}; // Unicode .
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test three byte UTF-8 characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_009) {
    std::string utf8 = "\xE2\x82\xAC"; // euro: €
    std::vector<uint16_t> expected_utf16 = {0x0}; // Unicode .
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test four byte UTF-8 characters and proxy pairs
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_010) {
    std::string utf8 = "\xF0\x9F\x98\x8E"; // Emoji 😎
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0}; // surrogates
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test UTF-8 data containing zero bytes
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_011) {
    std::string utf8 = "Hello\0World", utf8Nul = utf8 + '\0' + "World"; // Clearly including zero bytes
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
    std::vector<uint16_t> utf16(15);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8Nul.data()), utf8Nul.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: Utf8ToUtf16Size
* @tc.desc: Test continuous illegal sequences
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, Utf8ToUtf16Size_012) {
    std::string utf8 = "\xC0\x80\xC0\x80"; // Continuous illegal sequence
    std::vector<uint16_t> expected_utf16 = {0x0, 0x0};
    std::vector<uint16_t> utf16(10);
    size_t converted = Utf8ToUtf16Size(reinterpret_cast<const uint8_t*>(utf8.data()), utf8.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test single byte characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_001) {
    std::string utf8 = "Hello";
    std::vector<uint16_t> expected_utf16 = {0x0048, 0x0065, 0x006C, 0x006C, 0x006F}; // "Hello"
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test includes Chinese characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_002) {
    std::string utf8 = "你好，世界！";
    std::vector<uint16_t> expected_utf16 = {0x4F60, 0x597D, 0xFF0C, 0x4E16, 0x754C, 0xFF01}; // "你好，世界！"
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test empty string
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_003) {
    std::string utf8 = "";
    std::vector<uint16_t> expected_utf16 = {}; // Empty
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test section conversion
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_004) {
    std::string utf8 = "Hello, 你好";
    std::vector<uint16_t> expected_utf16 = {0x0048, 0x0065, 0x006C, 0x006C, 0x006F, 0x002C, 0x20, 0x4F60};
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), 10, utf16.size()); // Only process the first 9 bytes
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test buffer length limit
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_005) {
    std::string utf8 = "你好，世界！";
    std::vector<uint16_t> expected_utf16 = {0x4F60, 0x597D}; // "你好"
    std::vector<uint16_t> utf16(2); // Limit buffer length
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test for incorrect UTF-8 data
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_006) {
    std::string utf8 = "\xF0\x28\x8C\x28";
    std::vector<uint16_t> expected_utf16 = {}; // Expected empty output, handling erroneous data
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_NE(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test single byte UTF-8 characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_007) {
    std::string utf8 = "ABC"; // All are single byte characters
    std::vector<uint16_t> expected_utf16 = {0x0041, 0x0042, 0x0043}; // ASCII characters: A, B, C
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Testing Double Byte UTF-8 Characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_008) {
    std::string utf8 = "\xC2\xA2\xC3\xBC"; // They are ¢ and ü, respectively
    std::vector<uint16_t> expected_utf16 = {0x00A2, 0x00FC}; // Unicode .
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test three byte UTF-8 characters
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_009) {
    std::string utf8 = "\xE2\x82\xAC"; // euro €
    std::vector<uint16_t> expected_utf16 = {0x20AC}; // Unicode .
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test four byte UTF-8 characters and proxy pairs
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_010) {
    std::string utf8 = "\xF0\x9F\x98\x8E"; // Emoji 😎
    std::vector<uint16_t> expected_utf16 = {0xD83D, 0xDE0E}; // surrogates
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test UTF-8 data containing zero bytes
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_011) {
    std::string utf8 = "Hello\0World", utf8Nul = utf8 + '\0' + "World"; // Clearly including zero bytes
    std::vector<uint16_t> expected_utf16 = {0x0048, 0x0065, 0x006C, 0x006C, 0x006F,
        0x0000, 0x0057, 0x006F, 0x0072, 0x006C, 0x0064}; // Including NULL characters
    std::vector<uint16_t> utf16(15);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8Nul.data()),
                                                utf16.data(), utf8Nul.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test continuous illegal sequences
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_012) {
    std::string utf8 = "\xC0\x80\xC0\x80"; // Continuous illegal sequence
    std::vector<uint16_t> expected_utf16 = {};
    std::vector<uint16_t> utf16(10);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_NE(utf16, expected_utf16);
}

/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test four byte UTF-8 characters and proxy pairs
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_013) {
    std::string utf8 = "\xF0\x9F\x98\x8E"; // Emoji 😎
    std::vector<uint16_t> expected_utf16 = {0xD83D, 0xDE0E}; // surrogates
    std::vector<uint16_t> utf16(0);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(converted, 0);
}
/*
* @tc.name: ConvertRegionUtf8ToUtf16
* @tc.desc: Test four byte UTF-8 characters and proxy pairs
* @tc.type: FUNC
*/
HWTEST_F_L0(UtfHelperTest, ConvertRegionUtf8ToUtf16_014) {
    std::string utf8 = "\xF0\x9F\x98\x8E"; // Emoji 😎
    std::vector<uint16_t> expected_utf16 = {0xD83D, 0xDE0E}; // surrogates
    std::vector<uint16_t> utf16(1);
    size_t converted = ConvertRegionUtf8ToUtf16(reinterpret_cast<const uint8_t*>(utf8.data()),
                                                utf16.data(), utf8.size(), utf16.size());
    utf16.resize(converted);
    EXPECT_EQ(converted, 0);
}
} // namespace panda:test
