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

#include "ecmascript/builtins/builtins_object.h"

#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_array.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::builtins;

namespace panda::test {
class BuiltinsObjectTest : public BaseTestWithScope<false> {
};

JSTaggedValue CreateBuiltinJSObject(JSThread *thread, const CString keyCStr)
{
    EcmaVM *ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> globalEnv = ecmaVM->GetGlobalEnv();
    JSHandle<JSTaggedValue> objFun = globalEnv->GetObjectFunction();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();

    JSHandle<JSTaggedValue> obj(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFun), objFun));
    JSHandle<JSTaggedValue> key(factory->NewFromASCII(&keyCStr[0]));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, obj, key, value);
    return obj.GetTaggedValue();
}

JSFunction *BuiltinsObjectTestCreate(JSThread *thread)
{
    EcmaVM *ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> globalEnv = ecmaVM->GetGlobalEnv();
    return globalEnv->GetObjectFunction().GetObject<JSFunction>();
}

JSObject *TestNewJSObject(JSThread *thread, const JSHandle<JSHClass> &hclass)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSObject *obj = JSObject::Cast(factory->NewObject(hclass));

    obj->SetElements(thread, factory->EmptyArray().GetTaggedValue(), SKIP_BARRIER);
    return obj;
}

// 19.1.1.1Object ( [ value ] )
HWTEST_F_L0(BuiltinsObjectTest, ObjectConstructor)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSFunction> objectFunc(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> objFun(globalEnv->GetObjectFunction());

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::ObjectConstructor(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSHandle<JSObject> jtHandle(thread, JSTaggedValue(reinterpret_cast<TaggedObject *>(result.GetRawData())));
    JSTaggedValue resultProto = JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(jtHandle));
    JSTaggedValue funcProto = objectFunc->GetFunctionPrototype();
    ASSERT_EQ(resultProto, funcProto);
    ASSERT_TRUE(jtHandle->IsExtensible());

    // num_args = 0
    JSHandle<JSObject> object =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>::Cast(function), function);
    object->GetJSHClass()->SetCallable(true);
    auto tgObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue(*object), 4);
    tgObjCallInfo->SetFunction(JSTaggedValue(*objFun));
    tgObjCallInfo->SetThis(JSTaggedValue::Undefined());
    tgObjCallInfo->SetNewTarget(JSTaggedValue(*objFun));

    prev = TestHelper::SetupFrame(thread, tgObjCallInfo);
    JSTaggedValue resultTg = BuiltinsObject::ObjectConstructor(tgObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_TRUE(resultTg.IsObject());
    JSHandle<JSObject> jtHandleTg(thread, JSTaggedValue(reinterpret_cast<TaggedObject *>(resultTg.GetRawData())));
    JSTaggedValue resultProtoTg = JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(jtHandleTg));
    JSTaggedValue funcProtoTg = objectFunc->GetFunctionPrototype();
    ASSERT_EQ(resultProtoTg, funcProtoTg);
    ASSERT_TRUE(jtHandleTg->IsExtensible());

    // value is null
    JSHandle<JSObject> objectVn =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>::Cast(function), function);
    objectVn->GetJSHClass()->SetCallable(true);
    auto vnObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue(*objectVn), 6);
    vnObjCallInfo->SetFunction(JSTaggedValue(*objFun));
    vnObjCallInfo->SetThis(JSTaggedValue::Undefined());
    vnObjCallInfo->SetCallArg(0, JSTaggedValue::Null());
    vnObjCallInfo->SetNewTarget(JSTaggedValue(*objFun));

    prev = TestHelper::SetupFrame(thread, vnObjCallInfo);
    JSTaggedValue resultVn = BuiltinsObject::ObjectConstructor(vnObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultVn.IsObject());
    JSHandle<JSObject> jtHandleVn(thread, JSTaggedValue(reinterpret_cast<TaggedObject *>(resultVn.GetRawData())));
    JSTaggedValue resultProtoVn = JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(jtHandleVn));
    JSTaggedValue funcProtoVn = objectFunc->GetFunctionPrototype();
    ASSERT_EQ(resultProtoVn, funcProtoVn);
    ASSERT_TRUE(jtHandleVn->IsExtensible());
}

