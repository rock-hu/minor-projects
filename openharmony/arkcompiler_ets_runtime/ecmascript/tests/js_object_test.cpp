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

#include "ecmascript/base/builtins_base.h"

#include "ecmascript/ecma_runtime_call_info.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/ecma_vm.h"
#include "ecmascript/global_env.h"
#include "ecmascript/ic/proto_change_details.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_handle.h"
#include "ecmascript/js_hclass.h"
#include "ecmascript/js_object-inl.h"
#include "ecmascript/js_thread.h"
#include "ecmascript/mem/verification.h"
#include "ecmascript/object_factory.h"
#include "ecmascript/object_operator.h"
#include "ecmascript/property_attributes.h"
#include "ecmascript/tagged_array-inl.h"
#include "ecmascript/tagged_dictionary.h"
#include "ecmascript/tests/test_helper.h"
#include "ecmascript/weak_vector.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::base;

namespace panda::test {
class JSObjectTest : public BaseTestWithScope<false> {
};

static JSFunction *JSObjectTestCreate(JSThread *thread)
{
    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    return globalEnv->GetObjectFunction().GetObject<JSFunction>();
}

HWTEST_F_L0(JSObjectTest, Create)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> jsobject =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*jsobject != nullptr);
}

HWTEST_F_L0(JSObjectTest, SetProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> jsobject =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*jsobject != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key, value);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key).GetValue()->GetInt(), 1);

    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key, value2);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(jsobject), key).GetValue()->GetInt(), 2);
}

HWTEST_F_L0(JSObjectTest, GetProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    EXPECT_TRUE(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->IsUndefined());

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, value);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);
}

HWTEST_F_L0(JSObjectTest, DeleteProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "print";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    JSObject::DeleteProperty(thread, (obj), key);
    EXPECT_TRUE(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->IsUndefined());

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, value);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);

    JSHandle<JSTaggedValue> key2(thread->GetEcmaVM()->GetFactory()->NewFromASCII("print_test"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key2,
                          JSHandle<JSTaggedValue>(thread, JSTaggedValue(10)));
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key2).GetValue()->GetInt(), 10);

    JSObject::DeleteProperty(thread, (obj), key);
    EXPECT_TRUE(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->IsUndefined());
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key2).GetValue()->GetInt(), 10);
}

HWTEST_F_L0(JSObjectTest, DeletePropertyGlobal)
{
    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> global(thread, globalEnv->GetGlobalObject());
    JSHandle<JSTaggedValue> printKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("print"));
    JSHandle<JSTaggedValue> printTestKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("print_test"));

    JSHandle<JSTaggedValue> value = JSObject::GetProperty(thread, global, printKey).GetValue();

    JSObject::SetProperty(thread, global, printTestKey, value);

    JSTaggedValue val2 = JSObject::GetProperty(thread, global, printTestKey).GetValue().GetTaggedValue();
    EXPECT_EQ(val2, value.GetTaggedValue());
    JSTaggedValue::DeletePropertyOrThrow(thread, global, printTestKey);
    JSTaggedValue val3 = JSObject::GetProperty(thread, global, printKey).GetValue().GetTaggedValue();
    EXPECT_NE(val3, JSTaggedValue::Undefined());
}

void GetPropertyHasOwnPeroperty(JSThread *thread, bool hasOwnProperty = false)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> grandfather = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> father = JSObject::ObjectCreate(thread, grandfather);
    JSHandle<JSObject> son = JSObject::ObjectCreate(thread, father);

    JSHandle<JSTaggedValue> sonKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> fatherKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> grandfatherKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> sonValue(thread, JSTaggedValue(1)); // 1 : value
    JSHandle<JSTaggedValue> fatherValue(thread, JSTaggedValue(2)); // 2 : value
    JSHandle<JSTaggedValue> grandfatherValue(thread, JSTaggedValue(3)); // 3 : value

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(son), sonKey, sonValue);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(father), fatherKey, fatherValue);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(grandfather), grandfatherKey, grandfatherValue);

    if (hasOwnProperty) {
        bool flag = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(son), sonKey);
        EXPECT_TRUE(flag);
        flag = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(son), fatherKey);
        EXPECT_FALSE(flag);
        flag = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(son), grandfatherKey);
        EXPECT_FALSE(flag);
    } else {
        EXPECT_EQ(sonValue.GetTaggedValue(),
                  JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(son), sonKey).GetValue().GetTaggedValue());
        EXPECT_EQ(fatherValue.GetTaggedValue(),
                  JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(son), fatherKey).GetValue().GetTaggedValue());
        EXPECT_EQ(
            grandfatherValue.GetTaggedValue(),
            JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(son), grandfatherKey).GetValue().GetTaggedValue());
    }
}

HWTEST_F_L0(JSObjectTest, GetPropertyInPrototypeChain)
{
    GetPropertyHasOwnPeroperty(thread);
}

HWTEST_F_L0(JSObjectTest, PropertyAttribute)
{
    JSHandle<JSTaggedValue> constructor(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor);
    JSHandle<JSObject> obj2 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor);

    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> key2(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));

    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2));

    // test set property
    PropertyDescriptor desc(thread);
    desc.SetValue(value1);
    desc.SetWritable(false);
    JSObject::DefineOwnProperty(thread, obj1, key1, desc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value2);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key1, value1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key1, value2);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj2), key1).GetValue().GetTaggedValue(),
              value2.GetTaggedValue());

    // test delete property
    PropertyDescriptor desc1(thread);
    desc1.SetValue(value1);
    desc1.SetConfigurable(false);
    JSObject::DefineOwnProperty(thread, obj1, key2, desc1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key2, value1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key2, value1);
    JSObject::DeleteProperty(thread, (obj1), key2);
    JSObject::DeleteProperty(thread, (obj2), key2);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key2).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj2), key2).GetValue().GetTaggedValue(),
              JSTaggedValue::Undefined());
}

HWTEST_F_L0(JSObjectTest, CreateDataProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    bool success = JSObject::CreateDataProperty(thread, obj, key, value);
    EXPECT_TRUE(success);

    success = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(obj), key);
    EXPECT_TRUE(success);

    PropertyDescriptor desc(thread);
    success = JSObject::GetOwnProperty(thread, obj, key, desc);
    EXPECT_TRUE(success);
    EXPECT_EQ(true, desc.IsWritable());
    EXPECT_EQ(true, desc.IsEnumerable());
    EXPECT_EQ(true, desc.IsConfigurable());
}

HWTEST_F_L0(JSObjectTest, CreateMethodProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    bool success = JSObject::CreateMethodProperty(thread, obj, key, value);
    EXPECT_TRUE(success);

    success = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(obj), key);
    EXPECT_TRUE(success);

    PropertyDescriptor desc(thread);
    success = JSObject::GetOwnProperty(thread, obj, key, desc);
    EXPECT_TRUE(success);
    EXPECT_EQ(true, desc.IsWritable());
    EXPECT_EQ(false, desc.IsEnumerable());
    EXPECT_EQ(true, desc.IsConfigurable());
}

