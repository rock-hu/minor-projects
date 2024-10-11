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

#ifndef ECMASCRIPT_MEM_CHUNK_H
#define ECMASCRIPT_MEM_CHUNK_H

#include "ecmascript/common.h"
#include "ecmascript/log_wrapper.h"
#include "ecmascript/mem/ecma_list.h"
#include "ecmascript/mem/area.h"

namespace panda::ecmascript {
class NativeAreaAllocator;

class PUBLIC_API Chunk {
public:
    static constexpr size_t MEM_ALIGN = 8U;

    explicit Chunk(NativeAreaAllocator *allocator);
    Chunk() = default;
    ~Chunk()
    {
        ReleaseMemory();
    }

    NO_COPY_SEMANTIC(Chunk);
    NO_MOVE_SEMANTIC(Chunk);

    [[nodiscard]] void *Allocate(size_t size)
    {
        if (size == 0) {
            LOG_ECMA_MEM(FATAL) << "size must have a size bigger than 0";
            UNREACHABLE();
        }
        uintptr_t result = ptr_;
        size = AlignUp(size, MEM_ALIGN);
        if (UNLIKELY(size > end_ - ptr_)) {
            result = Expand(size);
        } else {
            ptr_ += size;
        }

        return reinterpret_cast<void *>(result);
    }

    template<class T>
    [[nodiscard]] T *NewArray(size_t size)
    {
        return static_cast<T *>(Allocate(size * sizeof(T)));
    }

    template<typename T, typename... Args>
    [[nodiscard]] T *New(Args &&... args)
    {
        auto p = reinterpret_cast<void *>(Allocate(sizeof(T)));
        new (p) T(std::forward<Args>(args)...);
        return reinterpret_cast<T *>(p);
    }

    template<class T>
    void Delete(T *ptr)
    {
        ASSERT(ptr != nullptr);
        // NOLINTNEXTLINE(readability-braces-around-statements,bugprone-suspicious-semicolon)
        if constexpr (std::is_class_v<T>) {
            ptr->~T();
        }
        Free(ptr);
    }

    void Free([[maybe_unused]] void *mem)
    {
        // do nothing
    }

protected:
    uintptr_t ptr_ {0};
    uintptr_t end_ {0};

    Area *currentArea_ {nullptr};
    NativeAreaAllocator *allocator_ {nullptr};

private:
    uintptr_t Expand(size_t size);
    Area *NewArea(size_t size);
    void ReleaseMemory();

    EcmaList<Area> areaList_ {};
};

class PUBLIC_API ChunkObject {
public:
    void *operator new(size_t size, Chunk* chunk)
    {
        return chunk->Allocate(size);
    }

    void operator delete([[maybe_unused]] void* ptr)
    {
        UNREACHABLE();
    }
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_MEM_CHUNK_H
