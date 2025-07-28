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

#include "common_components/heap/collector/collector_proxy.h"
#include "common_components/tests/test_helper.h"

using namespace common;
namespace common::test {
class CollectorProxyTest : public common::test::BaseTestWithScope {
protected:
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

HWTEST_F_L0(CollectorProxyTest, RunGarbageCollection)
{
    CollectorProxy collectorProxy(Heap::GetHeap().GetAllocator(), Heap::GetHeap().GetCollectorResources());
    Heap::GetHeap().SetGCReason(GCReason::GC_REASON_OOM);
    collectorProxy.RunGarbageCollection(0, GCReason::GC_REASON_OOM, GCType::GC_TYPE_BEGIN);
    ASSERT_TRUE(Heap::GetHeap().GetGCReason() == GCReason::GC_REASON_OOM);
}
} // namespace common::test