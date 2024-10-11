/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#ifndef ECMASCRIPT_DFX_HPROF_HEAP_PROFILER_INTERFACE_H
#define ECMASCRIPT_DFX_HPROF_HEAP_PROFILER_INTERFACE_H

#include <memory>

#include "libpandabase/macros.h"

namespace panda::ecmascript {
class EcmaVM;
class TaggedObject;
class Progress;
class Stream;
struct SamplingInfo;
enum class DumpFormat { JSON, BINARY, OTHER };
struct DumpSnapShotOption {
    DumpFormat dumpFormat; // dumpformat like JSON BINARY and OTHER
    bool isVmMode = true; // vmMode do more dump.
    bool isPrivate = false;
    bool captureNumericValue = false; // heapdump add numeric object.
    bool isFullGC = true; // whether do FullGC.
    bool isSimplify = false; // whether trim heapdump snapshot.
    bool isSync = true; // OOM and Ide dump need sync dump.
    bool isBeforeFill = true; // whether do fillmap on main thread.
    bool isDumpOOM = false; // whether dump oom heapdump.
};

class HeapProfilerInterface {
public:
    static HeapProfilerInterface *GetInstance(EcmaVM *vm);
    static void Destroy(EcmaVM *vm);

    HeapProfilerInterface() = default;
    virtual ~HeapProfilerInterface() = default;

    virtual size_t GetIdCount() = 0;
    virtual void AllocationEvent(TaggedObject *address, size_t size) = 0;
    virtual void MoveEvent(uintptr_t address, TaggedObject *forwardAddress, size_t size)= 0;
    virtual bool DumpHeapSnapshot(Stream *stream, const DumpSnapShotOption &dumpOption,
                                  Progress *progress = nullptr) = 0;
    // Provide an internal interface for oom dump.
    virtual void DumpHeapSnapshot(const DumpSnapShotOption &dumpOption) = 0;
    virtual bool GenerateHeapSnapshot(std::string &inputFilePath, std::string &outputPath) = 0;

    virtual bool StartHeapTracking(double timeInterval, bool isVmMode = true, Stream *stream = nullptr,
                                   bool traceAllocation = false, bool newThread = true) = 0;
    virtual bool UpdateHeapTracking(Stream *stream) = 0;
    virtual bool StopHeapTracking(Stream *stream, Progress *progress = nullptr, bool newThread = true) = 0;
    virtual bool StartHeapSampling(uint64_t samplingInterval, int stackDepth = 128) = 0;
    virtual void StopHeapSampling() = 0;
    virtual const struct SamplingInfo *GetAllocationProfile() = 0;

    NO_MOVE_SEMANTIC(HeapProfilerInterface);
    NO_COPY_SEMANTIC(HeapProfilerInterface);
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_DFX_HPROF_HEAP_PROFILER_INTERFACE_H