HWTEST_F_L0(JSObjectTest, DefinePropertyOrThrow)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));

    PropertyDescriptor desc1(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)), true, true, true);
    bool success = JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, desc1);
    EXPECT_TRUE(success);
    PropertyDescriptor descRes1(thread);
    success = JSObject::GetOwnProperty(thread, obj, key, descRes1);
    EXPECT_TRUE(success);
    EXPECT_EQ(1, descRes1.GetValue()->GetInt());
    EXPECT_EQ(true, descRes1.IsWritable());
    EXPECT_EQ(true, descRes1.IsEnumerable());
    EXPECT_EQ(true, descRes1.IsConfigurable());

    PropertyDescriptor desc2(thread, false, true, true);
    success = JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, desc2);
    EXPECT_TRUE(success);
    PropertyDescriptor descRes2(thread);
    success = JSObject::GetOwnProperty(thread, obj, key, descRes2);
    EXPECT_TRUE(success);
    EXPECT_EQ(1, descRes2.GetValue()->GetInt());
    EXPECT_EQ(false, descRes2.IsWritable());
    EXPECT_EQ(true, descRes2.IsEnumerable());
    EXPECT_EQ(true, descRes2.IsConfigurable());

    PropertyDescriptor desc3(thread);
    desc3.SetWritable(false);
    desc3.SetEnumerable(false);
    desc3.SetConfigurable(false);
    success = JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, desc3);
    EXPECT_TRUE(success);
    PropertyDescriptor descRes3(thread);
    success = JSObject::GetOwnProperty(thread, obj, key, descRes3);
    EXPECT_TRUE(success);
    EXPECT_EQ(1, descRes3.GetValue()->GetInt());
    EXPECT_EQ(false, descRes3.IsWritable());
    EXPECT_EQ(false, descRes3.IsEnumerable());
    EXPECT_EQ(false, descRes3.IsConfigurable());

    PropertyDescriptor desc4(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(2)));
    success = JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, desc4);
    EXPECT_FALSE(success);
}

HWTEST_F_L0(JSObjectTest, HasProperty)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    char array[] = "x";
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII(array));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    bool flag = JSObject::HasProperty(thread, obj, key);
    EXPECT_FALSE(flag);

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, value);
    flag = JSObject::HasProperty(thread, obj, key);
    EXPECT_TRUE(flag);

    JSObject::DeleteProperty(thread, (obj), key);
    flag = JSObject::HasProperty(thread, obj, key);
    EXPECT_FALSE(flag);
}

HWTEST_F_L0(JSObjectTest, HasPropertyWithPrototype)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> grandfather = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> father = JSObject::ObjectCreate(thread, grandfather);
    JSHandle<JSObject> son = JSObject::ObjectCreate(thread, father);

    JSHandle<JSTaggedValue> testGrand(thread,
        JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(grandfather)));
    JSHandle<JSTaggedValue> testFather(thread, JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(father)));
    JSHandle<JSTaggedValue> testSon(thread, JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(son)));
    EXPECT_TRUE(testSon.GetTaggedValue() != testFather.GetTaggedValue());
    EXPECT_TRUE(testGrand.GetTaggedValue() != testFather.GetTaggedValue());
    JSHandle<JSTaggedValue> sonKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> fatherKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> grandfatherKey(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> sonValue(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> fatherValue(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> grandfatherValue(thread, JSTaggedValue(3));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(son), sonKey, sonValue);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(father), fatherKey, fatherValue);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(grandfather), grandfatherKey, grandfatherValue);

    bool flag = JSObject::HasProperty(thread, son, sonKey);
    EXPECT_TRUE(flag);
    flag = JSObject::HasProperty(thread, son, fatherKey);
    EXPECT_TRUE(flag);
    flag = JSObject::HasProperty(thread, son, grandfatherKey);
    EXPECT_TRUE(flag);
}

HWTEST_F_L0(JSObjectTest, HasOwnProperty)
{
    GetPropertyHasOwnPeroperty(thread, true);
}

HWTEST_F_L0(JSObjectTest, GetOwnPropertyKeys)
{
    JSHandle<JSTaggedValue> constructor(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), constructor);

    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII("x"));
    JSHandle<JSTaggedValue> key2(thread->GetEcmaVM()->GetFactory()->NewFromASCII("y"));
    JSHandle<JSTaggedValue> key3(thread->GetEcmaVM()->GetFactory()->NewFromASCII("3"));
    JSHandle<JSTaggedValue> key4(thread->GetEcmaVM()->GetFactory()->NewFromASCII("4"));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> value2(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> value3(thread, JSTaggedValue(3));
    JSHandle<JSTaggedValue> value4(thread, JSTaggedValue(4));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key1, value1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key2, value2);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key3, value3);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key4, value4);

    JSHandle<TaggedArray> array = JSObject::GetOwnPropertyKeys(thread, obj);
    uint32_t length = array->GetLength();
    EXPECT_EQ(length, 4U);
    int sum = 0;
    for (uint32_t i = 0; i < length; i++) {
        JSHandle<JSTaggedValue> key(thread, array->Get(i));
        sum += JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt();
    }
    EXPECT_EQ(sum, 10);
}

HWTEST_F_L0(JSObjectTest, ObjectCreateMethod)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> grandfather = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> father = JSObject::ObjectCreate(thread, grandfather);
    JSHandle<JSObject> son = JSObject::ObjectCreate(thread, father);

    EXPECT_EQ(JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(son)), father.GetTaggedValue());
    EXPECT_EQ(JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(father)), grandfather.GetTaggedValue());
    EXPECT_EQ(JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(grandfather)), JSTaggedValue::Null());
}

HWTEST_F_L0(JSObjectTest, GetMethod)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> obj = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> func(thread->GetEcmaVM()->GetFactory()->NewJSFunction(env));
    JSHandle<JSFunction>::Cast(func)->GetJSHClass()->SetCallable(true);
    EXPECT_TRUE(*func != nullptr);
    JSHandle<JSTaggedValue> key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("1"));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, func);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue().GetTaggedValue(),
              func.GetTaggedValue());
}

HWTEST_F_L0(JSObjectTest, EnumerableOwnNames)
{
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(*obj != nullptr);

    CString tagCStr = "x";
    JSHandle<EcmaString> tagString = thread->GetEcmaVM()->GetFactory()->NewFromASCII(&tagCStr[0]);
    JSHandle<JSTaggedValue> key(tagString);

    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, value);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);

    JSHandle<TaggedArray> names = JSObject::EnumerableOwnNames(thread, obj);

    JSHandle<JSTaggedValue> keyFromNames(thread, JSTaggedValue(names->Get(0)));
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), keyFromNames).GetValue()->GetInt(), 1);

    PropertyDescriptor descNoEnum(thread);
    descNoEnum.SetEnumerable(false);
    JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, descNoEnum);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);

    JSHandle<TaggedArray> namesNoEnum = JSObject::EnumerableOwnNames(thread, obj);
    EXPECT_TRUE(namesNoEnum->GetLength() == 0U);

    PropertyDescriptor descEnum(thread);
    descEnum.SetConfigurable(false);
    descEnum.SetEnumerable(true);
    JSTaggedValue::DefinePropertyOrThrow(thread, JSHandle<JSTaggedValue>(obj), key, descEnum);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue()->GetInt(), 1);

    JSHandle<TaggedArray> namesNoConfig = JSObject::EnumerableOwnNames(thread, obj);

    JSHandle<JSTaggedValue> keyNoConfig(thread, JSTaggedValue(namesNoConfig->Get(0)));
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), keyNoConfig).GetValue()->GetInt(), 1);
}

