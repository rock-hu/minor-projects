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
#ifndef ARK_COMMON_REGION_MANAGER_H
#define ARK_COMMON_REGION_MANAGER_H

#include <list>
#include <map>
#include <set>
#include <thread>
#include <vector>

#include "common_components/common_runtime/src/common/run_type.h"
#include "common_components/common_runtime/src/heap/allocator/alloc_buffer.h"
#include "common_components/common_runtime/src/heap/allocator/allocator.h"
#include "common_components/common_runtime/src/heap/allocator/free_region_manager.h"
#include "common_components/common_runtime/src/heap/allocator/region_list.h"
#include "common_components/common_runtime/src/heap/allocator/slot_list.h"
#include "common_components/common_runtime/src/heap/gc_thread_pool.h"

namespace panda {
class TraceCollector;
class CompactCollector;

struct FreePinnedSlotLists {
    static constexpr size_t ATOMIC_OBJECT_SIZE = 16;
    SlotList freeAtomicSlotList;
    SlotList freeSyncSlotList;

    uintptr_t PopFront(size_t size)
    {
        switch (size) {
            case ATOMIC_OBJECT_SIZE:
                return freeAtomicSlotList.PopFront(size);
            default:
                return 0;
        }
    }

    void PushFront(BaseObject* slot)
    {
        size_t size = slot->GetSize();
        switch (size) {
            case ATOMIC_OBJECT_SIZE:
                freeAtomicSlotList.PushFront(slot);
                break;
            default:
                return;
        }
    }

    void Clear()
    {
        freeAtomicSlotList.Clear();
        freeSyncSlotList.Clear();
    }
};

// RegionManager needs to know header size and alignment in order to iterate objects linearly
// and thus its Alloc should be rewrite with AllocObj(objSize)
class RegionManager {
public:
    /* region memory layout:
        1. region info for each region, part of heap metadata
        2. region space for allocation, i.e., the heap
    */
    static size_t GetHeapMemorySize(size_t heapSize)
    {
        size_t regionNum = GetHeapUnitCount(heapSize);
        size_t metadataSize = GetMetadataSize(regionNum);
        size_t totalSize = metadataSize + RoundUp<size_t>(heapSize, RegionDesc::UNIT_SIZE);
        return totalSize;
    }

    static size_t GetHeapUnitCount(size_t heapSize)
    {
        heapSize = RoundUp<size_t>(heapSize, RegionDesc::UNIT_SIZE);
        size_t regionNum = heapSize / RegionDesc::UNIT_SIZE;
        return regionNum;
    }

    static size_t GetMetadataSize(size_t regionNum)
    {
        size_t metadataSize = regionNum * sizeof(RegionDesc);
        return RoundUp<size_t>(metadataSize, panda::ARK_COMMON_PAGE_SIZE);
    }

    void Initialize(size_t regionNum, uintptr_t regionInfoStart);

    RegionManager()
        : freeRegionManager_(*this), tlRegionList_("thread local regions"),
          recentFullRegionList_("recent full regions"), fullTraceRegions_("full trace regions"),
          fromRegionList_("from regions"), ghostFromRegionList_("ghost from regions"),
          exemptedFromRegionList_("escaped from regions"), toRegionList_("to-regions"),
          tlToRegionList_("tl-to-regions"), garbageRegionList_("garbage regions"),
          recentPinnedRegionList_("recent pinned regions"), oldPinnedRegionList_("old pinned regions"),
          rawPointerRegionList_("raw pointer pinned regions"), oldLargeRegionList_("old large regions"),
          recentLargeRegionList_("recent large regions"), largeTraceRegions_("large trace regions")
    {}

    RegionManager(const RegionManager&) = delete;

    RegionManager& operator=(const RegionManager&) = delete;

    RegionDesc* AllocateThreadLocalRegion(bool expectPhysicalMem = false);

