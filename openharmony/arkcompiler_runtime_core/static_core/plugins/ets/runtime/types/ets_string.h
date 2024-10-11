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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_STRING_H_
#define PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_STRING_H_

#include "runtime/include/runtime.h"
#include "runtime/include/coretypes/string-inl.h"
#include "plugins/ets/runtime/types/ets_array.h"
#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/napi/ets_napi.h"

namespace ark::ets {

// Private inheritance, because need to disallow implicit conversion to core type
class EtsString : private coretypes::String {
public:
    static EtsString *CreateFromMUtf8(const char *mutf8)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (mutf8 == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        const auto *data = reinterpret_cast<const uint8_t *>(mutf8);
        coretypes::String *s = coretypes::String::CreateFromMUtf8(data, ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(s);
    }

    static EtsString *CreateFromMUtf8(const char *mutf8, uint32_t utf16Length)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (utf16Length == 0) {
            return reinterpret_cast<EtsString *>(
                coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
        }
        if (mutf8 == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        const auto *data = reinterpret_cast<const uint8_t *>(mutf8);
        coretypes::String *s =
            coretypes::String::CreateFromMUtf8(data, utf16Length, ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(s);
    }

    static EtsString *CreateFromUtf8(const char *utf8, uint32_t length)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (length == 0) {
            return reinterpret_cast<EtsString *>(
                coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
        }
        if (utf8 == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        const auto *data = reinterpret_cast<const uint8_t *>(utf8);
        return reinterpret_cast<EtsString *>(
            coretypes::String::CreateFromUtf8(data, length, ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *CreateFromAscii(const char *str, uint32_t length)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        const auto *data = reinterpret_cast<const uint8_t *>(str);
        return reinterpret_cast<EtsString *>(
            coretypes::String::CreateFromMUtf8(data, length, length, true, ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *CreateFromUtf16(const ets_char *utf16, ets_int length)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (length == 0) {
            return reinterpret_cast<EtsString *>(
                coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
        }
        if (utf16 == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        coretypes::String *s =
            coretypes::String::CreateFromUtf16(utf16, length, ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(s);
    }

    static EtsString *CreateNewStringFromChars(uint32_t offset, uint32_t length, EtsArray *chararray)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        return reinterpret_cast<EtsString *>(coretypes::String::CreateNewStringFromChars(
            offset, length, reinterpret_cast<coretypes::Array *>(chararray), ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *CreateNewStringFromBytes(EtsArray *bytearray, ets_int high, ets_int offset, ets_int length)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (length == 0) {
            return reinterpret_cast<EtsString *>(
                coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
        }
        if (bytearray == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        if (offset < 0 || length < 0 || bytearray->GetLength() < static_cast<uint32_t>(offset + length)) {
            ThrowArrayIndexOutOfBoundsException(static_cast<uint32_t>(offset + length), bytearray->GetLength(), ctx,
                                                ManagedThread::GetCurrent());
            return nullptr;
        }
        return reinterpret_cast<EtsString *>(coretypes::String::CreateNewStringFromBytes(
            offset, length, static_cast<uint32_t>(high), reinterpret_cast<coretypes::Array *>(bytearray), ctx,
            Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *CreateNewStringFromString(EtsString *etsString)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (etsString == nullptr) {
            ThrowNullPointerException(ctx, ManagedThread::GetCurrent());
            return nullptr;
        }
        coretypes::String *string = etsString->GetCoreType();
        return reinterpret_cast<EtsString *>(
            coretypes::String::CreateFromString(string, ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *CreateNewEmptyString()
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        return reinterpret_cast<EtsString *>(
            coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static EtsString *Resolve(const uint8_t *mutf8, uint32_t length)
    {
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        return reinterpret_cast<EtsString *>(
            Runtime::GetCurrent()->ResolveString(Runtime::GetCurrent()->GetPandaVM(), mutf8, length, ctx));
    }

    static EtsString *Concat(EtsString *etsString1, EtsString *etsString2)
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        coretypes::String *string3 = coretypes::String::Concat(etsString1->GetCoreType(), etsString2->GetCoreType(),
                                                               ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(string3);
    }

    EtsString *TrimLeft()
    {
        if (IsEmpty() || !utf::IsWhiteSpaceChar(At(0))) {
            return this;
        }
        const auto numChars = GetLength();
        auto notWhiteSpaceIndex = numChars;
        for (int i = 1; i < numChars; ++i) {
            if (!utf::IsWhiteSpaceChar(At(i))) {
                notWhiteSpaceIndex = i;
                break;
            }
        }
        auto len = numChars - notWhiteSpaceIndex;
        return FastSubString(this, static_cast<uint32_t>(notWhiteSpaceIndex), static_cast<uint32_t>(len));
    }

    EtsString *TrimRight()
    {
        if (IsEmpty()) {
            return this;
        }
        auto last = GetLength() - 1;
        if (!utf::IsWhiteSpaceChar(At(last))) {
            return this;
        }
        int notWhiteSpaceIndex = -1;
        for (int i = last - 1; i >= 0; --i) {
            if (!utf::IsWhiteSpaceChar(At(i))) {
                notWhiteSpaceIndex = i;
                break;
            }
        }
        auto len = notWhiteSpaceIndex + 1;
        return EtsString::FastSubString(this, 0, static_cast<uint32_t>(len));
    }

    EtsString *Trim()
    {
        if (IsEmpty()) {
            return this;
        }
        int left = 0;
        int right = GetLength() - 1;
        while (utf::IsWhiteSpaceChar(At(right))) {
            if (right == left) {
                return EtsString::CreateNewEmptyString();
            }
            --right;
        }
        while (left < right && utf::IsWhiteSpaceChar(At(left))) {
            ++left;
        }
        return EtsString::FastSubString(this, left, static_cast<uint32_t>(right - left + 1));
    }

    EtsBoolean StartsWith(EtsString *prefix, EtsInt fromIndex)
    {
        ASSERT(prefix != nullptr);
        if (fromIndex < 0) {
            fromIndex = 0;
        }
        auto prefixLen = prefix->GetLength();
        if (fromIndex > GetLength() - prefixLen) {
            return ToEtsBoolean(prefix->IsEmpty());
        }
        auto *thisCoreType = GetCoreType();
        auto *prefCoreType = prefix->GetCoreType();
        for (EtsInt i = 0; i < prefixLen; ++i) {
            if (thisCoreType->At<false>(fromIndex + i) != prefCoreType->At<false>(i)) {
                return ToEtsBoolean(false);
            }
        }
        return ToEtsBoolean(true);
    }

    EtsBoolean EndsWith(EtsString *suffix, EtsInt endIndex)
    {
        ASSERT(suffix != nullptr);
        if (suffix->IsEmpty()) {
            return ToEtsBoolean(true);
        }
        auto strLen = GetLength();
        if (strLen == 0) {
            return ToEtsBoolean(false);
        }
        if (endIndex <= 0) {
            return ToEtsBoolean(false);
        }
        if (endIndex > strLen) {
            endIndex = strLen;
        }
        ASSERT(endIndex > 0);
        auto suffixLen = suffix->GetLength();
        auto fromIndex = endIndex - suffixLen;
        if (fromIndex < 0) {
            return ToEtsBoolean(false);
        }
        auto *thisCoreType = GetCoreType();
        auto *suffCoreType = suffix->GetCoreType();
        for (EtsInt i = 0; i < suffixLen; ++i) {
            if (thisCoreType->At<false>(fromIndex + i) != suffCoreType->At<false>(i)) {
                return ToEtsBoolean(false);
            }
        }
        return ToEtsBoolean(true);
    }

    int32_t Compare(EtsString *rhs)
    {
        return GetCoreType()->Compare(rhs->GetCoreType());
    }

    uint16_t At(int32_t index)
    {
        return GetCoreType()->At(index);
    }

    static EtsString *DoReplace(EtsString *src, ets_char oldC, ets_char newC)
    {
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        if (src->GetLength() == 0) {
            return reinterpret_cast<EtsString *>(
                coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
        }
        coretypes::String *result = coretypes::String::DoReplace(reinterpret_cast<coretypes::String *>(src), oldC, newC,
                                                                 ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(result);
    }

    static EtsString *FastSubString(EtsString *src, uint32_t start, uint32_t length)
    {
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        coretypes::String *string1 = src->GetCoreType();
        coretypes::String *string2 =
            coretypes::String::FastSubString(string1, start, length, ctx, Runtime::GetCurrent()->GetPandaVM());
        return reinterpret_cast<EtsString *>(string2);
    }

    EtsArray *ToCharArray()
    {
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        auto *ret = GetCoreType()->ToCharArray(ctx);
        if (UNLIKELY(ret == nullptr)) {
            return nullptr;
        }
        return reinterpret_cast<EtsArray *>(ret);
    }

    bool IsUtf16()
    {
        return GetCoreType()->IsUtf16();
    }

    ets_int GetLength()
    {
        return GetCoreType()->GetLength();
    }

    size_t CopyDataMUtf8(void *buf, size_t maxLength, bool isCString)
    {
        return GetCoreType()->CopyDataMUtf8(reinterpret_cast<uint8_t *>(buf), maxLength, isCString);
    }

    size_t CopyDataRegionMUtf8(void *buf, size_t start, size_t length, size_t maxLength)
    {
        return GetCoreType()->CopyDataRegionMUtf8(reinterpret_cast<uint8_t *>(buf), start, length, maxLength);
    }

    uint32_t CopyDataUtf16(void *buf, uint32_t maxLength)
    {
        return GetCoreType()->CopyDataUtf16(reinterpret_cast<uint16_t *>(buf), maxLength);
    }

    uint32_t CopyDataRegionUtf16(void *buf, uint32_t start, uint32_t length, uint32_t maxLength)
    {
        return GetCoreType()->CopyDataRegionUtf16(reinterpret_cast<uint16_t *>(buf), start, length, maxLength);
    }

    std::string_view ConvertToStringView(PandaVector<uint8_t> *buf)
    {
        if (IsUtf16()) {
            size_t len = utf::Utf16ToMUtf8Size(GetDataUtf16(), GetUtf16Length()) - 1;
            buf->reserve(len);
            utf::ConvertRegionUtf16ToMUtf8(GetDataUtf16(), buf->data(), GetUtf16Length(), len, 0);
            return {utf::Mutf8AsCString(buf->data()), len};
        }
        return {utf::Mutf8AsCString(GetDataMUtf8()), static_cast<size_t>(GetLength())};
    }

    uint32_t GetMUtf8Length()
    {
        return GetCoreType()->GetMUtf8Length();
    }

    uint32_t GetUtf16Length()
    {
        return GetCoreType()->GetUtf16Length();
    }

    bool IsEqual(const char *str)
    {
        auto *mutf8Str = utf::CStringAsMutf8(str);
        return coretypes::String::StringsAreEqualMUtf8(GetCoreType(), mutf8Str, utf::MUtf8ToUtf16Size(mutf8Str));
    }

    PandaString GetMutf8()
    {
        size_t len = GetMUtf8Length() - 1;
        PandaString out;
        out.resize(len);
        CopyDataMUtf8(out.data(), len, false);
        return out;
    }

    uint8_t *GetDataMUtf8()
    {
        return GetCoreType()->GetDataMUtf8();
    }

    uint16_t *GetDataUtf16()
    {
        return GetCoreType()->GetDataUtf16();
    }

    bool IsEmpty()
    {
        return GetCoreType()->IsEmpty();
    }

    bool StringsAreEqual(EtsObject *obj)
    {
        return coretypes::String::StringsAreEqual(GetCoreType(), FromEtsObject(obj)->GetCoreType());
    }

    coretypes::String *GetCoreType()
    {
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        return reinterpret_cast<coretypes::String *>(this);
    }

    EtsObject *AsObject()
    {
        return reinterpret_cast<EtsObject *>(this);
    }

    const EtsObject *AsObject() const
    {
        return reinterpret_cast<const EtsObject *>(this);
    }

    static EtsString *FromCoreType(coretypes::String *str)
    {
        return reinterpret_cast<EtsString *>(str);
    }

    static EtsString *FromEtsObject(EtsObject *obj)
    {
        ASSERT(obj->GetClass()->GetRuntimeClass() == Runtime::GetCurrent()
                                                         ->GetClassLinker()
                                                         ->GetExtension(panda_file::SourceLang::ETS)
                                                         ->GetClassRoot(ClassRoot::STRING));
        return reinterpret_cast<EtsString *>(obj);
    }

    EtsString() = delete;
    ~EtsString() = delete;

    NO_COPY_SEMANTIC(EtsString);
    NO_MOVE_SEMANTIC(EtsString);

private:
    friend EtsString *StringBuilderToString(ObjectHeader *sb);

    static EtsString *AllocateNonInitializedString(uint32_t length, bool compressed)
    {
        ASSERT(length != 0);
        ASSERT_HAVE_ACCESS_TO_MANAGED_OBJECTS();
        LanguageContext ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        return reinterpret_cast<EtsString *>(
            coretypes::String::AllocStringObject(length, compressed, ctx, Runtime::GetCurrent()->GetPandaVM()));
    }
};

}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_FFI_CLASSES_ETS_STRING_H_
