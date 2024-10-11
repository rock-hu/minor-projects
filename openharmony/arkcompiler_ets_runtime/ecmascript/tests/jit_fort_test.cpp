/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#include "ecmascript/ecma_vm.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/tests/test_helper.h"

#include "ecmascript/mem/concurrent_marker.h"
#include "ecmascript/mem/partial_gc.h"
#include "ecmascript/mem/jit_fort_memdesc.h"
#include "ecmascript/mem/jit_fort.h"

using namespace panda;
using namespace panda::ecmascript;

namespace panda::test {
    
class JitFortTest :  public BaseTestWithScope<false> {
public:
    void SetUp() override
    {
        JSRuntimeOptions options;
        instance = JSNApi::CreateEcmaVM(options);
        ASSERT_TRUE(instance != nullptr) << "Cannot create EcmaVM";
        thread = instance->GetJSThread();
        thread->ManagedCodeBegin();
        scope = new EcmaHandleScope(thread);
        auto heap = const_cast<Heap *>(thread->GetEcmaVM()->GetHeap());
        heap->GetConcurrentMarker()->EnableConcurrentMarking(EnableConcurrentMarkType::ENABLE);
        heap->GetSweeper()->EnableConcurrentSweep(EnableConcurrentSweepType::ENABLE);
    }
};

HWTEST_F_L0(JitFortTest, AddRegionTest001)
{
    JitFort *jitFort = new JitFort();
    bool res = jitFort->AddRegion();
    ASSERT_EQ(res, true);
}

HWTEST_F_L0(JitFortTest, AddRegionTest002)
{
    JitFort *jitFort = new JitFort();
    for (size_t i = 0; i < 16; i++) {
        jitFort->AddRegion();
    }
    ASSERT_EQ(jitFort->AddRegion(), false);
}

HWTEST_F_L0(JitFortTest, AllocateTest001)
{
    MachineCodeDesc desc;
    desc.instructionsSize = 18;
    JitFort *jitFort = new JitFort();
    ASSERT_NE(jitFort, nullptr);
    jitFort->Allocate(&desc);
}

HWTEST_F_L0(JitFortTest, RecordLiveJitCodeNoLockTest001)
{
    JitFort *jitFort = new JitFort();
    jitFort->RecordLiveJitCodeNoLock(1, 1);
    MemDesc *desc = jitFort->RecordLiveJitCodeNoLock(1, 1);
    ASSERT_EQ(desc, nullptr);
}

HWTEST_F_L0(JitFortTest, RecordLiveJitCodeNoLockTest002)
{
    JitFort *jitFort = new JitFort();
    MemDesc *desc = jitFort->RecordLiveJitCodeNoLock(1, 1);
    ASSERT_NE(desc, nullptr);
}

HWTEST_F_L0(JitFortTest, SortLiveMemDescLisTest001)
{
    JitFort *jitFort = new JitFort();
    MemDesc *desc = jitFort->RecordLiveJitCodeNoLock(1, 1);
    ASSERT_EQ(jitFort->IsMachineCodeGC(), false);
    ASSERT_NE(desc, nullptr);
    jitFort->SortLiveMemDescList();
}

HWTEST_F_L0(JitFortTest, UpdateFreeSpaceTest001)
{
    JitFort *jitFort = new JitFort();
    jitFort->SetMachineCodeGC(true);
    ASSERT_EQ(jitFort->IsMachineCodeGC(), true);
    ASSERT_NE(jitFort, nullptr);
    jitFort->UpdateFreeSpace();
}

HWTEST_F_L0(JitFortTest, GetDescTest001)
{
    MemDescPool *pool = new MemDescPool(1, 1);
    ASSERT_NE(pool, nullptr);
    pool->GetDescFromPool();
}

HWTEST_F_L0(JitFortTest, MemDescPoolFreeTest001)
{
    MemDescPool *pool = new MemDescPool(1, 1);
    ASSERT_NE(pool, nullptr);
    pool->~MemDescPool();
}

HWTEST_F_L0(JitFortTest, CollectFreeRangesTest001)
{
    JitFort *jitFort = new JitFort();
    jitFort->AddRegion();
    jitFort->AddRegion();
    JitFortRegion *region = jitFort->GetRegionList();
    jitFort->CollectFreeRanges(region);
    ASSERT_NE(region, nullptr);
    ASSERT_TRUE(region->GetBegin() > 0);
}

HWTEST_F_L0(JitFortTest, CollectFreeRangesTest002)
{
    JitFort *jitFort = new JitFort();
    jitFort->AddRegion();
    jitFort->AddRegion();
    JitFortRegion *region = jitFort->GetRegionList();
    jitFort->RecordLiveJitCodeNoLock(region->GetBegin(), 10);
    jitFort->CollectFreeRanges(region);
    ASSERT_NE(region, nullptr);
    ASSERT_TRUE(region->GetBegin() > 0);
}

HWTEST_F_L0(JitFortTest, CollectFreeRangesTest003)
{
    JitFort *jitFort = new JitFort();
    jitFort->AddRegion();
    jitFort->AddRegion();
    JitFortRegion *region = jitFort->GetRegionList();
    jitFort->RecordLiveJitCodeNoLock(region->GetBegin(), 10, true);
    jitFort->CollectFreeRanges(region);
    ASSERT_NE(region, nullptr);
    ASSERT_TRUE(region->GetBegin() > 0);
}

HWTEST_F_L0(JitFortTest, CollectFreeRangesTest004)
{
    JitFort *jitFort = new JitFort();
    jitFort->AddRegion();
    jitFort->AddRegion();
    JitFortRegion *region = jitFort->GetRegionList();
    jitFort->RecordLiveJitCodeNoLock(region->GetBegin() + 1, 10, true);
    jitFort->CollectFreeRanges(region);
    ASSERT_NE(region, nullptr);
    ASSERT_TRUE(region->GetBegin() > 0);
}

HWTEST_F_L0(JitFortTest, InitRegionTest001)
{
    JitFort *jitFort = new JitFort();
    ASSERT_NE(jitFort, nullptr);
    jitFort->InitRegions();
}

HWTEST_F_L0(JitFortTest, RecordLiveJitCodeTest001)
{
    JitFort *jitFort = new JitFort();
    MemDesc *desc = jitFort->RecordLiveJitCode(1, 1);
    ASSERT_NE(desc, nullptr);
}

HWTEST_F_L0(JitFortTest, InRangeTest001)
{
    JitFort *jitFort = new JitFort();
    bool result = jitFort->InRange(1);
    ASSERT_EQ(result, false);
}

}