// 19.1.2.1Object.assign ( target, ...sources )
HWTEST_F_L0(BuiltinsObjectTest, Assign)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSObject> objHandle1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSObject> objHandle2 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(objHandle1), key1, value1);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(objHandle1), key1).GetValue()->GetInt(), 1);

    JSHandle<JSTaggedValue> key2(thread->GetEcmaVM()->GetFactory()->NewFromASCII("y"));
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(objHandle2), key2, value2);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(objHandle2), key2).GetValue()->GetInt(), 2);

    auto assignObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    assignObjCallInfo->SetFunction(JSTaggedValue::Undefined());
    assignObjCallInfo->SetThis(JSTaggedValue::Undefined());
    assignObjCallInfo->SetCallArg(0, objHandle1.GetTaggedValue());
    assignObjCallInfo->SetCallArg(1, objHandle2.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, assignObjCallInfo);
    JSTaggedValue result = BuiltinsObject::Assign(assignObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSHandle<JSTaggedValue> jtHandle(thread, JSTaggedValue(reinterpret_cast<TaggedObject *>(result.GetRawData())));
    EXPECT_EQ(JSObject::GetProperty(thread, jtHandle, key1).GetValue()->GetInt(), 1);
    EXPECT_EQ(JSObject::GetProperty(thread, jtHandle, key2).GetValue()->GetInt(), 2);
}

// 19.1.2.2Object.create ( O [ , Properties ] )
HWTEST_F_L0(BuiltinsObjectTest, Create)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSFunction> objectFunc(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSTaggedValue> funcProto(thread, objectFunc->GetFunctionPrototype());

    // no prop
    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, funcProto.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::Create(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSHandle<JSTaggedValue> jtHandle(thread, result);
    JSTaggedValue resultProto = JSTaggedValue::GetPrototype(thread, jtHandle);
    ASSERT_EQ(resultProto, funcProto.GetTaggedValue());

    // has prop
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("prop"));
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>::Cast(function), function);
    EXPECT_TRUE(*objHandle != nullptr);

    PropertyDescriptor desc(thread);
    desc.SetWritable(false);
    JSHandle<JSObject> descHandle(JSObject::FromPropertyDescriptor(thread, desc));

    PropertyDescriptor descNw(thread, JSHandle<JSTaggedValue>::Cast(descHandle), true, true, true);
    JSObject::DefineOwnProperty(thread, objHandle, key, descNw);

    auto hpObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    hpObjCallInfo->SetFunction(JSTaggedValue::Undefined());
    hpObjCallInfo->SetThis(JSTaggedValue::Undefined());
    hpObjCallInfo->SetCallArg(0, funcProto.GetTaggedValue());
    hpObjCallInfo->SetCallArg(1, objHandle.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, hpObjCallInfo);
    JSTaggedValue resultHp = BuiltinsObject::Create(hpObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultHp.IsObject());
    PropertyDescriptor descRes(thread);
    bool success = JSObject::GetOwnProperty(thread, JSHandle<JSObject>(thread, resultHp), key, descRes);
    EXPECT_TRUE(success);
    EXPECT_TRUE(!descRes.IsWritable());

    // undefined
    auto unCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    unCallInfo->SetFunction(JSTaggedValue::Undefined());
    unCallInfo->SetThis(JSTaggedValue::Undefined());
    unCallInfo->SetCallArg(0, JSTaggedValue::Undefined());

    prev = TestHelper::SetupFrame(thread, unCallInfo);
    JSTaggedValue resultUn = BuiltinsObject::Create(unCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_EQ(resultUn.GetRawData(), JSTaggedValue::VALUE_EXCEPTION);
}

