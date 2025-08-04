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

#include "common_components/heap/allocator/fix_heap.h"

#include "common_components/heap/ark_collector/ark_collector.h"
#include "common_runtime/hooks.h"

namespace common {

void FixHeapWorker::CollectFixHeapTasks(FixHeapTaskList &taskList, RegionList &list, FixRegionType type)
{
    list.VisitAllRegions([&taskList, type](RegionDesc *region) { taskList.emplace_back(region, type); });
}

void FixHeapWorker::FixOldRegion(RegionDesc *region)
{
    auto visitFunc = [this, &region](BaseObject *object) {
        DLOG(FIX, "fix: old obj %p<%p>(%zu)", object, object->GetTypeInfo(), object->GetSize());
        collector_->FixObjectRefFields(object);
    };
    region->VisitRememberSet(visitFunc);
}

void FixHeapWorker::FixRecentOldRegion(RegionDesc *region)
{
    auto visitFunc = [this, &region](BaseObject *object) {
        DLOG(FIX, "fix: old obj %p<%p>(%zu)", object, object->GetTypeInfo(), object->GetSize());
        collector_->FixObjectRefFields(object);
    };
    region->VisitRememberSetBeforeCopy(visitFunc);
}

void FixHeapWorker::FixToRegion(RegionDesc *region)
{
    region->VisitAllObjects([this](BaseObject *object) { collector_->FixObjectRefFields(object); });
}

template <FixHeapWorker::DeadObjectHandlerType type>
void FixHeapWorker::FixRegion(RegionDesc *region)
{
    size_t cellCount = 0;
    if constexpr (type == FixHeapWorker::COLLECT_FIXED_PINNED) {
        cellCount = region->GetRegionCellCount();
    }

    region->VisitAllObjects([this, region, cellCount](BaseObject *object) {
        if (collector_->IsSurvivedObject(object)) {
            collector_->FixObjectRefFields(object);
        } else {
            if constexpr (type == FixHeapWorker::FILL_FREE) {
                FillFreeObject(object, RegionSpace::GetAllocSize(*object));
            } else if constexpr (type == FixHeapWorker::COLLECT_FIXED_PINNED) {
                result_.fixedPinnedGarbages.emplace_back(region, object, cellCount);
            } else if constexpr (type == FixHeapWorker::COLLECT_PINNED) {
                result_.pinnedGarbages.emplace_back(object, RegionSpace::GetAllocSize(*object));
            } else if constexpr (type == FixHeapWorker::IGNORED) {
                /* Ignore */
            }
            DLOG(FIX, "fix: skip dead obj %p<%p>(%zu)", object, object->GetTypeInfo(), object->GetSize());
        }
    });
}

template <FixHeapWorker::DeadObjectHandlerType type>
void FixHeapWorker::FixRecentRegion(RegionDesc *region)
{
    size_t cellCount = 0;
    if constexpr (type == FixHeapWorker::COLLECT_FIXED_PINNED) {
        cellCount = region->GetRegionCellCount();
    }

    region->VisitAllObjectsBeforeCopy([this, region, cellCount](BaseObject *object) {
        if (region->IsNewObjectSinceMarking(object) || collector_->IsSurvivedObject(object)) {
            collector_->FixObjectRefFields(object);
        } else {  // handle dead objects in tl-regions for concurrent gc.
            if constexpr (type == FixHeapWorker::FILL_FREE) {
                FillFreeObject(object, RegionSpace::GetAllocSize(*object));
            } else if constexpr (type == FixHeapWorker::COLLECT_FIXED_PINNED) {
                result_.fixedPinnedGarbages.emplace_back(region, object, cellCount);
            } else if constexpr (type == FixHeapWorker::COLLECT_PINNED) {
                result_.pinnedGarbages.emplace_back(object, RegionSpace::GetAllocSize(*object));
            } else if constexpr (type == FixHeapWorker::IGNORED) {
                /* Ignore */
            }
            DLOG(FIX, "skip dead obj %p<%p>(%zu)", object, object->GetTypeInfo(), object->GetSize());
        }
    });
}

bool FixHeapWorker::Run([[maybe_unused]] uint32_t threadIndex)
{
    ThreadLocal::SetThreadType(ThreadType::GC_THREAD);
    auto *task = getNextTask_();
    while (task != nullptr) {
        DispatchRegionFixTask(task);
        task = getNextTask_();
    }
    ThreadLocal::SetThreadType(ThreadType::ARK_PROCESSOR);
    monitor_.NotifyFinishOne();
    return true;
}

void FixHeapWorker::DispatchRegionFixTask(FixHeapTask *task)
{
    result_.numProcessedRegions += 1;
    RegionDesc *region = task->region;
    switch (task->type) {
        case FIX_OLD_REGION:
            FixOldRegion(region);
            break;
        case FIX_RECENT_OLD_REGION:
            FixRecentOldRegion(region);
            break;
        case FIX_RECENT_REGION:
            if (region->IsFixedRegion()) {
                FixRecentRegion<COLLECT_FIXED_PINNED>(region);
            } else if (region->IsPinnedRegion()) {
                FixRecentRegion<COLLECT_PINNED>(region);
            } else if (region->IsLargeRegion()) {
                FixRecentRegion<IGNORED>(region);
            } else {
                FixRecentRegion<FILL_FREE>(region);
            }
            break;
        case FIX_REGION:
            if (region->IsFixedRegion()) {
                FixRegion<COLLECT_FIXED_PINNED>(region);
            } else if (region->IsPinnedRegion()) {
                FixRegion<COLLECT_PINNED>(region);
            } else if (region->IsLargeRegion()) {
                FixRegion<IGNORED>(region);
            } else {
                FixRegion<FILL_FREE>(region);
            }
            break;
        case FIX_TO_REGION:
            FixToRegion(region);
            break;
        default:
            UNREACHABLE();
    }
}

std::stack<std::pair<RegionList *, RegionDesc *>> PostFixHeapWorker::emptyRegionsToCollect {};

void PostFixHeapWorker::PostClearTask()
{
    for (auto [region, object, cellCount] : result_.fixedPinnedGarbages) {
        region->CollectPinnedGarbage(object, cellCount);
    }
    for (auto [object, size] : result_.pinnedGarbages) {
        FillFreeObject(object, size);
    }
    DLOG(FIX, "Fix heap worker processed %d Regions, %d fixedPinnedGarbages, %d pinnedGarbages",
         result_.numProcessedRegions, result_.fixedPinnedGarbages.size(), result_.pinnedGarbages.size());
}

bool PostFixHeapWorker::Run([[maybe_unused]] uint32_t threadIndex)
{
    ThreadLocal::SetThreadType(ThreadType::GC_THREAD);
    PostClearTask();
    ThreadLocal::SetThreadType(ThreadType::ARK_PROCESSOR);
    monitor_.NotifyFinishOne();
    return true;
}

void PostFixHeapWorker::AddEmptyRegionToCollectDuringPostFix(RegionList *list, RegionDesc *region)
{
    PostFixHeapWorker::emptyRegionsToCollect.emplace(list, region);
}

void PostFixHeapWorker::CollectEmptyRegions()
{
    RegionSpace &theAllocator = reinterpret_cast<RegionSpace &>(Heap::GetHeap().GetAllocator());
    RegionManager &regionManager = theAllocator.GetRegionManager();
    GCStats &stats = Heap::GetHeap().GetCollector().GetGCStats();
    size_t garbageSize = 0;

    while (!PostFixHeapWorker::emptyRegionsToCollect.empty()) {
        auto [list, del] = PostFixHeapWorker::emptyRegionsToCollect.top();
        PostFixHeapWorker::emptyRegionsToCollect.pop();

        list->DeleteRegion(del);
        garbageSize += regionManager.CollectRegion(del);
    }
    stats.pinnedGarbageSize += garbageSize;
}

};  // namespace common
