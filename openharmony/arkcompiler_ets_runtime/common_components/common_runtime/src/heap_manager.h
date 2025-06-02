/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef ARK_COMMON_HEAP_MANAGER_H
#define ARK_COMMON_HEAP_MANAGER_H

#include "common_components/common_runtime/src/common/type_def.h"
#include "common_components/common_runtime/src/heap/collector/gc_request.h"
#include "common_components/common_runtime/src/heap/collector/collector.h"
#include "common_components/common_runtime/src/heap/heap.h"

namespace panda {
// replace this for Heap.
class HeapManager {
public:
    HeapManager();
    ~HeapManager() = default;

    // runtime required lifecycle interfaces
    void Init(const RuntimeParam& param);
    void Fini();

    static inline void RequestGC(GCReason reason, bool async)
    {
        if (!Heap::GetHeap().IsGCEnabled()) {
            return;
        }
        Collector& collector = Heap::GetHeap().GetCollector();
        collector.RequestGC(reason, async);
    }

    // alloc return memory address, not "object" pointers, since they're not
    // initialized yet
    static HeapAddress Allocate(size_t allocSize, AllocType allocType = AllocType::MOVEABLE_OBJECT,
                                bool allowGC = true);

    // For PostFork and Prefork.
    static void StartRuntimeThreads();
    static void StopRuntimeThreads();
};
} // namespace panda
#endif // ARK_COMMON_HEAP_MANAGER_H