// 19.1.2.3Object.defineProperties ( O, Properties )
HWTEST_F_L0(BuiltinsObjectTest, DefineProperties)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> objFunc(env->GetObjectFunction());
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("prop"));
    JSHandle<JSObject> jsobjHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    PropertyDescriptor desc(thread);
    desc.SetWritable(false);
    JSHandle<JSObject> descHandle(JSObject::FromPropertyDescriptor(thread, desc));

    PropertyDescriptor descNw(thread, JSHandle<JSTaggedValue>::Cast(descHandle), true, true, true);
    JSObject::DefineOwnProperty(thread, jsobjHandle, key, descNw);

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, objHandle.GetTaggedValue());
    objCallInfo->SetCallArg(1, jsobjHandle.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::DefineProperties(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSTaggedValue res(reinterpret_cast<TaggedObject *>(result.GetRawData()));
    PropertyDescriptor descRes(thread);
    JSObject::GetOwnProperty(thread, JSHandle<JSObject>(thread, res), key, descRes);
    EXPECT_TRUE(!descRes.IsWritable());
}

// 19.1.2.4Object.defineProperty ( O, P, Attributes )
HWTEST_F_L0(BuiltinsObjectTest, DefineProperty)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));

    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> objFunc(env->GetObjectFunction());
    JSHandle<JSObject> attHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    PropertyDescriptor desc(thread);
    desc.SetWritable(true);
    JSHandle<JSTaggedValue> writableStr = thread->GlobalConstants()->GetHandledWritableString();
    JSHandle<JSTaggedValue> writable(thread, JSTaggedValue(desc.IsWritable()));
    JSObject::CreateDataProperty(thread, attHandle, writableStr, writable);

    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));

    PropertyDescriptor descNw(thread);
    JSObject::GetOwnProperty(thread, objHandle, key, descNw);
    EXPECT_TRUE(!descNw.HasWritable());

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 10);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, objHandle.GetTaggedValue());
    objCallInfo->SetCallArg(1, key.GetTaggedValue());
    objCallInfo->SetCallArg(2, attHandle.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::DefineProperty(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSTaggedValue res(reinterpret_cast<TaggedObject *>(result.GetRawData()));
    PropertyDescriptor descRes(thread);
    JSObject::GetOwnProperty(thread, JSHandle<JSObject>(thread, res), key, descRes);
    EXPECT_TRUE(descRes.HasWritable());
}

// 19.1.2.5Object.freeze ( O )
HWTEST_F_L0(BuiltinsObjectTest, Freeze)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    // An object is extensible by default, so it is also non-frozen.
    JSHandle<JSObject> emptyObj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    emptyObj->GetJSHClass()->SetExtensible(true);
    auto nofreezeObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    nofreezeObjCallInfo->SetFunction(JSTaggedValue::Undefined());
    nofreezeObjCallInfo->SetThis(JSTaggedValue::Undefined());
    nofreezeObjCallInfo->SetCallArg(0, emptyObj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, nofreezeObjCallInfo);
    JSTaggedValue result = BuiltinsObject::IsFrozen(nofreezeObjCallInfo);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::False().GetRawData());

    BuiltinsObject::Freeze(nofreezeObjCallInfo);
    JSTaggedValue resultIs = BuiltinsObject::IsFrozen(nofreezeObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_EQ(resultIs.GetRawData(), JSTaggedValue::True().GetRawData());
}

// 19.1.2.6 Object.getOwnPropertyDescriptor ( O, P )
HWTEST_F_L0(BuiltinsObjectTest, GetOwnPropertyDescriptor)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));

    PropertyDescriptor descEnum(thread);
    descEnum.SetWritable(true);
    JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(objHandle), key, descEnum);

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, objHandle.GetTaggedValue());
    objCallInfo->SetCallArg(1, key.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::GetOwnPropertyDescriptor(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());

    JSHandle<JSTaggedValue> writableStr = thread->GlobalConstants()->GetHandledWritableString();
    JSTaggedValue jt(reinterpret_cast<TaggedObject *>(result.GetRawData()));
    PropertyDescriptor desc(thread);
    JSObject::GetOwnProperty(thread, JSHandle<JSObject>(thread, jt), writableStr, desc);
    ASSERT_TRUE(JSTaggedValue::SameValue(desc.GetValue().GetTaggedValue(), JSTaggedValue(true)));
}

