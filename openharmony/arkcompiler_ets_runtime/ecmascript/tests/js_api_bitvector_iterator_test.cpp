/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ecmascript/containers/containers_private.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_api/js_api_bitvector.h"
#include "ecmascript/js_api/js_api_bitvector_iterator.h"
#include "ecmascript/global_env.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda;
using namespace panda::ecmascript;

namespace panda::test {
class JSAPIBitVectorIteratorTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TearDownCase";
    }

    void SetUp() override
    {
        TestHelper::CreateEcmaVMWithScope(instance, thread, scope);
    }

    void TearDown() override
    {
        TestHelper::DestroyEcmaVMWithScope(instance, scope);
    }

    EcmaVM *instance {nullptr};
    EcmaHandleScope *scope {nullptr};
    JSThread *thread {nullptr};
protected:
    JSAPIBitVector *CreateBitVector()
    {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();

        JSHandle<JSTaggedValue> globalObject = env->GetJSGlobalObject();
        JSHandle<JSTaggedValue> key(factory->NewFromASCII("ArkPrivate"));
        JSHandle<JSTaggedValue> value =
            JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(globalObject), key).GetValue();

        auto objCallInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
        objCallInfo->SetFunction(JSTaggedValue::Undefined());
        objCallInfo->SetThis(value.GetTaggedValue());
        objCallInfo->SetCallArg(0, JSTaggedValue(static_cast<int>(containers::ContainerTag::BitVector)));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
        JSTaggedValue result = containers::ContainersPrivate::Load(objCallInfo);
        TestHelper::TearDownFrame(thread, prev);

        JSHandle<JSTaggedValue> constructor(thread, result);
        JSHandle<JSAPIBitVector> bitVector(
            factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor));
        auto *newBitSetVector = new std::vector<std::bitset<JSAPIBitVector::BIT_SET_LENGTH>>();
        auto deleter = []([[maybe_unused]] void *env, void *pointer, [[maybe_unused]] void *data) {
            if (pointer == nullptr) {
                return;
            }
            delete reinterpret_cast<std::vector<std::bitset<JSAPIBitVector::BIT_SET_LENGTH>> *>(pointer);
        };
        JSHandle<JSNativePointer> pointer = factory->NewSJSNativePointer(newBitSetVector, deleter, newBitSetVector);
        bitVector->SetNativePointer(thread, pointer);
        return *bitVector;
    }
};

/**
 * @tc.name: Next
 * @tc.desc:
 * @tc.type: FUNC
 * @tc.require:
 */
HWTEST_F_L0(JSAPIBitVectorIteratorTest, Next)
{
    JSHandle<JSAPIBitVector> bitVector(thread, CreateBitVector());
    uint32_t elementsNum = 256;
    for (uint32_t i = 0; i < elementsNum; i++) {
        JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
        JSAPIBitVector::Push(thread, bitVector, value);
    }
    JSHandle<JSAPIBitVectorIterator> bitVectorIterator(thread, JSAPIBitVector::GetIteratorObj(thread, bitVector));
    JSHandle<JSTaggedValue> valueStr = thread->GlobalConstants()->GetHandledValueString();
    uint32_t length = bitVector->GetLength();
    for (uint32_t i = 0; i < length; i++) {
        auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
        ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
        ecmaRuntimeCallInfo->SetThis(bitVectorIterator.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
        JSTaggedValue result = JSAPIBitVectorIterator::Next(ecmaRuntimeCallInfo);
        TestHelper::TearDownFrame(thread, prev);

        JSHandle<JSObject> resultObj(thread, result);
        if (i < elementsNum) {
            EXPECT_EQ(bitVectorIterator->GetNextIndex(), i + 1U);
            EXPECT_EQ((JSObject::GetProperty(thread, resultObj, valueStr).GetValue()).GetTaggedValue(),
                JSTaggedValue(1));
        } else {
            EXPECT_EQ(bitVectorIterator->GetIteratedBitVector(thread), JSTaggedValue::Undefined());
            EXPECT_EQ((JSObject::GetProperty(thread, resultObj, valueStr).GetValue()).GetTaggedValue(),
                JSTaggedValue::Undefined());
        }
    }

    // test Next exception
    {
        auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
        ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
        ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
        JSTaggedValue result = JSAPIBitVectorIterator::Next(ecmaRuntimeCallInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_EQ(result, JSTaggedValue::Exception());
        EXPECT_EXCEPTION();
    }
}
} // namespace panda::test
