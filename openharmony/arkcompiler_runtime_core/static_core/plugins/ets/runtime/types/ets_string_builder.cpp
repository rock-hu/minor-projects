/**
 * Copyright (c) 2023-2025 Huawei Device Co., Ltd.
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

#include "libpandabase/utils/utils.h"
#include "libpandabase/utils/utf.h"
#include "runtime/arch/memory_helpers.h"
#include "runtime/include/runtime.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/ets_handle.h"
#include "plugins/ets/runtime/ets_handle_scope.h"
#include "plugins/ets/runtime/types/ets_primitives.h"
#include "plugins/ets/runtime/types/ets_string.h"
#include "plugins/ets/runtime/types/ets_array.h"
#include "plugins/ets/runtime/types/ets_string_builder.h"
#include "plugins/ets/runtime/intrinsics/helpers/ets_intrinsics_helpers.h"
#include "plugins/ets/runtime/intrinsics/helpers/ets_to_string_cache.h"
#include "utils/math_helpers.h"
#include <cstdint>
#include <cmath>

namespace ark::ets {

/// StringBuilder fields offsets
static constexpr uint32_t SB_BUFFER_OFFSET = ark::ObjectHeader::ObjectHeaderSize();
static constexpr uint32_t SB_INDEX_OFFSET = SB_BUFFER_OFFSET + ark::OBJECT_POINTER_SIZE;
static constexpr uint32_t SB_LENGTH_OFFSET = SB_INDEX_OFFSET + sizeof(int32_t);
static constexpr uint32_t SB_COMPRESS_OFFSET = SB_LENGTH_OFFSET + sizeof(int32_t);

/// "null", "true" and "false" packed to integral types
static constexpr uint64_t TRUE_CODE = 0x0065007500720074;
static constexpr uint64_t FALS_CODE = 0x0073006c00610066;
static constexpr uint16_t E_CODE = 0x0065;

static_assert(std::is_same_v<EtsBoolean, uint8_t>);
static_assert(std::is_same_v<EtsChar, uint16_t> &&
              std::is_same_v<EtsCharArray, EtsPrimitiveArray<EtsChar, EtsClassRoot::CHAR_ARRAY>>);

// The following implementation is based on ObjectHeader::ShallowCopy
static EtsObjectArray *ReallocateBuffer(EtsHandle<EtsObjectArray> &bufHandle, uint32_t bufLen)
{
    // Allocate the new buffer - may trigger GC
    auto *newBuf = EtsObjectArray::Create(bufHandle->GetClass(), bufLen);
    ASSERT(newBuf != nullptr);
    // Copy the old buffer data
    bufHandle->CopyDataTo(newBuf);
    EVENT_SB_BUFFER_REALLOC(ManagedThread::GetCurrent()->GetId(), newBuf, newBuf->GetLength(), newBuf->GetElementSize(),
                            newBuf->ObjectSize());
    return newBuf;
}

// Increase buffer length needed to append `numElements` elements at the end
static uint32_t GetNewBufferLength(uint32_t currentLength, uint32_t numElements)
{
    return helpers::math::GetPowerOfTwoValue32(currentLength + numElements);
}

// A string representations of nullptr, bool, short, int, long, float and double
// do not contain uncompressable chars. So we may skip 'compress' check in these cases.
template <bool CHECK_IF_COMPRESSABLE = true>
ObjectHeader *AppendCharArrayToBuffer(VMHandle<EtsObject> &sbHandle, EtsCharArray *arr)
{
    auto *sb = sbHandle.GetPtr();
    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    auto index = sb->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto *buf = EtsObjectArray::FromCoreType(sb->GetFieldObject(SB_BUFFER_OFFSET)->GetCoreType());

    // Check the case of the buf overflow
    if (index >= buf->GetLength()) {
        auto *coroutine = EtsCoroutine::GetCurrent();
        EtsHandle<EtsCharArray> arrHandle(coroutine, arr);
        EtsHandle<EtsObjectArray> bufHandle(coroutine, buf);
        // May trigger GC
        buf = ReallocateBuffer(bufHandle, GetNewBufferLength(bufHandle->GetLength(), 1U));
        // Update sb and arr as corresponding objects might be moved by GC
        sb = sbHandle.GetPtr();
        arr = arrHandle.GetPtr();
        // Remember the new buffer
        sb->SetFieldObject(SB_BUFFER_OFFSET, EtsObject::FromCoreType(buf->GetCoreType()));
    }

    // Append array to the buf
    buf->Set(index, EtsObject::FromCoreType(arr->GetCoreType()));
    // Increment the index
    sb->SetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET, index + 1U);
    // Increase the length
    // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
    sb->SetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET, length + arr->GetLength());
    // If string compression is disabled in the runtime, then set 'StringBuilder.compress' to 'false',
    // as by default 'StringBuilder.compress' is 'true'.
    if (!Runtime::GetCurrent()->GetOptions().IsRuntimeCompressedStringsEnabled()) {
        if (sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET)) {
            sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
        }
    } else if (CHECK_IF_COMPRESSABLE && sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET)) {
        // Set the compress field to false if the array contains not compressable chars
        auto n = arr->GetLength();
        for (uint32_t i = 0; i < n; ++i) {
            if (!ark::coretypes::String::IsASCIICharacter(arr->Get(i))) {
                sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
                break;
            }
        }
    }
    return sb->GetCoreType();
}

static void ReconstructStringAsMUtf8(EtsString *dstString, EtsObjectArray *buffer, uint32_t index, uint32_t length,
                                     EtsClass *stringKlass)
{
    // All strings in the buf are MUtf8
    uint8_t *dstData = dstString->GetDataMUtf8();
    for (uint32_t i = 0; i < index; ++i) {
        EtsObject *obj = buffer->Get(i);
        if (obj->IsInstanceOf(stringKlass)) {
            coretypes::String *srcString = EtsString::FromEtsObject(obj)->GetCoreType();
            uint32_t n = srcString->CopyDataRegionMUtf8(dstData, 0, srcString->GetLength(), length);
            dstData += n;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            length -= n;
        } else {
            // obj is an array of chars
            coretypes::Array *srcArray = coretypes::Array::Cast(obj->GetCoreType());
            uint32_t n = srcArray->GetLength();
            for (uint32_t j = 0; j < n; ++j) {
                // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                dstData[j] = srcArray->GetPrimitive<uint16_t>(sizeof(uint16_t) * j);
            }
            dstData += n;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            length -= n;
        }
    }
}

static void ReconstructStringAsUtf16(EtsString *dstString, EtsObjectArray *buffer, uint32_t index, uint32_t length,
                                     EtsClass *stringKlass)
{
    // Some strings in the buf are Utf16
    uint16_t *dstData = dstString->GetDataUtf16();
    for (uint32_t i = 0; i < index; ++i) {
        EtsObject *obj = buffer->Get(i);
        if (obj->IsInstanceOf(stringKlass)) {
            coretypes::String *srcString = EtsString::FromEtsObject(obj)->GetCoreType();
            uint32_t n = srcString->CopyDataRegionUtf16(dstData, 0, srcString->GetLength(), length);
            dstData += n;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            length -= n;
        } else {
            // obj is an array of chars
            coretypes::Array *srcArray = coretypes::Array::Cast(obj->GetCoreType());
            auto *srcData = reinterpret_cast<EtsChar *>(srcArray->GetData());
            uint32_t n = srcArray->GetLength();
            ASSERT(IsAligned(ToUintPtr(srcData), sizeof(uint64_t)));
            auto bytes = n << 1UL;
            // equals to 2^(k + 1) when n is 2^k AND dst is aligned by 2^(k + 1)
            auto bytesAndAligned = bytes | (ToUintPtr(dstData) & (bytes - 1));
            switch (bytesAndAligned) {
                case 2U:  // 2 bytes
                    *dstData = *srcData;
                    break;
                case 4U:  // 4 bytes
                    *reinterpret_cast<uint32_t *>(dstData) = *reinterpret_cast<uint32_t *>(srcData);
                    break;
                case 8U:  // 8 bytes
                    *reinterpret_cast<uint64_t *>(dstData) = *reinterpret_cast<uint64_t *>(srcData);
                    break;
                default:
                    std::copy_n(srcData, n, dstData);
            }
            dstData += n;  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
            length -= n;
        }
    }
}

static inline EtsCharArray *NullToCharArray()
{
    static constexpr std::array<uint16_t, 9U> UNDEFINED_UTF16 = {0x7500, 0x6e00, 0x6400, 0x6500, 0x6600,
                                                                 0x6900, 0x6e00, 0x6500, 0x6400};

    EtsCharArray *arr = EtsCharArray::Create(UNDEFINED_UTF16.size());
    if (memcpy_s(arr->GetData<uint16_t>(), UNDEFINED_UTF16.size(), UNDEFINED_UTF16.data(), UNDEFINED_UTF16.size()) !=
        EOK) {
        UNREACHABLE();
    }
    return arr;
}

static inline EtsCharArray *BoolToCharArray(EtsBoolean v)
{
    auto arrLen = v != 0U ? std::char_traits<char>::length("true") : std::char_traits<char>::length("false");
    EtsCharArray *arr = EtsCharArray::Create(arrLen);
    auto *data = arr->GetData<uint64_t>();
    if (v != 0U) {
        *data = TRUE_CODE;
    } else {
        *data = FALS_CODE;
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *reinterpret_cast<EtsChar *>(data + 1) = E_CODE;
    }
    return arr;
}

static inline EtsCharArray *CharToCharArray(EtsChar v)
{
    EtsCharArray *arr = EtsCharArray::Create(1U);
    *arr->GetData<EtsChar>() = v;
    return arr;
}

VMHandle<EtsObject> &StringBuilderAppendNullString(VMHandle<EtsObject> &sbHandle)
{
    // May trigger GC
    EtsCharArray *arr = NullToCharArray();
    AppendCharArrayToBuffer<false>(sbHandle, arr);
    return sbHandle;
}

ObjectHeader *StringBuilderAppendNullString(ObjectHeader *sb)
{
    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);

    VMHandle<EtsObject> sbHandle(coroutine, sb);

    return StringBuilderAppendNullString(sbHandle)->GetCoreType();
}

/**
 * Implementation of public native append(s: String): StringBuilder.
 * Inserts the string 's' into a free buffer slot:
 *
 *    buf[index] = s;
 *    index++;
 *    length += s.length
 *    compress &= s.IsMUtf8()
 *
 * In case of the buf overflow, we create a new buffer of a larger size
 * and copy the data from the old buffer.
 */
