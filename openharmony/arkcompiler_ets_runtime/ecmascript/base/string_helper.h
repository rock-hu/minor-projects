/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_BASE_STRING_HELP_H
#define ECMASCRIPT_BASE_STRING_HELP_H

#include <algorithm>
#include <codecvt>
#include <locale>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include "ecmascript/base/utf_helper.h"
#include "ecmascript/mem/c_containers.h"
#include "ecmascript/mem/c_string.h"

#include "securec.h"
#include "unicode/unistr.h"

namespace panda::ecmascript::base {
// White Space Code Points and Line Terminators Code Point
// NOLINTNEXTLINE(modernize-avoid-c-arrays)
static constexpr uint16_t SPACE_OR_LINE_TERMINAL[] = {
    0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x0020, 0x00A0, 0x1680, 0x2000, 0x2001, 0x2002, 0x2003, 0x2004,
    0x2005, 0x2006, 0x2007, 0x2008, 0x2009, 0x200A, 0x2028, 0x2029, 0x202F, 0x205F, 0x3000, 0xFEFF,
};
static constexpr int UICODE_FROM_UTF8[] = {
    0x80, 0xc0, 0xdf, 0xe0, 0xef, 0xf0, 0xf7, 0xf8, 0xfb, 0xfc, 0xfd,
};
static constexpr int UTF8_MIN_CODE[] = {
    0x80, 0x800, 0x10000, 0x00200000, 0x04000000,
};
static constexpr char UTF8_FIRST_CODE[] = {
    0x1f, 0xf, 0x7, 0x3, 0x1,
};
class StringHelper {
public:
    static constexpr int INVALID_UNICODE_FROM_UTF8 = -1;

    static inline CString ReplaceAll(CString str, const CString &oldValue,
                                     const CString &newValue)
    {
        if (oldValue.empty() || oldValue == newValue) {
            return str;
        }
        CString::size_type pos(0);
        while ((pos = str.find(oldValue, pos)) != CString::npos) {
            str.replace(pos, oldValue.length(), newValue);
            pos += newValue.length();
        }
        return str;
    }

    static inline CString Replace(CString str, const CString &oldValue,
                                  const CString &newValue)
    {
        if (oldValue.empty() || oldValue == newValue) {
            return str;
        }
        CString::size_type pos(0);
        if ((pos = str.find(oldValue, pos)) != CString::npos) {
            str.replace(pos, oldValue.length(), newValue);
        }
        return str;
    }

    static inline std::u16string Utf16ToU16String(const uint16_t *utf16Data, uint32_t dataLen)
    {
        auto *char16tData = reinterpret_cast<const char16_t *>(utf16Data);
        std::u16string u16str(char16tData, dataLen);
        return u16str;
    }

    static inline std::string Utf8ToString(const uint8_t *utf8Data, uint32_t dataLen)
    {
        auto *charData = reinterpret_cast<const char *>(utf8Data);
        std::string str(charData, dataLen);
        return str;
    }

    static inline CString Utf8ToCString(const uint8_t *utf8Data, uint32_t dataLen)
    {
        auto *charData = reinterpret_cast<const char *>(utf8Data);
        return { charData, dataLen };
    }

    static inline std::u16string Utf8ToU16String(const uint8_t *utf8Data, uint32_t dataLen)
    {
        auto *charData = reinterpret_cast<const char *>(utf8Data);
        std::string str(charData, dataLen);
        std::u16string u16str = std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
        return u16str;
    }

    static inline std::string WstringToString(const std::wstring &wstr)
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.to_bytes(wstr);
    }

    static inline std::wstring StringToWstring(const std::string &str)
    {
        return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{}.from_bytes(str);
    }

