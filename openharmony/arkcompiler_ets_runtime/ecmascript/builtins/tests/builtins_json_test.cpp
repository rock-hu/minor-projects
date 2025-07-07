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

#include "ecmascript/builtins/builtins_json.h"

#include <algorithm>
#include <iomanip>
#include <sstream>

#include "ecmascript/base/builtins_base.h"
#include "ecmascript/builtins/builtins_bigint.h"
#include "ecmascript/builtins/builtins_errors.h"
#include "ecmascript/builtins/builtins_proxy.h"
#include "ecmascript/builtins/builtins_typedarray.h"
#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_array.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
class BuiltinsJsonTest : public BaseTestWithScope<false> {
public:
    class TestClass : public base::BuiltinsBase {
    public:
        static JSTaggedValue TestForCommon(EcmaRuntimeCallInfo *argv)
        {
            JSTaggedValue key = GetCallArg(argv, 0).GetTaggedValue();
            if (key.IsUndefined()) {
                return JSTaggedValue::Undefined();
            }
            JSTaggedValue value = GetCallArg(argv, 1).GetTaggedValue();
            if (value.IsUndefined()) {
                return JSTaggedValue::Undefined();
            }

            return JSTaggedValue(value);
        }

        static JSTaggedValue TestForParse(EcmaRuntimeCallInfo *argv)
        {
            return TestForCommon(argv);
        }

        static JSTaggedValue TestForParse1(EcmaRuntimeCallInfo *argv)
        {
            (void)argv;
            return JSTaggedValue::Undefined();
        }

        static JSTaggedValue TestForStringfy(EcmaRuntimeCallInfo *argv)
        {
            uint32_t argc = argv->GetArgsNumber();
            if (argc > 0) {
                return TestForCommon(argv);
            }

            return JSTaggedValue::Undefined();
        }
    };
};

JSTaggedValue CreateBuiltinJSObject1(JSThread *thread, const CString keyCStr)
{
    EcmaVM *ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> globalEnv = ecmaVM->GetGlobalEnv();
    ObjectFactory *factory = ecmaVM->GetFactory();
    JSHandle<JSTaggedValue> objectFunc(globalEnv->GetObjectFunction());

    JSHandle<JSObject> jsobject(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objectFunc), objectFunc));
    EXPECT_TRUE(*jsobject != nullptr);

    JSHandle<JSTaggedValue> key(factory->NewFromASCII(&keyCStr[0]));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key, value);

    CString str2 = "y";
    JSHandle<JSTaggedValue> key2(factory->NewFromASCII(str2));
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2.5)); // 2.5 : test case
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key2, value2);

    CString str3 = "z";
    JSHandle<JSTaggedValue> key3(factory->NewFromASCII(str3));
    JSHandle<JSTaggedValue> value3(factory->NewFromASCII("abc"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key3, value3);

    return jsobject.GetTaggedValue();
}
// Math.abs(-10)

HWTEST_F_L0(BuiltinsJsonTest, Parse10)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSTaggedValue> msg(factory->NewFromASCII(
        "\t\r \n{\t\r \n \"property\"\t\r \n:\t\r \n{\t\r \n}\t\r \n,\t\r \n \"prop2\"\t\r \n:\t\r \n [\t\r \ntrue\t\r "
        "\n,\t\r \nnull\t\r \n,123.456\t\r \n] \t\r \n}\t\r \n"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, str.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsECMAObject());
}

HWTEST_F_L0(BuiltinsJsonTest, Parse21)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();

    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("[100,2.5,\"abc\"]"));

    JSHandle<JSFunction> handleFunc = factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForParse));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, str.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handleFunc.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsECMAObject());
}

