/**
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "utf.h"
#include "base/log/log_wrapper.h"
#include <memory>

namespace OHOS::Ace {

/*
 * MUtf-8
 *
 * U+0000 => C0 80
 *
 * N  Bits for     First        Last        Byte 1      Byte 2      Byte 3      Byte 4      Byte 5      Byte 6
 *    code point   code point   code point
 * 1  7            U+0000       U+007F      0xxxxxxx
 * 2  11           U+0080       U+07FF      110xxxxx    10xxxxxx
 * 3  16           U+0800       U+FFFF      1110xxxx    10xxxxxx    10xxxxxx
 * 6  21           U+10000      U+10FFFF    11101101    1010xxxx    10xxxxxx    11101101    1011xxxx    10xxxxxx
 * for U+10000 -- U+10FFFF encodes the following (value - 0x10000)
 */

/*
 * Convert mutf8 sequence to utf16 pair and return pair: [utf16 code point, mutf8 size].
 * In case of invalid sequence return first byte of it.
 */
size_t MUtf8ToUtf16Size(const uint8_t* mutf8, size_t mutf8Len)
{
    size_t pos = 0;
    size_t res = 0;
    while (pos != mutf8Len) {
        auto [pair, nbytes] = ConvertMUtf8ToUtf16Pair(mutf8, mutf8Len - pos);
        if (nbytes == 0) {
            nbytes = 1;
        }
        res += pair > MAX_U16 ? CONST_2 : 1;
        mutf8 += nbytes; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        pos += nbytes;
    }
    return res;
}

std::pair<uint32_t, size_t> ConvertMUtf8ToUtf16Pair(const uint8_t* data, size_t maxBytes)
{
    uint8_t d0 = *data;
    if ((d0 & MASK1) == 0) {
        return { d0, 1 };
    }

    if (maxBytes < CONST_2) {
        return { d0, 1 };
    }
    uint8_t d1 = *(data + 1);
    if ((d0 & MASK2) == 0) {
        return { ((d0 & MASK_5BIT) << DATA_WIDTH) | (d1 & MASK_6BIT), 2 };
    }

    if (maxBytes < CONST_3) {
        return { d0, 1 };
    }
    uint8_t d2 = *(data + CONST_2);
    if ((d0 & MASK3) == 0) {
        return { ((d0 & MASK_4BIT) << (DATA_WIDTH * CONST_2)) | ((d1 & MASK_6BIT) << DATA_WIDTH) | (d2 & MASK_6BIT),
            CONST_3 };
    }

    if (maxBytes < CONST_4) {
        return { d0, 1 };
    }
    uint8_t d3 = *(data + CONST_3);
    uint32_t codePoint = ((d0 & MASK_4BIT) << (DATA_WIDTH * CONST_3)) | ((d1 & MASK_6BIT) << (DATA_WIDTH * CONST_2)) |
                         ((d2 & MASK_6BIT) << DATA_WIDTH) | (d3 & MASK_6BIT);

    uint32_t pair = 0;
    pair |= ((codePoint >> (PAIR_ELEMENT_WIDTH - DATA_WIDTH)) + U16_LEAD) & MASK_16BIT;
    pair <<= PAIR_ELEMENT_WIDTH;
    pair |= (codePoint & MASK_10BIT) + U16_TAIL;

    return { pair, CONST_4 };
}