    void CopyFromRegions(GCThreadPool* threadPool);
    void CopyFromRegions();
    void CopyRegion(RegionDesc* region);
    void FixAllRegionLists();

    using RootSet = MarkStack<BaseObject*>;

    void ExemptFromRegion(RegionDesc* region);

#if defined(GCINFO_DEBUG) && GCINFO_DEBUG
    void DumpRegionDesc() const;
#endif

    void DumpRegionStats(const char* msg) const;

    uintptr_t GetInactiveZone() const { return inactiveZone_; }

    uintptr_t GetRegionHeapStart() const { return regionHeapStart_; }

    RegionDesc* GetFirstRegion() const
    {
        if (regionHeapStart_ < inactiveZone_) {
            return RegionDesc::GetRegionDescAt(regionHeapStart_);
        }
        return nullptr;
    }

    ~RegionManager() = default;

    // take a region with *num* units for allocation
    RegionDesc* TakeRegion(size_t num, RegionDesc::UnitRole, bool expectPhysicalMem = false, bool allowgc = true);

    // only used for deserialize allocation, allocate one region and regard it as full region
    // adapt for concurrent gc
    uintptr_t AllocRegion();
    uintptr_t AllocPinnedRegion();
    uintptr_t AllocLargeReion(size_t size);

    uintptr_t AllocPinnedFromFreeList(size_t size);

    uintptr_t AllocPinned(size_t size, bool allowGC = true)
    {
        uintptr_t addr = 0;
        std::mutex& regionListMutex = recentPinnedRegionList_.GetListMutex();

        // enter saferegion when wait lock to avoid gc timeout.
        // note that release the mutex when function end.
        {
            // stw gc may deadlock here
            // ScopedEnterSaferegion enterSaferegion(true)
            // regionListMutex.lock()
        }
        std::lock_guard<std::mutex> lock(regionListMutex);
        RegionDesc* headRegion = recentPinnedRegionList_.GetHeadRegion();
        if (headRegion != nullptr) {
            addr = headRegion->Alloc(size);
        }
        if (addr == 0) {
            addr = AllocPinnedFromFreeList(size);
        }
        if (addr == 0) {
            RegionDesc* region =
                TakeRegion(maxUnitCountPerRegion_, RegionDesc::UnitRole::SMALL_SIZED_UNITS, false, allowGC);
            if (region == nullptr) {
                return 0;
            }
            DLOG(REGION, "alloc pinned region @0x%zx+%zu type %u", region->GetRegionStart(),
                 region->GetRegionAllocatedSize(),
                 region->GetRegionType());
            GCPhase phase = Mutator::GetMutator()->GetMutatorPhase();
            if (phase == GC_PHASE_ENUM || phase == GC_PHASE_MARK || phase == GC_PHASE_REMARK_SATB ||
                phase == GC_PHASE_POST_MARK) {
                region->SetTraceLine();
            } else if (phase == GC_PHASE_PRECOPY || phase == GC_PHASE_COPY) {
                region->SetCopyLine();
            } else if (phase == GC_PHASE_FIX) {
                region->SetCopyLine();
                region->SetFixLine();
            }

            // To make sure the allocedSize are consistent, it must prepend region first then alloc object.
            recentPinnedRegionList_.PrependRegionLocked(region, RegionDesc::RegionType::RECENT_PINNED_REGION);
            addr = region->Alloc(size);
        }

        DLOG(ALLOC, "alloc pinned obj 0x%zx(%zu)", addr, size);
        return addr;
    }