HWTEST_F_L0(BuiltinsJsonTest, Parse)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> lengthKeyHandle = thread->GlobalConstants()->GetHandledLengthString();

    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("[100,2.5,\"abc\"]"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));
    std::vector<JSTaggedValue> args{str.GetTaggedValue()};
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 6);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    JSTaggedValue value(static_cast<JSTaggedType>(result.GetRawData()));
    ASSERT_TRUE(value.IsECMAObject());
    JSHandle<JSObject> valueHandle(thread, value);
    JSHandle<JSTaggedValue> lenResult =
        JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(valueHandle), lengthKeyHandle).GetValue();
    uint32_t length = JSTaggedValue::ToLength(thread, lenResult).ToUint32();
    EXPECT_EQ(length, 3U);
}

HWTEST_F_L0(BuiltinsJsonTest, Parse2)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("{\"epf\":100,\"key1\":200}"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    std::vector<JSTaggedValue> args{str.GetTaggedValue()};
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 6);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    JSTaggedValue value(static_cast<JSTaggedType>(result.GetRawData()));
    ASSERT_TRUE(value.IsECMAObject());
    JSHandle<JSObject> valueHandle(thread, value);

    JSHandle<TaggedArray> nameList(JSObject::EnumerableOwnNames(thread, valueHandle));
    JSHandle<JSArray> nameResult = JSArray::CreateArrayFromList(thread, nameList);

    JSHandle<JSTaggedValue> handleKey(nameResult);
    JSHandle<JSTaggedValue> lengthKey(factory->NewFromASCII("length"));
    JSHandle<JSTaggedValue> lenResult = JSObject::GetProperty(thread, handleKey, lengthKey).GetValue();
    uint32_t length = JSTaggedValue::ToLength(thread, lenResult).ToUint32();
    ASSERT_EQ(length, 2U);
}

HWTEST_F_L0(BuiltinsJsonTest, Parse3)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<EcmaString> str = factory->NewFromStdString("\"\\u0000\"");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, str.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    uint32_t length = EcmaStringAccessor(result).GetLength();
    ASSERT_EQ(length, 1U);
}

HWTEST_F_L0(BuiltinsJsonTest, Parse4)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<EcmaString> str = factory->NewFromStdString("{\n\t\"on\":\t0\n}");
    JSHandle<EcmaString> key = factory->NewFromStdString("on");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, str.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Parse(ecmaRuntimeCallInfo);
    JSHandle<JSTaggedValue> value =
        JSTaggedValue::GetProperty(thread, JSHandle<JSTaggedValue>(thread, result), JSHandle<JSTaggedValue>(key))
            .GetValue();
    int32_t number = JSTaggedValue::ToInt32(thread, value);
    ASSERT_EQ(number, 0);
}