VMHandle<EtsObject> &StringBuilderAppendString(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &strHandle)
{
    if (strHandle.GetPtr() == nullptr) {
        return StringBuilderAppendNullString(sbHandle);
    }
    if (strHandle->GetLength() == 0) {
        return sbHandle;
    }

    auto sb = sbHandle->GetCoreType();
    auto str = strHandle.GetPtr();

    ASSERT(sb != nullptr);
    ASSERT(str->GetLength() > 0);

    auto index = sb->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto *buf = EtsObjectArray::FromCoreType(sb->GetFieldObject(SB_BUFFER_OFFSET));
    // Check buf overflow
    if (index >= buf->GetLength()) {
        auto *coroutine = EtsCoroutine::GetCurrent();
        EtsHandle<EtsObjectArray> bufHandle(coroutine, buf);
        // May trigger GC
        buf = ReallocateBuffer(bufHandle, GetNewBufferLength(bufHandle->GetLength(), 1U));
        // Update sb and s as corresponding objects might be moved by GC
        sb = sbHandle->GetCoreType();
        str = strHandle.GetPtr();
        // Remember the new buffer
        sb->SetFieldObject(SB_BUFFER_OFFSET, buf->GetCoreType());
    }
    // Append string to the buf
    // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
    buf->Set(index, EtsObject::FromCoreType(str->GetCoreType()));
    // Increment the index
    sb->SetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET, index + 1U);
    // Increase the length
    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    sb->SetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET, length + str->GetLength());
    // Set the compress field to false if the string is not compressable
    if (sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET) && str->IsUtf16()) {
        sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
    }

    return sbHandle;
}

