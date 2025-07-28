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

#include "runtime/include/object_header.h"
#include "runtime/mem/gc/cmc-gc-adapter/cmc-allocator-adapter.h"
#include "runtime/mem/runslots_allocator-inl.h"
#if defined(ARK_HYBRID)
#include "heap/heap_allocator.h"
#include "objects/base_state_word.h"
#endif

namespace ark::mem {

template <MTModeT MT_MODE>
CMCObjectAllocatorAdapter<MT_MODE>::CMCObjectAllocatorAdapter(MemStatsType *memStats, bool createPygoteSpaceAllocator)
    : ObjectAllocatorNoGen<MT_MODE>(memStats, createPygoteSpaceAllocator)
{
}

template <MTModeT MT_MODE>
void *CMCObjectAllocatorAdapter<MT_MODE>::Allocate([[maybe_unused]] size_t size, [[maybe_unused]] Alignment align,
                                                   [[maybe_unused]] ark::ManagedThread *thread,
                                                   [[maybe_unused]] ObjectAllocatorBase::ObjMemInitPolicy objInit,
                                                   [[maybe_unused]] bool pinned)
{
#if defined(ARK_HYBRID)
    return reinterpret_cast<void *>(panda::HeapAllocator::Allocate(size, panda::Language::STATIC));
#else
    return nullptr;
#endif
}

template <MTModeT MT_MODE>
void *CMCObjectAllocatorAdapter<MT_MODE>::AllocateNonMovable(
    [[maybe_unused]] size_t size, [[maybe_unused]] Alignment align,  // CC-OFF(G.FMT.06) project code style
    [[maybe_unused]] ark::ManagedThread *thread,                     // CC-OFF(G.FMT.06) project code style
    [[maybe_unused]] ObjectAllocatorBase::ObjMemInitPolicy objInit)  // CC-OFF(G.FMT.06) project code style
{
#if defined(ARK_HYBRID)
    return reinterpret_cast<ObjectHeader *>(panda::HeapAllocator::AllocateInNonmove(size, panda::Language::STATIC));
#else
    return nullptr;
#endif
}

template class CMCObjectAllocatorAdapter<MT_MODE_SINGLE>;
template class CMCObjectAllocatorAdapter<MT_MODE_MULTI>;
template class CMCObjectAllocatorAdapter<MT_MODE_TASK>;

}  // namespace ark::mem