HWTEST_F_L0(BuiltinsJsonTest, Stringify11)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject1(thread, "x"));
    JSHandle<JSFunction> handleFunc =
        factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForStringfy));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handleFunc.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify12)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject1(thread, "x"));
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> handleFunc =
        factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForStringfy));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handleFunc.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(2, JSTaggedValue(static_cast<int32_t>(10)));

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify13)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject1(thread, "x"));
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> handleFunc =
        factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForStringfy));
    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("tttt"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    std::vector<JSTaggedValue> args{obj.GetTaggedValue(), handleFunc.GetTaggedValue(), str.GetTaggedValue()};
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 10);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify14)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject1(thread, "x"));
    JSArray *arr = JSArray::Cast(JSArray::ArrayCreate(thread, JSTaggedNumber(0)).GetTaggedValue().GetTaggedObject());

    JSHandle<JSObject> obj1(thread, arr);
    JSHandle<JSTaggedValue> key0(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> value0(factory->NewFromASCII("x"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key0, value0);
    JSHandle<JSTaggedValue> key1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> value1(factory->NewFromASCII("z"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key1, value1);

    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("tttt"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, obj1.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(2, str.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify)
{
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject1(thread, "x"));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify1)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();
    JSHandle<GlobalEnv> env = ecmaVM->GetGlobalEnv();

    JSArray *arr = JSArray::Cast(JSArray::ArrayCreate(thread, JSTaggedNumber(0)).GetTaggedValue().GetTaggedObject());

    EXPECT_TRUE(arr != nullptr);
    JSHandle<JSObject> obj(thread, arr);
    JSHandle<JSTaggedValue> key0(thread, JSTaggedValue(0));

    JSHandle<JSTaggedValue> value(factory->NewFromASCII("def"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key0, value);

    JSHandle<JSTaggedValue> key1(thread, JSTaggedValue(1));
    PropertyDescriptor desc1(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(200)), true, true, true);
    JSArray::DefineOwnProperty(thread, obj, key1, desc1);

    JSHandle<JSTaggedValue> key2(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> value2(factory->NewFromASCII("abc"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key2, value2);

    JSHandle<JSFunction> handleFunc =
        factory->NewJSFunction(env, reinterpret_cast<void *>(TestClass::TestForStringfy));
    JSHandle<JSTaggedValue> msg(factory->NewFromASCII("tttt"));
    JSHandle<EcmaString> str(JSTaggedValue::ToString(thread, msg));

    std::vector<JSTaggedValue> args{obj.GetTaggedValue(), handleFunc.GetTaggedValue(), str.GetTaggedValue()};
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 10);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify2)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();

    JSArray *arr = JSArray::Cast(JSArray::ArrayCreate(thread, JSTaggedNumber(0)).GetTaggedValue().GetTaggedObject());
    EXPECT_TRUE(arr != nullptr);
    JSHandle<JSObject> obj(thread, arr);

    JSHandle<JSTaggedValue> key0(thread, JSTaggedValue(0));
    PropertyDescriptor desc0(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)), true, true, true);
    JSArray::DefineOwnProperty(thread, obj, key0, desc0);
    JSHandle<JSTaggedValue> key1(thread, JSTaggedValue(1));
    // 2.5 : test case
    PropertyDescriptor desc1(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(2.5)), true, true, true);
    JSArray::DefineOwnProperty(thread, obj, key1, desc1);
    // 2 : test case
    JSHandle<JSTaggedValue> key2(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> value2(factory->NewFromASCII("abc"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key2, value2);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(result.IsString());
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify3)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();

    uint16_t data[1];
    data[0] = 0;
    JSHandle<EcmaString> str = factory->NewFromUtf16(data, 1);
    JSHandle<EcmaString> test = factory->NewFromStdString("\"\\u0000\"");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, str.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(EcmaStringAccessor::StringsAreEqual(thread, *test, EcmaString::Cast(result.GetTaggedObject())));
}

JSHandle<JSTaggedValue> CreateJSObject(JSThread *thread)
{
    EcmaVM *ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> globalEnv = ecmaVM->GetGlobalEnv();
    JSHandle<JSTaggedValue> objFun = globalEnv->GetObjectFunction();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSTaggedValue> obj(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFun), objFun));
    JSHandle<JSTaggedValue> key(factory->NewFromStdString("x"));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, obj, key, value);
    return obj;
}