ObjectHeader *StringBuilderAppendString(ObjectHeader *sb, EtsString *str)
{
    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);

    VMHandle<EtsObject> sbHandle(coroutine, sb);
    EtsHandle<EtsString> strHandle(coroutine, str);

    return StringBuilderAppendString(sbHandle, strHandle)->GetCoreType();
}

VMHandle<EtsObject> &StringBuilderAppendStringsChecked(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                       EtsHandle<EtsString> &str1Handle)
{
    auto sb = sbHandle->GetCoreType();
    auto str0 = str0Handle.GetPtr();
    auto str1 = str1Handle.GetPtr();

    // sb.append(str0, str1)
    auto index = sb->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto *buf = EtsObjectArray::FromCoreType(sb->GetFieldObject(SB_BUFFER_OFFSET));
    // Check buf overflow
    if (index + 1U >= buf->GetLength()) {
        auto *coroutine = EtsCoroutine::GetCurrent();
        EtsHandle<EtsObjectArray> bufHandle(coroutine, buf);
        // May trigger GC
        buf = ReallocateBuffer(bufHandle, GetNewBufferLength(bufHandle->GetLength(), 2U));
        // Update sb and strings as corresponding objects might be moved by GC
        sb = sbHandle->GetCoreType();
        str0 = str0Handle.GetPtr();
        str1 = str1Handle.GetPtr();
        // Remember the new buffer
        sb->SetFieldObject(SB_BUFFER_OFFSET, buf->GetCoreType());
    }
    // Append strings to the buf
    // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
    buf->Set(index + 0U, EtsObject::FromCoreType(str0->GetCoreType()));
    buf->Set(index + 1U, EtsObject::FromCoreType(str1->GetCoreType()));
    // Increment the index
    sb->SetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET, index + 2U);
    // Increase the length
    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    sb->SetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET, length + str0->GetLength() + str1->GetLength());
    // Set the compress field to false if strings are not compressable
    if (sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET) && (str0->IsUtf16() || str1->IsUtf16())) {
        sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
    }

    return sbHandle;
}

