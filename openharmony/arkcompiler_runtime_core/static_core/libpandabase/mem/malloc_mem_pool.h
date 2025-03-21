/*
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

#ifndef LIBPANDABASE_MEM_MALLOC_MEM_POOL_H
#define LIBPANDABASE_MEM_MALLOC_MEM_POOL_H

#include "mem_pool.h"

namespace ark {

// Simple Mem Pool without cache
class MallocMemPool : public MemPool<MallocMemPool> {
private:
    template <class ArenaT = Arena, OSPagesAllocPolicy OS_ALLOC_POLICY>
    ArenaT *AllocArenaImpl(size_t size, SpaceType spaceType, AllocatorType allocatorType, const void *allocatorAddr);

    template <class ArenaT = Arena, OSPagesPolicy OS_PAGES_POLICY>
    void FreeArenaImpl(ArenaT *arena);

    template <OSPagesAllocPolicy OS_ALLOC_POLICY>
    static Pool AllocPoolImpl(size_t size, SpaceType spaceType, AllocatorType allocatorType, const void *allocatorAddr);

    template <OSPagesPolicy OS_PAGES_POLICY>
    static void FreePoolImpl(void *mem, size_t size);

    static AllocatorInfo GetAllocatorInfoForAddrImpl(const void *addr);

    static SpaceType GetSpaceTypeForAddrImpl(const void *addr);

    static void *GetStartAddrPoolForAddrImpl(const void *addr);

    MallocMemPool();

    ~MallocMemPool() override = default;

    NO_COPY_SEMANTIC(MallocMemPool);
    NO_MOVE_SEMANTIC(MallocMemPool);

    friend class PoolManager;
    friend class MemPool<MallocMemPool>;
};

}  // namespace ark

#endif  // LIBPANDABASE_MEM_MALLOC_MEM_POOL_H