HWTEST_F_L0(JSObjectTest, SetIntegrityLevelSealed)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    EXPECT_TRUE(*obj1 != nullptr);
    CString undefinedCStr = "x";
    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII(&undefinedCStr[0]));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value1);

    // test SetIntegrityLevel::SEALED
    JSHandle<JSObject> jsobject(obj1);
    bool status1 = JSObject::SetIntegrityLevel(thread, jsobject, IntegrityLevel::SEALED);
    EXPECT_TRUE(status1);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());
    PropertyDescriptor desc1(thread);
    bool success1 = JSObject::GetOwnProperty(thread, jsobject, key1, desc1);
    EXPECT_TRUE(success1);
    EXPECT_EQ(true, desc1.IsWritable());
    EXPECT_EQ(true, desc1.IsEnumerable());
    EXPECT_EQ(false, desc1.IsConfigurable());
}

HWTEST_F_L0(JSObjectTest, SetIntegrityLevelFrozen)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    EXPECT_TRUE(*obj1 != nullptr);

    CString undefinedCStr = "x";
    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII(&undefinedCStr[0]));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value1);

    // test SetIntegrityLevel::FROZEN
    bool status1 = JSObject::SetIntegrityLevel(thread, obj1, IntegrityLevel::FROZEN);
    EXPECT_TRUE(status1);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());
    PropertyDescriptor desc1(thread);
    bool success1 = JSObject::GetOwnProperty(thread, obj1, key1, desc1);
    EXPECT_TRUE(success1);
    EXPECT_EQ(false, desc1.IsWritable());
    EXPECT_EQ(true, desc1.IsEnumerable());
    EXPECT_EQ(false, desc1.IsConfigurable());
}

HWTEST_F_L0(JSObjectTest, TestIntegrityLevelSealed)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    CString undefinedCStr = "level";
    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII(&undefinedCStr[0]));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value1);
    obj1->GetJSHClass()->SetExtensible(false);

    // test SetIntegrityLevel::SEALED
    bool status1 = JSObject::SetIntegrityLevel(thread, obj1, IntegrityLevel::SEALED);
    EXPECT_TRUE(status1);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());

    PropertyDescriptor desc1(thread);
    bool success1 = JSObject::GetOwnProperty(thread, obj1, key1, desc1);
    EXPECT_TRUE(success1);
    EXPECT_EQ(true, JSObject::TestIntegrityLevel(thread, obj1, IntegrityLevel::SEALED));
    EXPECT_EQ(false, JSObject::TestIntegrityLevel(thread, obj1, IntegrityLevel::FROZEN));
}

HWTEST_F_L0(JSObjectTest, TestIntegrityLevelFrozen)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 =
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    CString undefinedCStr = "level";
    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII(&undefinedCStr[0]));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value1);
    obj1->GetJSHClass()->SetExtensible(false);

    // test SetIntegrityLevel::FROZEN
    bool status1 = JSObject::SetIntegrityLevel(thread, obj1, IntegrityLevel::FROZEN);
    EXPECT_TRUE(status1);
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1).GetValue().GetTaggedValue(),
              value1.GetTaggedValue());

    PropertyDescriptor desc1(thread);
    bool success1 = JSObject::GetOwnProperty(thread, obj1, key1, desc1);
    EXPECT_TRUE(success1);
    EXPECT_EQ(true, JSObject::TestIntegrityLevel(thread, obj1, IntegrityLevel::SEALED));
    EXPECT_EQ(true, JSObject::TestIntegrityLevel(thread, obj1, IntegrityLevel::FROZEN));
}

HWTEST_F_L0(JSObjectTest, TestIntegrityLevelWithoutProperty)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> obj1(
        thread->GetEcmaVM()->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1));
    JSHandle<JSObject>::Cast(obj1)->GetJSHClass()->SetExtensible(false);
    CString undefinedCStr = "level";
    JSHandle<JSTaggedValue> key1(thread->GetEcmaVM()->GetFactory()->NewFromASCII(&undefinedCStr[0]));

    // test SetIntegrityLevel::FROZEN
    JSHandle<JSObject> jsobject(obj1);
    bool status1 = JSObject::SetIntegrityLevel(thread, jsobject, IntegrityLevel::SEALED);
    EXPECT_TRUE(status1);

    PropertyDescriptor desc1(thread);
    bool success1 = JSObject::GetOwnProperty(thread, jsobject, key1, desc1);
    EXPECT_TRUE(!success1);
    EXPECT_EQ(true, JSObject::TestIntegrityLevel(thread, jsobject, IntegrityLevel::SEALED));
    EXPECT_EQ(true, JSObject::TestIntegrityLevel(thread, jsobject, IntegrityLevel::FROZEN));
}

JSTaggedValue TestGetter(EcmaRuntimeCallInfo *argv)
{
    auto thread = argv->GetThread();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj(BuiltinsBase::GetThis(argv));
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("y"));
    JSTaggedValue value = JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue().GetTaggedValue();

    return JSTaggedValue(value.GetInt() + 1);
}

HWTEST_F_L0(JSObjectTest, Getter)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    JSHandle<JSTaggedValue> key1(factory->NewFromASCII("x"));
    JSHandle<JSTaggedValue> key2(factory->NewFromASCII("y"));
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> getter =
        thread->GetEcmaVM()->GetFactory()->NewJSFunction(env, reinterpret_cast<void *>(TestGetter));

    PropertyDescriptor desc1(thread);
    desc1.SetGetter(JSHandle<JSTaggedValue>::Cast(getter));
    bool success1 = JSObject::DefineOwnProperty(thread, obj, key1, desc1);
    EXPECT_TRUE(success1);

    PropertyDescriptor desc2(thread);
    desc2.SetValue(JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)));
    success1 = JSObject::DefineOwnProperty(thread, obj, key2, desc2);
    EXPECT_TRUE(success1);

    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key1).GetValue().GetTaggedValue(),
              JSTaggedValue(2));
}

JSTaggedValue TestSetter(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj(BuiltinsBase::GetThis(argv));
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("y"));
    JSTaggedValue value(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue().GetTaggedValue());
    JSHandle<JSTaggedValue> valueHandle(thread, JSTaggedValue(value.GetInt() + 1));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key, valueHandle);

    return JSTaggedValue(JSTaggedValue::True());
}

HWTEST_F_L0(JSObjectTest, Setter)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);
    JSHandle<JSTaggedValue> key1(factory->NewFromASCII("x"));
    JSHandle<JSTaggedValue> key2(factory->NewFromASCII("y"));
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> setter =
        thread->GetEcmaVM()->GetFactory()->NewJSFunction(env, reinterpret_cast<void *>(TestSetter));

    PropertyDescriptor desc1(thread);
    desc1.SetSetter(JSHandle<JSTaggedValue>::Cast(setter));
    bool success1 = JSObject::DefineOwnProperty(thread, obj, key1, desc1);
    EXPECT_TRUE(success1);

    PropertyDescriptor desc2(thread, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)), true, true, true);
    success1 = JSObject::DefineOwnProperty(thread, obj, key2, desc2);
    EXPECT_TRUE(success1);

    JSHandle<JSTaggedValue> valueHandle(thread, JSTaggedValue::Undefined());
    EXPECT_TRUE(JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj), key1, valueHandle));
    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key2).GetValue().GetTaggedValue(),
              JSTaggedValue(2));
}