VMHandle<EtsObject> &StringBuilderAppendStrings(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                EtsHandle<EtsString> &str1Handle)
{
    // sb.append(null, ...)
    if (str0Handle.GetPtr() == nullptr) {
        return StringBuilderAppendString(StringBuilderAppendNullString(sbHandle), str1Handle);
    }
    // sb.append(str0, null)
    if (str1Handle.GetPtr() == nullptr) {
        return StringBuilderAppendNullString(StringBuilderAppendString(sbHandle, str0Handle));
    }

    ASSERT(str0Handle.GetPtr() != nullptr && str1Handle.GetPtr() != nullptr);

    // sb.append("", str1)
    if (str0Handle.GetPtr()->GetLength() == 0) {
        return StringBuilderAppendString(sbHandle, str1Handle);
    }
    // sb.append(str0, "")
    if (str1Handle.GetPtr()->GetLength() == 0) {
        return StringBuilderAppendString(sbHandle, str0Handle);
    }

    ASSERT(sbHandle.GetPtr() != nullptr);
    ASSERT(str0Handle->GetLength() > 0 && str1Handle->GetLength() > 0);

    return StringBuilderAppendStringsChecked(sbHandle, str0Handle, str1Handle);
}

ObjectHeader *StringBuilderAppendStrings(ObjectHeader *sb, EtsString *str0, EtsString *str1)
{
    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);

    VMHandle<EtsObject> sbHandle(coroutine, sb);
    EtsHandle<EtsString> str0Handle(coroutine, str0);
    EtsHandle<EtsString> str1Handle(coroutine, str1);

    return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle)->GetCoreType();
}

