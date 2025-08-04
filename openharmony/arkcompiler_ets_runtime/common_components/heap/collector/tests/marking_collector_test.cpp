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

#include "common_components/heap/collector/marking_collector.h"
#include "common_components/heap/heap_manager.h"
#include "common_components/heap/ark_collector/ark_collector.h"
#include "common_components/mutator/mutator_manager.h"
#include "common_components/tests/test_helper.h"
#include <cstdint>

using namespace common;

namespace common::test {
class MarkingCollectorTest : public common::test::BaseTestWithScope {
protected:
    static void SetUpTestCase()
    {
        BaseRuntime::GetInstance()->Init();
    }

    static void TearDownTestCase()
    {
        BaseRuntime::GetInstance()->Fini();
    }
    void SetUp() override
    {
        MutatorManager::Instance().CreateRuntimeMutator(ThreadType::ARK_PROCESSOR);
    }

    void TearDown() override
    {
        MutatorManager::Instance().DestroyRuntimeMutator(ThreadType::ARK_PROCESSOR);
    }

    StaticRootTable rootTable_;
    bool ContainsRoot(StaticRootTable& table, const StaticRootTable::StaticRootArray* array, uint32_t size)
    {
        bool found = false;
        auto visitor = [&found, array, size](RefField<>& root) {
            for (uint32_t i = 0; i < size; ++i) {
                if (&root == array->content[i]) {
                    found = true;
                    return;
                }
            }
        };
        table.VisitRoots(visitor);
        return found;
    }
    class TableMarkingCollctor : public MarkingCollector {
    public:
        using MarkingCollector::SetGCReason;
        using MarkingCollector::MarkingRoots;
        using MarkingCollector::PushRootToWorkStack;
        using MarkingCollector::UpdateNativeThreshold;
    };
};

HWTEST_F_L0(MarkingCollectorTest, RunGarbageCollection)
{
    MarkingCollector& collector = reinterpret_cast<MarkingCollector&>(Heap::GetHeap().GetCollector());
    Heap::GetHeap().SetGCReason(GCReason::GC_REASON_YOUNG);
    collector.RunGarbageCollection(0, GCReason::GC_REASON_USER, common::GC_TYPE_FULL);
    ASSERT_FALSE(Heap::GetHeap().GetCollector().GetGCStats().isYoungGC());

    Heap::GetHeap().SetGCReason(GCReason::GC_REASON_BACKUP);
    collector.RunGarbageCollection(0, GCReason::GC_REASON_OOM, common::GC_TYPE_FULL);
    ASSERT_FALSE(Heap::GetHeap().GetCollector().GetGCStats().isYoungGC());
}

HWTEST_F_L0(MarkingCollectorTest, RunGarbageCollectionTest2)
{
    MarkingCollector& collector = reinterpret_cast<MarkingCollector&>(Heap::GetHeap().GetCollector());
    Heap::GetHeap().SetGCReason(GCReason::GC_REASON_YOUNG);
    collector.RunGarbageCollection(0, GCReason::GC_REASON_YOUNG, common::GC_TYPE_FULL);
    ASSERT_TRUE(Heap::GetHeap().GetCollector().GetGCStats().isYoungGC());
}

HWTEST_F_L0(MarkingCollectorTest, UpdateNativeThresholdTest)
{
    TableMarkingCollctor& collector = reinterpret_cast<TableMarkingCollctor&>(Heap::GetHeap().GetCollector());
    GCParam gcParam;
    gcParam.minGrowBytes = 1024;
    Heap::GetHeap().SetNativeHeapThreshold(512);
    auto oldThreshold = Heap::GetHeap().GetNativeHeapThreshold();
    collector.UpdateNativeThreshold(gcParam);
    auto newThreshold = Heap::GetHeap().GetNativeHeapThreshold();
    EXPECT_NE(newThreshold, oldThreshold);
}

HWTEST_F_L0(MarkingCollectorTest, UpdateNativeThresholdTest2)
{
    TableMarkingCollctor& collector = reinterpret_cast<TableMarkingCollctor&>(Heap::GetHeap().GetCollector());
    Heap::GetHeap().NotifyNativeAllocation(1100 * MB);

    GCParam param;
    collector.UpdateNativeThreshold(param);
    ASSERT_TRUE(Heap::GetHeap().GetNotifiedNativeSize() > MAX_NATIVE_SIZE_INC);
}

HWTEST_F_L0(MarkingCollectorTest, MarkingRootsTest)
{
    TableMarkingCollctor& collector = reinterpret_cast<TableMarkingCollctor&>(Heap::GetHeap().GetCollector());
    CArrayList<BaseObject *> roots;
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::OLD_REGION);
    Heap::GetHeap().SetGCReason(GC_REASON_YOUNG);
    collector.SetGCReason(GC_REASON_YOUNG);

    roots.push_back(obj);
    collector.MarkingRoots(roots);
    ASSERT_TRUE(region->IsInOldSpace());
}

HWTEST_F_L0(MarkingCollectorTest, PushRootToWorkStackTest)
{
    TableMarkingCollctor& collector = reinterpret_cast<TableMarkingCollctor&>(Heap::GetHeap().GetCollector());
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RootSet roots;
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::RECENT_LARGE_REGION);
    collector.SetGCReason(GC_REASON_NATIVE);
    region->MarkObject(obj);
    bool result = collector.PushRootToWorkStack(&roots, obj);
    ASSERT_FALSE(result);

    region->SetRegionType(RegionDesc::RegionType::RECENT_LARGE_REGION);
    collector.SetGCReason(GC_REASON_YOUNG);
    result = collector.PushRootToWorkStack(&roots, obj);
    ASSERT_FALSE(result);

    region->SetRegionType(RegionDesc::RegionType::OLD_REGION);
    collector.SetGCReason(GC_REASON_NATIVE);
    result = collector.PushRootToWorkStack(&roots, obj);
    ASSERT_FALSE(result);
}
}