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

#include "ecmascript/js_api/js_api_lightweightset.h"

#include "ecmascript/containers/containers_lightweightset.h"
#include "ecmascript/containers/containers_private.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/global_env.h"

#include "ecmascript/js_api/js_api_lightweightset_iterator.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"
#include "ecmascript/containers/tests/containers_test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::containers;

namespace panda::test {
class ContainersLightWeightSetTest : public testing::Test {
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

    class TestClass : public base::BuiltinsBase {
    public:
        static JSTaggedValue TestForEachFunc(EcmaRuntimeCallInfo *argv)
        {
            JSThread *thread = argv->GetThread();
            JSHandle<JSTaggedValue> value = GetCallArg(argv, 0);    // 0 means the first argument vector
            JSHandle<JSAPILightWeightSet> jSAPILightWeightSet(GetThis(argv));
            JSAPILightWeightSet::Add(thread, jSAPILightWeightSet, value);
            return JSTaggedValue::Undefined();
        }
    };
protected:
    JSTaggedValue InitializeLightWeightSetConstructor()
    {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
        JSHandle<JSTaggedValue> globalObject = env->GetJSGlobalObject();
        JSHandle<JSTaggedValue> key(factory->NewFromASCII("ArkPrivate"));
        JSHandle<JSTaggedValue> value =
            JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(globalObject), key).GetValue();
        auto objCallInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6); // 6 means the value
        objCallInfo->SetFunction(JSTaggedValue::Undefined());
        objCallInfo->SetThis(value.GetTaggedValue());
        objCallInfo->SetCallArg(
            0, JSTaggedValue(static_cast<int>(ContainerTag::LightWeightSet))); // 0 means the argument
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
        JSTaggedValue result = ContainersPrivate::Load(objCallInfo);
        TestHelper::TearDownFrame(thread, prev);

        return result;
    }

    JSHandle<JSAPILightWeightSet> CreateJSAPILightWeightSet()
    {
        JSHandle<JSFunction> newTarget(thread, InitializeLightWeightSetConstructor());
        auto objCallInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4); // 4 means the value
        objCallInfo->SetFunction(newTarget.GetTaggedValue());
        objCallInfo->SetNewTarget(newTarget.GetTaggedValue());
        objCallInfo->SetThis(JSTaggedValue::Undefined());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
        JSTaggedValue result = ContainersLightWeightSet::LightWeightSetConstructor(objCallInfo);
        TestHelper::TearDownFrame(thread, prev);
        JSHandle<JSAPILightWeightSet> map(thread, result);
        return map;
    }
};

HWTEST_F_L0(ContainersLightWeightSetTest, LightWeightSetConstructor)
{
    InitializeLightWeightSetConstructor();
    JSHandle<JSFunction> newTarget(thread, InitializeLightWeightSetConstructor());
    auto objCallInfo =
        TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);   // 4 means the value
    objCallInfo->SetFunction(newTarget.GetTaggedValue());
    objCallInfo->SetNewTarget(newTarget.GetTaggedValue());
    objCallInfo->SetThis(JSTaggedValue::Undefined());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = ContainersLightWeightSet::LightWeightSetConstructor(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsJSAPILightWeightSet());
    JSHandle<JSAPILightWeightSet> mapHandle(thread, result);
    JSTaggedValue resultProto = JSObject::GetPrototype(JSHandle<JSObject>::Cast(mapHandle));
    JSTaggedValue funcProto = newTarget->GetFunctionPrototype();
    ASSERT_EQ(resultProto, funcProto);
    int length = mapHandle->GetLength();
    ASSERT_EQ(length, 0);   // 0 means the value

    // test LightWeightSetConstructor exception
    objCallInfo->SetNewTarget(JSTaggedValue::Undefined());
    CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, LightWeightSetConstructor, objCallInfo);
}

