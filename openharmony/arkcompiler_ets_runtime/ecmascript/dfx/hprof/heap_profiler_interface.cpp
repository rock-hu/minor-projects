/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ecmascript/dfx/hprof/heap_profiler_interface.h"
#include "ecmascript/dfx/hprof/heap_profiler.h"
#include "ecmascript/runtime.h"


namespace panda::ecmascript {
HeapProfilerInterface *HeapProfilerInterface::GetInstance(EcmaVM *vm)
{
    return vm->GetOrNewHeapProfile();
}

void HeapProfilerInterface::Destroy(EcmaVM *vm)
{
    vm->DeleteHeapProfile();
}

HeapProfilerInterface *HeapProfilerInterface::CreateNewInstance(const EcmaVM *vm)
{
    return new HeapProfiler(vm);
}

void HeapProfilerInterface::DestroyInstance(HeapProfilerInterface *heapProfiler)
{
    delete heapProfiler;
}

void panda::ecmascript::HeapProfilerInterface::DumpHeapSnapshotForCMCOOM(void *thread)
{
#if defined(ECMASCRIPT_SUPPORT_SNAPSHOT) && defined(ENABLE_DUMP_IN_FAULTLOG)
    EcmaVM *vm = Runtime::GetInstance()->GetMainThread()->GetEcmaVM();
    if (thread != nullptr) {
        vm = reinterpret_cast<JSThread *>(thread)->GetEcmaVM();
    }

    auto appfreezeCallback = Runtime::GetInstance()->GetAppFreezeFilterCallback();
    std::string eventConfig;
    bool shouldDump = appfreezeCallback == nullptr || appfreezeCallback(getprocpid(), true, eventConfig);
    vm->GetEcmaGCKeyStats()->SendSysEventBeforeDump("OOMDump", 0, 0, eventConfig);
    if (!shouldDump) {
        LOG_ECMA(INFO) << "DumpHeapSnapshotForCMCOOM, no dump quota.";
        return;
    }

    DumpSnapShotOption dumpOption;
    dumpOption.dumpFormat = panda::ecmascript::DumpFormat::BINARY;
    dumpOption.isFullGC = false;
    dumpOption.isDumpOOM = true;

    vm->GetOrNewHeapProfile()->DumpHeapSnapshotForOOM(dumpOption);
#endif
}
}  // namespace panda::ecmascript