// 19.1.2.7 Object.getOwnPropertyNames ( O )
HWTEST_F_L0(BuiltinsObjectTest, GetOwnPropertyNames)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(objHandle), key, value);

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, objHandle.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::GetOwnPropertyNames(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
}

// 19.1.2.8 Object.getOwnPropertySymbols ( O )
HWTEST_F_L0(BuiltinsObjectTest, GetOwnPropertySymbols)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSObject> objHandle =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSSymbol> symbolKey = thread->GetEcmaVM()->GetFactory()->NewJSSymbol();
    JSHandle<JSTaggedValue> key(symbolKey);
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(objHandle), key, value);
    thread->ClearException();

    auto objCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    objCallInfo->SetFunction(JSTaggedValue::Undefined());
    objCallInfo->SetThis(JSTaggedValue::Undefined());
    objCallInfo->SetCallArg(0, objHandle.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo);
    JSTaggedValue result = BuiltinsObject::GetOwnPropertySymbols(objCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
}

// 19.1.2.10 Object.is ( value1, value2 )
HWTEST_F_L0(BuiltinsObjectTest, Is)
{
    // js object compare
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));

    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSObject> obj2 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);

    auto objCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    objCallInfo1->SetFunction(JSTaggedValue::Undefined());
    objCallInfo1->SetThis(JSTaggedValue::Undefined());
    objCallInfo1->SetCallArg(0, obj1.GetTaggedValue());
    objCallInfo1->SetCallArg(1, obj2.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, objCallInfo1);
    JSTaggedValue objResult1 = BuiltinsObject::Is(objCallInfo1);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(objResult1.GetRawData(), JSTaggedValue::False().GetRawData());

    objCallInfo1->SetCallArg(1, obj1.GetTaggedValue());
    JSTaggedValue objResult2 = BuiltinsObject::Is(objCallInfo1);
    ASSERT_EQ(objResult2.GetRawData(), JSTaggedValue::True().GetRawData());

    // string compare
    JSHandle<EcmaString> testStrValue1 = thread->GetEcmaVM()->GetFactory()->NewFromASCII("helloworld");
    JSHandle<EcmaString> testStrValue2 = thread->GetEcmaVM()->GetFactory()->NewFromASCII("helloworld");

    auto strCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    strCallInfo->SetFunction(JSTaggedValue::Undefined());
    strCallInfo->SetThis(JSTaggedValue::Undefined());
    strCallInfo->SetCallArg(0, testStrValue1.GetTaggedValue());
    strCallInfo->SetCallArg(1, testStrValue2.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, strCallInfo);
    JSTaggedValue strResult = BuiltinsObject::Is(strCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(strResult.GetRawData(), JSTaggedValue::True().GetRawData());

    // bool compare
    auto boolCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    boolCallInfo->SetFunction(JSTaggedValue::Undefined());
    boolCallInfo->SetThis(JSTaggedValue::Undefined());
    boolCallInfo->SetCallArg(0, JSTaggedValue::True());
    boolCallInfo->SetCallArg(1, JSTaggedValue::False());

    prev = TestHelper::SetupFrame(thread, boolCallInfo);
    JSTaggedValue boolResult = BuiltinsObject::Is(boolCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(boolResult.GetRawData(), JSTaggedValue::False().GetRawData());

    // number compare
    auto numCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    numCallInfo->SetFunction(JSTaggedValue::Undefined());
    numCallInfo->SetThis(JSTaggedValue::Undefined());
    numCallInfo->SetCallArg(0, JSTaggedValue(static_cast<double>(0)));
    numCallInfo->SetCallArg(1, JSTaggedValue(-0.0));

    prev = TestHelper::SetupFrame(thread, numCallInfo);
    JSTaggedValue numResult = BuiltinsObject::Is(numCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(numResult.GetRawData(), JSTaggedValue::False().GetRawData());

    // undefined or null compare
    auto nullCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    nullCallInfo->SetFunction(JSTaggedValue::Undefined());
    nullCallInfo->SetThis(JSTaggedValue::Undefined());
    nullCallInfo->SetCallArg(0, JSTaggedValue::Null());
    nullCallInfo->SetCallArg(1, JSTaggedValue::Null());

    prev = TestHelper::SetupFrame(thread, nullCallInfo);
    JSTaggedValue nullResult = BuiltinsObject::Is(nullCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(nullResult.GetRawData(), JSTaggedValue::True().GetRawData());

    auto undefineCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    undefineCallInfo->SetFunction(JSTaggedValue::Undefined());
    undefineCallInfo->SetThis(JSTaggedValue::Undefined());
    undefineCallInfo->SetCallArg(0, JSTaggedValue::Undefined());
    undefineCallInfo->SetCallArg(1, JSTaggedValue::Undefined());

    prev = TestHelper::SetupFrame(thread, undefineCallInfo);
    JSTaggedValue undefineResult = BuiltinsObject::Is(undefineCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(undefineResult.GetRawData(), JSTaggedValue::True().GetRawData());
}

// 19.1.2.11 Object.isExtensible ( O )
HWTEST_F_L0(BuiltinsObjectTest, IsExtensible)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));

    // New objects can be extended by default.
    JSHandle<JSObject> emptyObj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    emptyObj->GetJSHClass()->SetExtensible(true);
    std::vector<JSTaggedValue> args{emptyObj.GetTaggedValue()};
    auto emptyObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 6);

    auto prev = TestHelper::SetupFrame(thread, emptyObjCallInfo);
    JSTaggedValue result = BuiltinsObject::IsExtensible(emptyObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::True().GetRawData());

    emptyObj->GetJSHClass()->SetExtensible(false);
    JSTaggedValue result2 = BuiltinsObject::IsExtensible(emptyObjCallInfo);
    ASSERT_EQ(result2.GetRawData(), JSTaggedValue::False().GetRawData());
}

