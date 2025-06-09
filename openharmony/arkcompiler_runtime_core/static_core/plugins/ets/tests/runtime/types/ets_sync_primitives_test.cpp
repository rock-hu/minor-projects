/**
 * Copyright (c) 2024-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <gtest/gtest.h>

#include "ets_coroutine.h"

#include "ets_vm.h"
#include "types/ets_class.h"
#include "types/ets_sync_primitives.h"
#include "tests/runtime/types/ets_test_mirror_classes.h"

namespace ark::ets::test {

class EtsSyncPrimitivesTest : public testing::Test {
public:
    EtsSyncPrimitivesTest()
    {
        RuntimeOptions options;
        options.SetShouldLoadBootPandaFiles(true);
        options.SetShouldInitializeIntrinsics(false);
        options.SetCompilerEnableJit(false);
        options.SetGcType("epsilon");
        options.SetLoadRuntimes({"ets"});

        auto stdlib = std::getenv("PANDA_STD_LIB");
        if (stdlib == nullptr) {
            std::cerr << "PANDA_STD_LIB env variable should be set and point to mock_stdlib.abc" << std::endl;
            std::abort();
        }
        options.SetBootPandaFiles({stdlib});

        Runtime::Create(options);
        EtsCoroutine *coroutine = EtsCoroutine::GetCurrent();
        vm_ = coroutine->GetPandaVM();
    }

    ~EtsSyncPrimitivesTest() override
    {
        Runtime::Destroy();
    }

    NO_COPY_SEMANTIC(EtsSyncPrimitivesTest);
    NO_MOVE_SEMANTIC(EtsSyncPrimitivesTest);

    static std::vector<MirrorFieldInfo> GetWaitersListMembers()
    {
        return std::vector<MirrorFieldInfo> {MIRROR_FIELD_INFO(EtsWaitersList, head_, "head"),
                                             MIRROR_FIELD_INFO(EtsWaitersList, tail_, "tail")};
    }

    static std::vector<MirrorFieldInfo> GetMutexMembers()
    {
        return std::vector<MirrorFieldInfo> {MIRROR_FIELD_INFO(EtsMutex, waitersList_, "waitersList"),
                                             MIRROR_FIELD_INFO(EtsMutex, waiters_, "waiters")};
    }

    static std::vector<MirrorFieldInfo> GetEventMembers()
    {
        return std::vector<MirrorFieldInfo> {MIRROR_FIELD_INFO(EtsEvent, waitersList_, "waitersList"),
                                             MIRROR_FIELD_INFO(EtsEvent, state_, "state")};
    }

    static std::vector<MirrorFieldInfo> GetCondVarMembers()
    {
        return std::vector<MirrorFieldInfo> {MIRROR_FIELD_INFO(EtsCondVar, waitersList_, "waitersList"),
                                             MIRROR_FIELD_INFO(EtsCondVar, waiters_, "waiters")};
    }

protected:
    PandaEtsVM *vm_ = nullptr;  // NOLINT(misc-non-private-member-variables-in-classes)
};

// Check both EtsMutex and ark::Class<Mutex> has the same number of fields
// and at the same offsets
TEST_F(EtsSyncPrimitivesTest, WaitersListMemoryLayout)
{
    auto *waitersListClass = PlatformTypes(vm_)->coreWaitersList;
    MirrorFieldInfo::CompareMemberOffsets(waitersListClass, GetWaitersListMembers());
}

// Check both EtsMutex and ark::Class<Mutex> has the same number of fields
// and at the same offsets
TEST_F(EtsSyncPrimitivesTest, MutexMemoryLayout)
{
    auto *mutexClass = PlatformTypes(vm_)->coreMutex;
    MirrorFieldInfo::CompareMemberOffsets(mutexClass, GetMutexMembers());
}

// Check both EtsEvent and ark::Class<Event> has the same number of fields
// and at the same offsets
TEST_F(EtsSyncPrimitivesTest, EventMemoryLayout)
{
    auto *eventClass = PlatformTypes(vm_)->coreEvent;
    MirrorFieldInfo::CompareMemberOffsets(eventClass, GetEventMembers());
}

// Check both EtsCondVar and ark::Class<CondVar> has the same number of fields
// and at the same offsets
TEST_F(EtsSyncPrimitivesTest, CondVarMemoryLayout)
{
    auto *condVarClass = PlatformTypes(vm_)->coreCondVar;
    MirrorFieldInfo::CompareMemberOffsets(condVarClass, GetCondVarMembers());
}

}  // namespace ark::ets::test