HWTEST_F_L0(JSObjectTest, SpeciesConstructor)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    const GlobalEnvConstants *globalConst = thread->GlobalConstants();
    JSHandle<JSFunction> constructorFunc =
        factory->NewJSFunction(env, static_cast<void *>(nullptr), FunctionKind::BASE_CONSTRUCTOR);
    JSHandle<JSTaggedValue> constructorFuncValue(constructorFunc);
    constructorFunc->GetJSHClass()->SetExtensible(true);
    JSFunction::NewJSFunctionPrototype(thread, constructorFunc);

    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSTaggedValue> undefinedValue(thread, JSTaggedValue::Undefined());
    JSHandle<JSObject> protoObj = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSTaggedValue> protoObjValue(protoObj);

    JSHandle<JSTaggedValue> constructorKey = globalConst->GetHandledConstructorString();
    JSObject::SetProperty(thread, protoObjValue, constructorKey, constructorFuncValue);

    factory->NewJSObjectByConstructor(constructorFunc, JSHandle<JSTaggedValue>::Cast(constructorFunc));
    JSHandle<JSFunction> speciesConstruct =
        factory->NewJSFunction(env, static_cast<void *>(nullptr), FunctionKind::BASE_CONSTRUCTOR);
    JSHandle<JSTaggedValue> speciesConstructValue(speciesConstruct);
    constructorFunc->GetJSHClass()->SetExtensible(true);
    JSFunction::MakeConstructor(thread, speciesConstruct, undefinedValue);

    JSHandle<JSTaggedValue> speciesSymbol = env->GetSpeciesSymbol();
    JSObject::SetProperty(thread, constructorFuncValue, speciesSymbol, speciesConstructValue);

    JSTaggedValue speciesValue =
        JSObject::SpeciesConstructor(thread, protoObj, constructorFuncValue).GetTaggedValue();
    EXPECT_EQ(speciesValue, speciesConstructValue.GetTaggedValue());
}

JSTaggedValue TestUndefinedGetter([[maybe_unused]] EcmaRuntimeCallInfo *argv)
{
    // 10 : test case
    return JSTaggedValue(10);
}

JSTaggedValue TestUndefinedSetter([[maybe_unused]] EcmaRuntimeCallInfo *argv)
{
    // 10 : test case
    return JSTaggedValue(10);
}

JSHandle<JSObject> GetterOrSetterIsUndefined(JSThread *thread, JSHandle<JSTaggedValue>& key)
{
    JSHandle<JSTaggedValue> hclass1(thread, JSObjectTestCreate(thread));
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass1), hclass1);

    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> getter =
        thread->GetEcmaVM()->GetFactory()->NewJSFunction(env, reinterpret_cast<void *>(TestUndefinedGetter));
    JSHandle<JSFunction> setter =
        thread->GetEcmaVM()->GetFactory()->NewJSFunction(env, reinterpret_cast<void *>(TestUndefinedSetter));

    PropertyDescriptor desc1(thread);
    desc1.SetGetter(JSHandle<JSTaggedValue>::Cast(getter));
    desc1.SetSetter(JSHandle<JSTaggedValue>::Cast(setter));
    desc1.SetConfigurable(true);
    desc1.SetEnumerable(true);
    bool success1 = JSObject::DefineOwnProperty(thread, obj, key, desc1);
    EXPECT_TRUE(success1);
    return obj;
}

HWTEST_F_L0(JSObjectTest, GetterIsUndefined)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> unGetter(thread, JSTaggedValue::Undefined());
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("property"));
    auto obj = GetterOrSetterIsUndefined(thread, key);
    PropertyDescriptor desc2(thread);
    desc2.SetGetter(unGetter);
    bool success2 = JSObject::DefineOwnProperty(thread, obj, key, desc2);
    EXPECT_TRUE(success2);

    PropertyDescriptor desc(thread);
    bool success = JSObject::GetOwnProperty(thread, obj, key, desc);
    EXPECT_TRUE(success);
    EXPECT_TRUE(desc.GetSetter()->IsJSFunction());
    EXPECT_TRUE(desc.GetGetter()->IsUndefined());
}

HWTEST_F_L0(JSObjectTest, SetterIsUndefined)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> unSetter(thread, JSTaggedValue::Undefined());
    JSHandle<JSTaggedValue> key(factory->NewFromASCII("property"));
    auto obj = GetterOrSetterIsUndefined(thread, key);
    PropertyDescriptor desc2(thread);
    desc2.SetSetter(unSetter);
    bool success2 = JSObject::DefineOwnProperty(thread, obj, key, desc2);
    EXPECT_TRUE(success2);

    PropertyDescriptor desc(thread);
    bool success = JSObject::GetOwnProperty(thread, obj, key, desc);
    EXPECT_TRUE(success);
    EXPECT_TRUE(desc.GetSetter()->IsUndefined());

    EXPECT_EQ(JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj), key).GetValue().GetTaggedValue(),
              JSTaggedValue(10));
}

HWTEST_F_L0(JSObjectTest, Transitions)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSHandle<JSObject> obj2 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);

    JSHandle<JSHClass> hc1(thread, obj1->GetJSHClass());
    JSHandle<JSHClass> hc2(thread, obj2->GetJSHClass());
    EXPECT_EQ(hc1.GetTaggedValue(), hc2.GetTaggedValue());

    JSHandle<JSTaggedValue> key1(factory->NewFromASCII("x"));
    JSHandle<JSTaggedValue> key2(factory->NewFromASCII("y"));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    // key1
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, value);
    JSHandle<JSHClass> hc3(thread, obj1->GetJSHClass());
    EXPECT_NE(hc1.GetTaggedValue(), hc3.GetTaggedValue());

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key1, value);
    JSHandle<JSHClass> hc4(thread, obj2->GetJSHClass());
    EXPECT_EQ(hc3.GetTaggedValue(), hc4.GetTaggedValue());

    // key2
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key2, value);
    JSHandle<JSHClass> hc5(thread, obj1->GetJSHClass());
    EXPECT_NE(hc3.GetTaggedValue(), hc5.GetTaggedValue());

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key2, value);
    JSHandle<JSHClass> hc6(thread, obj2->GetJSHClass());
    EXPECT_EQ(hc5.GetTaggedValue(), hc6.GetTaggedValue());
}