// 19.1.2.12 Object.isFrozen ( O )
HWTEST_F_L0(BuiltinsObjectTest, IsFrozen)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    // An object is extensible by default, so it is also non-frozen.
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>::Cast(function), function);
    obj->GetJSHClass()->SetExtensible(true);
    auto emptyObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    emptyObjCallInfo->SetFunction(JSTaggedValue::Undefined());
    emptyObjCallInfo->SetThis(JSTaggedValue::Undefined());
    emptyObjCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, emptyObjCallInfo);
    JSTaggedValue result = BuiltinsObject::IsFrozen(emptyObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::False().GetRawData());

    obj->GetJSHClass()->SetExtensible(false);
    prev = TestHelper::SetupFrame(thread, emptyObjCallInfo);
    JSTaggedValue resultNex = BuiltinsObject::IsFrozen(emptyObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_EQ(resultNex.GetRawData(), JSTaggedValue::True().GetRawData());

    PropertyDescriptor descEnum(thread);
    descEnum.SetConfigurable(true);
    descEnum.SetWritable(false);
    obj->GetJSHClass()->SetExtensible(true);
    JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, descEnum);
    obj->GetJSHClass()->SetExtensible(false);
    auto emptyObjCallInfo2 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    emptyObjCallInfo2->SetFunction(JSTaggedValue::Undefined());
    emptyObjCallInfo2->SetThis(JSTaggedValue::Undefined());
    emptyObjCallInfo2->SetCallArg(0, obj.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, emptyObjCallInfo2);
    JSTaggedValue resultNw = BuiltinsObject::IsFrozen(emptyObjCallInfo2);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(resultNw.GetRawData(), JSTaggedValue::False().GetRawData());

    descEnum.SetConfigurable(false);
    obj->GetJSHClass()->SetExtensible(true);
    JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, descEnum);
    obj->GetJSHClass()->SetExtensible(false);
    auto emptyObjCallInfo3 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    emptyObjCallInfo3->SetFunction(JSTaggedValue::Undefined());
    emptyObjCallInfo3->SetThis(JSTaggedValue::Undefined());
    emptyObjCallInfo3->SetCallArg(0, obj.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, emptyObjCallInfo3);
    JSTaggedValue resultNc = BuiltinsObject::IsFrozen(emptyObjCallInfo3);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(resultNc.GetRawData(), JSTaggedValue::True().GetRawData());
}