VMHandle<EtsObject> &StringBuilderAppendStringsChecked(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                       EtsHandle<EtsString> &str1Handle,
                                                       EtsHandle<EtsString> &str2Handle)
{
    auto sb = sbHandle->GetCoreType();
    auto str0 = str0Handle.GetPtr();
    auto str1 = str1Handle.GetPtr();
    auto str2 = str2Handle.GetPtr();

    // sb.append(str0, str2, str3)
    auto index = sb->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto *buf = EtsObjectArray::FromCoreType(sb->GetFieldObject(SB_BUFFER_OFFSET));
    // Check buf overflow
    if (index + 2U >= buf->GetLength()) {
        auto *coroutine = EtsCoroutine::GetCurrent();
        EtsHandle<EtsObjectArray> bufHandle(coroutine, buf);
        // May trigger GC
        buf = ReallocateBuffer(bufHandle, GetNewBufferLength(bufHandle->GetLength(), 3U));
        // Update sb and strings as corresponding objects might be moved by GC
        sb = sbHandle->GetCoreType();
        str0 = str0Handle.GetPtr();
        str1 = str1Handle.GetPtr();
        str2 = str2Handle.GetPtr();
        // Remember the new buffer
        sb->SetFieldObject(SB_BUFFER_OFFSET, buf->GetCoreType());
    }
    // Append strings to the buf
    // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
    buf->Set(index + 0U, EtsObject::FromCoreType(str0->GetCoreType()));
    buf->Set(index + 1U, EtsObject::FromCoreType(str1->GetCoreType()));
    buf->Set(index + 2U, EtsObject::FromCoreType(str2->GetCoreType()));
    // Increment the index
    sb->SetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET, index + 3U);
    // Increase the length
    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    sb->SetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET,
                                    length + str0->GetLength() + str1->GetLength() + str2->GetLength());
    // Set the compress field to false if strings are not compressable
    if (sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET) && (str0->IsUtf16() || str1->IsUtf16() || str2->IsUtf16())) {
        sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
    }

    return sbHandle;
}

VMHandle<EtsObject> &StringBuilderAppendStrings(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                EtsHandle<EtsString> &str1Handle, EtsHandle<EtsString> &str2Handle)
{
    // sb.append(null, ..., ...)
    if (str0Handle.GetPtr() == nullptr) {
        return StringBuilderAppendStrings(StringBuilderAppendNullString(sbHandle), str1Handle, str2Handle);
    }
    // sb.append(str0, null, ...)
    if (str1Handle.GetPtr() == nullptr) {
        return StringBuilderAppendString(StringBuilderAppendNullString(StringBuilderAppendString(sbHandle, str0Handle)),
                                         str2Handle);
    }
    // sb.append(str0, str1, null)
    if (str2Handle.GetPtr() == nullptr) {
        return StringBuilderAppendNullString(StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle));
    }

    ASSERT(str0Handle.GetPtr() != nullptr && str1Handle.GetPtr() != nullptr && str2Handle.GetPtr() != nullptr);

    // sb.append("", str1, str2)
    if (str0Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str1Handle, str2Handle);
    }
    // sb.append(str0, "", str2)
    if (str1Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str0Handle, str2Handle);
    }
    // sb.append(str0, str1, "")
    if (str2Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle);
    }

    ASSERT(sbHandle.GetPtr() != nullptr);
    ASSERT(str0Handle->GetLength() > 0 && str1Handle->GetLength() > 0 && str2Handle->GetLength() > 0);

    return StringBuilderAppendStringsChecked(sbHandle, str0Handle, str1Handle, str2Handle);
}

ObjectHeader *StringBuilderAppendStrings(ObjectHeader *sb, EtsString *str0, EtsString *str1, EtsString *str2)
{
    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);

    VMHandle<EtsObject> sbHandle(coroutine, sb);
    EtsHandle<EtsString> str0Handle(coroutine, str0);
    EtsHandle<EtsString> str1Handle(coroutine, str1);
    EtsHandle<EtsString> str2Handle(coroutine, str2);

    return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle, str2Handle)->GetCoreType();
}

