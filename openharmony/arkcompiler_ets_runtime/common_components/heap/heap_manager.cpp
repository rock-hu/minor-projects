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
#include "common_components/heap/heap_manager.h"

#include "common_components/heap/heap.h"
#include "common_components/heap/collector/collector.h"
#include "common_components/heap/allocator/region_manager.h"
#include "common_components/heap/allocator/region_space.h"

namespace common {
HeapManager::HeapManager() {}
void HeapManager::RequestGC(GCReason reason, bool async, GCType gcType)
{
    if (!Heap::GetHeap().IsGCEnabled()) {
        return;
    }
    Collector& collector = Heap::GetHeap().GetCollector();
    collector.RequestGC(reason, async, gcType);
}

HeapAddress HeapManager::Allocate(size_t allocSize, AllocType allocType, bool allowGC)
{
    return Heap::GetHeap().Allocate(allocSize, allocType, allowGC);
}

void HeapManager::Init(const RuntimeParam& param) { Heap::GetHeap().Init(param); }

void HeapManager::Fini() { Heap::GetHeap().Fini(); }

void HeapManager::StartRuntimeThreads() { Heap::GetHeap().StartRuntimeThreads(); }

void HeapManager::StopRuntimeThreads() { Heap::GetHeap().StopRuntimeThreads(); }

void HeapManager::MarkJitFortMemInstalled(void *vm, void *obj)
{
    RegionManager &manager = reinterpret_cast<RegionSpace &>(Heap::GetHeap().GetAllocator()).GetRegionManager();
    manager.MarkJitFortMemInstalled(vm, reinterpret_cast<BaseObject *>(obj));
}

void HeapManager::SetReadOnlyToROSpace()
{
    RegionManager& manager = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator()).GetRegionManager();
    manager.SetReadOnlyToRORegionList();
}

void HeapManager::ClearReadOnlyFromROSpace()
{
    RegionManager& manager = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator()).GetRegionManager();
    manager.ClearReadOnlyFromRORegionList();
}

bool HeapManager::IsInROSpace(BaseObject *obj)
{
    return RegionSpace::IsReadOnlyObject(obj);
}
} // namespace common