    // note: AllocSmall() is always performed by region owned by mutator thread
    // thus no need to do in RegionManager
    // caller assures size is truely large (> region size)
    uintptr_t AllocLarge(size_t size, bool allowGC = true)
    {
        size_t regionCount = (size + RegionDesc::UNIT_SIZE - 1) / RegionDesc::UNIT_SIZE;
        RegionDesc* region = TakeRegion(regionCount, RegionDesc::UnitRole::LARGE_SIZED_UNITS, false, allowGC);
        if (region == nullptr) {
            return 0;
        }
        GCPhase phase = Mutator::GetMutator()->GetMutatorPhase();
        if (phase == GC_PHASE_ENUM || phase == GC_PHASE_MARK || phase == GC_PHASE_REMARK_SATB ||
            phase == GC_PHASE_POST_MARK) {
            region->SetTraceLine();
        } else if (phase == GC_PHASE_PRECOPY || phase == GC_PHASE_COPY) {
            region->SetCopyLine();
        } else if (phase == GC_PHASE_FIX) {
            region->SetCopyLine();
            region->SetFixLine();
        }

        DLOG(REGION, "alloc large region @0x%zx+%zu type %u", region->GetRegionStart(),
             region->GetRegionSize(), region->GetRegionType());
        uintptr_t addr = region->Alloc(size);
        recentLargeRegionList_.PrependRegion(region, RegionDesc::RegionType::RECENT_LARGE_REGION);
        return addr;
    }

    void EnlistFullThreadLocalRegion(RegionDesc* region) noexcept
    {
        ASSERT_LOGF(region->IsThreadLocalRegion(), "unexpected region type");
        if (IsGcThread()) {
            EnlistToRegion(region);
        } else {
            recentFullRegionList_.PrependRegion(region, RegionDesc::RegionType::RECENT_FULL_REGION);
        }
    }

    void EnlistToRegion(RegionDesc* region) noexcept
    {
        ASSERT_LOGF(Heap::GetHeap().IsGcStarted(), "GC is not started");
        toRegionList_.PrependRegion(region, RegionDesc::RegionType::TO_REGION);
    }

    void RemoveThreadLocalRegion(RegionDesc* region) noexcept
    {
        ASSERT_LOGF(region->IsThreadLocalRegion(), "unexpected region type");
        if (IsGcThread()) {
            tlToRegionList_.DeleteRegion(region);
        } else {
            tlRegionList_.DeleteRegion(region);
        }
    }

    void CountLiveObject(const BaseObject* obj);

    void AssembleSmallGarbageCandidates();
    void AssembleLargeGarbageCandidates();
    void AssemblePinnedGarbageCandidates(bool collectAll);

    void CollectFromSpaceGarbage()
    {
        garbageRegionList_.MergeRegionList(fromRegionList_, RegionDesc::RegionType::GARBAGE_REGION);
    }

    size_t CollectRegion(RegionDesc* region)
    {
        DLOG(REGION, "collect region %p@%#zx+%zu type %u", region, region->GetRegionStart(),
             region->GetLiveByteCount(), region->GetRegionType());

        garbageRegionList_.PrependRegion(region, RegionDesc::RegionType::GARBAGE_REGION);

        if (region->IsLargeRegion()) {
            return region->GetRegionSize();
        } else {
            return region->GetRegionSize() - region->GetLiveByteCount();
        }
    }

    void AddRawPointerObject(BaseObject* obj)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        region->IncRawPointerObjectCount();
        if (region->IsFromRegion() &&
            fromRegionList_.TryDeleteRegion(region, RegionDesc::RegionType::FROM_REGION,
                                            RegionDesc::RegionType::RAW_POINTER_REGION)) {
            GCPhase phase = Heap::GetHeap().GetGCPhase();
            CHECK_CC(phase != GCPhase::GC_PHASE_COPY && phase != GCPhase::GC_PHASE_PRECOPY);
            if (phase == GCPhase::GC_PHASE_POST_MARK) {
                // region->ClearGhostRegionBit()
            };
            rawPointerRegionList_.PrependRegion(region, RegionDesc::RegionType::RAW_POINTER_REGION);
        } else {
            CHECK_CC(region->GetRegionType() != RegionDesc::RegionType::LONE_FROM_REGION);
        }
    }