VMHandle<EtsObject> &StringBuilderAppendStringsChecked(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                       EtsHandle<EtsString> &str1Handle,
                                                       EtsHandle<EtsString> &str2Handle,
                                                       EtsHandle<EtsString> &str3Handle)
{
    auto sb = sbHandle->GetCoreType();
    auto str0 = str0Handle.GetPtr();
    auto str1 = str1Handle.GetPtr();
    auto str2 = str2Handle.GetPtr();
    auto str3 = str3Handle.GetPtr();

    // sb.append(str0, str2, str3, str4)
    auto index = sb->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto *buf = EtsObjectArray::FromCoreType(sb->GetFieldObject(SB_BUFFER_OFFSET));
    // Check buf overflow
    if (index + 3U >= buf->GetLength()) {
        auto *coroutine = EtsCoroutine::GetCurrent();
        EtsHandle<EtsObjectArray> bufHandle(coroutine, buf);
        // May trigger GC
        buf = ReallocateBuffer(bufHandle, GetNewBufferLength(bufHandle->GetLength(), 4U));
        // Update sb and strings as corresponding objects might be moved by GC
        sb = sbHandle->GetCoreType();
        str0 = str0Handle.GetPtr();
        str1 = str1Handle.GetPtr();
        str2 = str2Handle.GetPtr();
        str3 = str3Handle.GetPtr();
        // Remember the new buffer
        sb->SetFieldObject(SB_BUFFER_OFFSET, buf->GetCoreType());
    }
    // Append strings to the buf
    // NOLINTNEXTLINE(clang-analyzer-core.CallAndMessage)
    buf->Set(index + 0U, EtsObject::FromCoreType(str0->GetCoreType()));
    buf->Set(index + 1U, EtsObject::FromCoreType(str1->GetCoreType()));
    buf->Set(index + 2U, EtsObject::FromCoreType(str2->GetCoreType()));
    buf->Set(index + 3U, EtsObject::FromCoreType(str3->GetCoreType()));
    // Increment the index
    sb->SetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET, index + 4U);
    // Increase the length
    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    sb->SetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET, length + str0->GetLength() + str1->GetLength() +
                                                          str2->GetLength() + str3->GetLength());
    // Set the compress field to false if strings are not compressable
    if (sb->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET) &&
        (str0->IsUtf16() || str1->IsUtf16() || str2->IsUtf16() || str3->IsUtf16())) {
        sb->SetFieldPrimitive<bool>(SB_COMPRESS_OFFSET, false);
    }

    return sbHandle;
}

VMHandle<EtsObject> &StringBuilderAppendStrings(VMHandle<EtsObject> &sbHandle, EtsHandle<EtsString> &str0Handle,
                                                EtsHandle<EtsString> &str1Handle, EtsHandle<EtsString> &str2Handle,
                                                EtsHandle<EtsString> &str3Handle)
{
    // sb.append(null, ..., ..., ...)
    if (str0Handle.GetPtr() == nullptr) {
        return StringBuilderAppendStrings(StringBuilderAppendNullString(sbHandle), str1Handle, str2Handle, str3Handle);
    }
    // sb.append(str0, null, ..., ...)
    if (str1Handle.GetPtr() == nullptr) {
        return StringBuilderAppendStrings(
            StringBuilderAppendNullString(StringBuilderAppendString(sbHandle, str0Handle)), str2Handle, str3Handle);
    }
    // sb.append(str0, str1, null, ...)
    if (str2Handle.GetPtr() == nullptr) {
        return StringBuilderAppendString(
            StringBuilderAppendNullString(StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle)), str3Handle);
    }
    // sb.append(str0, str1, str2, null)
    if (str3Handle.GetPtr() == nullptr) {
        return StringBuilderAppendNullString(StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle, str2Handle));
    }

    ASSERT(str0Handle.GetPtr() != nullptr && str1Handle.GetPtr() != nullptr && str2Handle.GetPtr() != nullptr &&
           str3Handle.GetPtr() != nullptr);

    // sb.append("", str1, str2, str3)
    if (str0Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str1Handle, str2Handle, str3Handle);
    }
    // sb.append(str0, "", str2, str3)
    if (str1Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str0Handle, str2Handle, str3Handle);
    }
    // sb.append(str0, str1, "", str3)
    if (str2Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle, str3Handle);
    }
    // sb.append(str0, str1, str2, "")
    if (str3Handle->GetLength() == 0) {
        return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle, str2Handle);
    }

    ASSERT(sbHandle.GetPtr() != nullptr);
    ASSERT(str0Handle->GetLength() > 0 && str1Handle->GetLength() > 0 && str2Handle->GetLength() > 0 &&
           str3Handle->GetLength() > 0);

    return StringBuilderAppendStringsChecked(sbHandle, str0Handle, str1Handle, str2Handle, str3Handle);
}