HWTEST_F_L0(JSObjectTest, FastToSlow)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();

    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);

    JSMutableHandle<EcmaString> key(thread, factory->NewFromASCII("x"));
    JSMutableHandle<JSTaggedValue> number(thread, JSTaggedValue(0));
    JSMutableHandle<JSTaggedValue> newkey(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    ecmaVM->SetEnableForceGC(false);
    for (uint32_t i = 0; i < PropertyAttributes::MAX_FAST_PROPS_CAPACITY; i++) {
        number.Update(JSTaggedValue(i));
        number.Update(JSTaggedValue::ToString(thread, number).GetTaggedValue());
        EcmaString *newString = *factory->ConcatFromString(key, JSTaggedValue::ToString(thread, number));
        newkey.Update(JSTaggedValue(newString));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), newkey, value);
    }
    ecmaVM->SetEnableForceGC(true);

    EXPECT_FALSE(TaggedArray::Cast(obj1->GetProperties().GetTaggedObject())->IsDictionaryMode());

    number.Update(JSTaggedValue(PropertyAttributes::MAX_FAST_PROPS_CAPACITY));
    number.Update(JSTaggedValue::ToString(thread, number).GetTaggedValue());
    EcmaString *newString = *factory->ConcatFromString(key, JSTaggedValue::ToString(thread, number));
    newkey.Update(JSTaggedValue(newString));
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), newkey, value);

    EXPECT_TRUE(TaggedArray::Cast(obj1->GetProperties().GetTaggedObject())->IsDictionaryMode());
    NameDictionary *dict = NameDictionary::Cast(obj1->GetProperties().GetTaggedObject());
    EXPECT_EQ(dict->EntriesCount(), static_cast<int>(PropertyAttributes::MAX_FAST_PROPS_CAPACITY + 1));
    EXPECT_EQ(dict->NextEnumerationIndex(thread),
              static_cast<int>(PropertyAttributes::MAX_FAST_PROPS_CAPACITY + 1));
}

HWTEST_F_L0(JSObjectTest, DeleteMiddle)
{
    auto ecmaVM = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVM->GetFactory();

    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj1 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);

    JSMutableHandle<EcmaString> key(thread, factory->NewFromASCII("x"));
    JSMutableHandle<JSTaggedValue> number(thread, JSTaggedValue(0));
    JSMutableHandle<JSTaggedValue> newkey(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> value(thread, JSTaggedValue(1));

    for (uint32_t i = 0; i < 10; i++) {
        number.Update(JSTaggedValue(i));
        number.Update(JSTaggedValue::ToString(thread, number).GetTaggedValue());
        EcmaString *newString = *factory->ConcatFromString(key, JSTaggedValue::ToString(thread, number));
        newkey.Update(JSTaggedValue(newString));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), newkey, value);
    }

    EXPECT_FALSE(TaggedArray::Cast(obj1->GetProperties().GetTaggedObject())->IsDictionaryMode());

    JSMutableHandle<JSTaggedValue> key5(thread, factory->NewFromASCII("x5"));
    JSObject::DeleteProperty(thread, (obj1), key5);

    EXPECT_TRUE(TaggedArray::Cast(obj1->GetProperties().GetTaggedObject())->IsDictionaryMode());
    NameDictionary *dict = NameDictionary::Cast(obj1->GetProperties().GetTaggedObject());
    EXPECT_EQ(dict->EntriesCount(), 9);
    EXPECT_FALSE(JSObject::HasProperty(thread, obj1, key5));
}

HWTEST_F_L0(JSObjectTest, ElementFastToSlow)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> key0(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> key1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> key2(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> key2000(thread, JSTaggedValue(2000));
    JSHandle<JSTaggedValue> keyStr(factory->NewFromASCII("str"));

    // test dictionary [0,1,2,...,2000]
    JSHandle<JSObject> obj1 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    EXPECT_TRUE(!TaggedArray::Cast(obj1->GetElements().GetTaggedObject())->IsDictionaryMode());
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), keyStr, key2);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key0, key0);
    EXPECT_TRUE(!TaggedArray::Cast(obj1->GetElements().GetTaggedObject())->IsDictionaryMode());
    JSHandle<JSHClass> hclass(thread, obj1->GetJSHClass());
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key1, key1);
    EXPECT_TRUE(!TaggedArray::Cast(obj1->GetElements().GetTaggedObject())->IsDictionaryMode());
    EXPECT_EQ(obj1->GetJSHClass(), *hclass);

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), key2000, key2000);
    EXPECT_TRUE(TaggedArray::Cast(obj1->GetElements().GetTaggedObject())->IsDictionaryMode());
    JSTaggedValue value =
        JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj1), keyStr).GetValue().GetTaggedValue();
    EXPECT_EQ(value, key2.GetTaggedValue());
    // test holey [0,,2]
    JSHandle<JSObject> obj2 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key0, key0);
    EXPECT_TRUE(!TaggedArray::Cast(obj2->GetElements().GetTaggedObject())->IsDictionaryMode());
    JSHandle<JSHClass> hclass2(thread, obj2->GetJSHClass());
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), key2, key2);
    EXPECT_TRUE(!TaggedArray::Cast(obj2->GetElements().GetTaggedObject())->IsDictionaryMode());
    EXPECT_EQ(obj2->GetJSHClass(), *hclass2);
    // test change attr
    JSHandle<JSObject> obj3 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), key0, key0);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), key1, key1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), key2, key2);
    EXPECT_TRUE(!TaggedArray::Cast(obj3->GetElements().GetTaggedObject())->IsDictionaryMode());
    PropertyDescriptor desc(thread);
    desc.SetValue(key1);
    desc.SetWritable(false);
    JSObject::DefineOwnProperty(thread, obj3, key1, desc);
    EXPECT_TRUE(TaggedArray::Cast(obj3->GetElements().GetTaggedObject())->IsDictionaryMode());
    // test delete element
    JSHandle<JSObject> obj4 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), key0, key0);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), key1, key1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), key2, key2);
    EXPECT_TRUE(!TaggedArray::Cast(obj4->GetElements().GetTaggedObject())->IsDictionaryMode());
    JSObject::DeleteProperty(thread, (obj4), key1);
    EXPECT_TRUE(TaggedArray::Cast(obj4->GetElements().GetTaggedObject())->IsDictionaryMode());

    JSHandle<JSTaggedValue> value1001(thread, JSTaggedValue(1001));
    JSHandle<JSObject> obj100 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    PropertyDescriptor desc1(thread);
    desc1.SetValue(value1001);
    desc1.SetWritable(false);
    desc1.SetEnumerable(false);
    desc1.SetConfigurable(false);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj100), key0, key1);
    JSObject::DefineOwnProperty(thread, obj100, key0, desc1);
    JSTaggedValue result1001 =
        JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(obj100), key0).GetValue().GetTaggedValue();
    EXPECT_EQ(result1001, value1001.GetTaggedValue());
}

