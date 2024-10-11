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

#ifndef ECMASCRIPT_REGEXP_PARSER_H
#define ECMASCRIPT_REGEXP_PARSER_H

#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include "ecmascript/js_thread.h"
#include "ecmascript/ecma_macros.h"
#include "ecmascript/mem/chunk.h"
#include "ecmascript/mem/c_containers.h"
#include "ecmascript/mem/c_string.h"
#include "ecmascript/mem/dyn_chunk.h"
#include "ecmascript/regexp/regexp_opcode.h"
#include "unicode/stringpiece.h"
#include "unicode/uchar.h"
#include "unicode/utf16.h"
#include "unicode/utf8.h"
#include "unicode/utypes.h"
#include "unicode/udata.h"
#include "unicode/uniset.h"

namespace panda::ecmascript {
class RegExpParser {
public:
    static constexpr auto FLAG_GLOBAL = (1U << 0U);
    static constexpr auto FLAG_IGNORECASE = (1U << 1U);
    static constexpr auto FLAG_MULTILINE = (1U << 2U);
    static constexpr auto FLAG_DOTALL = (1U << 3U);
    static constexpr auto FLAG_UTF16 = (1U << 4U);
    static constexpr auto FLAG_STICKY = (1U << 5U);
    static constexpr auto FLAG_HASINDICES = (1U << 6U);
    static constexpr uint32_t FLAG_NUM = 7;
    static const uint32_t KEY_EOF = UINT32_MAX;
    static constexpr int CLASS_RANGE_BASE = 0x40000000;
    static constexpr uint32_t NUM_CAPTURE__OFFSET = 4;
    static constexpr uint32_t NUM_STACK_OFFSET = 8;
    static constexpr uint32_t OCTAL_VALUE = 8;
    static constexpr uint32_t OCTAL_VALUE_RANGE = 32;
    static constexpr uint32_t HEX_VALUE = 16;
    static constexpr uint32_t DECIMAL_DIGITS_ADVANCE = 10;
    static constexpr uint32_t FLAGS_OFFSET = 12;
    static constexpr uint32_t PREFILTER_OFFSET = 16;
    static constexpr uint32_t OP_START_OFFSET = 20;
    static constexpr uint32_t UNICODE_HEX_VALUE = 4;
    static constexpr uint32_t UNICODE_HEX_ADVANCE = 2;
    static constexpr uint32_t CAPTURE_CONUT_ADVANCE = 3;
    static constexpr uint32_t UTF8_CHAR_LEN_MAX = 6;
    static constexpr size_t SPARSE_HEAD_OFFSET = 3;
    static constexpr size_t SPARSE_OFF_OFFSET = 2;
    static constexpr size_t SPARSE_MAX_OFFSET = 6;
    static int Canonicalize(int c, bool isUnicode);
    
    explicit RegExpParser(JSThread *thread, Chunk *chunk)
        : thread_(thread),
          base_(nullptr),
          pc_(nullptr),
          end_(nullptr),
          flags_(0),
          c0_(KEY_EOF),
          captureCount_(0),
          stackCount_(0),
          isError_(false),
          isEmpty_(false),
          buffer_(chunk),
          groupNames_(chunk)
    {
    }

    ~RegExpParser()
    {
        Clear();
    }

    NO_COPY_SEMANTIC(RegExpParser);
    NO_MOVE_SEMANTIC(RegExpParser);

    inline void Init(char *source, size_t length, uint32_t flags)
    {
        pc_ = reinterpret_cast<uint8_t *>(source);
        base_ = pc_;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        end_ = reinterpret_cast<uint8_t *>(source) + length - 1;
        flags_ = flags;
    }

    void Parse();
    void ParseDisjunction(bool isBackward);
    void ParseAlternative(bool isBackward);
    bool ParseAssertionCapture(int *captureIndex, bool isBackward);
    void ParseQuantifier(size_t atomBcStart, int captureStart, int captureEnd);
    int ParseDecimalDigits();
    int ParseAtomEscape(bool isBackward);
    int ParseCharacterEscape();
    bool ParseGroupSpecifier(const uint8_t **pp, CString &name);
    int ParseCaptureCount(const char *groupName);
    bool ParseClassRanges(RangeSet *result);
    void ParseNonemptyClassRangesNoDash(DynChunk *buffer);
    uint32_t ParseClassAtom(RangeSet *atom);
    int ParseClassEscape(RangeSet *atom);
    void ParseError(const char *errorMessage);
    bool ParseUnicodePropertyValueCharacters(CString &categoryName, CString &valueName);
    int FindGroupName(const CString &name);
    uint32_t ParseOctalLiteral();
    bool ParseHexEscape(int length, uint32_t *value);
    bool ParseUnlimitedLengthHexNumber(uint32_t maxValue, uint32_t *value);
    bool ParseUnicodeEscape(uint32_t *value);
    bool ParserIntervalQuantifier(int *pmin, int *pmax);
    bool HasNamedCaptures();
    int ParseEscape(const uint8_t **pp, int isUtf16);
    int RecountCaptures();
    int IsIdentFirst(uint32_t c);
    bool NeedIntersection(uint32_t c);
    void DoParserStackOverflowCheck(const char *errorMessage);
    bool MatchUnicodeProperty(UProperty property, const char *propertyName, RangeSet *atom, bool negate);
    bool IsExactPropertyValueAlis(const char *valueName, UProperty property, int32_t propertyValue);
    bool ParseUnicodePropertyClassRange(CString &propertyName, CString &valueName, RangeSet *atom, bool negate);
    bool GetUnicodePropertyName(CString &propertyName);
    bool GetUnicodePropertyValueName(CString &valueName);
    bool IsExactPropertyAlias(const char *propertyName, UProperty property);
    bool MatchSepcialUnicodeProperty(CString &name, bool negate, RangeSet *atom);
    bool IsSupportedBinaryProperty(UProperty property);
    bool IsBinaryPropertyOfStrings(UProperty property);
    inline CVector<CString> GetGroupNames() const
    {
        return newGroupNames_;
    }