ObjectHeader *StringBuilderAppendStrings(ObjectHeader *sb, EtsString *str0, EtsString *str1, EtsString *str2,
                                         EtsString *str3)
{
    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);

    VMHandle<EtsObject> sbHandle(coroutine, sb);
    EtsHandle<EtsString> str0Handle(coroutine, str0);
    EtsHandle<EtsString> str1Handle(coroutine, str1);
    EtsHandle<EtsString> str2Handle(coroutine, str2);
    EtsHandle<EtsString> str3Handle(coroutine, str3);

    return StringBuilderAppendStrings(sbHandle, str0Handle, str1Handle, str2Handle, str3Handle)->GetCoreType();
}

ObjectHeader *StringBuilderAppendChar(ObjectHeader *sb, EtsChar v)
{
    ASSERT(sb != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    // May trigger GC
    auto *arr = CharToCharArray(v);
    return AppendCharArrayToBuffer(sbHandle, arr);
}

ObjectHeader *StringBuilderAppendBool(ObjectHeader *sb, EtsBoolean v)
{
    ASSERT(sb != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    // May trigger GC
    auto *arr = BoolToCharArray(v);
    return AppendCharArrayToBuffer<false>(sbHandle, arr);
}

ObjectHeader *StringBuilderAppendLong(ObjectHeader *sb, EtsLong v)
{
    ASSERT(sb != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    // May trigger GC
    auto *cache = PandaEtsVM::GetCurrent()->GetLongToStringCache();
    ASSERT(cache != nullptr);
    auto *str = cache->GetOrCache(EtsCoroutine::GetCurrent(), v);
    return StringBuilderAppendString(sbHandle->GetCoreType(), str);
}

template <typename FpType, std::enable_if_t<std::is_floating_point_v<FpType>, bool> = true>
static inline EtsCharArray *FloatingPointToCharArray(FpType number)
{
    return intrinsics::helpers::FpToStringDecimalRadix(number, [](std::string_view str) {
        auto *arr = EtsCharArray::Create(str.length());
        Span<uint16_t> data(arr->GetData<uint16_t>(), str.length());
        for (size_t i = 0; i < str.length(); ++i) {
            ASSERT(ark::coretypes::String::IsASCIICharacter(str[i]));
            data[i] = static_cast<uint16_t>(str[i]);
        }
        return arr;
    });
}

ObjectHeader *StringBuilderAppendFloat(ObjectHeader *sb, EtsFloat v)
{
    ASSERT(sb != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    auto *cache = PandaEtsVM::GetCurrent()->GetFloatToStringCache();
    ASSERT(cache != nullptr);
    auto *str = cache->GetOrCache(EtsCoroutine::GetCurrent(), v);
    return StringBuilderAppendString(sbHandle->GetCoreType(), str);
}

ObjectHeader *StringBuilderAppendDouble(ObjectHeader *sb, EtsDouble v)
{
    ASSERT(sb != nullptr);

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    auto *cache = PandaEtsVM::GetCurrent()->GetDoubleToStringCache();
    ASSERT(cache != nullptr);
    auto *str = cache->GetOrCache(EtsCoroutine::GetCurrent(), v);
    return StringBuilderAppendString(sbHandle->GetCoreType(), str);
}

EtsString *StringBuilderToString(ObjectHeader *sb)
{
    ASSERT(sb != nullptr);

    auto length = sb->GetFieldPrimitive<uint32_t>(SB_LENGTH_OFFSET);
    if (length == 0) {
        return EtsString::CreateNewEmptyString();
    }

    auto *coroutine = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope(coroutine);
    VMHandle<EtsObject> sbHandle(coroutine, sb);

    auto index = sbHandle->GetFieldPrimitive<uint32_t>(SB_INDEX_OFFSET);
    auto compress = sbHandle->GetFieldPrimitive<bool>(SB_COMPRESS_OFFSET);
    EtsString *s = EtsString::AllocateNonInitializedString(length, compress);
    EtsClass *sKlass = EtsClass::FromRuntimeClass(s->GetCoreType()->ClassAddr<Class>());
    auto *buf = EtsObjectArray::FromCoreType(sbHandle->GetFieldObject(SB_BUFFER_OFFSET)->GetCoreType());
    if (compress) {
        ReconstructStringAsMUtf8(s, buf, index, length, sKlass);
    } else {
        ReconstructStringAsUtf16(s, buf, index, length, sKlass);
    }
    return s;
}

}  // namespace ark::ets
