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

#include "common_components/heap/allocator/region_desc.h"
#include "common_components/heap/allocator/region_space.h"
#include "common_components/mutator/satb_buffer.h"
#include "common_components/tests/test_helper.h"
#include "common_interfaces/base_runtime.h"

using namespace common;
namespace common::test {

class SatbBufferTest : public BaseTestWithScope {
    void SetUp() override
    {
        BaseRuntime::GetInstance()->Init();
        holder_ = ThreadHolder::CreateAndRegisterNewThreadHolder(nullptr);
        scope_ = new ThreadHolder::TryBindMutatorScope(holder_);
    }

    void TearDown() override
    {
        if (scope_ != nullptr) {
            delete scope_;
            scope_ = nullptr;
        }

        BaseRuntime::GetInstance()->Fini();
    }

    ThreadHolder *holder_ {nullptr};
    ThreadHolder::TryBindMutatorScope *scope_ {nullptr};
};

HWTEST_F_L0(SatbBufferTest, NullptrReturnsFalse)
{
    EXPECT_FALSE(SatbBuffer::Instance().ShouldEnqueue(nullptr));
}

HWTEST_F_L0(SatbBufferTest, IsYoungSpaceObject1)
{
    auto* mutator = common::Mutator::GetMutator();
    mutator->SetMutatorPhase(GCPhase::GC_PHASE_ENUM);
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::ALIVE_REGION_FIRST);
    Heap::GetHeap().SetGCReason(GC_REASON_YOUNG);

    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    EXPECT_FALSE(SatbBuffer::Instance().ShouldEnqueue(obj));
}

HWTEST_F_L0(SatbBufferTest, IsYoungSpaceObject2)
{
    auto* mutator = common::Mutator::GetMutator();
    mutator->SetMutatorPhase(GCPhase::GC_PHASE_ENUM);
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::FROM_REGION);
    Heap::GetHeap().SetGCReason(GC_REASON_HEU);

    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    EXPECT_FALSE(SatbBuffer::Instance().ShouldEnqueue(obj));
}

HWTEST_F_L0(SatbBufferTest, IsYoungSpaceObject3)
{
    auto* mutator = common::Mutator::GetMutator();
    mutator->SetMutatorPhase(GCPhase::GC_PHASE_ENUM);
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::FROM_REGION);
    Heap::GetHeap().SetGCReason(GC_REASON_YOUNG);

    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    EXPECT_FALSE(SatbBuffer::Instance().ShouldEnqueue(obj));
}

HWTEST_F_L0(SatbBufferTest, IsYoungSpaceObject4)
{
    auto* mutator = common::Mutator::GetMutator();
    mutator->SetMutatorPhase(GCPhase::GC_PHASE_ENUM);
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0);
    RegionDesc* region = RegionDesc::GetRegionDescAt(addr);
    region->SetRegionType(RegionDesc::RegionType::ALIVE_REGION_FIRST);
    Heap::GetHeap().SetGCReason(GC_REASON_HEU);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    EXPECT_FALSE(SatbBuffer::Instance().ShouldEnqueue(obj));
}

HWTEST_F_L0(SatbBufferTest, IsMarkedObject)
{
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0U);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    Heap::GetHeap().SetGCReason(GC_REASON_HEU);

    RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(obj));
    region->SetMarkedRegionFlag(1);

    size_t offset = region->GetAddressOffset(reinterpret_cast<HeapAddress>(obj));
    RegionBitmap* markBitmap = region->GetOrAllocMarkBitmap();
    markBitmap->MarkBits(offset);

    bool result = SatbBuffer::Instance().ShouldEnqueue(obj);
    EXPECT_FALSE(result);
}

void ClearMarkBit(RegionBitmap* bitmap, size_t offset)
{
    uintptr_t* bits = *reinterpret_cast<uintptr_t**>(bitmap);

    size_t wordIndex = offset / (sizeof(uintptr_t) * 8);
    size_t bitIndex = offset % (sizeof(uintptr_t) * 8);

    uintptr_t mask = ~(static_cast<uintptr_t>(1) << bitIndex);

    uintptr_t* addr = const_cast<uintptr_t*>(bits + wordIndex);
    uintptr_t oldVal;
    uintptr_t newVal;

    do {
        oldVal = __atomic_load_n(addr, __ATOMIC_ACQUIRE);
        newVal = oldVal & mask;
        if (oldVal == newVal) {
            return;
        }
    } while (!__atomic_compare_exchange_n(addr, &oldVal, newVal, false, __ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE));
}

HWTEST_F_L0(SatbBufferTest, EnqueueObject)
{
    RegionSpace& theAllocator = reinterpret_cast<RegionSpace&>(Heap::GetHeap().GetAllocator());
    uintptr_t addr = theAllocator.AllocOldRegion();
    ASSERT_NE(addr, 0U);

    uintptr_t objAddress = addr + 0x100;
    BaseObject* obj = reinterpret_cast<BaseObject*>(objAddress);
    Heap::GetHeap().SetGCReason(GC_REASON_HEU);

    RegionDesc* region = RegionDesc::GetRegionDescAt(reinterpret_cast<uintptr_t>(obj));
    region->SetRegionType(RegionDesc::RegionType::FROM_REGION);
    region->SetTraceLine();
    region->SetMarkedRegionFlag(0);
    RegionBitmap* markBitmap = region->GetMarkBitmap();
    size_t offset = region->GetAddressOffset(reinterpret_cast<HeapAddress>(obj));
    if (markBitmap != nullptr) {
        ClearMarkBit(markBitmap, offset);
    }

    region->SetEnqueuedRegionFlag(1);
    RegionBitmap* enqueueBitmap = region->GetOrAllocEnqueueBitmap();
    enqueueBitmap->MarkBits(offset);

    bool result = SatbBuffer::Instance().ShouldEnqueue(obj);
    EXPECT_FALSE(result);
}
}  // namespace common::test