    void RemoveRawPointerObject(BaseObject* obj)
    {
        RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<HeapAddress>(obj));
        region->DecRawPointerObjectCount();
    }

    void ReclaimRegion(RegionDesc* region);
    size_t ReleaseRegion(RegionDesc* region);

    void ReclaimGarbageRegions()
    {
        RegionDesc* garbage = garbageRegionList_.TakeHeadRegion();
        while (garbage != nullptr) {
            ReclaimRegion(garbage);
            garbage = garbageRegionList_.TakeHeadRegion();
        }
    }

    size_t CollectLargeGarbage();

    size_t CollectPinnedGarbage();
    void CollectFreePinnedSlots(RegionDesc* region);

    // targetSize: size of memory which we do not release and keep it as cache for future allocation.
    size_t ReleaseGarbageRegions(size_t targetSize) { return freeRegionManager_.ReleaseGarbageRegions(targetSize); }

    // these methods are helpers for compaction. Since pinned object can not be moved during compaction,
    // we first virtually reclaim all compactable heap memory, which are handled in cartesian tree. So far we get a map
    // of heap memory about which region can be used for compaction.
    void ExemptFromRegions();
    void ReassembleFromSpace();

    void ForEachObjectUnsafe(const std::function<void(BaseObject*)>& visitor) const;
    void ForEachObjectSafe(const std::function<void(BaseObject*)>& visitor) const;

    size_t GetUsedRegionSize() const { return GetUsedUnitCount() * RegionDesc::UNIT_SIZE; }

    size_t GetRecentAllocatedSize() const
    {
        return recentFullRegionList_.GetAllocatedSize() + recentLargeRegionList_.GetAllocatedSize() +
            recentPinnedRegionList_.GetAllocatedSize();
    }

    size_t GetSurvivedSize() const
    {
        return fromRegionList_.GetAllocatedSize() + exemptedFromRegionList_.GetAllocatedSize() +
            toRegionList_.GetAllocatedSize() + tlToRegionList_.GetAllocatedSize() +
            oldPinnedRegionList_.GetAllocatedSize() + oldLargeRegionList_.GetAllocatedSize();
    }

    size_t GetUsedUnitCount() const
    {
        return fromRegionList_.GetUnitCount() + exemptedFromRegionList_.GetUnitCount() + toRegionList_.GetUnitCount() +
            tlToRegionList_.GetUnitCount() + recentFullRegionList_.GetUnitCount() + oldLargeRegionList_.GetUnitCount() +
            recentLargeRegionList_.GetUnitCount() + oldPinnedRegionList_.GetUnitCount() +
            recentPinnedRegionList_.GetUnitCount() + rawPointerRegionList_.GetUnitCount() +
            largeTraceRegions_.GetUnitCount() + fullTraceRegions_.GetUnitCount() + tlRegionList_.GetUnitCount();
    }

    size_t GetDirtyUnitCount() const { return freeRegionManager_.GetDirtyUnitCount(); }

    size_t GetInactiveUnitCount() const { return (regionHeapEnd_ - inactiveZone_) / RegionDesc::UNIT_SIZE; }

    inline size_t GetLargeObjectSize() const
    {
        return oldLargeRegionList_.GetAllocatedSize() + recentLargeRegionList_.GetAllocatedSize();
    }

    size_t GetAllocatedSize() const
    {
        size_t threadLocalSize = 0;
        AllocBufferVisitor visitor = [&threadLocalSize](AllocationBuffer& regionBuffer) {
            RegionDesc* region = regionBuffer.GetRegion();
            if (UNLIKELY_CC(region == RegionDesc::NullRegion())) {
                return;
            }
            threadLocalSize += region->GetRegionAllocatedSize();
        };
        Heap::GetHeap().GetAllocator().VisitAllocBuffers(visitor);
        // exclude garbageRegionList for live object set.
        return fromRegionList_.GetAllocatedSize() + exemptedFromRegionList_.GetAllocatedSize() +
            toRegionList_.GetAllocatedSize() + tlToRegionList_.GetAllocatedSize() +
            recentFullRegionList_.GetAllocatedSize() + oldLargeRegionList_.GetAllocatedSize() +
            recentLargeRegionList_.GetAllocatedSize() + oldPinnedRegionList_.GetAllocatedSize() +
            recentPinnedRegionList_.GetAllocatedSize() + rawPointerRegionList_.GetAllocatedSize() +
            largeTraceRegions_.GetAllocatedSize() + fullTraceRegions_.GetAllocatedSize() + threadLocalSize;
    }

    inline size_t GetFromSpaceSize() const { return fromRegionList_.GetAllocatedSize(); }

    inline size_t GetPinnedSpaceSize() const
    {
        return oldPinnedRegionList_.GetAllocatedSize() + recentPinnedRegionList_.GetAllocatedSize();
    }

    // valid only between forwarding phase and flipping phase.
    size_t GetToSpaceSize() const { return toRegionList_.GetAllocatedSize() + tlToRegionList_.GetAllocatedSize(); }

    RegionDesc* GetNextNeighborRegion(RegionDesc* region) const
    {
        HeapAddress address = region->GetRegionEnd();
        if (address < inactiveZone_.load()) {
            return RegionDesc::GetRegionDescAt(address);
        }
        return nullptr;
    }

    size_t GetLargeObjectThreshold() const { return largeObjectThreshold_; }

    // this method checks whether allocation is permitted for now, otherwise, it is suspened
    // until allocation does no harm to gc.
    void RequestForRegion(size_t size);

    void SetMaxUnitCountForRegion();
    void SetLargeObjectThreshold();
    void SetGarbageThreshold();

    void PrepareTrace()
    {
        AllocBufferVisitor visitor = [](AllocationBuffer& regionBuffer) {
            RegionDesc* region = regionBuffer.GetRegion();
            if (region != RegionDesc::NullRegion()) {
                region->SetTraceLine();
            }
        };
        Heap::GetHeap().GetAllocator().VisitAllocBuffers(visitor);

        RegionDesc* region = recentPinnedRegionList_.GetHeadRegion();
        if (region != nullptr && region != RegionDesc::NullRegion()) {
            region->SetTraceLine();
        }
    }

    void PrepareForward()
    {
        AllocBufferVisitor visitor = [](AllocationBuffer& regionBuffer) {
            RegionDesc* region = regionBuffer.GetRegion();
            if (region != RegionDesc::NullRegion()) {
                region->SetCopyLine();
            }
        };
        Heap::GetHeap().GetAllocator().VisitAllocBuffers(visitor);
    }

    void PrepareFix()
    {
        AllocationBuffer* buffer = AllocationBuffer::GetAllocBuffer();
        if (LIKELY_CC(buffer != nullptr)) {
            RegionDesc* region = buffer->GetRegion();
            if (region != RegionDesc::NullRegion()) {
                region->SetFixLine();
            }
        }

        RegionDesc* region = nullptr;
        // works only on single thread senario
        region = recentPinnedRegionList_.GetHeadRegion();
        if (region != nullptr && region != RegionDesc::NullRegion()) {
            region->SetFixLine();
        }
    }

    void ClearAllGCInfo()
    {
        ClearGCInfo(toRegionList_);
        ClearGCInfo(tlToRegionList_);
        ClearGCInfo(exemptedFromRegionList_);
        ClearGCInfo(oldLargeRegionList_);
        ClearGCInfo(tlRegionList_);
        ClearGCInfo(recentFullRegionList_);
        ClearGCInfo(recentLargeRegionList_);
        ClearGCInfo(recentPinnedRegionList_);
        ClearGCInfo(rawPointerRegionList_);
        ClearGCInfo(oldPinnedRegionList_);
    }
