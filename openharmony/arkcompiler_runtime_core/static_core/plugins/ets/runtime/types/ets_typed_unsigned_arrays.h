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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPED_UNSIGNED_ARRAYS_H
#define PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPED_UNSIGNED_ARRAYS_H

#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/types/ets_string.h"

namespace ark::ets {
class EtsEscompatTypedUArrayBase : public EtsObject {
public:
    EtsEscompatTypedUArrayBase() = delete;
    ~EtsEscompatTypedUArrayBase() = delete;

    NO_COPY_SEMANTIC(EtsEscompatTypedUArrayBase);
    NO_MOVE_SEMANTIC(EtsEscompatTypedUArrayBase);

    static constexpr size_t GetClassSize()
    {
        return sizeof(EtsEscompatTypedUArrayBase);
    }

    static constexpr size_t GetBufferOffset()
    {
        return MEMBER_OFFSET(EtsEscompatTypedUArrayBase, buffer_);
    }

    static constexpr size_t GetBytesPerElementOffset()
    {
        return MEMBER_OFFSET(EtsEscompatTypedUArrayBase, bytesPerElement_);
    }

    static constexpr size_t GetByteOffsetOffset()
    {
        return MEMBER_OFFSET(EtsEscompatTypedUArrayBase, byteOffset_);
    }

    static constexpr size_t GetByteLengthOffset()
    {
        return MEMBER_OFFSET(EtsEscompatTypedUArrayBase, byteLength_);
    }

    static constexpr size_t GetLengthIntOffset()
    {
        return MEMBER_OFFSET(EtsEscompatTypedUArrayBase, lengthInt_);
    }

    ObjectPointer<EtsObject> GetBuffer() const
    {
        return EtsObject::FromCoreType(ObjectAccessor::GetObject(this, GetBufferOffset()));
    }

    void SetBuffer(ObjectPointer<EtsObject> buffer)
    {
        buffer_ = buffer;
    }

    EtsDouble GetByteOffset() const
    {
        return byteOffset_;
    }

    void SetByteOffset(EtsDouble offset)
    {
        byteOffset_ = offset;
    }

    EtsDouble GetByteLength() const
    {
        return byteLength_;
    }

    void SetByteLength(EtsDouble byteLength)
    {
        byteLength_ = byteLength;
    }

    EtsDouble GetBytesPerElement() const
    {
        return bytesPerElement_;
    }

    EtsInt GetLengthInt() const
    {
        return lengthInt_;
    }

    void SetLengthInt(EtsInt length)
    {
        lengthInt_ = length;
    }

    ObjectPointer<EtsString> GetName() const
    {
        return reinterpret_cast<EtsString *>(
            ObjectAccessor::GetObject(this, MEMBER_OFFSET(EtsEscompatTypedUArrayBase, name_)));
    }

private:
    ObjectPointer<EtsObject> buffer_;
    ObjectPointer<EtsString> name_;
    EtsDouble bytesPerElement_;
    EtsInt byteOffset_;
    EtsInt byteLength_;
    EtsInt lengthInt_;
};

template <typename T>
class EtsEscompatTypedUArray : public EtsEscompatTypedUArrayBase {
public:
    using ElementType = T;
};

class EtsEscompatUInt8ClampedArray : public EtsEscompatTypedUArray<uint8_t> {
public:
    static constexpr int MIN = 0;
    static constexpr int MAX = 255;
};
class EtsEscompatUInt8Array : public EtsEscompatTypedUArray<uint8_t> {};
class EtsEscompatUInt16Array : public EtsEscompatTypedUArray<uint16_t> {};
class EtsEscompatUInt32Array : public EtsEscompatTypedUArray<uint32_t> {};
class EtsEscompatBigUInt64Array : public EtsEscompatTypedUArray<uint64_t> {};
}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_TYPES_ETS_TYPED_UNSIGNED_ARRAYS_H