// 19.1.2.13 Object.isSealed ( O )
HWTEST_F_L0(BuiltinsObjectTest, IsSealed)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));

    // An object is extensible by default, so it is also non-frozen.
    JSHandle<JSObject> emptyObj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    emptyObj->GetJSHClass()->SetExtensible(true);

    std::vector<JSTaggedValue> args{emptyObj.GetTaggedValue()};
    auto emptyObjCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, args, 6);

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, emptyObjCallInfo);
    JSTaggedValue result = BuiltinsObject::IsSealed(emptyObjCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::False().GetRawData());
}

// Object.keys(obj)
HWTEST_F_L0(BuiltinsObjectTest, Keys)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::Keys(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
}

// Object.values(obj)
HWTEST_F_L0(BuiltinsObjectTest, Values)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::Values(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
}

// Object.preventExtensions(obj)
HWTEST_F_L0(BuiltinsObjectTest, PreventExtensions)
{
    JSHandle<JSObject> obj(thread, CreateBuiltinJSObject(thread, "x"));
    obj->GetJSHClass()->SetExtensible(true);
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::PreventExtensions(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    JSTaggedValue jt(reinterpret_cast<TaggedObject *>(result.GetRawData()));
    JSHandle<JSObject> jtHandle(thread, jt);
    ASSERT_TRUE(!jtHandle->IsExtensible());
}

// Object.seal(obj)
HWTEST_F_L0(BuiltinsObjectTest, Seal)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::Seal(ecmaRuntimeCallInfo);

    ASSERT_TRUE(result.IsECMAObject());
    ASSERT_EQ(result.GetRawData(), obj->GetRawData());

    // test isSealed().
    JSTaggedValue res = BuiltinsObject::IsSealed(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_EQ(res.GetRawData(), JSTaggedValue::True().GetRawData());
}

// Object.setPrototypeOf(obj, prototype)
HWTEST_F_L0(BuiltinsObjectTest, SetPrototypeOf)
{
    JSHandle<JSObject> obj(thread, CreateBuiltinJSObject(thread, "x"));
    JSHandle<JSObject> objFather(thread, CreateBuiltinJSObject(thread, "y"));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, objFather.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::SetPrototypeOf(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
    ASSERT_EQ(result.GetRawData(), obj.GetTaggedValue().GetRawData());

    // test obj has property "y".
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("y"));
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);
}

// obj.hasOwnProperty(prop)
HWTEST_F_L0(BuiltinsObjectTest, HasOwnProperty)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    CString keyCStr = "x";
    JSHandle<EcmaString> keyString = thread->GetEcmaVM()->GetFactory()->NewFromASCII(&keyCStr[0]);
    JSHandle<JSTaggedValue> key(keyString);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, key.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::HasOwnProperty(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::True().GetRawData());
}

// prototypeObj.isPrototypeOf(object)
HWTEST_F_L0(BuiltinsObjectTest, IsPrototypeOfFalse)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    JSHandle<JSTaggedValue> objFather(thread, CreateBuiltinJSObject(thread, "y"));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(objFather.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result1 = BuiltinsObject::IsPrototypeOf(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result1.GetRawData(), JSTaggedValue::False().GetRawData());
}