HWTEST_F_L0(JSObjectTest, EnableProtoChangeMarker)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> obj1 = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> obj2 = JSObject::ObjectCreate(thread, obj1);
    JSHandle<JSObject> obj3 = JSObject::ObjectCreate(thread, obj2);

    JSHandle<JSTaggedValue> obj1Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> obj2Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> obj3Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> obj1Value(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> obj2Value(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> obj3Value(thread, JSTaggedValue(3));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), obj1Key, obj1Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), obj2Key, obj2Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), obj3Key, obj3Value);
    JSHandle<JSHClass> obj3Class(thread, obj3->GetJSHClass());
    JSHandle<JSTaggedValue> resultMarker = JSHClass::EnableProtoChangeMarker(thread, obj3Class);
    EXPECT_TRUE(resultMarker->IsProtoChangeMarker());
    bool hasChanged = ProtoChangeMarker::Cast(resultMarker->GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(!hasChanged);

    JSHandle<JSHClass> obj1Class(thread, obj1->GetJSHClass());
    JSHandle<JSHClass> obj2Class(thread, obj2->GetJSHClass());
    JSTaggedValue obj2Marker = obj2Class->GetProtoChangeMarker();
    EXPECT_TRUE(obj2Marker.IsProtoChangeMarker());
    bool hasChanged2 = ProtoChangeMarker::Cast(obj2Marker.GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(!hasChanged2);

    JSTaggedValue obj1Marker = obj1Class->GetProtoChangeMarker();
    EXPECT_TRUE(!obj1Marker.IsProtoChangeMarker());

    JSTaggedValue protoDetails2 = obj2Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails2.IsProtoChangeDetails());
    JSTaggedValue protoDetails1 = obj1Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails1.IsProtoChangeDetails());
    JSTaggedValue listeners1 = ProtoChangeDetails::Cast(protoDetails1.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(!listeners1.IsUndefined());
    JSTaggedValue listeners2 = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners2.IsUndefined());
    uint32_t index = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetRegisterIndex();
    JSTaggedValue listenersResult = ChangeListener::Cast(listeners1.GetTaggedObject())->Get(index);
    EXPECT_TRUE(listenersResult == obj2Class.GetTaggedValue());
}

HWTEST_F_L0(JSObjectTest, BuildRegisterTree)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> obj1 = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> obj2 = JSObject::ObjectCreate(thread, obj1);
    JSHandle<JSObject> obj3 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj4 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj5 = JSObject::ObjectCreate(thread, obj4);
    JSHandle<JSObject> obj6 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj7 = JSObject::ObjectCreate(thread, obj6);

    JSHandle<JSTaggedValue> obj1Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> obj2Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> obj3Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> obj4Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key4"));
    JSHandle<JSTaggedValue> obj5Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key5"));
    JSHandle<JSTaggedValue> obj6Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key6"));
    JSHandle<JSTaggedValue> obj7Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key7"));

    JSHandle<JSTaggedValue> obj1Value(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> obj2Value(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> obj3Value(thread, JSTaggedValue(3));
    JSHandle<JSTaggedValue> obj4Value(thread, JSTaggedValue(4));
    JSHandle<JSTaggedValue> obj5Value(thread, JSTaggedValue(5));
    JSHandle<JSTaggedValue> obj6Value(thread, JSTaggedValue(6));
    JSHandle<JSTaggedValue> obj7Value(thread, JSTaggedValue(7));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), obj1Key, obj1Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), obj2Key, obj2Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), obj3Key, obj3Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), obj4Key, obj4Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj5), obj5Key, obj5Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj6), obj6Key, obj6Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj7), obj7Key, obj7Value);

    JSHandle<JSHClass> obj1Class(thread, obj1->GetJSHClass());
    JSHandle<JSHClass> obj2Class(thread, obj2->GetJSHClass());
    JSHandle<JSHClass> obj3Class(thread, obj3->GetJSHClass());
    JSHandle<JSHClass> obj4Class(thread, obj4->GetJSHClass());
    JSHandle<JSHClass> obj5Class(thread, obj5->GetJSHClass());
    JSHandle<JSHClass> obj6Class(thread, obj6->GetJSHClass());
    JSHandle<JSHClass> obj7Class(thread, obj7->GetJSHClass());

    JSHandle<JSTaggedValue> result3Marker = JSHClass::EnableProtoChangeMarker(thread, obj3Class);
    JSHandle<JSTaggedValue> result5Marker = JSHClass::EnableProtoChangeMarker(thread, obj5Class);
    EXPECT_TRUE(result3Marker->IsProtoChangeMarker());
    EXPECT_TRUE(!(ProtoChangeMarker::Cast(result3Marker->GetTaggedObject())->GetHasChanged()));
    EXPECT_TRUE(result5Marker->IsProtoChangeMarker());
    EXPECT_TRUE(!(ProtoChangeMarker::Cast(result5Marker->GetTaggedObject())->GetHasChanged()));

    EXPECT_TRUE(obj4Class->GetProtoChangeMarker().IsProtoChangeMarker());
    EXPECT_TRUE(!obj6Class->GetProtoChangeMarker().IsProtoChangeMarker());

    JSHandle<JSTaggedValue> result7Marker = JSHClass::EnableProtoChangeMarker(thread, obj7Class);
    EXPECT_TRUE(result7Marker->IsProtoChangeMarker());
    EXPECT_TRUE(!(ProtoChangeMarker::Cast(result7Marker->GetTaggedObject())->GetHasChanged()));

    JSTaggedValue protoDetails1 = obj1Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails1.IsProtoChangeDetails());
    JSTaggedValue listeners1Value = ProtoChangeDetails::Cast(protoDetails1.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners1Value != JSTaggedValue(0));
    JSHandle<ChangeListener> listeners1(thread, listeners1Value.GetTaggedObject());
    JSTaggedValue protoDetails2 = obj2Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails2.IsProtoChangeDetails());
    uint32_t index2 = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetRegisterIndex();
    EXPECT_TRUE(listeners1->Get(index2) == obj2Class.GetTaggedValue());

    JSTaggedValue listeners2Value = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners2Value != JSTaggedValue(0));
    JSHandle<ChangeListener> listeners2(thread, listeners2Value.GetTaggedObject());
    JSTaggedValue protoDetails4 = obj4Class->GetProtoChangeDetails();
    JSTaggedValue protoDetails6 = obj6Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails4.IsProtoChangeDetails());
    EXPECT_TRUE(protoDetails6.IsProtoChangeDetails());
    uint32_t index4 = ProtoChangeDetails::Cast(protoDetails4.GetTaggedObject())->GetRegisterIndex();
    EXPECT_TRUE(listeners2->Get(index4) == obj4Class.GetTaggedValue());
    uint32_t index6 = ProtoChangeDetails::Cast(protoDetails6.GetTaggedObject())->GetRegisterIndex();
    EXPECT_TRUE(listeners2->Get(index6) == obj6Class.GetTaggedValue());

    EXPECT_TRUE(listeners1->GetEnd() == 1U);
    EXPECT_TRUE(listeners2->GetEnd() == 2U);
}

