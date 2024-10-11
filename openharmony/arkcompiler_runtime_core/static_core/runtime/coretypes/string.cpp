/**
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <cstddef>
#include <cstring>
#include <limits>

#include "libpandabase/utils/utf.h"
#include "libpandabase/utils/hash.h"
#include "libpandabase/utils/span.h"
#include "runtime/arch/memory_helpers.h"
#include "runtime/include/coretypes/array.h"
#include "runtime/include/coretypes/string-inl.h"
#include "runtime/include/runtime.h"
#include "runtime/handle_base-inl.h"
#include "runtime/include/panda_vm.h"

namespace ark::coretypes {

bool String::compressedStringsEnabled_ = true;

/* static */
String *String::CreateFromString(String *str, const LanguageContext &ctx, PandaVM *vm)
{
    ASSERT(str != nullptr);
    // allocator may trig gc and move str, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> strHandle(thread, str);
    auto string = AllocStringObject(strHandle->GetLength(), !strHandle->IsUtf16(), ctx, vm);
    if (string == nullptr) {
        return nullptr;
    }

    // retrive str after gc
    str = strHandle.GetPtr();
    string->hashcode_ = str->hashcode_;

    uint32_t length = str->GetLength();
    // After memcpy we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (str->IsUtf16()) {
        memcpy_s(string->GetDataUtf16(), ComputeDataSizeUtf16(string->GetLength()), str->GetDataUtf16(),
                 ComputeDataSizeUtf16(length));
    } else {
        memcpy_s(string->GetDataMUtf8(), string->GetLength(), str->GetDataMUtf8(), length);
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();

    return string;
}

