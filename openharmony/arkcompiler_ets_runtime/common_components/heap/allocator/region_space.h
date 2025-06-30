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

#ifndef COMMON_COMPONENTS_HEAP_ALLOCATOR_REGION_SPACE_H
#define COMMON_COMPONENTS_HEAP_ALLOCATOR_REGION_SPACE_H

#include <list>
#include <map>
#include <set>
#include <thread>
#include <vector>

#include "common_components/heap/allocator/alloc_util.h"
#include "common_components/heap/allocator/allocator.h"
#include "common_components/heap/allocator/region_manager.h"
#include "common_components/heap/space/young_space.h"
#include "common_components/heap/space/old_space.h"
#include "common_components/heap/space/from_space.h"
#include "common_components/heap/space/to_space.h"
#include "common_components/mutator/mutator.h"
#if defined(COMMON_SANITIZER_SUPPORT)
#include "common_components/sanitizer/sanitizer_interface.h"
#endif
#include "common_interfaces/base_runtime.h"

namespace common {
class Taskpool;

// RegionSpace aims to be the API for other components of runtime
// the complication of implementation is delegated to RegionManager
// allocator should not depend on any assumptions on the details of RegionManager

// todo: Allocator -> BaseAllocator, RegionSpace -> RegionalHeap
class RegionSpace : public Allocator {
public:
    static size_t ToAllocatedSize(size_t objSize)
    {
        size_t size = objSize + HEADER_SIZE;
        return RoundUp<size_t>(size, ALLOC_ALIGN);
    }

    static size_t GetAllocSize(const BaseObject& obj)
    {
        size_t objSize = obj.GetSize();
        return ToAllocatedSize(objSize);
    }

    RegionSpace() : youngSpace_(regionManager_), oldSpace_(regionManager_),
        fromSpace_(regionManager_, *this), toSpace_(regionManager_) {}
    NO_INLINE_CC virtual ~RegionSpace()
    {
        if (allocBufferManager_ != nullptr) {
            delete allocBufferManager_;
            allocBufferManager_ = nullptr;
        }
#if defined(COMMON_SANITIZER_SUPPORT)
        Sanitizer::OnHeapDeallocated(map->GetBaseAddr(), map->GetMappedSize());
#endif
        MemoryMap::DestroyMemoryMap(map_);
    }

    void Init(const RuntimeParam &param) override;

    RegionDesc* AllocateThreadLocalRegion(bool expectPhysicalMem = false,
                                          AllocType allocType = AllocType::MOVEABLE_OBJECT);

    template <AllocBufferType type = AllocBufferType::YOUNG>
    void HandleFullThreadLocalRegion(RegionDesc* region) noexcept
    {
        ASSERT_LOGF(region->IsThreadLocalRegion(), "unexpected region type");

        if constexpr (type == AllocBufferType::YOUNG) {
            if (IsGcThread()) {
                toSpace_.HandleFullThreadLocalRegion(region);
            } else {
                youngSpace_.HandleFullThreadLocalRegion(region);
            }
        } else if constexpr (type == AllocBufferType::OLD) {
            ASSERT_LOGF(!IsGcThread(), "unexpected gc thread for old space");
            oldSpace_.HandleFullThreadLocalRegion(region);
        }
    }

    // only used for deserialize allocation, allocate one region and regard it as full region
    // todo: adapt for concurrent gc
    uintptr_t AllocRegion();
    uintptr_t AllocPinnedRegion();
    uintptr_t AllocLargeRegion(size_t size);
    uintptr_t AllocJitFortRegion(size_t size);

    HeapAddress Allocate(size_t size, AllocType allocType) override;

    HeapAddress AllocateNoGC(size_t size, AllocType allocType) override;

    RegionManager& GetRegionManager() noexcept { return regionManager_; }

    FromSpace& GetFromSpace() noexcept { return fromSpace_; }

    ToSpace& GetToSpace() noexcept { return toSpace_; }

    OldSpace& GetOldSpace() noexcept { return oldSpace_; }

    YoungSpace& GetYoungSpace() noexcept { return youngSpace_; }

    HeapAddress GetSpaceStartAddress() const override { return reservedStart_; }

    HeapAddress GetSpaceEndAddress() const override { return reservedEnd_; }

    size_t GetCurrentCapacity() const override { return regionManager_.GetInactiveZone() - reservedStart_; }
    size_t GetMaxCapacity() const override { return reservedEnd_ - reservedStart_; }

    inline size_t GetRecentAllocatedSize() const
    {
        return youngSpace_.GetRecentAllocatedSize() + regionManager_.GetRecentAllocatedSize();
    }