private:
    static const size_t MAX_UNIT_COUNT_PER_REGION;
    static const size_t HUGE_PAGE;
    inline void TagHugePage(RegionDesc* region, size_t num) const;
    inline void UntagHugePage(RegionDesc* region, size_t num) const;

    void ClearGCInfo(RegionList& list)
    {
        RegionList tmp("temp region list");
        list.CopyListTo(tmp);
        tmp.VisitAllRegions([](RegionDesc* region) {
            region->ClearTraceCopyFixLine();
            region->ClearLiveInfo();
            region->ResetMarkBit();
        });
    }

    FreeRegionManager freeRegionManager_;

    // region lists actually represent life cycle of regions.
    // each region must belong to only one list at any time.

    // regions for movable (small-sized) objects.
    // regions for thread-local allocation.
    // regions in this list are already used for allocation but not full yet, i.e. local regions.
    RegionList tlRegionList_;

    // recentFullRegionList is a list of regions which is already full, thus escape current gc.
    RegionList recentFullRegionList_;
    // RegionList fullRegionList; // mimic old space

    // if region is allocated during gc trace phase, it is called a trace-region, it is recorded here when it is full.
    RegionCache fullTraceRegions_;

    // fromRegionList is a list of full regions waiting to be collected (i.e. for forwarding).
    // region type must be FROM_REGION.
    RegionList fromRegionList_;
    RegionList ghostFromRegionList_;

    // regions exempted by ExemptFromRegions, which will not be moved during current GC.
    RegionList exemptedFromRegionList_;

    // toRegionList is a list of to-space regions produced by gc threads.
    // when a region is prepended to this list, the region is probably not full, so the statistics
    // of this region-list are not reliable and need to be updated.
    RegionList toRegionList_;
    RegionList tlToRegionList_;

    // cache for fromRegionList after forwarding.
    RegionList garbageRegionList_;

    // regions for small-sized atomic objects when read-barrier is not used.
    // region lists for small-sized pinned objects which are not be moved during concurrent gc, but
    // may be moved during stw-compaction.
    RegionList recentPinnedRegionList_;
    RegionList oldPinnedRegionList_;

    // region lists for small-sized raw-pointer objects (i.e. future, monitor)
    // which can not be moved ever (even during compaction).
    RegionList rawPointerRegionList_; // delete rawPointerRegion, use PinnedRegion

    // regions for large-sized objects.
    // large region is recorded here after large object is allocated.
    RegionList oldLargeRegionList_;

    // if large region is allocated when gc is not running, it is recorded here.
    RegionList recentLargeRegionList_;

    // if large region is allocated during gc trace phase, it is called a trace-region,
    // it is recorded here when it is full.
    RegionCache largeTraceRegions_;

    uintptr_t regionInfoStart_ = 0; // the address of first RegionDesc

    uintptr_t regionHeapStart_ = 0; // the address of first region to allocate object
    uintptr_t regionHeapEnd_ = 0;

    // the time when previous region was allocated, which is assigned with returned value by timeutil::NanoSeconds().
    std::atomic<uint64_t> prevRegionAllocTime_ = { 0 };

    // heap space not allocated yet for even once. this value should not be decreased.
    std::atomic<uintptr_t> inactiveZone_ = { 0 };
    size_t maxUnitCountPerRegion_ = MAX_UNIT_COUNT_PER_REGION; // max units count for threadLocal buffer.
    size_t largeObjectThreshold_;
    double fromSpaceGarbageThreshold_ = 0.5; // 0.5: default garbage ratio.
    double exemptedRegionThreshold_;

    std::mutex freePinnedSlotListMutex_;
    FreePinnedSlotLists freePinnedSlotLists_;

    friend class VerifyIterator;
};

using FillFreeObjectHookType = void (*)(void *object, size_t size);

extern "C" PUBLIC_API void ArkRegisterFillFreeObjectHook(FillFreeObjectHookType hook);

} // namespace panda
#endif // ARK_COMMON_REGION_MANAGER_H
