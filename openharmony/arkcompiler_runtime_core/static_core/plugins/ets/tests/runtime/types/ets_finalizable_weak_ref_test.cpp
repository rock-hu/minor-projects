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
#include <array>

#include "libpandabase/os/mutex.h"
#include "runtime/include/thread_scopes.h"
#include "runtime/include/gc_task.h"
#include "runtime/tests/test_utils.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/ets_handle.h"
#include "plugins/ets/runtime/ets_handle_scope.h"
#include "plugins/ets/runtime/types/ets_class.h"
#include "plugins/ets/runtime/types/ets_array.h"
#include "plugins/ets/runtime/types/ets_finalizable_weak_ref.h"
#include "plugins/ets/tests/runtime/types/ets_test_mirror_classes.h"

namespace ark::ets::test {

class EtsFinalizableWeakRefTest : public testing::Test {
public:
    EtsFinalizableWeakRefTest()
    {
        RuntimeOptions options;
        options.SetShouldLoadBootPandaFiles(true);
        options.SetShouldInitializeIntrinsics(false);
        options.SetCompilerEnableJit(false);
        options.SetGcType("g1-gc");
        options.SetLoadRuntimes({"ets"});
        options.SetUseTlabForAllocations(false);
        options.SetGcTriggerType("debug");
        options.SetGcDebugTriggerStart(std::numeric_limits<int>::max());
        options.SetExplicitConcurrentGcEnabled(false);

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

    ~EtsFinalizableWeakRefTest() override
    {
        Runtime::Destroy();
    }

    NO_COPY_SEMANTIC(EtsFinalizableWeakRefTest);
    NO_MOVE_SEMANTIC(EtsFinalizableWeakRefTest);

    static void CompareMemberOffsets(EtsClass *klass, const std::vector<MirrorFieldInfo> &members)
    {
        ASSERT_NE(nullptr, klass);
        ASSERT_EQ(members.size(), klass->GetInstanceFieldsNumber());

        // Check both EtsPromise and ark::Class<Promise> has the same number of fields
        // and at the same offsets
        for (const MirrorFieldInfo &memb : members) {
            EtsField *field = klass->GetFieldIDByName(memb.Name());
            ASSERT_NE(nullptr, field);
            ASSERT_EQ(memb.Offset(), field->GetOffset())
                << "Offsets of the field '" << memb.Name() << "' are different";
        }
    }

    static std::vector<MirrorFieldInfo> GetFinalizableWeakRefMembers()
    {
        return std::vector<MirrorFieldInfo> {
            MIRROR_FIELD_INFO(EtsFinalizableWeakRef, referent_, "referent"),
            MIRROR_FIELD_INFO(EtsFinalizableWeakRef, prev_, "prevRef"),
            MIRROR_FIELD_INFO(EtsFinalizableWeakRef, next_, "nextRef"),
            MIRROR_FIELD_INFO(EtsFinalizableWeakRef, finalizerPtr_, "finalizerPtr"),
            MIRROR_FIELD_INFO(EtsFinalizableWeakRef, finalizerArgPtr_, "finalizerArgPtr")};
    }

    static ObjectHeader *AllocObjectInYoung()
    {
        auto ctx = Runtime::GetCurrent()->GetLanguageContext(panda_file::SourceLang::ETS);
        return reinterpret_cast<ObjectHeader *>(
            coretypes::String::CreateEmptyString(ctx, Runtime::GetCurrent()->GetPandaVM()));
    }

    static void TriggerGC()
    {
        auto *coro = EtsCoroutine::GetCurrent();
        auto *obj = AllocObjectInYoung();
        EtsHandleScope arrayScope(coro);
        VMHandle<ObjectHeader> hObj(EtsCoroutine::GetCurrent(), obj);
        while (hObj.GetPtr() == obj) {
            AllocObjectInYoung();
        }
    }

protected:
    PandaEtsVM *vm_ = nullptr;  // NOLINT(misc-non-private-member-variables-in-classes)
};

TEST_F(EtsFinalizableWeakRefTest, FinalizableWeakRefMemoryLayout)
{
    EtsClass *finalizableWeakRefClass = PlatformTypes(vm_)->coreFinalizableWeakRef;
    CompareMemberOffsets(finalizableWeakRefClass, GetFinalizableWeakRefMembers());
}

template <size_t EVENT_COUNT>
class TestEvent {
public:
    static void Finalize(void *arg)
    {
        auto *finalizer = reinterpret_cast<TestEvent *>(arg);
        finalizer->Fire();
    }

    bool IsHappened() const
    {
        return happened_;
    }

    void Wait()
    {
        os::memory::LockHolder lh(mutex_);
        while (!happened_) {
            cv_.Wait(&mutex_);
        }
    }

    void Fire()
    {
        os::memory::LockHolder lh(mutex_);
        if (++eventCount_ == EVENT_COUNT) {
            happened_ = true;
            cv_.Signal();
        }
    }

private:
    size_t eventCount_ = 0;
    std::atomic<bool> happened_ = false;
    os::memory::Mutex mutex_;
    os::memory::ConditionVariable cv_;
};

TEST_F(EtsFinalizableWeakRefTest, RegisterFinalizerTest)
{
    // Initialize
    auto *coro = EtsCoroutine::GetCurrent();
    ScopedManagedCodeThread managedScope(coro);
    static constexpr size_t EVENT_COUNT = 10U;
    TestEvent<EVENT_COUNT> event;
    // Create referents
    {
        auto *objectClass = vm_->GetClassLinker()->GetClassRoot(EtsClassRoot::OBJECT);
        static constexpr size_t ARRAY_SIZE = 1024U;
        EtsHandleScope arrayScope(coro);
        std::array<EtsHandle<EtsObject>, EVENT_COUNT> arrayHandles;
        for (auto &handle : arrayHandles) {
            handle = EtsHandle<EtsObject>(coro, EtsObjectArray::Create(objectClass, ARRAY_SIZE)->AsObject());
            vm_->RegisterFinalizerForObject(coro, handle, TestEvent<EVENT_COUNT>::Finalize, &event);
        }

        // Trigger GC
        TriggerGC();

        // Check finalizer was not called because referent is alive
        ASSERT(!event.IsHappened());
    }
    // Referents are dead

    // Trigger Full GC to handle references
    vm_->GetGC()->WaitForGCInManaged(GCTask(GCTaskCause::OOM_CAUSE));

    // Check that finalizer was called
    event.Wait();
    ASSERT(event.IsHappened());
}

}  // namespace ark::ets::test