    // size of objects survived in previous gc.
    inline size_t GetSurvivedSize() const
    {
        return fromSpace_.GetSurvivedSize() + toSpace_.GetAllocatedSize() +
            youngSpace_.GetAllocatedSize() + oldSpace_.GetAllocatedSize() + regionManager_.GetSurvivedSize();
    }

    inline size_t GetUsedUnitCount() const
    {
        return fromSpace_.GetUsedUnitCount() + toSpace_.GetUsedUnitCount() +
            youngSpace_.GetUsedUnitCount() + oldSpace_.GetUsedUnitCount() + regionManager_.GetUsedUnitCount();
    }

    size_t GetUsedPageSize() const override { return regionManager_.GetUsedRegionSize(); }

    inline size_t GetTargetSize() const
    {
        double heapUtilization = BaseRuntime::GetInstance()->GetHeapParam().heapUtilization;
        return static_cast<size_t>(GetUsedPageSize() / heapUtilization);
    }

    size_t GetAllocatedBytes() const override
    {
        return fromSpace_.GetAllocatedSize() + toSpace_.GetAllocatedSize() +
            youngSpace_.GetAllocatedSize() + oldSpace_.GetAllocatedSize() + regionManager_.GetAllocatedSize();
    }

    size_t LargeObjectSize() const override { return regionManager_.GetLargeObjectSize(); }

    size_t FromSpaceSize() const { return fromSpace_.GetAllocatedSize(); }
    size_t ToSpaceSize() const { return toSpace_.GetAllocatedSize(); }

    size_t PinnedSpaceSize() const { return regionManager_.GetPinnedSpaceSize(); }

#ifndef NDEBUG
    bool IsHeapObject(HeapAddress addr) const override;
#endif

    size_t ReclaimGarbageMemory(bool releaseAll) override
    {
        {
            COMMON_PHASE_TIMER("ReclaimGarbageRegions");
            regionManager_.ReclaimGarbageRegions();
        }

        COMMON_PHASE_TIMER("ReleaseGarbageMemory");
        if (releaseAll) {
            return regionManager_.ReleaseGarbageRegions(0);
        } else {
            size_t size = GetAllocatedBytes();
            double cachedRatio = 1 - BaseRuntime::GetInstance()->GetHeapParam().heapUtilization;
            size_t targetCachedSize = static_cast<size_t>(size * cachedRatio);
            return regionManager_.ReleaseGarbageRegions(targetCachedSize);
        }
    }

    bool ForEachObject(const std::function<void(BaseObject*)>& visitor, bool safe) const override
    {
        if (UNLIKELY_CC(safe)) {
            regionManager_.ForEachObjectSafe(visitor);
        } else {
            regionManager_.ForEachObjectUnsafe(visitor);
        }
        return true;
    }

    void ExemptFromSpace()
    {
        COMMON_PHASE_TIMER("ExemptFromRegions");
        fromSpace_.ExemptFromRegions();
    }

    BaseObject* RouteObject(BaseObject* fromObj, size_t size)
    {
        AllocationBuffer* buffer = AllocationBuffer::GetOrCreateAllocBuffer();
        uintptr_t toAddr = buffer->ToSpaceAllocate(size, AllocType::MOVEABLE_OBJECT);
        return reinterpret_cast<BaseObject*>(toAddr);
    }

    void CopyFromSpace(Taskpool *threadPool)
    {
        COMMON_PHASE_TIMER("CopyFromRegions");
        fromSpace_.CopyFromRegions(threadPool);
    }

    void FixHeap()
    {
        youngSpace_.FixAllRegions();
        oldSpace_.FixAllRegions();
        fromSpace_.FixAllRegions();
        toSpace_.FixAllRegions();
        regionManager_.FixAllRegionLists();
    }

    void MarkAwaitingJitFort()
    {
        regionManager_.ForEachAwaitingJitFortUnsafe(MarkObject);
    }

    using RootSet = MarkStack<BaseObject*>;

    size_t CollectLargeGarbage() { return regionManager_.CollectLargeGarbage(); }

    void CollectFromSpaceGarbage()
    {
        regionManager_.CollectFromSpaceGarbage(fromSpace_.GetFromRegionList());
    }

    void HandlePromotion()
    {
        fromSpace_.GetPromotedTo(oldSpace_);
        toSpace_.GetPromotedTo(oldSpace_);
    }

    void AssembleSmallGarbageCandidates()
    {
        youngSpace_.AssembleGarbageCandidates(fromSpace_);
        oldSpace_.AssembleRecentFull();
        if (Heap::GetHeap().GetGCReason() != GC_REASON_YOUNG) {
            oldSpace_.ClearRSet();
            oldSpace_.AssembleGarbageCandidates(fromSpace_);
            regionManager_.ClearRSet();
        }
    }