HWTEST_F_L0(JSObjectTest, NoticeThroughChain)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> obj1 = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> obj2 = JSObject::ObjectCreate(thread, obj1);
    JSHandle<JSObject> obj3 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj4 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj5 = JSObject::ObjectCreate(thread, obj4);
    JSHandle<JSObject> obj6 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj7 = JSObject::ObjectCreate(thread, obj6);

    JSHandle<JSTaggedValue> obj1Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> obj2Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> obj3Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> obj4Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key4"));
    JSHandle<JSTaggedValue> obj5Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key5"));
    JSHandle<JSTaggedValue> obj6Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key6"));
    JSHandle<JSTaggedValue> obj7Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key7"));

    JSHandle<JSTaggedValue> obj1Value(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> obj2Value(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> obj3Value(thread, JSTaggedValue(3));
    JSHandle<JSTaggedValue> obj4Value(thread, JSTaggedValue(4));
    JSHandle<JSTaggedValue> obj5Value(thread, JSTaggedValue(5));
    JSHandle<JSTaggedValue> obj6Value(thread, JSTaggedValue(6));
    JSHandle<JSTaggedValue> obj7Value(thread, JSTaggedValue(7));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), obj1Key, obj1Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), obj2Key, obj2Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), obj3Key, obj3Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), obj4Key, obj4Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj5), obj5Key, obj5Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj6), obj6Key, obj6Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj7), obj7Key, obj7Value);

    JSHandle<JSHClass> obj1Class(thread, obj1->GetJSHClass());
    JSHandle<JSHClass> obj2Class(thread, obj2->GetJSHClass());
    JSHandle<JSHClass> obj3Class(thread, obj3->GetJSHClass());
    JSHandle<JSHClass> obj4Class(thread, obj4->GetJSHClass());
    JSHandle<JSHClass> obj5Class(thread, obj5->GetJSHClass());
    JSHandle<JSHClass> obj6Class(thread, obj6->GetJSHClass());
    JSHandle<JSHClass> obj7Class(thread, obj7->GetJSHClass());

    JSHClass::EnableProtoChangeMarker(thread, obj3Class);
    JSHClass::EnableProtoChangeMarker(thread, obj7Class);
    JSHClass::EnableProtoChangeMarker(thread, obj5Class);

    JSHClass::NoticeThroughChain(thread, obj2Class);
    JSHClass::UnregisterOnProtoChain(thread, obj2Class);
    JSTaggedValue protoDetails1 = obj1Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails1.IsProtoChangeDetails());
    JSTaggedValue listeners1Value = ProtoChangeDetails::Cast(protoDetails1.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners1Value != JSTaggedValue(0));
    JSHandle<ChangeListener> listeners1(thread, listeners1Value.GetTaggedObject());
    uint32_t holeIndex = ChangeListener::CheckHole(listeners1);
    EXPECT_TRUE(holeIndex == 0U);

    JSTaggedValue protoDetails2 = obj2Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails2.IsProtoChangeDetails());
    JSTaggedValue listeners2Value = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners2Value != JSTaggedValue(0));
    uint32_t index2 = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetRegisterIndex();
    EXPECT_TRUE(listeners1->Get(index2).IsHole());

    JSTaggedValue obj6Marker = obj6Class->GetProtoChangeMarker();
    EXPECT_TRUE(obj6Marker.IsProtoChangeMarker());
    bool hasChanged6 = ProtoChangeMarker::Cast(obj6Marker.GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(hasChanged6);

    JSTaggedValue obj4Marker = obj4Class->GetProtoChangeMarker();
    EXPECT_TRUE(obj4Marker.IsProtoChangeMarker());
    bool hasChanged4 = ProtoChangeMarker::Cast(obj4Marker.GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(hasChanged4);
}

HWTEST_F_L0(JSObjectTest, ChangeProtoAndNoticeTheChain)
{
    JSHandle<JSObject> nullHandle(thread, JSTaggedValue::Null());
    JSHandle<JSObject> obj1 = JSObject::ObjectCreate(thread, nullHandle);
    JSHandle<JSObject> obj2 = JSObject::ObjectCreate(thread, obj1);
    JSHandle<JSObject> obj3 = JSObject::ObjectCreate(thread, obj1);
    JSHandle<JSObject> obj4 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj5 = JSObject::ObjectCreate(thread, obj4);
    JSHandle<JSObject> obj6 = JSObject::ObjectCreate(thread, obj2);
    JSHandle<JSObject> obj7 = JSObject::ObjectCreate(thread, obj6);

    JSHandle<JSTaggedValue> obj1Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key1"));
    JSHandle<JSTaggedValue> obj2Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key2"));
    JSHandle<JSTaggedValue> obj3Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key3"));
    JSHandle<JSTaggedValue> obj4Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key4"));
    JSHandle<JSTaggedValue> obj5Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key5"));
    JSHandle<JSTaggedValue> obj6Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key6"));
    JSHandle<JSTaggedValue> obj7Key(thread->GetEcmaVM()->GetFactory()->NewFromASCII("key7"));

    JSHandle<JSTaggedValue> obj1Value(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> obj2Value(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> obj3Value(thread, JSTaggedValue(3));
    JSHandle<JSTaggedValue> obj4Value(thread, JSTaggedValue(4));
    JSHandle<JSTaggedValue> obj5Value(thread, JSTaggedValue(5));
    JSHandle<JSTaggedValue> obj6Value(thread, JSTaggedValue(6));
    JSHandle<JSTaggedValue> obj7Value(thread, JSTaggedValue(7));

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj1), obj1Key, obj1Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj2), obj2Key, obj2Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj3), obj3Key, obj3Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj4), obj4Key, obj4Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj5), obj5Key, obj5Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj6), obj6Key, obj6Value);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(obj7), obj7Key, obj7Value);

    JSHandle<JSHClass> obj5Class(thread, obj5->GetJSHClass());
    JSHandle<JSHClass> obj7Class(thread, obj7->GetJSHClass());

    JSHClass::EnableProtoChangeMarker(thread, obj7Class);
    JSHClass::EnableProtoChangeMarker(thread, obj5Class);

    JSObject::SetPrototype(thread, obj2, JSHandle<JSTaggedValue>(obj3));

    JSHandle<JSHClass> obj1Class(thread, obj1->GetJSHClass());
    JSHandle<JSHClass> obj2Class(thread, obj2->GetJSHClass());
    JSHandle<JSHClass> obj3Class(thread, obj3->GetJSHClass());
    JSHandle<JSHClass> obj4Class(thread, obj4->GetJSHClass());
    JSHandle<JSHClass> obj6Class(thread, obj6->GetJSHClass());

    JSTaggedValue obj6Marker = obj6Class->GetProtoChangeMarker();
    EXPECT_TRUE(obj6Marker.IsProtoChangeMarker());
    bool hasChanged6 = ProtoChangeMarker::Cast(obj6Marker.GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(hasChanged6);

    JSTaggedValue obj4Marker = obj4Class->GetProtoChangeMarker();
    EXPECT_TRUE(obj4Marker.IsProtoChangeMarker());
    bool hasChanged4 = ProtoChangeMarker::Cast(obj4Marker.GetTaggedObject())->GetHasChanged();
    EXPECT_TRUE(hasChanged4);

    JSTaggedValue protoDetails1 = obj1Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails1.IsProtoChangeDetails());
    JSTaggedValue protoDetails2 = obj2Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails2.IsProtoChangeDetails());
    JSTaggedValue protoDetails3 = obj3Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails3.IsProtoChangeDetails());
    JSTaggedValue protoDetails4 = obj4Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails4.IsProtoChangeDetails());
    JSTaggedValue protoDetails6 = obj6Class->GetProtoChangeDetails();
    EXPECT_TRUE(protoDetails6.IsProtoChangeDetails());

    JSTaggedValue listeners1 = ProtoChangeDetails::Cast(protoDetails1.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners1 != JSTaggedValue(0));
    JSTaggedValue listeners2 = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners2 != JSTaggedValue(0));
    JSTaggedValue listeners3 = ProtoChangeDetails::Cast(protoDetails3.GetTaggedObject())->GetChangeListener();
    EXPECT_TRUE(listeners3 != JSTaggedValue(0));

    uint32_t index2 = ProtoChangeDetails::Cast(protoDetails2.GetTaggedObject())->GetRegisterIndex();
    uint32_t index3 = ProtoChangeDetails::Cast(protoDetails3.GetTaggedObject())->GetRegisterIndex();
    uint32_t index4 = ProtoChangeDetails::Cast(protoDetails4.GetTaggedObject())->GetRegisterIndex();
    uint32_t index6 = ProtoChangeDetails::Cast(protoDetails6.GetTaggedObject())->GetRegisterIndex();

    JSTaggedValue result2 = ChangeListener::Cast(listeners3.GetTaggedObject())->Get(index2);
    JSTaggedValue result3 = ChangeListener::Cast(listeners1.GetTaggedObject())->Get(index3);
    JSTaggedValue result4 = ChangeListener::Cast(listeners2.GetTaggedObject())->Get(index4);
    JSTaggedValue result6 = ChangeListener::Cast(listeners2.GetTaggedObject())->Get(index6);

    EXPECT_TRUE(result2 == obj2Class.GetTaggedValue());
    EXPECT_TRUE(result3 == obj3Class.GetTaggedValue());
    EXPECT_TRUE(result4 == obj4Class.GetTaggedValue());
    EXPECT_TRUE(result6 == obj6Class.GetTaggedValue());
}

