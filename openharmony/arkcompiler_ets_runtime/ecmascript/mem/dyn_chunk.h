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

#ifndef ECMASCRIPT_REGEXP_DYN_BUFFER_H
#define ECMASCRIPT_REGEXP_DYN_BUFFER_H

#include <cstring>

#include "ecmascript/common.h"
#include "ecmascript/mem/regexp_cached_chunk.h"
#include "ecmascript/mem/chunk.h"

namespace panda::ecmascript {
class PUBLIC_API DynChunk {
public:
    static constexpr size_t ALLOCATE_MIN_SIZE = 256;
    static constexpr int FAILURE = -1;
    static constexpr int SUCCESS = 0;
    explicit DynChunk(Chunk *chunk) : chunk_(chunk)
    {
        ASSERT(chunk_ != nullptr);
    };

    ~DynChunk() = default;

    NO_COPY_SEMANTIC(DynChunk);
    NO_MOVE_SEMANTIC(DynChunk);

    int Expand(size_t newSize);

    int Insert(uint32_t position, size_t len);

    int PUBLIC_API Emit(const uint8_t *data, size_t len);

    int EmitSelf(size_t offset, size_t len);

    int PUBLIC_API EmitChar(uint8_t c);

    int EmitStr(const char *str);

    inline int EmitU16(uint16_t data)
    {
        return Emit(reinterpret_cast<uint8_t *>(&data), U16_SIZE);
    }

    inline int EmitU32(uint32_t data)
    {
        return Emit(reinterpret_cast<uint8_t *>(&data), U32_SIZE);
    }

    inline int EmitU64(uint64_t data)
    {
        return Emit(reinterpret_cast<uint8_t *>(&data), U64_SIZE);
    }

    inline void SetError()
    {
        error_ = true;
    }

    inline size_t GetSize() const
    {
        return size_;
    }

    inline size_t GetAllocatedSize() const
    {
        return allocatedSize_;
    }

    inline bool GetError() const
    {
        return error_;
    }

    inline uint64_t GetU64(size_t offset) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *reinterpret_cast<uint64_t *>(buf_ + offset);
    }

    inline uint32_t GetU32(size_t offset) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *reinterpret_cast<uint32_t *>(buf_ + offset);
    }

    inline void PutU32(size_t offset, uint32_t data) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *reinterpret_cast<uint32_t *>(buf_ + offset) = data;
    }

    inline uint32_t GetU16(size_t offset) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *reinterpret_cast<uint16_t *>(buf_ + offset);
    }

    inline void PutU16(size_t offset, uint16_t data) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *reinterpret_cast<uint16_t *>(buf_ + offset) = data;
    }

    inline uint32_t GetU8(size_t offset) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        return *(buf_ + offset);
    }

    inline void PutU8(size_t offset, uint8_t data) const
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        *(buf_ + offset) = data;
    }

    ALWAYS_INLINE static inline constexpr uint32_t GetBufferOffset()
    {
        return MEMBER_OFFSET(DynChunk, buf_);
    }

    uint8_t *GetBegin() const
    {
        return buf_;
    }

private:
    static constexpr size_t ALLOCATE_MULTIPLIER = 2;
    static constexpr size_t U16_SIZE = 2;
    static constexpr size_t U32_SIZE = 4;
    static constexpr size_t U64_SIZE = 8;
    friend class RegExpParser;
    friend class RegExpOpCode;
    friend class RegExpExecutor;

    DynChunk(uint8_t *buf, RegExpCachedChunk *regExpCachedChunk) : buf_(buf), regExpCachedChunk_(regExpCachedChunk)
    {
        ASSERT(regExpCachedChunk_ != nullptr);
    };

    uint8_t *buf_ {nullptr};
    size_t size_ {0};
    size_t allocatedSize_ {0};
    bool error_ {false};
    RegExpCachedChunk *regExpCachedChunk_ {nullptr};
    Chunk *chunk_ {nullptr};
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_REGEXP_DYN_BUFFER_H
