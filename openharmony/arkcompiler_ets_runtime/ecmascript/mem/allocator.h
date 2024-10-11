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

#ifndef ECMASCRIPT_MEM_ALLOCATOR_H
#define ECMASCRIPT_MEM_ALLOCATOR_H

#include <memory>

#include "ecmascript/mem/free_object_list.h"
#include "ecmascript/mem/mem.h"

namespace panda::ecmascript {
class Region;
class BaseHeap;
class JitFort;

class Allocator {
public:
    Allocator() = default;
    virtual ~Allocator() = default;
    NO_COPY_SEMANTIC(Allocator);
    NO_MOVE_SEMANTIC(Allocator);
};

class BumpPointerAllocator : public Allocator {
public:
    BumpPointerAllocator() = default;
    ~BumpPointerAllocator() override = default;
    NO_COPY_SEMANTIC(BumpPointerAllocator);
    NO_MOVE_SEMANTIC(BumpPointerAllocator);

    inline BumpPointerAllocator(uintptr_t begin, uintptr_t end);

    inline void Reset();
    inline void Reset(uintptr_t begin, uintptr_t end);
    inline void Reset(uintptr_t begin, uintptr_t end, uintptr_t top);
    inline void ResetTopPointer(uintptr_t top);
    inline uintptr_t Allocate(size_t size);

    uintptr_t GetTop() const
    {
        return top_;
    }

    uintptr_t GetEnd() const
    {
        return end_;
    }

    const uintptr_t *GetTopAddress()
    {
        return &top_;
    }

    const uintptr_t *GetEndAddress()
    {
        return &end_;
    }

    size_t Available() const
    {
        return (end_ - top_);
    }

private:
    uintptr_t begin_ {0};
    uintptr_t top_ {0};
    uintptr_t end_ {0};
};

template <typename T>
class FreeListAllocator : public Allocator {
public:
    FreeListAllocator() = delete;
    ~FreeListAllocator() override = default;

    NO_COPY_SEMANTIC(FreeListAllocator);
    NO_MOVE_SEMANTIC(FreeListAllocator);

    inline explicit FreeListAllocator(BaseHeap *heap);
    inline explicit FreeListAllocator(BaseHeap *heap, MemDescPool *pool, JitFort *fort);
    inline void Initialize(Region *region);

    inline void Reset(BaseHeap *heap);

    inline uintptr_t Allocate(size_t size);
    inline void AddFree(Region *region);
    inline uintptr_t LookupSuitableFreeObject(size_t size);

    inline void RebuildFreeList();

    inline bool MatchFreeObjectSet(Region *region, size_t size);
    inline void CollectFreeObjectSet(Region *region);
    inline void DetachFreeObjectSet(Region *region);

    inline void FreeBumpPoint();
    // Only fill free object
    inline void FillBumpPointer();

    inline void ResetBumpPointer(uintptr_t begin, uintptr_t end, uintptr_t top);
    inline void ResetTopPointer(uintptr_t top);

    inline void Free(uintptr_t begin, size_t size, bool isAdd = true);

    inline size_t GetAvailableSize() const;
    inline size_t GetWastedSize() const;

    uintptr_t GetTop() const
    {
        return bpAllocator_.GetTop();
    }

    size_t GetAllocatedSize() const
    {
        return allocationSizeAccumulator_;
    }

    void IncreaseAllocatedSize(size_t allocatedSize)
    {
        allocationSizeAccumulator_ += allocatedSize;
    }

private:
    inline uintptr_t Allocate(T *object, size_t size);
    std::unique_ptr<FreeObjectList<T>> freeList_ {nullptr};
    MemDescPool *memDescPool_ {nullptr};
    BumpPointerAllocator bpAllocator_;
    BaseHeap *heap_{nullptr};
    size_t allocationSizeAccumulator_ {0};
};
}  // namespace panda::ecmascript

#endif  // ECMASCRIPT_MEM_ALLOCATOR_H