size_t ConvertRegionUtf8ToUtf16(
    const uint8_t* mutf8In, uint16_t* utf16Out, size_t mutf8Len, size_t utf16Len, size_t start)
{
    size_t inPos = 0;
    size_t outPos = 0;
    while (inPos < mutf8Len) {
        auto [pair, nbytes] = ConvertMUtf8ToUtf16Pair(mutf8In, mutf8Len - inPos);
        auto [pHi, pLo] = SplitUtf16Pair(pair);

        mutf8In += nbytes; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        inPos += nbytes;
        if (start > 0) {
            start -= nbytes;
            continue;
        }

        if (pHi != 0) {
            if (outPos++ >= utf16Len - 1) { // check for place for two uint16
                --outPos;
                break;
            }
            *utf16Out++ = pHi; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        }
        if (outPos++ >= utf16Len) {
            --outPos;
            break;
        }
        *utf16Out++ = pLo; // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    return outPos;
}

bool IsUTF16HighSurrogate(uint16_t ch)
{
    return DECODE_LEAD_LOW <= ch && ch <= DECODE_LEAD_HIGH;
}

bool IsUTF16LowSurrogate(uint16_t ch)
{
    return DECODE_TRAIL_LOW <= ch && ch <= DECODE_TRAIL_HIGH;
}

size_t UTF8Length(uint32_t codePoint)
{
    if (codePoint <= UTF8_1B_MAX) {
        return UtfLength::ONE;
    }
    if (codePoint <= UTF8_2B_MAX) {
        return UtfLength::TWO;
    }
    if (codePoint <= UTF8_3B_MAX) {
        return UtfLength::THREE;
    }
    return UtfLength::FOUR;
}

// Methods for encode unicode to unicode
size_t EncodeUTF8(uint32_t codePoint, uint8_t* utf8, size_t len, size_t index)
{
    size_t size = UTF8Length(codePoint);
    if (index + size > len) {
        return 0;
    }
    for (size_t j = size - 1; j > 0; j--) {
        uint8_t cont = ((codePoint | BYTE_MARK) & BYTE_MASK);
        utf8[index + j] = cont;
        codePoint >>= UTF8_OFFSET;
    }
    utf8[index] = codePoint | FIRST_BYTE_MARK[size];
    return size;
}

uint32_t HandleAndDecodeInvalidUTF16(uint16_t const* utf16, size_t len, size_t* index)
{
    uint16_t first = utf16[*index];
    // A valid surrogate pair should always start with a High Surrogate
    if (IsUTF16LowSurrogate(first)) {
        return UTF16_REPLACEMENT_CHARACTER;
    }
    if (IsUTF16HighSurrogate(first) || (first & SURROGATE_MASK) == DECODE_LEAD_LOW) {
        if (*index == len - 1) {
            // A High surrogate not paired with another surrogate
            return UTF16_REPLACEMENT_CHARACTER;
        }
        uint16_t second = utf16[*index + 1];
        if (!IsUTF16LowSurrogate(second)) {
            // A High surrogate not followed by a low surrogate
            return UTF16_REPLACEMENT_CHARACTER;
        }
        // A valid surrogate pair, decode normally
        (*index)++;
        return ((first - DECODE_LEAD_LOW) << UTF16_OFFSET) + (second - DECODE_TRAIL_LOW) + DECODE_SECOND_FACTOR;
    }
    // A unicode not fallen into the range of representing by surrogate pair, return as it is
    return first;
}

size_t DebuggerConvertRegionUtf16ToUtf8(const uint16_t* utf16In, uint8_t* utf8Out, size_t utf16Len, size_t utf8Len,
    size_t start)
{
    if (utf16In == nullptr || utf8Out == nullptr || utf8Len == 0) {
        return 0;
    }
    size_t utf8Pos = 0;
    size_t end = start + utf16Len;
    for (size_t i = start; i < end; ++i) {
        uint32_t codePoint = HandleAndDecodeInvalidUTF16(utf16In, end, &i);
        if (codePoint == 0) {
            continue;
        }
        utf8Pos += EncodeUTF8(codePoint, utf8Out, utf8Len, utf8Pos);
    }
    return utf8Pos;
}

bool IsContinuationByte(const std::string& input, size_t startIndex, uint8_t continueCount)
{
    uint8_t i = 0;
    while (i < continueCount) {
        unsigned char utfByte = input[startIndex + i];
        if ((utfByte & MUTF8_2B_FIRST) != MUTF8_2B_SECOND) {
            return false;
        }
        i++;
    }
    return true;
}

bool IsUTF8(std::string& data)
{
    if (data.empty()) {
        return false;
    }

    size_t i = 0;
    while (i < data.size()) {
        unsigned char byte = data[i];
        if (byte <= MUTF8_1B_MAX) {
            i++;
        } else if ((byte & MUTF8_3B_FIRST) == MUTF8_2B_FIRST) {
            if (i + INDEX_ONE >= data.size()) {
                return false;
            }
            if (!IsContinuationByte(data, i + 1, 1)) {
                return false;
            }
            i += CONST_2;
        } else if ((byte & MUTF8_4B_FIRST) == MUTF8_3B_FIRST) {
            if (i + INDEX_TWO >= data.size()) {
                return false;
            }
            if (!IsContinuationByte(data, i + 1, CONST_2)) {
                return false;
            }
            i += CONST_3;
        } else if ((byte & MUTF8_4B_FIRST_MASK) == MUTF8_4B_FIRST) {
            if (i + INDEX_THREE >= data.size()) {
                return false;
            }
            if (!IsContinuationByte(data, i + 1, CONST_3)) {
                return false;
            }
            i += CONST_4;
        } else {
            return false;
        }
    }
    return true;
}

std::string RemoveInvalidUft8Bytes(const std::string& input)
{
    std::string result;
    result.reserve(input.size());
    size_t i = 0;

    while (i < input.size()) {
        unsigned char byte = input[i];
        if (byte <= MUTF8_1B_MAX) {
            result += byte;
            ++i;
        } else if ((byte & MUTF8_3B_FIRST) == MUTF8_2B_FIRST) {
            if (i + 1 < input.size() && IsContinuationByte(input, i + 1, 1)) {
                result += input.substr(i, CONST_2);
                i += CONST_2;
            } else {
                ++i;
            }
        } else if ((byte & MUTF8_4B_FIRST) == MUTF8_3B_FIRST) {
            if (i + CONST_2 < input.size() && IsContinuationByte(input, i + 1, CONST_2)) {
                result += input.substr(i, CONST_3);
                i += CONST_3;
            } else {
                ++i;
            }
        } else if ((byte & MUTF8_4B_FIRST) == MUTF8_3B_FIRST) {
            if (i + CONST_3 < input.size() && IsContinuationByte(input, i + 1, CONST_3)) {
                result += input.substr(i, CONST_4);
                i += CONST_4;
            } else {
                ++i;
            }
        } else {
            ++i;
        }
    }
    return result;
}

void ConvertIllegalStr(std::string& str)
{
    bool isRemove = false;
    if (!IsUTF8(str)) {
        TAG_LOGW(AceLogTag::ACE_LAYOUT_INSPECTOR, "the str is not valid utf-8 string");
        str = RemoveInvalidUft8Bytes(str);
        isRemove = true;
    }
    if (!isRemove || IsUTF8(str)) {
        uint8_t* buf8 =  reinterpret_cast<uint8_t*>(const_cast<char*>(str.c_str()));
        size_t utf8Len = str.size();
        auto utf16Len = MUtf8ToUtf16Size(buf8, utf8Len);
        std::unique_ptr<uint16_t[]> buf16 = std::make_unique<uint16_t[]>(utf16Len);
        auto resultLen = ConvertRegionUtf8ToUtf16(buf8, buf16.get(), utf8Len, utf16Len, 0);
        if (resultLen == utf16Len) {
            DebuggerConvertRegionUtf16ToUtf8(buf16.get(), buf8, utf16Len, utf8Len, 0);
        } else {
            TAG_LOGW(AceLogTag::ACE_LAYOUT_INSPECTOR, "resultLen is %{public}d, utf16Len is %{public}d",
                static_cast<uint16_t>(resultLen), static_cast<uint16_t>(utf16Len));
        }
    } else {
        TAG_LOGW(AceLogTag::ACE_LAYOUT_INSPECTOR, "the str is still not valid utf-8 string");
    }
}

} // namespace OHOS::Ace