HWTEST_F_L0(ContainersLightWeightSetTest, AddAndGetValueAt)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i + 1));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_EQ(length, static_cast<int>(i + 1));
    }

    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> value(thread, JSTaggedValue::Undefined());
    std::string myValue("myvalue");
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        std::string ivalue = myValue + std::to_string(i);
        value.Update(factory->NewFromStdString(ivalue).GetTaggedValue());
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6); // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, value.GetTaggedValue());
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        EXPECT_EQ(length, static_cast<int>(NODE_NUMBERS + 1 + i));
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, AddAll)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i + 1));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_EQ(length, static_cast<int>(i + 1));
    }

    JSHandle<JSAPILightWeightSet> lws = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lws.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i + 1 + 10));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        TestHelper::TearDownFrame(thread, prev);
        int length = lws->GetLength();
        EXPECT_EQ(length, static_cast<int>(i + 1));
    }
    // test AddAll
    {
        auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, lws.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::AddAll(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_EQ(length, static_cast<int>(NODE_NUMBERS * 2));
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, HasAllAndHas)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i + 1));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_EQ(length, static_cast<int>(i + 1));
    }

    JSHandle<JSAPILightWeightSet> lws = CreateJSAPILightWeightSet();
    for (uint32_t i = 3; i < NODE_NUMBERS - 2; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lws.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i + 1));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        TestHelper::TearDownFrame(thread, prev);
    }
    // test HasAll true
    {
        auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, lws.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::HasAll(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    }
    // test HasAll fales
    JSHandle<JSAPILightWeightSet> lwsFalse = CreateJSAPILightWeightSet();
    for (uint32_t i = 1; i < NODE_NUMBERS - 2; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lwsFalse.GetTaggedValue());

        if (i == 2) {
            callInfo->SetCallArg(0, JSTaggedValue(i + 1 + 10));
            [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
            JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
            EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                        JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        } else {
            callInfo->SetCallArg(0, JSTaggedValue(i + 1));
            [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
            JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
            EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                        JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        }
    }
    {
        auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, lwsFalse.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::HasAll(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::False())));
    }
    // test Has Value
    {
        auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(3));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Has(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, Equal)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSMutableHandle<JSTaggedValue> value(thread, JSTaggedValue::Undefined());
    std::string myValue("myvalue");
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        std::string ivalue = myValue + std::to_string(i);
        value.Update(factory->NewFromStdString(ivalue).GetTaggedValue());
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6); // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, value.GetTaggedValue());
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        EXPECT_EQ(length, static_cast<int>(1 + i));
    }

    JSHandle<JSAPILightWeightSet> lws = CreateJSAPILightWeightSet();
    JSMutableHandle<JSTaggedValue> value1(thread, JSTaggedValue::Undefined());
    myValue = "myvalue";
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        std::string ivalue = myValue + std::to_string(i);
        value1.Update(factory->NewFromStdString(ivalue).GetTaggedValue());
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6); // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lws.GetTaggedValue());
        callInfo->SetCallArg(0, value1.GetTaggedValue());
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int length = lws->GetLength();
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        EXPECT_EQ(length, static_cast<int>(1 + i));
    }
    // test equal
    {
        auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, lws.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::HasAll(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                                         JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, HasAndValuesAndEntries)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 4 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int length = lightWeightSet->GetLength();
        EXPECT_TRUE(JSTaggedValue::Equal(thread, JSHandle<JSTaggedValue>(thread, result),
                    JSHandle<JSTaggedValue>(thread, JSTaggedValue::True())));
        EXPECT_EQ(length, static_cast<int>(i + 1));
    }

    JSMutableHandle<JSTaggedValue> result(thread, JSTaggedValue::Undefined());
    // test values
    auto callInfo =
        TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);   // 4 means the value
    callInfo->SetFunction(JSTaggedValue::Undefined());
    callInfo->SetThis(lightWeightSet.GetTaggedValue());
    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
    JSHandle<JSTaggedValue> iterValues(thread, ContainersLightWeightSet::Values(callInfo));
    TestHelper::TearDownFrame(thread, prev);
    EXPECT_TRUE(iterValues->IsJSAPILightWeightSetIterator());

    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);   // 4 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(iterValues.GetTaggedValue());

        prev = TestHelper::SetupFrame(thread, callInfo);
        result.Update(JSAPILightWeightSetIterator::Next(callInfo));
        TestHelper::TearDownFrame(thread, prev);
        JSHandle<JSTaggedValue> ValueHandle = JSIterator::IteratorValue(thread, result);

        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, ValueHandle.GetTaggedValue());

        prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue valueResult = ContainersLightWeightSet::Has(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_EQ(valueResult, JSTaggedValue::True());
    }
    // test entries
    callInfo =
        TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);   // 4 means the value
    callInfo->SetFunction(JSTaggedValue::Undefined());
    callInfo->SetThis(lightWeightSet.GetTaggedValue());
    prev = TestHelper::SetupFrame(thread, callInfo);
    JSHandle<JSTaggedValue> iter(thread, ContainersLightWeightSet::Entries(callInfo));
    TestHelper::TearDownFrame(thread, prev);
    EXPECT_TRUE(iter->IsJSAPILightWeightSetIterator());

    JSHandle<JSTaggedValue> first(thread, JSTaggedValue(0));    // 0 means the value
    JSHandle<JSTaggedValue> second(thread, JSTaggedValue(1));   // 1 means the value
    JSMutableHandle<JSTaggedValue> entries(thread, JSTaggedValue::Undefined());
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);   // 4 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(iter.GetTaggedValue());

        prev = TestHelper::SetupFrame(thread, callInfo);
        result.Update(JSAPILightWeightSetIterator::Next(callInfo));
        TestHelper::TearDownFrame(thread, prev);
        entries.Update(JSIterator::IteratorValue(thread, result).GetTaggedValue());

        int entriesKey = JSObject::GetProperty(thread, entries, first).GetValue()->GetInt();
        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(entriesKey));
        prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue keyResult = ContainersLightWeightSet::HasHash(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_EQ(keyResult, JSTaggedValue::True());

        int entriesValue = JSObject::GetProperty(thread, entries, second).GetValue()->GetInt();
        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(entriesValue));
        prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue valueResult = ContainersLightWeightSet::Has(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_EQ(valueResult, JSTaggedValue::True());
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, ForEach)
{
    constexpr uint32_t NODE_NUMBERS = 8;    // 8 means the value
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);   // 8 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        int len = lightWeightSet->GetLength();
        EXPECT_EQ(result, JSTaggedValue::True());
        EXPECT_EQ(len, static_cast<int>(i + 1));
    }
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSAPILightWeightSet> newLightWeightSet = CreateJSAPILightWeightSet();
    {
        JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
        JSHandle<JSFunction> func = factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForEachFunc));
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);   // 8 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, func.GetTaggedValue());
        callInfo->SetCallArg(1, newLightWeightSet.GetTaggedValue());

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        ContainersLightWeightSet::ForEach(callInfo);
        TestHelper::TearDownFrame(thread, prev);
    }

    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        auto callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, JSTaggedValue(i));

        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue result = ContainersLightWeightSet::GetValueAt(callInfo);
        TestHelper::TearDownFrame(thread, prev);

        callInfo =
            TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);   // 6 means the value
        callInfo->SetFunction(JSTaggedValue::Undefined());
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        callInfo->SetCallArg(0, result);

        prev = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue valueResult = ContainersLightWeightSet::Has(callInfo);
        TestHelper::TearDownFrame(thread, prev);
        EXPECT_EQ(valueResult, JSTaggedValue::True());
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, ProxyOfGetSizeAndHasHash)
{
    constexpr uint32_t NODE_NUMBERS = 8;
    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    auto callInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    callInfo->SetFunction(JSTaggedValue::Undefined());
    JSHandle<JSProxy> proxy = CreateJSProxyHandle(thread);
    proxy->SetTarget(thread, lightWeightSet.GetTaggedValue());
    callInfo->SetThis(proxy.GetTaggedValue());

    for (uint32_t i = 0; i < NODE_NUMBERS; i++) {
        callInfo->SetCallArg(0, JSTaggedValue(i));
        callInfo->SetCallArg(1, JSTaggedValue(i + 1));
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, callInfo);
        ContainersLightWeightSet::Add(callInfo);
        TestHelper::TearDownFrame(thread, prev);

        [[maybe_unused]] auto prev1 = TestHelper::SetupFrame(thread, callInfo);
        JSTaggedValue retult = ContainersLightWeightSet::GetSize(callInfo);
        TestHelper::TearDownFrame(thread, prev1);
        EXPECT_EQ(retult, JSTaggedValue(i + 1));

        [[maybe_unused]] auto prev2 = TestHelper::SetupFrame(thread, callInfo);
        EXPECT_EQ(ContainersLightWeightSet::HasHash(callInfo), JSTaggedValue::True());
        TestHelper::TearDownFrame(thread, prev2);
    }
    callInfo->SetCallArg(0, JSTaggedValue(NODE_NUMBERS));
    [[maybe_unused]] auto prev2 = TestHelper::SetupFrame(thread, callInfo);
    EXPECT_EQ(ContainersLightWeightSet::HasHash(callInfo), JSTaggedValue::False());
    TestHelper::TearDownFrame(thread, prev2);
}