HWTEST_F_L0(JSObjectTest, NativePointerField)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    ECMAObject::SetHash(thread, 87, JSHandle<ECMAObject>::Cast(obj));
    EXPECT_TRUE(obj->GetHash() == 87);

    ECMAObject::SetNativePointerFieldCount(thread, obj, 1);
    char array[] = "Hello World!";
    ECMAObject::SetNativePointerField(thread, obj, 0, array, nullptr, nullptr);
    int32_t count = obj->GetNativePointerFieldCount();
    EXPECT_TRUE(count == 1);
    void *pointer = obj->GetNativePointerField(0);
    EXPECT_TRUE(pointer == array);
}

static JSHandle<JSHClass> CreateTestHClass(JSThread *thread)
{
    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objectFuncPrototype = globalEnv->GetObjectFunctionPrototype();
    JSHandle<JSHClass> hclass = factory->NewEcmaHClass(JSObject::SIZE, JSType::JS_OBJECT, objectFuncPrototype);
    return hclass;
}

HWTEST_F_L0(JSObjectTest, UpdateWeakTransitions)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    EcmaVM *vm = thread->GetEcmaVM();
    PropertyAttributes attr = PropertyAttributes::Default();

    // Initialize three objects by hc0
    JSHandle<JSHClass> hc0 = CreateTestHClass(thread);
    JSMutableHandle<JSHClass> hca(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSHClass> hcb(thread, JSTaggedValue::Undefined());
    [[maybe_unused]] JSHandle<JSObject> obj0 = factory->NewJSObject(hc0);  // need it to ensure hc0 not be collected
    JSHandle<JSObject> obj1 = factory->NewJSObject(hc0);
    JSHandle<JSObject> obj2 = factory->NewJSObject(hc0);

    JSHandle<JSTaggedValue> keyA(factory->NewFromASCII("a"));
    JSHandle<JSTaggedValue> keyB(factory->NewFromASCII("b"));

    {
        // need a new handle scope to ensure no scope refers hc1, hc2.
        [[maybe_unused]] EcmaHandleScope handleScope(thread);

        //                               / hc1 (obj1)
        // occur transition,  hc0 (obj0)
        //                               \ hc2 (obj2)
        JSObject::SetProperty(thread, obj1, keyA, JSHandle<JSTaggedValue>(thread, JSTaggedValue(1)));
        JSObject::SetProperty(thread, obj2, keyB, JSHandle<JSTaggedValue>(thread, JSTaggedValue(2)));

        EXPECT_TRUE(hc0->GetTransitions().IsTaggedArray());
        EXPECT_EQ(hc0->FindTransitions(keyA.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
            obj1->GetClass());
        EXPECT_EQ(hc0->FindTransitions(keyB.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
            obj2->GetClass());
        hca.Update(JSTaggedValue(obj1->GetClass()));
        hcb.Update(JSTaggedValue(obj2->GetClass()));

        //            / hc1 --> hc3 (obj1)
        // hc0 (obj0)
        //            \ hc2 --> hc4 (obj2)
        JSObject::SetProperty(thread, obj1, keyB, JSHandle<JSTaggedValue>(thread, JSTaggedValue(3)));
        JSObject::SetProperty(thread, obj2, keyA, JSHandle<JSTaggedValue>(thread, JSTaggedValue(4)));
    }

    // collect hc1, hc2
    vm->CollectGarbage(TriggerGCType::FULL_GC);

    EXPECT_EQ(hc0->FindTransitions(keyA.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
        hca.GetObject<JSHClass>());
    EXPECT_EQ(hc0->FindTransitions(keyB.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
        hcb.GetObject<JSHClass>());

    JSHandle<JSObject> obj3 = factory->NewJSObject(hc0);
    JSHandle<JSObject> obj4 = factory->NewJSObject(hc0);

    //                                  / hc5 (obj3)
    // re-occur transition,  hc0 (obj0)
    //                                  \ hc6 (obj4)
    JSObject::SetProperty(thread, obj3, keyA, JSHandle<JSTaggedValue>(thread, JSTaggedValue(5)));
    JSObject::SetProperty(thread, obj4, keyB, JSHandle<JSTaggedValue>(thread, JSTaggedValue(6)));

    EXPECT_TRUE(hc0->GetTransitions().IsTaggedArray());
    EXPECT_EQ(hc0->FindTransitions(keyA.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
        obj3->GetClass());
    EXPECT_EQ(hc0->FindTransitions(keyB.GetTaggedValue(), attr.GetTaggedValue(), Representation::NONE),
        obj4->GetClass());
    EXPECT_EQ(obj3->GetClass(), hca.GetObject<JSHClass>());
    EXPECT_EQ(obj4->GetClass(), hcb.GetObject<JSHClass>());
}

HWTEST_F_L0(JSObjectTest, CreateObjectFromProperties)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<TaggedArray> properties = factory->NewTaggedArray(2060); // 2060: array length

    JSHandle<EcmaString> key1(factory->NewFromASCII("aa"));
    JSHandle<JSTaggedValue> value1(thread, JSTaggedValue(1));

    for (int i = 0; i < 1030; i++) {
        JSHandle<EcmaString> key2(factory->NewFromASCII(std::to_string(i)));
        JSHandle<EcmaString> key3 =
            JSHandle<EcmaString>(thread, EcmaStringAccessor::Concat(thread->GetEcmaVM(), key1, key2));
        properties->Set(thread, 2 * i, key3.GetTaggedValue());
        properties->Set(thread, 2 * i + 1, value1.GetTaggedValue());
    }

    JSHandle<JSObject> newObj = JSObject::CreateObjectFromProperties(thread, properties);
    Heap *heap = const_cast<Heap *>(thread->GetEcmaVM()->GetHeap());
    heap->GetSweeper()->EnsureAllTaskFinished();
    Verification(heap).VerifyAll();
    auto sHeap = SharedHeap::GetInstance();
    sHeap->WaitGCFinished(thread);
    SuspendAllScope suspendScope(thread);
    SharedHeapVerification(sHeap, VerifyKind::VERIFY_PRE_SHARED_GC).VerifyAll();
    EXPECT_TRUE(newObj->GetClass()->IsDictionaryMode());
}
}  // namespace panda::test