    static inline std::string U16stringToString(const std::u16string &u16str)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16str);
    }

    static inline std::u16string StringToU16string(const std::string &str)
    {
        return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(str);
    }

    static inline size_t Find(const std::string &thisStr, const std::string &searchStr, int32_t pos)
    {
        size_t idx = thisStr.find(searchStr, pos);
        return idx;
    }

    static inline size_t Find(const std::u16string &thisStr, const std::u16string &searchStr, int32_t pos)
    {
        size_t idx = thisStr.find(searchStr, pos);
        return idx;
    }

    static inline size_t RFind(const std::u16string &thisStr, const std::u16string &searchStr, int32_t pos)
    {
        size_t idx = thisStr.rfind(searchStr, pos);
        return idx;
    }

    static inline std::string ToUpper(const std::u16string &str)
    {
        std::u16string tmpStr = str;
        const char16_t *constChar16tData = tmpStr.data();
        icu::UnicodeString uString(constChar16tData);
        icu::UnicodeString up = uString.toUpper();
        std::string res;
        up.toUTF8String(res);
        return res;
    }

    static inline std::string ToLocaleUpper(const std::u16string &str, const icu::Locale &locale)
    {
        std::u16string tmpStr = str;
        const char16_t *constChar16tData = tmpStr.data();
        icu::UnicodeString uString(constChar16tData);
        icu::UnicodeString up = uString.toUpper(locale);
        std::string res;
        up.toUTF8String(res);
        return res;
    }

    static inline std::string ToLower(const std::u16string &str)
    {
        const char16_t *constChar16tData = str.data();
        icu::UnicodeString uString(constChar16tData, str.length());
        std::string res;
        uString.toLower().toUTF8String(res);
        return res;
    }

    static inline std::string ToLocaleLower(const std::u16string &str, const icu::Locale &locale)
    {
        std::u16string tmpStr = str;
        const char16_t *constChar16tData = tmpStr.data();
        icu::UnicodeString uString(constChar16tData);
        icu::UnicodeString low = uString.toLower(locale);
        std::string res;
        low.toUTF8String(res);
        return res;
    }

    static inline size_t FindFromU16ToUpper(const std::u16string &thisStr, uint16_t *u16Data)
    {
        std::u16string tmpStr = Utf16ToU16String(u16Data, 1);
        const char16_t *constChar16tData = tmpStr.data();
        icu::UnicodeString uString(constChar16tData);
        icu::UnicodeString up = uString.toUpper();
        std::string res;
        up.toUTF8String(res);
        std::u16string searchStr = StringToU16string(res);
        size_t idx = Find(thisStr, searchStr, 0);
        return idx;
    }

    static inline size_t FindFromU8ToUpper(const std::string &thisStr, uint8_t *u8Data)
    {
        std::string tmpStr = Utf8ToString(u8Data, 1);
        std::transform(tmpStr.begin(), tmpStr.end(), tmpStr.begin(), [](unsigned char c) { return std::toupper(c); });
        size_t idx = Find(thisStr, tmpStr, 0);
        return idx;
    }

    static int UnicodeFromUtf8(const uint8_t *p, int maxLen, const uint8_t **pp)
    {
        int c = *p++;
        if (c < UICODE_FROM_UTF8[0]) {
            *pp = p;
            return c;
        }
        int l = 0;
        if (c >= UICODE_FROM_UTF8[1] && c <= UICODE_FROM_UTF8[2]) { // 1 - 2: 0000 0080 - 0000 07FF
            l = 1; // 1: 0000 0080 - 0000 07FF Unicode
        } else if (c >= UICODE_FROM_UTF8[3] && c <= UICODE_FROM_UTF8[4]) { // 3 - 4: 0000 0800 - 0000 FFFF
            l = 2; // 2: 0000 0800 - 0000 FFFF Unicode
        } else if (c >= UICODE_FROM_UTF8[5] && c <= UICODE_FROM_UTF8[6]) { // 5 - 6: 0001 0000 - 0010 FFFF
            l = 3; // 3: 0001 0000 - 0010 FFFF Unicode
        } else if (c >= UICODE_FROM_UTF8[7] && c <= UICODE_FROM_UTF8[8]) { // 7 - 8: 0020 0000 - 03FF FFFF
            l = 4; // 4: 0020 0000 - 03FF FFFF Unicode
        } else if (c == UICODE_FROM_UTF8[9] || c == UICODE_FROM_UTF8[10]) { // 9 - 10: 0400 0000 - 7FFF FFFF
            l = 5; // 5: 0400 0000 - 7FFF FFFF Unicode
        } else {
            return INVALID_UNICODE_FROM_UTF8;
        }
        /* check that we have enough characters */
        if ((l + 1) > maxLen) {
            return INVALID_UNICODE_FROM_UTF8;
        }
        return FromUtf8(c, l, p, pp);
    }

    static int FromUtf8(int c, int l, const uint8_t *p, const uint8_t **pp)
    {
        uint32_t b;
        c &= UTF8_FIRST_CODE[l - 1];
        for (int i = 0; i < l; i++) {
            b = *p++;
            if (b < utf_helper::UTF8_2B_SECOND || b >= utf_helper::UTF8_2B_FIRST) {
                return INVALID_UNICODE_FROM_UTF8;
            }
            c = (c << 6) | (b & utf_helper::UTF8_2B_THIRD); // 6: Maximum Unicode range
        }
        if (c < UTF8_MIN_CODE[l - 1]) {
            return INVALID_UNICODE_FROM_UTF8;
        }
        *pp = p;
        return c;
    }

    static inline void InplaceAppend(std::u16string &str1, const std::u16string &str2)
    {
        str1.append(str2);
    }

    static inline std::u16string Append(const std::u16string &str1, const std::u16string &str2)
    {
        std::u16string tmpStr = str1;
        return tmpStr.append(str2);
    }

    static inline uint32_t Utf8ToU32String(const std::vector<uint8_t> &data)
    {
        std::string str(data.begin(), data.end());
        std::u32string u32str = std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t>{}.from_bytes(str);
        auto u32data = reinterpret_cast<uint32_t *>(u32str.data());
        return *u32data;
    }

    static inline std::string Utf32ToString(uint32_t u32Data)
    {
        UChar32 charData = static_cast<int32_t>(u32Data);
        icu::UnicodeString uString(charData);
        std::string res;
        uString.toUTF8String(res);
        return res;
    }

    static inline std::string GetSpecifiedLine(const std::string &srcStr, int lineNumber)
    {
        if (lineNumber < 1) {
            return "";
        }
        bool escape = true;
        if (srcStr.find('\n') == std::string::npos) {
            escape = false;
        }
        size_t prePos = 0;
        int findPrePos = lineNumber - 1;
        for (int i = 0; i < findPrePos; i++) {
            if (escape) {
                prePos = srcStr.find('\n', prePos);
                if (prePos == std::string::npos) {
                    return "";
                }
                prePos += 1;
            } else {
                prePos = srcStr.find("\\n", prePos);
                if (prePos == std::string::npos) {
                    return "";
                }
                prePos += 2; // 2 : add the two characters found to start searching again
            }
        }
        size_t findEndPos = 0;
        if (escape) {
            findEndPos = srcStr.find('\n', prePos);
        } else {
            findEndPos = srcStr.find("\\n", prePos);
        }
        if (findEndPos == std::string::npos) {
            return srcStr.substr(prePos, srcStr.length() - prePos);
        }
        ASSERT(findEndPos > prePos);
        return srcStr.substr(prePos, findEndPos - prePos);
    }

    static inline bool IsNonspace(uint16_t c)
    {
        uint32_t len = sizeof(SPACE_OR_LINE_TERMINAL) / sizeof(SPACE_OR_LINE_TERMINAL[0]);
        for (uint32_t i = 0; i < len; i++) {
            if (c == SPACE_OR_LINE_TERMINAL[i]) {
                return true;
            }
            if (c < SPACE_OR_LINE_TERMINAL[i]) {
                return false;
            }
        }
        return false;
    }

    template<typename T>
    static inline uint32_t GetStart(Span<T> &data, uint32_t length)
    {
        uint32_t start = 0;
        while (start < length && IsNonspace(data[start])) {
            start++;
        }
        return start;
    }

    template<typename T>
    static inline int32_t GetEnd(Span<T> &data, int32_t start, uint32_t length)
    {
        if (length == 0U) {
            return 0;
        }
        int32_t end = static_cast<int32_t>(length - 1);
        while (end >= start && IsNonspace(data[end])) {
            end--;
        }
        return end;
    }

    static bool Utf8CharInRange(uint8_t value, char start, char end)
    {
        ASSERT(start <= end);
        return (value >= static_cast<uint8_t>(start)) && (value <= static_cast<uint8_t>(end));
    }

    static inline std::string Vformat(const char *fmt, va_list args)
    {
        static constexpr size_t SIZE = 1024;

        std::string result;
        result.resize(SIZE);

        bool is_truncated = true;
        while (is_truncated) {
            va_list copy_args;
            va_copy(copy_args, args);
            int r = vsnprintf_truncated_s(result.data(), result.size() + 1, fmt, copy_args);
            va_end(copy_args);

            if (r < 0) {
                return "";
            }

            is_truncated = static_cast<size_t>(r) == result.size();
            result.resize(result.size() * 2U);
        }

        result.erase(std::find(result.begin(), result.end(), '\0'), result.end());

        return result;
    }

    static std::vector<std::string> SplitString(const std::string &str, const std::string &delimiter)
    {
        std::size_t strIndex = 0;
        std::vector<std::string> value;
        std::size_t pos = str.find_first_of(delimiter, strIndex);
        while ((pos < str.size()) && (pos > strIndex)) {
            std::string subStr = str.substr(strIndex, pos - strIndex);
            value.push_back(std::move(subStr));
            strIndex = pos;
            strIndex = str.find_first_not_of(delimiter, strIndex);
            pos = str.find_first_of(delimiter, strIndex);
        }
        if (pos > strIndex) {
            std::string subStr = str.substr(strIndex, pos - strIndex);
            if (!subStr.empty()) {
                value.push_back(std::move(subStr));
            }
        }
        return value;
    }

    static bool EndsWith(const std::string &str, const std::string &suffix)
    {
        if (str.length() < suffix.length()) {
            return false;
        }
        std::string subStr = str.substr(str.length() - suffix.length(), str.length());
        return subStr == suffix;
    }

    static bool StrToUInt32(const char *content, uint32_t *result)
    {
        const int DEC = 10;
        char *endPtr = nullptr;
        *result = std::strtoul(content, &endPtr, DEC);
        if (endPtr == content || *endPtr != '\0') {
            return false;
        }
        return true;
    }

    static bool StringStartWith(const CString& str, const CString& startStr)
    {
        size_t startStrLen = startStr.length();
        return ((str.length() >= startStrLen) && (str.compare(0, startStrLen, startStr) == 0));
    }

    static bool StringEndWith(const CString& str, const CString& endStr)
    {
        size_t endStrLen = endStr.length();
        size_t len = str.length();
        return ((len >= endStrLen) && (str.compare(len - endStrLen, endStrLen, endStr) == 0));
    }

    static void SplitString(const CString& str, CVector<CString>& out, size_t startPos, size_t times = 0, char c = '/')
    {
        size_t left = startPos;
        size_t pos = 0;
        size_t index = 0;
        while ((pos = str.find(c, left)) != CString::npos) {
            if (times != 0 && index >= times) {
                return;
            }
            out.emplace_back(str.substr(left, pos - left));
            left = pos + 1;
            index++;
        }

        if ((times == 0 || index < times) && left < str.length()) {
            out.emplace_back(str.substr(left));
        }
    }

    static CString JoinString(const CVector<CString>& strs, size_t startIndex, size_t endIndex, char c = '/')
    {
        CString out;
        for (size_t index = startIndex; index < strs.size() && index <= endIndex; ++index) {
            if (!strs[index].empty()) {
                out.append(strs[index]) += c;
            }
        }
        if (!out.empty()) {
            out.pop_back();
        }
        return out;
    }
};
}  // namespace panda::ecmascript::base
#endif  // ECMASCRIPT_BASE_STRING_HELP_H