HWTEST_F_L0(ContainersLightWeightSetTest, ExceptionReturn1)
{
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, AddAll);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, GetValueAt);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, HasAll);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, IncreaseCapacityTo);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Add);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, IsEmpty);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Has);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, HasHash);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Equal);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, GetIteratorObj);

    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    {
        auto callInfo = NewEmptyCallInfo(thread);
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, AddAll, callInfo);
    }
    {
        auto callInfo = NewEmptyCallInfo(thread);
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, GetValueAt, callInfo);
    }
    {
        auto callInfo = NewEmptyCallInfo(thread);
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, HasAll, callInfo);
    }
    {
        auto callInfo = NewEmptyCallInfo(thread);
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, IncreaseCapacityTo, callInfo);
    }
}

HWTEST_F_L0(ContainersLightWeightSetTest, ExceptionReturn2)
{
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, RemoveAt);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Values);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Entries);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, ForEach);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, GetIndexOf);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Remove);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, Clear);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, ToString);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, ToArray);
    CONTAINERS_API_TYPE_MISMATCH_EXCEPTION_TEST(ContainersLightWeightSet, GetSize);

    JSHandle<JSAPILightWeightSet> lightWeightSet = CreateJSAPILightWeightSet();
    {
        auto callInfo = NewEmptyCallInfo(thread);
        callInfo->SetThis(lightWeightSet.GetTaggedValue());
        CONTAINERS_API_EXCEPTION_TEST(ContainersLightWeightSet, RemoveAt, callInfo);
    }
}
}  // namespace panda::test
