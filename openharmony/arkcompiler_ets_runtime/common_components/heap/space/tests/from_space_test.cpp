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

#include "common_components/heap/allocator/region_space.h"
#include "common_components/heap/space/from_space.h"
#include "common_components/mutator/thread_local.h"
#include "common_components/tests/test_helper.h"

using namespace common;
namespace common::test {
class FromSpaceTest : public common::test::BaseTestWithScope {
protected:
    class TestRegionList : public RegionList {
    public:
        TestRegionList() : RegionList("TestList") {}
        void setHeadRegion(RegionDesc* head) { listHead_ = head; }
    };
    static void SetUpTestCase()
    {
        BaseRuntime::GetInstance()->Init();
    }

    static void TearDownTestCase()
    {
        BaseRuntime::GetInstance()->Fini();
    }

    void SetUp() override {}
    void TearDown() override {}
};

HWTEST_F_L0(FromSpaceTest, CopyFromRegions)
{
    RegionManager regionManager;
    RegionSpace heap;
    FromSpace fromSpace(regionManager, heap);
    ThreadLocal::SetAllocBuffer(nullptr);
    fromSpace.CopyFromRegions(nullptr);
    ASSERT_FALSE(fromSpace.GetFromRegionList().GetHeadRegion() != nullptr);
}

HWTEST_F_L0(FromSpaceTest, CopyFromRegionsTest)
{
    size_t unitIdx = 0;
    size_t nUnit = 4;
    RegionDesc* region = RegionDesc::InitRegion(unitIdx, nUnit, RegionDesc::UnitRole::LARGE_SIZED_UNITS);
    TestRegionList list;
    list.setHeadRegion(region);
    ASSERT_TRUE(list.GetHeadRegion() != nullptr);

    RegionSpace heap;
    RegionManager manager;
    FromSpace fromSpace(manager, heap);
    fromSpace.AssembleGarbageCandidates(list);

    AllocationBuffer* buffer1 = new (std::nothrow) AllocationBuffer();
    ThreadLocal::SetAllocBuffer(buffer1);
    fromSpace.CopyFromRegions();
    fromSpace.ExemptFromRegions();
    ASSERT_TRUE(AllocationBuffer::GetAllocBuffer() != nullptr);
}

HWTEST_F_L0(FromSpaceTest, ParallelCopyFromRegions)
{
    RegionManager regionManager;
    RegionSpace heap;
    FromSpace fromSpace(regionManager, heap);
    AllocationBuffer* buffer1 = new (std::nothrow) AllocationBuffer();
    ThreadLocal::SetAllocBuffer(buffer1);
    fromSpace.ParallelCopyFromRegions(nullptr, 5);
    ASSERT_TRUE(AllocationBuffer::GetAllocBuffer() != nullptr);

    ThreadLocal::SetAllocBuffer(nullptr);
    fromSpace.ParallelCopyFromRegions(nullptr, 5);
    ASSERT_FALSE(AllocationBuffer::GetAllocBuffer() != nullptr);
}
} // namespace common::test