    inline size_t GetGroupNamesSize() const
    {
        return groupNames_.size_;
    }

    inline bool IsError() const
    {
        return isError_;
    }

    inline uint8_t *GetOriginBuffer() const
    {
        return buffer_.buf_;
    }

    inline size_t GetOriginBufferSize() const
    {
        return buffer_.size_;
    }

    inline CString GetErrorMsg() const
    {
        if (isError_) {
            return CString(errorMsg_);
        }
        return CString("");
    }

    inline bool IsGlobal() const
    {
        return (flags_ & FLAG_GLOBAL) != 0;
    }

    inline bool IsIgnoreCase() const
    {
        return (flags_ & FLAG_IGNORECASE) != 0;
    }

    inline bool IsMultiline() const
    {
        return (flags_ & FLAG_MULTILINE) != 0;
    }

    inline bool IsDotAll() const
    {
        return (flags_ & FLAG_DOTALL) != 0;
    }

    inline bool IsUtf16() const
    {
        return (flags_ & FLAG_UTF16) != 0;
    }

    inline bool IsStick() const
    {
        return (flags_ & FLAG_STICKY) != 0;
    }

    inline bool IsUnicodePropertyValueCharacter(char c) const
    {
        if (c >= 'a' && c <= 'z') {
            return true;
        }
        if (c >= 'A' && c <= 'Z') {
            return true;
        }
        if (c >= '0' && c <= '9') {
            return true;
        }
        return (c == '_');
    }

    inline static int GetcurrentCharNext(int c)
    {
        int cur = c;
        c = u_tolower(static_cast<UChar32>(c));
        if (c == cur) {
            c = u_toupper(static_cast<UChar32>(c));
        }
        if (((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) &&
            !((cur >= 'A' && cur <= 'Z') || (cur >= 'a' && cur <= 'z'))) {
            c = cur;
        }
        return c;
    }
    inline static void ProcessIntersection(RangeSet *result)
    {
        RangeSet cr;
        RangeSet cr1;
        const uint32_t MINLOWERCHAR = 'a';
        const uint32_t MAXLOWERCHAR = 'z' + 1;
        const uint32_t MINUPPERCHAR = 'A';
        const uint32_t MAXUPPERCHAR = 'Z' + 1;
        // Range values for a and z + 1
        cr.Insert(MINLOWERCHAR, MAXLOWERCHAR);
        // Range values for A and Z + 1
        cr.Insert(MINUPPERCHAR, MAXUPPERCHAR);
        result->Inter(cr1, cr);
        result->Insert(cr1);
    }
private:
    friend class RegExpExecutor;
    static constexpr int TMP_BUF_SIZE = 128;
    void Clear()
    {
        base_ = nullptr;
        pc_ = nullptr;
        end_ = nullptr;
        c0_ = KEY_EOF;
        isError_ = false;
        isEmpty_ = false;
    }

    void Advance()
    {
        if (pc_ <= end_) {
            DoParserStackOverflowCheck("Advance stack overflow!");
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            c0_ = *pc_++;
        } else {
            c0_ = KEY_EOF;
        }
    }

    void Advance(int offset)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        pc_ += offset - 1;
        Advance();
    }

    void Prev()
    {
        if (pc_ >= base_) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            c0_ = *pc_--;
        } else {
            c0_ = KEY_EOF;
        }
    }

    void SetIsError()
    {
        isError_ = true;
    }

    void PrintF(const char *fmt, ...);
    JSThread *thread_;
    uint8_t *base_;
    uint8_t *pc_;
    uint8_t *end_;
    uint32_t flags_;
    uint32_t c0_;
    int captureCount_;
    int stackCount_;
    bool isError_;
    bool isEmpty_;
    char errorMsg_[TMP_BUF_SIZE] = {0};  // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    int hasNamedCaptures_ = -1;
    int totalCaptureCount_ = -1;
    DynChunk buffer_;
    DynChunk groupNames_;
    CVector<CString> newGroupNames_;
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_REGEXP_PARSER_H