/* static */
String *String::CreateFromMUtf8(const uint8_t *mutf8Data, size_t mutf8Length, uint32_t utf16Length,
                                bool canBeCompressed, const LanguageContext &ctx, PandaVM *vm, bool movable,
                                bool pinned)
{
    auto string = AllocStringObject(utf16Length, canBeCompressed, ctx, vm, movable, pinned);
    if (string == nullptr) {
        return nullptr;
    }

    ASSERT(string->hashcode_ == 0);
    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (canBeCompressed) {
        memcpy_s(string->GetDataMUtf8(), string->GetLength(), mutf8Data, utf16Length);
    } else {
        utf::ConvertMUtf8ToUtf16(mutf8Data, mutf8Length, string->GetDataUtf16());
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

/* static */
String *String::CreateFromMUtf8(const uint8_t *mutf8Data, uint32_t utf16Length, const LanguageContext &ctx, PandaVM *vm,
                                bool movable, bool pinned)
{
    bool canBeCompressed = CanBeCompressedMUtf8(mutf8Data);
    auto mutf8Length = utf::Mutf8Size(mutf8Data);
    ASSERT(utf16Length == utf::MUtf8ToUtf16Size(mutf8Data, mutf8Length));
    return CreateFromMUtf8(mutf8Data, mutf8Length, utf16Length, canBeCompressed, ctx, vm, movable, pinned);
}

/* static */
String *String::CreateFromMUtf8(const uint8_t *mutf8Data, uint32_t utf16Length, bool canBeCompressed,
                                const LanguageContext &ctx, PandaVM *vm, bool movable, bool pinned)
{
    auto mutf8Length = utf::Mutf8Size(mutf8Data);
    ASSERT(utf16Length == utf::MUtf8ToUtf16Size(mutf8Data, mutf8Length));
    ASSERT(canBeCompressed == CanBeCompressedMUtf8(mutf8Data));
    return CreateFromMUtf8(mutf8Data, mutf8Length, utf16Length, canBeCompressed, ctx, vm, movable, pinned);
}

/* static */
String *String::CreateFromMUtf8(const uint8_t *mutf8Data, const LanguageContext &ctx, PandaVM *vm, bool movable,
                                bool pinned)
{
    size_t mutf8Length = utf::Mutf8Size(mutf8Data);
    size_t utf16Length = utf::MUtf8ToUtf16Size(mutf8Data, mutf8Length);
    bool canBeCompressed = CanBeCompressedMUtf8(mutf8Data);
    return CreateFromMUtf8(mutf8Data, mutf8Length, utf16Length, canBeCompressed, ctx, vm, movable, pinned);
}

/* static */
String *String::CreateFromMUtf8(const uint8_t *mutf8Data, uint32_t mutf8Length, uint32_t utf16Length,
                                const LanguageContext &ctx, PandaVM *vm, bool movable, bool pinned)
{
    ASSERT(utf16Length == utf::MUtf8ToUtf16Size(mutf8Data, mutf8Length));
    auto canBeCompressed = CanBeCompressedMUtf8(mutf8Data, mutf8Length);
    return CreateFromMUtf8(mutf8Data, mutf8Length, utf16Length, canBeCompressed, ctx, vm, movable, pinned);
}

/* static */
String *String::CreateFromUtf8(const uint8_t *utf8Data, uint32_t utf8Length, const LanguageContext &ctx, PandaVM *vm,
                               bool movable, bool pinned)
{
    coretypes::String *s = nullptr;
    auto utf16Length = utf::Utf8ToUtf16Size(utf8Data, utf8Length);
    if (CanBeCompressedMUtf8(utf8Data, utf8Length)) {
        // ascii string have equal representation in utf8 and mutf8 formats
        s = coretypes::String::CreateFromMUtf8(utf8Data, utf8Length, utf16Length, true, ctx, vm, movable, pinned);
    } else {
        PandaVector<uint16_t> tmpBuffer(utf16Length);
        [[maybe_unused]] auto len =
            utf::ConvertRegionUtf8ToUtf16(utf8Data, tmpBuffer.data(), utf8Length, utf16Length, 0);
        ASSERT(len == utf16Length);
        s = coretypes::String::CreateFromUtf16(tmpBuffer.data(), utf16Length, ctx, vm, movable, pinned);
    }
    return s;
}

/* static */
String *String::CreateFromUtf16(const uint16_t *utf16Data, uint32_t utf16Length, const LanguageContext &ctx,
                                PandaVM *vm, bool movable, bool pinned)
{
    bool canBeCompressed = CanBeCompressed(utf16Data, utf16Length);
    auto string = AllocStringObject(utf16Length, canBeCompressed, ctx, vm, movable, pinned);
    if (string == nullptr) {
        return nullptr;
    }

    ASSERT(string->hashcode_ == 0);
    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (canBeCompressed) {
        CopyUtf16AsMUtf8(utf16Data, string->GetDataMUtf8(), utf16Length);
    } else {
        memcpy_s(string->GetDataUtf16(), ComputeDataSizeUtf16(string->GetLength()), utf16Data, utf16Length << 1UL);
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

/* static */
String *String::CreateEmptyString(const LanguageContext &ctx, PandaVM *vm)
{
    uint16_t data = 0;
    return CreateFromUtf16(&data, 0, ctx, vm);
}

/* static */
void String::CopyUtf16AsMUtf8(const uint16_t *utf16From, uint8_t *mutf8To, uint32_t utf16Length)
{
    Span<const uint16_t> from(utf16From, utf16Length);
    Span<uint8_t> to(mutf8To, utf16Length);
    for (uint32_t i = 0; i < utf16Length; i++) {
        to[i] = from[i];
    }
}

// static
String *String::CreateNewStringFromChars(uint32_t offset, uint32_t length, Array *chararray, const LanguageContext &ctx,
                                         PandaVM *vm)
{
    ASSERT(chararray != nullptr);
    // allocator may trig gc and move array, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<Array> arrayHandle(thread, chararray);

    // There is a potential data race between read of src in CanBeCompressed and write of destination buf
    // in CopyDataRegionUtf16. The src is a cast from chararray comming from managed object.
    // Hence the race is reported on managed object, which has a synchronization on a high level.
    // TSAN does not see such synchronization, thus we ignore such races here.
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    // NOLINTNEXTLINE(readability-identifier-naming)
    const uint16_t *src = reinterpret_cast<uint16_t *>(ToUintPtr<uint32_t>(chararray->GetData()) + (offset << 1UL));
    bool canBeCompressed = CanBeCompressed(src, length);
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    auto string = AllocStringObject(length, canBeCompressed, ctx, vm);
    if (string == nullptr) {
        return nullptr;
    }

    // retrieve src since gc may move it
    src = reinterpret_cast<uint16_t *>(ToUintPtr<uint32_t>(arrayHandle->GetData()) + (offset << 1UL));
    ASSERT(string->hashcode_ == 0);
    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (canBeCompressed) {
        CopyUtf16AsMUtf8(src, string->GetDataMUtf8(), length);
    } else {
        memcpy_s(string->GetDataUtf16(), ComputeDataSizeUtf16(string->GetLength()), src, length << 1UL);
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

// static
String *String::CreateNewStringFromBytes(uint32_t offset, uint32_t length, uint32_t highByte, Array *bytearray,
                                         const LanguageContext &ctx, PandaVM *vm)
{
    ASSERT(length != 0);
    ASSERT(bytearray != nullptr);
    // allocator may trig gc and move array, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<Array> arrayHandle(thread, bytearray);

    constexpr size_t BYTE_MASK = 0xFF;

    // NOLINTNEXTLINE(readability-identifier-naming)
    const uint8_t *src = reinterpret_cast<uint8_t *>(ToUintPtr<uint32_t>(bytearray->GetData()) + offset);
    highByte &= BYTE_MASK;
    bool canBeCompressed = CanBeCompressedMUtf8(src, length) && (highByte == 0);
    auto string = AllocStringObject(length, canBeCompressed, ctx, vm);
    if (string == nullptr) {
        return nullptr;
    }

    // retrieve src since gc may move it
    src = reinterpret_cast<uint8_t *>(ToUintPtr<uint32_t>(arrayHandle->GetData()) + offset);
    ASSERT(string->hashcode_ == 0);
    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (canBeCompressed) {
        Span<const uint8_t> from(src, length);
        Span<uint8_t> to(string->GetDataMUtf8(), length);
        for (uint32_t i = 0; i < length; ++i) {
            to[i] = (from[i] & BYTE_MASK);
        }
    } else {
        Span<const uint8_t> from(src, length);
        Span<uint16_t> to(string->GetDataUtf16(), length);
        for (uint32_t i = 0; i < length; ++i) {
            to[i] = (highByte << 8U) + (from[i] & BYTE_MASK);
        }
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();

    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

template <typename T1, typename T2>
int32_t CompareStringSpan(Span<T1> &lhsSp, Span<T2> &rhsSp, int32_t count)
{
    for (int32_t i = 0; i < count; ++i) {
        int32_t charDiff = static_cast<int32_t>(lhsSp[i]) - static_cast<int32_t>(rhsSp[i]);
        if (charDiff != 0) {
            return charDiff;
        }
    }
    return 0;
}

template <typename T>
int32_t CompareBytesBlock(T *lstrPt, T *rstrPt, int32_t minCount)
{
    constexpr int32_t BYTES_CNT = sizeof(size_t);
    static_assert(BYTES_CNT >= sizeof(T));
    static_assert(BYTES_CNT % sizeof(T) == 0);
    int32_t totalBytes = minCount * sizeof(T);
    auto lhsBlock = reinterpret_cast<size_t *>(lstrPt);
    auto rhsBlock = reinterpret_cast<size_t *>(rstrPt);
    int32_t curBytePos = 0;
    while (curBytePos + BYTES_CNT <= totalBytes) {
        if (*lhsBlock == *rhsBlock) {
            curBytePos += BYTES_CNT;
            lhsBlock++;
            rhsBlock++;
        } else {
            break;
        }
    }
    int32_t curElementPos = curBytePos / sizeof(T);
    for (int32_t i = curElementPos; i < minCount; ++i) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        int32_t charDiff = static_cast<int32_t>(lstrPt[i]) - static_cast<int32_t>(rstrPt[i]);
        if (charDiff != 0) {
            return charDiff;
        }
    }

    return 0;
}

int32_t String::Compare(String *rstr)
{
    String *lstr = this;
    if (lstr == rstr) {
        return 0;
    }
    ASSERT(lstr->GetLength() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
    ASSERT(rstr->GetLength() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
    auto lstrLeng = static_cast<int32_t>(lstr->GetLength());
    auto rstrLeng = static_cast<int32_t>(rstr->GetLength());
    int32_t lengRet = lstrLeng - rstrLeng;
    int32_t minCount = (lengRet < 0) ? lstrLeng : rstrLeng;
    bool lstrIsUtf16 = lstr->IsUtf16();
    bool rstrIsUtf16 = rstr->IsUtf16();
    if (!lstrIsUtf16 && !rstrIsUtf16) {
        int32_t charDiff = CompareBytesBlock(lstr->GetDataMUtf8(), rstr->GetDataMUtf8(), minCount);
        if (charDiff != 0) {
            return charDiff;
        }
    } else if (!lstrIsUtf16) {
        Span<uint8_t> lhsSp(lstr->GetDataMUtf8(), lstrLeng);
        Span<uint16_t> rhsSp(rstr->GetDataUtf16(), rstrLeng);
        int32_t charDiff = CompareStringSpan(lhsSp, rhsSp, minCount);
        if (charDiff != 0) {
            return charDiff;
        }
    } else if (!rstrIsUtf16) {
        Span<uint16_t> lhsSp(lstr->GetDataUtf16(), lstrLeng);
        Span<uint8_t> rhsSp(rstr->GetDataMUtf8(), rstrLeng);
        int32_t charDiff = CompareStringSpan(lhsSp, rhsSp, minCount);
        if (charDiff != 0) {
            return charDiff;
        }
    } else {
        int32_t charDiff = CompareBytesBlock(lstr->GetDataUtf16(), rstr->GetDataUtf16(), minCount);
        if (charDiff != 0) {
            return charDiff;
        }
    }
    return lengRet;
}

template <typename T1, typename T2>
static inline ALWAYS_INLINE int32_t SubstringEquals(Span<const T1> &string, Span<const T2> &pattern, int32_t pos)
{
    ASSERT(pos + pattern.size() <= string.size());
    if constexpr (std::is_same_v<T1, T2>) {
        return std::memcmp(string.begin() + pos, pattern.begin(), pattern.size()) == 0;
    }
    return std::equal(pattern.begin(), pattern.end(), string.begin() + pos);
}

/*
 * Tailed Substring method (based on D. Cantone and S. Faro: Searching for a substring with constant extra-space
 * complexity). O(nm) worst-case but reported to have good performance both on random and natural language data
 * Substring s of t is called tailed-substring, if the last character of s does not repeat elsewhere in s
 */
/* static */
template <typename T1, typename T2>
static int32_t IndexOf(Span<const T1> &lhsSp, Span<const T2> &rhsSp, int32_t pos, int32_t max)
{
    int32_t maxTailedLen = 1;
    auto tailedEnd = static_cast<int32_t>(rhsSp.size() - 1);
    int32_t maxTailedEnd = tailedEnd;
    // Phase 1: search in the beginning of string while computing maximal tailed-substring length
    auto searchChar = rhsSp[tailedEnd];
    auto *shiftedLhs = lhsSp.begin() + tailedEnd;
    while (pos <= max) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (searchChar != shiftedLhs[pos]) {
            pos++;
            continue;
        }
        if (SubstringEquals(lhsSp, rhsSp, pos)) {
            return pos;
        }
        auto tailedStart = tailedEnd - 1;
        while (tailedStart >= 0 && rhsSp[tailedStart] != searchChar) {
            tailedStart--;
        }
        if (maxTailedLen < tailedEnd - tailedStart) {
            maxTailedLen = tailedEnd - tailedStart;
            maxTailedEnd = tailedEnd;
        }
        if (maxTailedLen >= tailedEnd) {
            break;
        }
        pos += tailedEnd - tailedStart;
        tailedEnd--;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        shiftedLhs--;
        searchChar = rhsSp[tailedEnd];
    }
    // Phase 2: search in the remainder of string using computed maximal tailed-substring length
    searchChar = rhsSp[maxTailedEnd];
    shiftedLhs = lhsSp.begin() + maxTailedEnd;
    while (pos <= max) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (searchChar != shiftedLhs[pos]) {
            pos++;
            continue;
        }
        if (SubstringEquals(lhsSp, rhsSp, pos)) {
            return pos;
        }
        pos += maxTailedLen;
    }
    return -1;
}

// Search of the last occurence is equivalent to search of the first occurence of
// reversed pattern in reversed string
template <typename T1, typename T2>
static int32_t LastIndexOf(Span<const T1> &lhsSp, Span<const T2> &rhsSp, int32_t pos)
{
    int32_t maxTailedLen = 1;
    int32_t tailedStart = 0;
    int32_t maxTailedStart = tailedStart;
    auto patternSize = static_cast<int32_t>(rhsSp.size());
    // Phase 1: search in the end of string while computing maximal tailed-substring length
    auto searchChar = rhsSp[tailedStart];
    auto *shiftedLhs = lhsSp.begin() + tailedStart;
    while (pos >= 0) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (searchChar != shiftedLhs[pos]) {
            pos--;
            continue;
        }
        if (SubstringEquals(lhsSp, rhsSp, pos)) {
            return pos;
        }
        auto tailedEnd = tailedStart + 1;
        while (tailedEnd < patternSize && rhsSp[tailedEnd] != searchChar) {
            tailedEnd++;
        }
        if (maxTailedLen < tailedEnd - tailedStart) {
            maxTailedLen = tailedEnd - tailedStart;
            maxTailedStart = tailedStart;
        }
        if (maxTailedLen >= patternSize - tailedStart) {
            break;
        }
        pos -= tailedEnd - tailedStart;
        tailedStart++;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        shiftedLhs++;
        searchChar = rhsSp[tailedStart];
    }
    // Phase 2: search in the remainder of string using computed maximal tailed-substring length
    searchChar = rhsSp[maxTailedStart];
    shiftedLhs = lhsSp.begin() + maxTailedStart;
    while (pos >= 0) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (searchChar != shiftedLhs[pos]) {
            pos--;
            continue;
        }
        if (SubstringEquals(lhsSp, rhsSp, pos)) {
            return pos;
        }
        pos -= maxTailedLen;
    }
    return -1;
}

static inline ALWAYS_INLINE std::pair<bool, int32_t> GetCompressionAndLength(ark::coretypes::String *string)
{
    ASSERT(string->GetLength() <= static_cast<uint32_t>(std::numeric_limits<int32_t>::max()));
    ASSERT(string != nullptr);
    return {string->IsMUtf8(), static_cast<int32_t>(string->GetLength())};
}

int32_t String::IndexOf(String *rhs, int32_t pos)
{
    String *lhs = this;
    auto [lhs_utf8, lhs_count] = GetCompressionAndLength(lhs);
    auto [rhs_utf8, rhs_count] = GetCompressionAndLength(rhs);

    if (pos < 0) {
        pos = 0;
    }

    if (rhs_count == 0) {
        return std::min(lhs_count, pos);
    }

    int32_t max = lhs_count - rhs_count;
    // for pos > max IndexOf impl will return -1
    if (lhs_utf8 && rhs_utf8) {
        Span<const uint8_t> lhsSp(lhs->GetDataMUtf8(), lhs_count);
        Span<const uint8_t> rhsSp(rhs->GetDataMUtf8(), rhs_count);
        return ark::coretypes::IndexOf(lhsSp, rhsSp, pos, max);
    } else if (!lhs_utf8 && !rhs_utf8) {  // NOLINT(readability-else-after-return)
        Span<const uint16_t> lhsSp(lhs->GetDataUtf16(), lhs_count);
        Span<const uint16_t> rhsSp(rhs->GetDataUtf16(), rhs_count);
        return ark::coretypes::IndexOf(lhsSp, rhsSp, pos, max);
    } else if (rhs_utf8) {
        Span<const uint16_t> lhsSp(lhs->GetDataUtf16(), lhs_count);
        Span<const uint8_t> rhsSp(rhs->GetDataMUtf8(), rhs_count);
        return ark::coretypes::IndexOf(lhsSp, rhsSp, pos, max);
    } else {  // NOLINT(readability-else-after-return)
        Span<const uint8_t> lhsSp(lhs->GetDataMUtf8(), lhs_count);
        Span<const uint16_t> rhsSp(rhs->GetDataUtf16(), rhs_count);
        return ark::coretypes::IndexOf(lhsSp, rhsSp, pos, max);
    }
}

int32_t String::LastIndexOf(String *rhs, int32_t pos)
{
    String *lhs = this;
    auto [lhs_utf8, lhs_count] = GetCompressionAndLength(lhs);
    auto [rhs_utf8, rhs_count] = GetCompressionAndLength(rhs);

    int32_t max = lhs_count - rhs_count;

    if (pos > max) {
        pos = max;
    }

    if (pos < 0) {
        return -1;
    }

    if (rhs_count == 0) {
        return pos;
    }

    if (lhs_utf8 && rhs_utf8) {
        Span<const uint8_t> lhsSp(lhs->GetDataMUtf8(), lhs_count);
        Span<const uint8_t> rhsSp(rhs->GetDataMUtf8(), rhs_count);
        return ark::coretypes::LastIndexOf(lhsSp, rhsSp, pos);
    } else if (!lhs_utf8 && !rhs_utf8) {  // NOLINT(readability-else-after-return)
        Span<const uint16_t> lhsSp(lhs->GetDataUtf16(), lhs_count);
        Span<const uint16_t> rhsSp(rhs->GetDataUtf16(), rhs_count);
        return ark::coretypes::LastIndexOf(lhsSp, rhsSp, pos);
    } else if (rhs_utf8) {
        Span<const uint16_t> lhsSp(lhs->GetDataUtf16(), lhs_count);
        Span<const uint8_t> rhsSp(rhs->GetDataMUtf8(), rhs_count);
        return ark::coretypes::LastIndexOf(lhsSp, rhsSp, pos);
    } else {  // NOLINT(readability-else-after-return)
        Span<const uint8_t> lhsSp(lhs->GetDataMUtf8(), lhs_count);
        Span<const uint16_t> rhsSp(rhs->GetDataUtf16(), rhs_count);
        return ark::coretypes::LastIndexOf(lhsSp, rhsSp, pos);
    }
}

/* static */
bool String::CanBeCompressed(const uint16_t *utf16Data, uint32_t utf16Length)
{
    if (!compressedStringsEnabled_) {
        return false;
    }
    bool isCompressed = true;
    Span<const uint16_t> data(utf16Data, utf16Length);
    for (uint32_t i = 0; i < utf16Length; i++) {
        if (!IsASCIICharacter(data[i])) {
            isCompressed = false;
            break;
        }
    }
    return isCompressed;
}

// static
bool String::CanBeCompressedMUtf8(const uint8_t *mutf8Data, uint32_t mutf8Length)
{
    if (!compressedStringsEnabled_) {
        return false;
    }
    bool isCompressed = true;
    Span<const uint8_t> data(mutf8Data, mutf8Length);
    for (uint32_t i = 0; i < mutf8Length; i++) {
        if (!IsASCIICharacter(data[i])) {
            isCompressed = false;
            break;
        }
    }
    return isCompressed;
}

// static
bool String::CanBeCompressedMUtf8(const uint8_t *mutf8Data)
{
    return compressedStringsEnabled_ ? utf::IsMUtf8OnlySingleBytes(mutf8Data) : false;
}

/* static */
bool String::CanBeCompressedUtf16(const uint16_t *utf16Data, uint32_t utf16Length, uint16_t non)
{
    if (!compressedStringsEnabled_) {
        return false;
    }
    bool isCompressed = true;
    Span<const uint16_t> data(utf16Data, utf16Length);
    for (uint32_t i = 0; i < utf16Length; i++) {
        if (!IsASCIICharacter(data[i]) && data[i] != non) {
            isCompressed = false;
            break;
        }
    }
    return isCompressed;
}

/* static */
bool String::CanBeCompressedMUtf8(const uint8_t *mutf8Data, uint32_t mutf8Length, uint16_t non)
{
    if (!compressedStringsEnabled_) {
        return false;
    }
    bool isCompressed = true;
    Span<const uint8_t> data(mutf8Data, mutf8Length);
    for (uint32_t i = 0; i < mutf8Length; i++) {
        if (!IsASCIICharacter(data[i]) && data[i] != non) {
            isCompressed = false;
            break;
        }
    }
    return isCompressed;
}

/* static */
bool String::StringsAreEqual(String *str1, String *str2)
{
    ASSERT(str1 != nullptr);
    ASSERT(str2 != nullptr);

    if ((str1->IsUtf16() != str2->IsUtf16()) || (str1->GetLength() != str2->GetLength())) {
        return false;
    }

    if (str1->IsUtf16()) {
        Span<const uint16_t> data1(str1->GetDataUtf16(), str1->GetLength());
        Span<const uint16_t> data2(str2->GetDataUtf16(), str1->GetLength());
        return String::StringsAreEquals(data1, data2);
    } else {  // NOLINT(readability-else-after-return)
        Span<const uint8_t> data1(str1->GetDataMUtf8(), str1->GetLength());
        Span<const uint8_t> data2(str2->GetDataMUtf8(), str1->GetLength());
        return String::StringsAreEquals(data1, data2);
    }
}

/* static */
bool String::StringsAreEqualMUtf8(String *str1, const uint8_t *mutf8Data, uint32_t utf16Length)
{
    ASSERT(utf16Length == utf::MUtf8ToUtf16Size(mutf8Data));
    if (str1->GetLength() != utf16Length) {
        return false;
    }
    bool canBeCompressed = CanBeCompressedMUtf8(mutf8Data);
    return StringsAreEqualMUtf8(str1, mutf8Data, utf16Length, canBeCompressed);
}

/* static */
bool String::StringsAreEqualMUtf8(String *str1, const uint8_t *mutf8Data, uint32_t utf16Length, bool canBeCompressed)
{
    bool result = true;
    if (str1->GetLength() != utf16Length) {
        result = false;
    } else {
        bool str1CanBeCompressed = !str1->IsUtf16();
        bool data2CanBeCompressed = canBeCompressed;
        if (str1CanBeCompressed != data2CanBeCompressed) {
            return false;
        }

        ASSERT(str1CanBeCompressed == data2CanBeCompressed);
        if (str1CanBeCompressed) {
            Span<const uint8_t> data1(str1->GetDataMUtf8(), str1->GetLength());
            Span<const uint8_t> data2(mutf8Data, utf16Length);
            result = String::StringsAreEquals(data1, data2);
        } else {
            result = IsMutf8EqualsUtf16(mutf8Data, str1->GetDataUtf16(), str1->GetLength());
        }
    }
    return result;
}

/* static */
bool String::StringsAreEqualUtf16(String *str1, const uint16_t *utf16Data, uint32_t utf16DataLength)
{
    bool result = true;
    if (str1->GetLength() != utf16DataLength) {
        result = false;
    } else if (!str1->IsUtf16()) {
        result = IsMutf8EqualsUtf16(str1->GetDataMUtf8(), str1->GetLength(), utf16Data, utf16DataLength);
    } else {
        Span<const uint16_t> data1(str1->GetDataUtf16(), str1->GetLength());
        Span<const uint16_t> data2(utf16Data, utf16DataLength);
        result = String::StringsAreEquals(data1, data2);
    }
    return result;
}

/* static */
bool String::IsMutf8EqualsUtf16(const uint8_t *utf8Data, uint32_t utf8DataLength, const uint16_t *utf16Data,
                                uint32_t utf16DataLength)
{
    auto allocator = Runtime::GetCurrent()->GetInternalAllocator();
    auto tmpBuffer = allocator->AllocArray<uint16_t>(utf16DataLength);
    [[maybe_unused]] auto convertedStringSize =
        utf::ConvertRegionMUtf8ToUtf16(utf8Data, tmpBuffer, utf8DataLength, utf16DataLength, 0);
    ASSERT(convertedStringSize == utf16DataLength);

    Span<const uint16_t> data1(tmpBuffer, utf16DataLength);
    Span<const uint16_t> data2(utf16Data, utf16DataLength);
    bool result = String::StringsAreEquals(data1, data2);
    allocator->Delete(tmpBuffer);
    return result;
}

/* static */
bool String::IsMutf8EqualsUtf16(const uint8_t *utf8Data, const uint16_t *utf16Data, uint32_t utf16DataLength)
{
    auto allocator = Runtime::GetCurrent()->GetInternalAllocator();
    auto tmpBuffer = allocator->AllocArray<uint16_t>(utf16DataLength);
    utf::ConvertMUtf8ToUtf16(utf8Data, utf::Mutf8Size(utf8Data), tmpBuffer);

    Span<const uint16_t> data1(tmpBuffer, utf16DataLength);
    Span<const uint16_t> data2(utf16Data, utf16DataLength);
    bool result = String::StringsAreEquals(data1, data2);
    allocator->Delete(tmpBuffer);
    return result;
}

/* static */
template <typename T>
bool String::StringsAreEquals(Span<const T> &str1, Span<const T> &str2)
{
    return 0 == std::memcmp(str1.Data(), str2.Data(), str1.SizeBytes());
}

Array *String::ToCharArray(const LanguageContext &ctx)
{
    // allocator may trig gc and move 'this', need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> str(thread, this);
    auto *klass = Runtime::GetCurrent()->GetClassLinker()->GetExtension(ctx)->GetClassRoot(ClassRoot::ARRAY_U16);
    Array *array = Array::Create(klass, GetLength());
    if (array == nullptr) {
        return nullptr;
    }

    if (str->IsUtf16()) {
        Span<uint16_t> sp(str->GetDataUtf16(), str->GetLength());
        for (size_t i = 0; i < sp.size(); i++) {
            array->Set<uint16_t>(i, sp[i]);
        }
    } else {
        Span<uint8_t> sp(str->GetDataMUtf8(), str->GetLength());
        for (size_t i = 0; i < sp.size(); i++) {
            array->Set<uint16_t>(i, sp[i]);
        }
    }

    return array;
}

/* static */
Array *String::GetChars(String *src, uint32_t start, uint32_t utf16Length, const LanguageContext &ctx)
{
    // allocator may trig gc and move 'src', need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> str(thread, src);
    auto *klass = Runtime::GetCurrent()->GetClassLinker()->GetExtension(ctx)->GetClassRoot(ClassRoot::ARRAY_U16);
    Array *array = Array::Create(klass, utf16Length);
    if (array == nullptr) {
        return nullptr;
    }

    if (str->IsUtf16()) {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        Span<uint16_t> sp(str->GetDataUtf16() + start, utf16Length);
        for (size_t i = 0; i < sp.size(); i++) {
            array->Set<uint16_t>(i, sp[i]);
        }
    } else {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        Span<uint8_t> sp(str->GetDataMUtf8() + start, utf16Length);
        for (size_t i = 0; i < sp.size(); i++) {
            array->Set<uint16_t>(i, sp[i]);
        }
    }

    return array;
}

template <class T>
static int32_t ComputeHashForData(const T *data, size_t size)
{
    uint32_t hash = 0;
#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-attributes"
    Span<const T> sp(data, size);
#pragma GCC diagnostic pop
#endif
    for (auto c : sp) {
        constexpr size_t SHIFT = 5;
        hash = (hash << SHIFT) - hash + c;
    }
    return static_cast<int32_t>(hash);
}

static int32_t ComputeHashForMutf8(const uint8_t *mutf8Data)
{
    uint32_t hash = 0;
    while (*mutf8Data != '\0') {  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        constexpr size_t SHIFT = 5;
        hash = (hash << SHIFT) - hash + *mutf8Data++;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    return static_cast<int32_t>(hash);
}

uint32_t String::ComputeHashcode()
{
    uint32_t hash;
    if (compressedStringsEnabled_) {
        if (!IsUtf16()) {
            hash = static_cast<uint32_t>(ComputeHashForData(GetDataMUtf8(), GetLength()));
        } else {
            hash = static_cast<uint32_t>(ComputeHashForData(GetDataUtf16(), GetLength()));
        }
    } else {
        ASSERT(static_cast<size_t>(GetLength()) < (std::numeric_limits<size_t>::max() >> 1U));
        hash = static_cast<uint32_t>(ComputeHashForData(GetDataUtf16(), GetLength()));
    }
    return hash;
}

/* static */
uint32_t String::ComputeHashcodeMutf8(const uint8_t *mutf8Data, uint32_t utf16Length)
{
    bool canBeCompressed = CanBeCompressedMUtf8(mutf8Data);
    return ComputeHashcodeMutf8(mutf8Data, utf16Length, canBeCompressed);
}

/* static */
uint32_t String::ComputeHashcodeMutf8(const uint8_t *mutf8Data, uint32_t utf16Length, bool canBeCompressed)
{
    uint32_t hash;
    if (canBeCompressed) {
        hash = static_cast<uint32_t>(ComputeHashForMutf8(mutf8Data));
    } else {
        // NOTE(alovkov): optimize it without allocation a temporary buffer
        auto allocator = Runtime::GetCurrent()->GetInternalAllocator();
        auto tmpBuffer = allocator->AllocArray<uint16_t>(utf16Length);
        utf::ConvertMUtf8ToUtf16(mutf8Data, utf::Mutf8Size(mutf8Data), tmpBuffer);
        hash = static_cast<uint32_t>(ComputeHashForData(tmpBuffer, utf16Length));
        allocator->Delete(tmpBuffer);
    }
    return hash;
}

/* static */
uint32_t String::ComputeHashcodeUtf16(const uint16_t *utf16Data, uint32_t length)
{
    return ComputeHashForData(utf16Data, length);
}

/* static */
String *String::DoReplace(String *src, uint16_t oldC, uint16_t newC, const LanguageContext &ctx, PandaVM *vm)
{
    ASSERT(src != nullptr);
    auto length = static_cast<int32_t>(src->GetLength());
    bool canBeCompressed = IsASCIICharacter(newC);
    if (src->IsUtf16()) {
        canBeCompressed = canBeCompressed && CanBeCompressedUtf16(src->GetDataUtf16(), length, oldC);
    } else {
        canBeCompressed = canBeCompressed && CanBeCompressedMUtf8(src->GetDataMUtf8(), length, oldC);
    }

    // allocator may trig gc and move src, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> srcHandle(thread, src);
    auto string = AllocStringObject(length, canBeCompressed, ctx, vm);
    if (string == nullptr) {
        return nullptr;
    }

    // retrieve src after gc
    src = srcHandle.GetPtr();
    ASSERT(string->hashcode_ == 0);

    // After replacing we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (src->IsUtf16()) {
        if (canBeCompressed) {
            auto replace = [oldC, newC](uint16_t c) { return static_cast<uint8_t>((oldC != c) ? c : newC); };
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::transform(src->GetDataUtf16(), src->GetDataUtf16() + length, string->GetDataMUtf8(), replace);
        } else {
            auto replace = [oldC, newC](uint16_t c) { return (oldC != c) ? c : newC; };
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::transform(src->GetDataUtf16(), src->GetDataUtf16() + length, string->GetDataUtf16(), replace);
        }
    } else {
        if (canBeCompressed) {
            auto replace = [oldC, newC](uint16_t c) { return static_cast<uint8_t>((oldC != c) ? c : newC); };
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::transform(src->GetDataMUtf8(), src->GetDataMUtf8() + length, string->GetDataMUtf8(), replace);
        } else {
            auto replace = [oldC, newC](uint16_t c) { return (oldC != c) ? c : newC; };
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            std::transform(src->GetDataMUtf8(), src->GetDataMUtf8() + length, string->GetDataUtf16(), replace);
        }
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

/* static */
String *String::FastSubString(String *src, uint32_t start, uint32_t utf16Length, const LanguageContext &ctx,
                              PandaVM *vm)
{
    ASSERT(src != nullptr);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    bool canBeCompressed = !src->IsUtf16() || CanBeCompressed(src->GetDataUtf16() + start, utf16Length);

    // allocator may trig gc and move src, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> srcHandle(thread, src);
    auto string = AllocStringObject(utf16Length, canBeCompressed, ctx, vm);
    if (string == nullptr) {
        return nullptr;
    }

    // retrieve src after gc
    src = srcHandle.GetPtr();
    ASSERT(string->hashcode_ == 0);

    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (src->IsUtf16()) {
        if (canBeCompressed) {
            // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            CopyUtf16AsMUtf8(src->GetDataUtf16() + start, string->GetDataMUtf8(), utf16Length);
        } else {
            memcpy_s(string->GetDataUtf16(), ComputeDataSizeUtf16(string->GetLength()),
                     // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                     src->GetDataUtf16() + start, utf16Length << 1UL);
        }
    } else {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        memcpy_s(string->GetDataMUtf8(), string->GetLength(), src->GetDataMUtf8() + start, utf16Length);
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();
    return string;
}

/* static */
String *String::Concat(String *string1, String *string2, const LanguageContext &ctx, PandaVM *vm)
{
    ASSERT(string1 != nullptr);
    ASSERT(string2 != nullptr);
    // allocator may trig gc and move src, need to hold it
    auto thread = ManagedThread::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(thread);
    VMHandle<String> str1Handle(thread, string1);
    VMHandle<String> str2Handle(thread, string2);

    uint32_t length1 = string1->GetLength();
    uint32_t length2 = string2->GetLength();
    uint32_t newLength = length1 + length2;
    bool compressed = compressedStringsEnabled_ && (!string1->IsUtf16() && !string2->IsUtf16());
    auto newString = AllocStringObject(newLength, compressed, ctx, vm);
    if (UNLIKELY(newString == nullptr)) {
        return nullptr;
    }

    ASSERT(newString->hashcode_ == 0);

    // retrieve strings after gc
    string1 = str1Handle.GetPtr();
    string2 = str2Handle.GetPtr();

    // After copying we should have a full barrier, so this writes should happen-before barrier
    TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
    if (compressed) {
        Span<uint8_t> sp(newString->GetDataMUtf8(), newLength);
        memcpy_s(sp.Data(), sp.SizeBytes(), string1->GetDataMUtf8(), length1);
        sp = sp.SubSpan(length1);
        memcpy_s(sp.Data(), sp.SizeBytes(), string2->GetDataMUtf8(), length2);
    } else {
        Span<uint16_t> sp(newString->GetDataUtf16(), newLength);
        if (!string1->IsUtf16()) {
            for (uint32_t i = 0; i < length1; ++i) {
                sp[i] = string1->At<false>(i);
            }
        } else {
            memcpy_s(sp.Data(), sp.SizeBytes(), string1->GetDataUtf16(), length1 << 1U);
        }
        sp = sp.SubSpan(length1);
        if (!string2->IsUtf16()) {
            for (uint32_t i = 0; i < length2; ++i) {
                sp[i] = string2->At<false>(i);
            }
        } else {
            memcpy_s(sp.Data(), sp.SizeBytes(), string2->GetDataUtf16(), length2 << 1U);
        }
    }
    TSAN_ANNOTATE_IGNORE_WRITES_END();
    // String is supposed to be a constant object, so all its data should be visible by all threads
    arch::FullMemoryBarrier();

    return newString;
}

/* static */
String *String::AllocStringObject(size_t length, bool compressed, const LanguageContext &ctx, PandaVM *vm, bool movable,
                                  bool pinned)
{
    ASSERT(vm != nullptr);
    auto *thread = ManagedThread::GetCurrent();
    auto *stringClass = Runtime::GetCurrent()->GetClassLinker()->GetExtension(ctx)->GetClassRoot(ClassRoot::STRING);
    size_t size = compressed ? String::ComputeSizeMUtf8(length) : String::ComputeSizeUtf16(length);
    auto string =
        movable
            ? reinterpret_cast<String *>(
                  vm->GetHeapManager()->AllocateObject(stringClass, size, DEFAULT_ALIGNMENT, thread,
                                                       mem::ObjectAllocatorBase::ObjMemInitPolicy::NO_INIT, pinned))
            : reinterpret_cast<String *>(vm->GetHeapManager()->AllocateNonMovableObject(
                  stringClass, size, DEFAULT_ALIGNMENT, thread, mem::ObjectAllocatorBase::ObjMemInitPolicy::NO_INIT));
    if (string != nullptr) {
        // After setting length we should have a full barrier, so this write should happens-before barrier
        TSAN_ANNOTATE_IGNORE_WRITES_BEGIN();
        string->SetLength(length, compressed);
        string->SetHashcode(0);
        TSAN_ANNOTATE_IGNORE_WRITES_END();
        // Witout full memory barrier it is possible that architectures with weak memory order can try fetching string
        // legth before it's set
        arch::FullMemoryBarrier();
    }
    return string;
}

}  // namespace ark::coretypes