    void CollectAppSpawnSpaceGarbage()
    {
        regionManager_.CollectFromSpaceGarbage(fromSpace_.GetFromRegionList());
        regionManager_.ReassembleAppspawnSpace(fromSpace_.GetExemptedRegionList());
        regionManager_.ReassembleAppspawnSpace(toSpace_.GetTlToRegionList());
        regionManager_.ReassembleAppspawnSpace(toSpace_.GetFullToRegionList());
    }

    void ClearAllGCInfo()
    {
        regionManager_.ClearAllGCInfo();
        youngSpace_.ClearAllGCInfo();
        oldSpace_.ClearAllGCInfo();
        toSpace_.ClearAllGCInfo();
        fromSpace_.ClearAllGCInfo();
    }

    void AssembleGarbageCandidates()
    {
        AssembleSmallGarbageCandidates();
        regionManager_.AssemblePinnedGarbageCandidates();
        regionManager_.AssembleLargeGarbageCandidates();
    }

    void DumpAllRegionStats(const char* msg) const;

    void CountLiveObject(const BaseObject* obj) { regionManager_.CountLiveObject(obj); }

    void PrepareTrace() { regionManager_.PrepareTrace(); }
    void PrepareForward() { regionManager_.PrepareForward(); }
    void PrepareFix() { regionManager_.PrepareFix(); }
    void PrepareFixForPin() { regionManager_.PrepareFixForPin(); }
    void FeedHungryBuffers() override;

    // markObj
    static bool MarkObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->MarkObject(obj);
    }
    static bool ResurrentObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->ResurrentObject(obj);
    }

    static bool EnqueueObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->EnqueueObject(obj);
    }

    static bool IsMarkedObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->IsMarkedObject(obj);
    }

    static bool IsResurrectedObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->IsResurrectedObject(obj);
    }

    static bool IsEnqueuedObject(const BaseObject* obj)
    {
        RegionDesc* regionInfo = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        return regionInfo->IsEnqueuedObject(obj);
    }

    static bool IsNewObjectSinceTrace(const BaseObject* object)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(object));
        ASSERT_LOGF(region != nullptr, "region is nullptr");
        return region->IsNewObjectSinceTrace(object);
    }

    static bool IsReadOnlyObject(const BaseObject* object)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(object));
        ASSERT_LOGF(region != nullptr, "region is nullptr");
        return region->IsReadOnlyRegion();
    }

    static bool IsYoungSpaceObject(const BaseObject* object)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(object));
        ASSERT_LOGF(region != nullptr, "region is nullptr");
        return region->IsInYoungSpace();
    }

    static bool IsInRememberSet(const BaseObject* object)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(object));
        ASSERT_LOGF(region != nullptr, "region is nullptr");
        return region->IsInRSet(const_cast<BaseObject*>(object));
    }

    void AddRawPointerObject(BaseObject* obj)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        region->IncRawPointerObjectCount();
        if (region->IsFromRegion() && fromSpace_.TryDeleteFromRegion(region, RegionDesc::RegionType::FROM_REGION,
                RegionDesc::RegionType::RAW_POINTER_REGION)) {
            GCPhase phase = Heap::GetHeap().GetGCPhase();
            CHECK(phase != GCPhase::GC_PHASE_COPY && phase != GCPhase::GC_PHASE_PRECOPY);
            regionManager_.AddRawPointerRegion(region);
        } else {
            CHECK(region->GetRegionType() != RegionDesc::RegionType::LONE_FROM_REGION);
        }
    }

    void RemoveRawPointerObject(BaseObject* obj)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        region->DecRawPointerObjectCount();
    }

    void AddRawPointerRegion(RegionDesc* region)
    {
        regionManager_.AddRawPointerRegion(region);
    }

    void CopyRegion(RegionDesc* region);

    void VisitRememberSet(const std::function<void(BaseObject *)> &func);

    friend class Allocator;
private:
    enum class TryAllocationThreshold {
        RESCHEDULE = 3,
        TRIGGER_OOM = 5,
    };
    HeapAddress TryAllocateOnce(size_t allocSize, AllocType allocType);
    bool ShouldRetryAllocation(size_t& tryTimes) const;
    HeapAddress reservedStart_ = 0;
    HeapAddress reservedEnd_ = 0;
    RegionManager regionManager_;
    MemoryMap* map_{ nullptr };

    YoungSpace youngSpace_;
    OldSpace oldSpace_;

    FromSpace fromSpace_;
    ToSpace toSpace_;
};

using RegionalHeap = RegionSpace;
} // namespace common

#endif // COMMON_COMPONENTS_HEAP_ALLOCATOR_REGION_SPACE_H