HWTEST_F_L0(BuiltinsObjectTest, IsPrototypeOfTrue)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    JSHandle<JSTaggedValue> objFather(thread, CreateBuiltinJSObject(thread, "y"));

    auto ecmaRuntimeCallInfo1 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo1->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo1->SetCallArg(0, obj.GetTaggedValue());
    ecmaRuntimeCallInfo1->SetCallArg(1, objFather.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo1);
    JSTaggedValue result1 = BuiltinsObject::SetPrototypeOf(ecmaRuntimeCallInfo1);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result1.IsObject());
    ASSERT_EQ(result1.GetRawData(), obj->GetRawData());

    auto ecmaRuntimeCallInfo2 = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo2->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo2->SetThis(objFather.GetTaggedValue());
    ecmaRuntimeCallInfo2->SetCallArg(0, obj.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo2);
    JSTaggedValue result2 = BuiltinsObject::IsPrototypeOf(ecmaRuntimeCallInfo2);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result2.GetRawData(), JSTaggedValue::True().GetRawData());
}

// obj.propertyIsEnumerable(prop)
HWTEST_F_L0(BuiltinsObjectTest, PropertyIsEnumerable)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, key.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::PropertyIsEnumerable(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_EQ(result.GetRawData(), JSTaggedValue::True().GetRawData());
}

// obj.toLocaleString()
HWTEST_F_L0(BuiltinsObjectTest, ToLocaleString)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    JSHandle<JSFunction> calleeFunc = thread->GetEcmaVM()->GetFactory()->NewJSFunction(
        thread->GetEcmaVM()->GetGlobalEnv(), reinterpret_cast<void *>(BuiltinsObject::ToString));
    calleeFunc->GetClass()->SetCallable(true);
    JSHandle<JSTaggedValue> calleeValue(calleeFunc);
    JSHandle<JSTaggedValue> calleeKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("toString"));
    JSObject::SetProperty(thread, obj, calleeKey, calleeValue);

    JSHandle<EcmaString> resultValue = thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Object]");

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::ToLocaleString(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, resultValue, JSHandle<EcmaString>(thread, result)), 0);
}

