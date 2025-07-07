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
#ifndef COMMON_COMPONENTS_HEAP_COLLECTOR_GCREQUEST_H
#define COMMON_COMPONENTS_HEAP_COLLECTOR_GCREQUEST_H

#include <cstdint>
#include <limits>

#include "common_components/base/globals.h"

namespace common {
// Minimum time between async GC (heuristic, native).
constexpr uint64_t MIN_ASYNC_GC_INTERVAL_NS = SECOND_TO_NANO_SECOND;
constexpr uint64_t LONG_MIN_HEU_GC_INTERVAL_NS = 200 * MILLI_SECOND_TO_NANO_SECOND;

// Used by Collector::RequestGC.
// It tells why GC is triggered.
//
// sync: Caller of Collector::RequestGC will wait until GC completes.
// async: Collector::RequestGC returns immediately and caller continues to run.
enum GCReason : uint32_t {
    GC_REASON_USER = 0,     // Triggered by user explicitly.
    GC_REASON_OOM,          // Out of memory. Failed to allocate object.
    GC_REASON_BACKUP,       // backup gc is triggered if no other reason triggers gc for a long time.
    GC_REASON_HEU,          // Statistics show it is worth doing GC. Does not have to be immediate.
    GC_REASON_YOUNG,        // Statistics show it is worth doing Young GC. Does not have to be immediate.
    GC_REASON_NATIVE,       // Native-Allocation-Registry shows it's worth doing GC.
    GC_REASON_HEU_SYNC,     // Just wait one gc request to reduce heap fragmentation.
    GC_REASON_NATIVE_SYNC,  // Just wait one gc request to reduce native heap consumption.
    GC_REASON_FORCE,        // force gc is triggered when runtime triggers gc actively.
    GC_REASON_APPSPAWN,     // appspawn gc is triggered when prefork.
    GC_REASON_BACKGROUND,   // trigger gc caused by switching to background.
    GC_REASON_HINT,         // trigger gc caused by hint gc.
    GC_REASON_MAX,
    GC_REASON_INVALID = std::numeric_limits<uint32_t>::max(),
};

struct GCRequest {
    const GCReason reason;
    const char* name; // Human-readable names of GC reasons.

    const bool isSync;
    const bool isConcurrent;

    uint64_t minIntervelNs;
    uint64_t prevRequestTime;

    inline bool IsFrequentGC() const;
    inline bool IsFrequentAsyncGC() const;
    inline bool IsFrequentHeuristicGC() const;
    bool ShouldBeIgnored() const;

    bool IsSyncGC() const { return isSync; }

    void SetMinInterval(const uint64_t intervalNs) { minIntervelNs = intervalNs; }
    void SetPrevRequestTime(uint64_t timestamp) { prevRequestTime = timestamp; }
};

// Defined in gcRequest.cpp
extern GCRequest g_gcRequests[];
} // namespace common

#endif  // COMMON_COMPONENTS_HEAP_COLLECTOR_GC_DEBUGGER_H
