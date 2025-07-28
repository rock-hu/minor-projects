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

#include "common_components/heap/w_collector/copy_barrier.h"
#include "common_components/heap/w_collector/tests/mock_barrier_collector.h"
#include "common_components/mutator/mutator_manager.h"
#include "common_components/tests/test_helper.h"
#include "common_components/heap/heap_manager.h"
#include "common_interfaces/base_runtime.h"

using namespace common;

namespace common::test {
class CopyBarrierTest : public common::test::BaseTestWithScope {
protected:
    static void SetUpTestCase()
    {
        BaseRuntime::GetInstance()->Init();
    }

    static void TearDownTestCase() {}

    void SetUp() override
    {
        MutatorManager::Instance().CreateRuntimeMutator(ThreadType::ARK_PROCESSOR);
    }

    void TearDown() override
    {
        MutatorManager::Instance().DestroyRuntimeMutator(ThreadType::ARK_PROCESSOR);
    }
};

HWTEST_F_L0(CopyBarrierTest, ReadRefField_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj);

    BaseObject* resultObj = copyBarrier->ReadRefField(obj, field);
    ASSERT_TRUE(resultObj != nullptr);
    EXPECT_EQ(resultObj, obj);
}

HWTEST_F_L0(CopyBarrierTest, ReadRefField_TEST2)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj);

    BaseObject* resultObj = copyBarrier->ReadRefField(nullptr, field);
    ASSERT_TRUE(resultObj != nullptr);
    EXPECT_EQ(resultObj, obj);
}

HWTEST_F_L0(CopyBarrierTest, ReadRefField_TEST3)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj, true);

    BaseObject* resultObj = copyBarrier->ReadRefField(obj, field);
    ASSERT_TRUE(resultObj != nullptr);
    constexpr uint64_t TAG_WEAK = 0x01ULL;
    BaseObject* newObj = reinterpret_cast<BaseObject*>(reinterpret_cast<uintptr_t>(obj) | TAG_WEAK);
    EXPECT_EQ(resultObj, newObj);
}

HWTEST_F_L0(CopyBarrierTest, ReadRefField_TEST4)
{
    MockCollectorForwardTest collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(nullptr);

    BaseObject* resultObj = copyBarrier->ReadRefField(obj, field);
    ASSERT_TRUE(resultObj == nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadRefField_TEST5)
{
    MockCollectorForwardTest collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj, true);

    BaseObject* resultObj = copyBarrier->ReadRefField(obj, field);
    ASSERT_TRUE(resultObj != nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadStaticRef_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj);

    BaseObject* resultObj = copyBarrier->ReadStaticRef(field);
    ASSERT_TRUE(resultObj != nullptr);
    EXPECT_EQ(resultObj, obj);
}

HWTEST_F_L0(CopyBarrierTest, ReadStringTableStaticRef_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    RefField<false> field(nullptr);

    BaseObject* resultObj = copyBarrier->ReadStringTableStaticRef(field);
    ASSERT_TRUE(resultObj == nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadStringTableStaticRef_TEST2)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RegionDesc *regionInfo = RegionDesc::GetRegionDescAt(addr);
    regionInfo->SetRegionAllocPtr(addr - 1);
    regionInfo->SetTraceLine();
    RefField<false> field(obj);

    BaseObject* resultObj = copyBarrier->ReadStringTableStaticRef(field);
    ASSERT_TRUE(resultObj != nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadStringTableStaticRef_TEST3)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RegionDesc *regionInfo = RegionDesc::GetRegionDescAt(addr);
    regionInfo->SetRegionType(RegionDesc::RegionType::ALIVE_REGION_FIRST);
    RefField<false> field(obj);

    BaseObject* resultObj = copyBarrier->ReadStringTableStaticRef(field);
    ASSERT_TRUE(resultObj == nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadStringTableStaticRef_TEST4)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    RefField<false> field(obj);

    Heap::GetHeap().SetGCReason(GC_REASON_YOUNG);
    BaseObject* resultObj = copyBarrier->ReadStringTableStaticRef(field);
    ASSERT_TRUE(resultObj != nullptr);
}

HWTEST_F_L0(CopyBarrierTest, ReadStruct_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress addr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* obj = reinterpret_cast<BaseObject*>(addr);
    HeapAddress src = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* srcObj = reinterpret_cast<BaseObject*>(src);
    srcObj->SetForwardState(BaseStateWord::ForwardState::FORWARDING);

    constexpr size_t size = sizeof(BaseObject);
    uint8_t dstBuffer[size] = {};
    HeapAddress dst = reinterpret_cast<HeapAddress>(dstBuffer);
    BaseObject* dstObj = reinterpret_cast<BaseObject*>(dst);
    dstObj->SetForwardState(BaseStateWord::ForwardState::FORWARDED);
    EXPECT_NE(dstObj->IsForwarding(), srcObj->IsForwarding());

    copyBarrier->ReadStruct(dst, obj, src, size);
    EXPECT_EQ(dstObj->IsForwarding(), srcObj->IsForwarding());
}

HWTEST_F_L0(CopyBarrierTest, ReadStruct_TEST2)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress src = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* srcObj = reinterpret_cast<BaseObject*>(src);
    srcObj->SetForwardState(BaseStateWord::ForwardState::FORWARDING);

    constexpr size_t size = sizeof(BaseObject);
    uint8_t dstBuffer[size] = {};
    HeapAddress dst = reinterpret_cast<HeapAddress>(dstBuffer);
    BaseObject* dstObj = reinterpret_cast<BaseObject*>(dst);
    dstObj->SetForwardState(BaseStateWord::ForwardState::FORWARDED);
    EXPECT_NE(dstObj->IsForwarding(), srcObj->IsForwarding());

    copyBarrier->ReadStruct(dst, nullptr, src, size);
    EXPECT_EQ(dstObj->IsForwarding(), srcObj->IsForwarding());
}

