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
#include "common_components/common_runtime/src/heap/collector/gc_stats.h"

#include "common_components/common_runtime/src/base/log_file.h"
#include "common_components/common_runtime/src/common_runtime.h"
#include "common_components/common_runtime/src/heap/heap.h"

namespace panda {
size_t g_gcCount = 0;
uint64_t g_gcTotalTimeUs = 0;
size_t g_gcCollectedTotalBytes = 0;

uint64_t GCStats::prevGcStartTime = TimeUtil::NanoSeconds() - LONG_MIN_HEU_GC_INTERVAL_NS;
uint64_t GCStats::prevGcFinishTime = TimeUtil::NanoSeconds() - LONG_MIN_HEU_GC_INTERVAL_NS;

void GCStats::Init()
{
    isConcurrentMark = false;
    async = false;
    stw1Time = 0;
    stw2Time = 0;
    gcStartTime = TimeUtil::NanoSeconds();
    gcEndTime = TimeUtil::NanoSeconds();
    collectedObjects = 0;
    collectedBytes = 0;

    fromSpaceSize = 0;
    smallGarbageSize = 0;

    pinnedSpaceSize = 0;
    pinnedGarbageSize = 0;

    largeSpaceSize = 0;
    largeGarbageSize = 0;

    liveBytesBeforeGC = 0;
    liveBytesAfterGC = 0;

    garbageRatio = 0.0;
    collectionRate = 0.0;

    // 20 MB:set 20 MB as intial value
    heapThreshold = std::min(ArkCommonRuntime::GetGCParam().gcThreshold, 20 * MB);
    // 0.2:set 20% heap size as intial value
    heapThreshold = std::min(static_cast<size_t>(Heap::GetHeap().GetMaxCapacity() * 0.2), heapThreshold);
}

void GCStats::Dump() const
{
    // Print a summary of the last GC.
    size_t liveSize = Heap::GetHeap().GetAllocatedSize();
    size_t heapSize = Heap::GetHeap().GetUsedPageSize();
    double utilization = (heapSize == 0) ? 0 : (static_cast<double>(liveSize) / heapSize);
    // Do not change this GC log format.
    // Output one line statistic info after each gc task,
    // include the gc type, collected objects and current heap utilization, etc.
    // display to std-output. take care to modify.
    LOG_COMMON(INFO) <<
        "GC for " << g_gcRequests[reason].name << ": " << (async ? "async:" : "sync:") << " collected objects: " <<
        collectedObjects << "(" << collectedBytes << "->" << PrettyOrderInfo(collectedBytes, "B").Str() << "), " <<
        (utilization * 100) << "% utilization (" << liveSize <<  // 100 for percentage.
        "->" << PrettyOrderInfo(liveSize, "B").Str() << "/" << heapSize << "->" <<
        PrettyOrderInfo(heapSize, "B").Str() << "), max pause: " << MaxSTWTime() <<
        "->" << PrettyOrderMathNano(MaxSTWTime(), "s").Str() << ", total pause: " <<
        TotalSTWTime() << "->" << PrettyOrderMathNano(TotalSTWTime(), "s").Str() <<
        ", total GC time: " << (gcEndTime - gcStartTime) << "->" <<
        PrettyOrderMathNano(gcEndTime - gcStartTime, "s").Str();

    VLOG(REPORT, "allocated size: %s, heap size: %s, heap utilization: %.2f%%", Pretty(liveSize).Str(),
         Pretty(heapSize).Str(), utilization * 100); // 100 for percentage.
}
} // namespace panda