JSHandle<JSTaggedValue> CreateProxy(JSThread *thread)
{
    JSHandle<JSTaggedValue> target = CreateJSObject(thread);
    JSHandle<JSTaggedValue> handler = CreateJSObject(thread);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Null(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, target.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, handler.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsProxy::ProxyConstructor(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    return JSHandle<JSTaggedValue>(thread, result);
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify4)  // Test for proxy object
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();

    JSHandle<JSTaggedValue> proxy = CreateProxy(thread);
    JSHandle<EcmaString> test = factory->NewFromStdString("{\"x\":1}");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, proxy.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    ASSERT_TRUE(EcmaStringAccessor::StringsAreEqual(thread, *test, EcmaString::Cast(result.GetTaggedObject())));
    TestHelper::TearDownFrame(thread, prev);
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify5)  // Test for typedarray object
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();
    [[maybe_unused]] JSHandle<TaggedArray> array(factory->NewTaggedArray(3));
    array->Set(thread, 0, JSTaggedValue(2));
    array->Set(thread, 1, JSTaggedValue(3));
    array->Set(thread, 2, JSTaggedValue(4));

    JSHandle<GlobalEnv> env = ecmaVM->GetGlobalEnv();
    JSHandle<JSTaggedValue> jsArray(JSArray::CreateArrayFromList(thread, array));
    JSHandle<JSFunction> int8Func(env->GetInt8ArrayFunction());
    JSHandle<JSObject> globalObject(thread, env->GetGlobalObject());
    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo1->SetNewTarget(JSTaggedValue(*int8Func));
    ecmaRuntimeCallInfo1->SetThis(JSTaggedValue(*globalObject));
    ecmaRuntimeCallInfo1->SetCallArg(0, jsArray.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo1);
    JSHandle<JSTaggedValue> int8Array(thread, BuiltinsTypedArray::Int8ArrayConstructor(ecmaRuntimeCallInfo1));
    TestHelper::TearDownFrame(thread, prev);

    JSHandle<EcmaString> test = factory->NewFromStdString("{\"0\":2,\"1\":3,\"2\":4}");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, int8Array.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_TRUE(result.IsString());
    ASSERT_TRUE(EcmaStringAccessor::StringsAreEqual(thread, *test, EcmaString::Cast(result.GetTaggedObject())));
}

HWTEST_F_L0(BuiltinsJsonTest, Stringify6)  // Test for bigint object
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> numericValue(factory->NewFromASCII("123456789123456789"));

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetCallArg(0, numericValue.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo1);
    JSTaggedValue result1 = BuiltinsBigInt::BigIntConstructor(ecmaRuntimeCallInfo1);
    TestHelper::TearDownFrame(thread, prev);

    JSHandle<JSTaggedValue> bigIntHandle(thread, result1);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, bigIntHandle.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    [[maybe_unused]] JSTaggedValue result = BuiltinsJson::Stringify(ecmaRuntimeCallInfo);
    bool hasPendingException = false;
    if (thread->HasPendingException()) {
        hasPendingException = true;
        thread->ClearException();
    }
    ASSERT_TRUE(hasPendingException);
}

HWTEST_F_L0(BuiltinsJsonTest, StringifyAndParse)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();
    JSHandle<JSTaggedValue> obj = CreateJSObject(thread);
    JSHandle<JSTaggedValue> ykey(factory->NewFromASCII("y"));
    JSHandle<JSTaggedValue> yvalue(thread, JSTaggedValue(2.2)); // 2.2: use to test double value
    JSObject::SetProperty(thread, obj, ykey, yvalue);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());
    JSMutableHandle<JSTaggedValue> result(thread, JSTaggedValue::Hole());
    {
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
        result.Update(BuiltinsJson::Stringify(ecmaRuntimeCallInfo));
        TestHelper::TearDownFrame(thread, prev);
    }
    {
        ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
        ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
        ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
        ecmaRuntimeCallInfo->SetCallArg(0, result.GetTaggedValue());
        [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
        result.Update(BuiltinsJson::Parse(ecmaRuntimeCallInfo));
        TestHelper::TearDownFrame(thread, prev);
    }
    ASSERT_TRUE(result->IsECMAObject());

    JSHandle<JSObject> resultObj(result);
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("x"));
    JSHandle<JSTaggedValue> res = JSObject::GetProperty(thread, resultObj, key).GetValue();
    ASSERT_TRUE(res->IsInt());
    ASSERT_EQ(res->GetInt(), 1);

    res = JSObject::GetProperty(thread, resultObj, ykey).GetValue();
    ASSERT_TRUE(res->IsDouble());
    ASSERT_EQ(res->GetDouble(), 2.2); // 2.2:use to test double value
}
}  // namespace panda::test