// obj.toString()
HWTEST_F_L0(BuiltinsObjectTest, ToString)
{
    JSHandle<JSTaggedValue> obj = JSHandle<JSTaggedValue>(thread, CreateBuiltinJSObject(thread, "x"));

    // object
    JSHandle<EcmaString> resultValue = thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Object]");
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::ToString(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, resultValue, JSHandle<EcmaString>(thread, result)), 0);

    // array
    JSHandle<JSArray> arr = thread->GetEcmaVM()->GetFactory()->NewJSArray();
    JSHandle<EcmaString> resultArrValue =
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Array]");
    auto arrEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    arrEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    arrEcmaRuntimeCallInfo->SetThis(arr.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, arrEcmaRuntimeCallInfo);
    JSTaggedValue resultArr = BuiltinsObject::ToString(arrEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultArr.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, resultArrValue, JSHandle<EcmaString>(thread, resultArr)), 0);

    // string
    JSHandle<EcmaString> str = thread->GetEcmaVM()->GetFactory()->NewFromASCII("hello");
    JSHandle<EcmaString> resultStrValue =
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object String]");
    auto strEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    strEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    strEcmaRuntimeCallInfo->SetThis(str.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, strEcmaRuntimeCallInfo);
    JSTaggedValue resultStr = BuiltinsObject::ToString(strEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultStr.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, resultStrValue, JSHandle<EcmaString>(thread, resultStr)), 0);

    // function
    JSHandle<JSFunction> func = thread->GetEcmaVM()->GetFactory()->NewJSFunction(thread->GetEcmaVM()->GetGlobalEnv());
    JSHandle<EcmaString> resultFuncValue =
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Function]");
    auto funcEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    funcEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    funcEcmaRuntimeCallInfo->SetThis(func.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, funcEcmaRuntimeCallInfo);
    JSTaggedValue resultFunc = BuiltinsObject::ToString(funcEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultFunc.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance,
        resultFuncValue, JSHandle<EcmaString>(thread, resultFunc)), 0);

    // error
    auto ecmaVM = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = ecmaVM->GetGlobalEnv();
    JSHandle<JSTaggedValue> errorObject = env->GetErrorFunction();
    JSHandle<JSObject> error =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(errorObject), errorObject);
    JSHandle<EcmaString> errorValue = thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Error]");
    auto errorEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    errorEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    errorEcmaRuntimeCallInfo->SetThis(error.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, errorEcmaRuntimeCallInfo);
    JSTaggedValue resultError = BuiltinsObject::ToString(errorEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultError.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, errorValue, JSHandle<EcmaString>(thread, resultError)), 0);

    // boolean
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue::False());
    JSHandle<JSFunction> booleanObject(env->GetBooleanFunction());
    JSHandle<JSPrimitiveRef> boolean = thread->GetEcmaVM()->GetFactory()->NewJSPrimitiveRef(booleanObject, value);
    JSHandle<EcmaString> resultBoolValue =
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Boolean]");
    auto boolEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    boolEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    boolEcmaRuntimeCallInfo->SetThis(boolean.GetTaggedValue());

    prev = TestHelper::SetupFrame(thread, boolEcmaRuntimeCallInfo);
    JSTaggedValue resultBool = BuiltinsObject::ToString(boolEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultBool.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance,
        resultBoolValue, JSHandle<EcmaString>(thread, resultBool)), 0);

    // number
    JSHandle<EcmaString> resultNumValue =
        thread->GetEcmaVM()->GetFactory()->NewFromASCII("[object Number]");
    auto numEcmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 4);
    numEcmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    numEcmaRuntimeCallInfo->SetThis(JSTaggedValue(static_cast<double>(0)));

    prev = TestHelper::SetupFrame(thread, numEcmaRuntimeCallInfo);
    JSTaggedValue resultNum = BuiltinsObject::ToString(numEcmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(resultNum.IsString());
    ASSERT_EQ(EcmaStringAccessor::Compare(instance, resultNumValue, JSHandle<EcmaString>(thread, resultNum)), 0);
}

// object.valueOf()
HWTEST_F_L0(BuiltinsObjectTest, ValueOf)
{
    JSHandle<JSTaggedValue> obj(thread, CreateBuiltinJSObject(thread, "x"));
    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 6);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(obj.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(0, obj.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::ValueOf(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);

    ASSERT_TRUE(result.IsECMAObject());
}

// Object.hasOwn (O, P)
HWTEST_F_L0(BuiltinsObjectTest, HasOwn)
{
    JSHandle<JSTaggedValue> function(thread, BuiltinsObjectTestCreate(thread));
    JSHandle<JSObject> object =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(function), function);
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(object), key, value);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(object), key).GetValue()->GetInt(), 1);

    auto ecmaRuntimeCallInfo = TestHelper::CreateEcmaRuntimeCallInfo(thread, JSTaggedValue::Undefined(), 8);
    ecmaRuntimeCallInfo->SetFunction(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetThis(JSTaggedValue::Undefined());
    ecmaRuntimeCallInfo->SetCallArg(0, object.GetTaggedValue());
    ecmaRuntimeCallInfo->SetCallArg(1, key.GetTaggedValue());

    [[maybe_unused]] auto prev = TestHelper::SetupFrame(thread, ecmaRuntimeCallInfo);
    JSTaggedValue result = BuiltinsObject::HasOwn(ecmaRuntimeCallInfo);
    TestHelper::TearDownFrame(thread, prev);
    ASSERT_EQ(result.GetRawData(), JSTaggedValue::True().GetRawData());
}
}  // namespace panda::test