HWTEST_F_L0(CopyBarrierTest, AtomicWriteRefField_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress oldAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* oldObj = reinterpret_cast<BaseObject*>(oldAddr);
    constexpr size_t oldSize = 100;
    oldObj->SetSizeForwarded(oldSize);
    EXPECT_EQ(oldObj->GetSizeForwarded(), oldSize);
    RefField<true> oldField(oldObj);
    MAddress oldAddress = oldField.GetFieldValue();

    HeapAddress newAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* newObj = reinterpret_cast<BaseObject*>(newAddr);
    constexpr size_t newSize = 200;
    newObj->SetSizeForwarded(newSize);
    EXPECT_EQ(newObj->GetSizeForwarded(), newSize);
    RefField<true> newField(newObj);
    MAddress neWAddress = newField.GetFieldValue();
    EXPECT_NE(oldAddress, neWAddress);

    copyBarrier->AtomicWriteRefField(oldObj, oldField, newObj, std::memory_order_relaxed);
    EXPECT_EQ(oldField.GetFieldValue(), neWAddress);
}

HWTEST_F_L0(CopyBarrierTest, AtomicWriteRefField_TEST2)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress oldAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* oldObj = reinterpret_cast<BaseObject*>(oldAddr);
    constexpr size_t oldSize = 100;
    oldObj->SetSizeForwarded(oldSize);
    EXPECT_EQ(oldObj->GetSizeForwarded(), oldSize);
    RefField<true> oldField(oldObj);
    MAddress oldAddress = oldField.GetFieldValue();

    HeapAddress newAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* newObj = reinterpret_cast<BaseObject*>(newAddr);
    constexpr size_t newSize = 200;
    newObj->SetSizeForwarded(newSize);
    EXPECT_EQ(newObj->GetSizeForwarded(), newSize);
    RefField<true> newField(newObj);
    MAddress neWAddress = newField.GetFieldValue();
    EXPECT_NE(oldAddress, neWAddress);

    copyBarrier->AtomicWriteRefField(nullptr, oldField, newObj, std::memory_order_relaxed);
    EXPECT_EQ(oldField.GetFieldValue(), neWAddress);
}

HWTEST_F_L0(CopyBarrierTest, CompareAndSwapRefField_TEST1)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress oldAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* oldObj = reinterpret_cast<BaseObject*>(oldAddr);
    constexpr size_t oldSize = 100;
    oldObj->SetSizeForwarded(oldSize);
    EXPECT_EQ(oldObj->GetSizeForwarded(), oldSize);
    RefField<true> oldField(oldObj);
    MAddress oldAddress = oldField.GetFieldValue();
    HeapAddress newAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* newObj = reinterpret_cast<BaseObject*>(newAddr);
    constexpr size_t newSize = 200;
    newObj->SetSizeForwarded(newSize);
    EXPECT_EQ(newObj->GetSizeForwarded(), newSize);
    RefField<true> newField(newObj);
    MAddress neWAddress = newField.GetFieldValue();
    EXPECT_NE(oldAddress, neWAddress);
    bool result = copyBarrier->CompareAndSwapRefField(oldObj, oldField, oldObj, newObj,
        std::memory_order_seq_cst, std::memory_order_seq_cst);
    ASSERT_TRUE(result);
}

HWTEST_F_L0(CopyBarrierTest, CompareAndSwapRefField_TEST2)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress oldAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* oldObj = reinterpret_cast<BaseObject*>(oldAddr);
    constexpr size_t oldSize = 100;
    oldObj->SetSizeForwarded(oldSize);
    EXPECT_EQ(oldObj->GetSizeForwarded(), oldSize);
    RefField<true> oldField(oldObj);

    bool result = copyBarrier->CompareAndSwapRefField(oldObj, oldField, oldObj, oldObj,
        std::memory_order_seq_cst, std::memory_order_seq_cst);
    ASSERT_TRUE(result);
}

HWTEST_F_L0(CopyBarrierTest, CompareAndSwapRefField_TEST3)
{
    MockCollector collector;
    auto copyBarrier = std::make_unique<CopyBarrier>(collector);
    ASSERT_TRUE(copyBarrier != nullptr);

    HeapAddress oldAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* oldObj = reinterpret_cast<BaseObject*>(oldAddr);
    constexpr size_t oldSize = 100;
    oldObj->SetSizeForwarded(oldSize);
    EXPECT_EQ(oldObj->GetSizeForwarded(), oldSize);
    RefField<true> oldField(oldObj);
    MAddress oldAddress = oldField.GetFieldValue();

    HeapAddress newAddr = HeapManager::Allocate(sizeof(BaseObject), AllocType::MOVEABLE_OBJECT, true);
    BaseObject* newObj = reinterpret_cast<BaseObject*>(newAddr);
    constexpr size_t newSize = 200;
    newObj->SetSizeForwarded(newSize);
    EXPECT_EQ(newObj->GetSizeForwarded(), newSize);
    RefField<true> newField(newObj);
    MAddress neWAddress = newField.GetFieldValue();
    EXPECT_NE(oldAddress, neWAddress);

    bool result = copyBarrier->CompareAndSwapRefField(oldObj, newField, oldObj, newObj,
        std::memory_order_seq_cst, std::memory_order_seq_cst);
    ASSERT_FALSE(result);
}
}  // namespace common::test
