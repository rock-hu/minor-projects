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
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/mem/space.h"
#include "ecmascript/mem/verification.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tagged_array-inl.h"
#include "ecmascript/tests/ecma_test_common.h"

using namespace panda::ecmascript;

namespace panda::test {
class HugeObjectTest : public BaseTestWithScope<false> {
public:
    void SetUp() override
    {
        TestHelper::CreateEcmaVMWithScope(instance, thread, scope);
        thread->GetEcmaVM()->SetEnableForceGC(false);
        const_cast<Heap *>(thread->GetEcmaVM()->GetHeap())->SetMarkType(MarkType::MARK_FULL);
    }
};

static TaggedArray *LargeArrayTestCreate(JSThread *thread)
{
    [[maybe_unused]] ecmascript::EcmaHandleScope scope(thread);
    static constexpr size_t SIZE = 1ULL << 20UL;
    JSHandle<TaggedArray> array = thread->GetEcmaVM()->GetFactory()->NewTaggedArray(SIZE);
    return *array;
}

HWTEST_F_L0(HugeObjectTest, LargeArrayKeep)
{
    TaggedArray *array = LargeArrayTestCreate(thread);
    EXPECT_TRUE(array != nullptr);
    JSHandle<TaggedArray> arrayHandle(thread, array);
    JSHandle<JSObject> newObj(thread, EcmaContainerCommon::JSObjectTestCreate(thread));
    arrayHandle->Set(thread, 0, newObj.GetTaggedValue());
    auto ecmaVm = thread->GetEcmaVM();
    EXPECT_EQ(*arrayHandle, reinterpret_cast<TaggedObject *>(array));
    ecmaVm->CollectGarbage(TriggerGCType::YOUNG_GC);   // Trigger GC.
    ecmaVm->CollectGarbage(TriggerGCType::OLD_GC);  // Trigger GC.
    EXPECT_EQ(*newObj, array->Get(0).GetTaggedObject());
    EXPECT_EQ(*arrayHandle, reinterpret_cast<TaggedObject *>(array));
}

HWTEST_F_L0(HugeObjectTest, MultipleArrays)
{
    auto ecmaVm = thread->GetEcmaVM();
    auto heap = ecmaVm->GetHeap();
    {
        [[maybe_unused]] ecmascript::EcmaHandleScope scope(thread);
        for (int i = 0; i <= 14; i++) {
            JSHandle<TaggedArray> array1(thread, LargeArrayTestCreate(thread));
        }
    }

    {
        [[maybe_unused]] ecmascript::EcmaHandleScope scope(thread);
        for (int i = 0; i <= 14; i++) {
            JSHandle<TaggedArray> array2(thread, LargeArrayTestCreate(thread));
        }
    }

    {
        [[maybe_unused]] ecmascript::EcmaHandleScope scope(thread);
        for (int i = 0; i <= 14; i++) {
            JSHandle<TaggedArray> array2(thread, LargeArrayTestCreate(thread));
        }
    }
    size_t failCount = 0;
    VerifyObjectVisitor objVerifier(heap, &failCount);
    heap->GetHugeObjectSpace()->IterateOverObjects(objVerifier);  // newspace reference the old space
    EXPECT_EQ(failCount, 0U);
}
}  // namespace panda::test
