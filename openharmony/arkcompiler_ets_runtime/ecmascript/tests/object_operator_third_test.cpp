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

#include "ecmascript/object_operator.h"
#include "ecmascript/ecma_string.h"
#include "ecmascript/global_env.h"
#include "ecmascript/global_dictionary-inl.h"
#include "ecmascript/js_array.h"
#include "ecmascript/property_attributes.h"
#include "ecmascript/tagged_array.h"
#include "ecmascript/tagged_dictionary.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;

namespace panda::test {
class ObjectOperatorTest : public BaseTestWithScope<false> {
};

static JSFunction *JSObjectTestCreate(JSThread *thread)
{
    JSHandle<GlobalEnv> globalEnv = thread->GetEcmaVM()->GetGlobalEnv();
    return globalEnv->GetObjectFunction().GetObject<JSFunction>();
}

JSTaggedValue TestDefinedSetter([[maybe_unused]] EcmaRuntimeCallInfo *argv)
{
    // 12 : test case
    return JSTaggedValue(12);
}

HWTEST_F_L0(ObjectOperatorTest, WriteDataProperty_001)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(4));
    uint32_t index = 1;
    PropertyDescriptor handleDesc(thread);
    ObjectOperator objectOperator(thread, handleKey);
    PropertyAttributes handleAttr(4);
    handleDesc.SetConfigurable(true); // Desc Set Configurable
    objectOperator.SetAttr(PropertyAttributes(3));
    objectOperator.SetIndex(index);
    // object class is not DictionaryElement and object is Element
    JSHandle<JSObject> handleObject = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 3; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), newKey, newKey);
    }
    EXPECT_TRUE(objectOperator.WriteDataProperty(handleObject, handleDesc));
    auto resultDict = NumberDictionary::Cast(handleObject->GetElements().GetTaggedObject());
    int resultEntry = resultDict->FindEntry(JSTaggedValue(index));
    int resultAttrValue = resultDict->GetAttributes(resultEntry).GetPropertyMetaData();

    EXPECT_EQ(objectOperator.GetAttr().GetPropertyMetaData(), resultAttrValue);
    EXPECT_EQ(objectOperator.GetAttr().GetDictionaryOrder(), 1U);
    EXPECT_TRUE(objectOperator.GetAttr().IsConfigurable());
    EXPECT_EQ(objectOperator.GetIndex(), static_cast<uint32_t>(resultEntry));
    EXPECT_FALSE(objectOperator.IsFastMode());
    EXPECT_TRUE(objectOperator.IsTransition());
}

HWTEST_F_L0(ObjectOperatorTest, WriteDataProperty_002)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> handleKey(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleValue1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> handleValue2(thread, JSTaggedValue(2));
    JSHandle<PropertyBox> cellHandle1 = factory->NewPropertyBox(handleValue1);
    JSHandle<PropertyBox> cellHandle2 = factory->NewPropertyBox(handleValue2);
    PropertyDescriptor handleDesc(thread);
    handleDesc.SetConfigurable(true);
    PropertyAttributes handleAttr(2);
    handleAttr.SetConfigurable(true);
    // object is JSGlobalObject and not Element
    JSHandle<JSTaggedValue> globalObj = env->GetJSGlobalObject();
    JSHandle<JSObject> handleGlobalObject(globalObj);
    ObjectOperator objectOperator(thread, handleGlobalObject, handleKey);

    JSMutableHandle<GlobalDictionary> globalDict(thread, handleGlobalObject->GetProperties());
    JSHandle<GlobalDictionary> handleDict = GlobalDictionary::Create(thread, 4);
    globalDict.Update(handleDict.GetTaggedValue());
    JSHandle<GlobalDictionary> handleProperties = GlobalDictionary::PutIfAbsent(
        thread, globalDict, handleKey, JSHandle<JSTaggedValue>(cellHandle1), PropertyAttributes(4));
    handleProperties->SetAttributes(thread, handleAttr.GetDictionaryOrder(), handleAttr);
    handleProperties->SetValue(thread, handleAttr.GetDictionaryOrder(), cellHandle2.GetTaggedValue());
    handleGlobalObject->SetProperties(thread, handleProperties.GetTaggedValue());
    int resultEntry = handleProperties->FindEntry(handleKey.GetTaggedValue());
    objectOperator.SetIndex(resultEntry);

    EXPECT_TRUE(objectOperator.WriteDataProperty(handleGlobalObject, handleDesc));
    auto resultDict = GlobalDictionary::Cast(handleGlobalObject->GetProperties().GetTaggedObject());
    EXPECT_EQ(resultDict->GetAttributes(objectOperator.GetIndex()).GetPropertyMetaData(), 4);
    EXPECT_TRUE(resultDict->GetAttributes(objectOperator.GetIndex()).IsConfigurable());

    EXPECT_EQ(resultDict->GetAttributes(resultEntry).GetBoxType(), PropertyBoxType::MUTABLE);
    EXPECT_EQ(resultDict->GetValue(resultEntry).GetInt(), 1);
}

HWTEST_F_L0(ObjectOperatorTest, WriteDataProperty_003)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(2));
    PropertyDescriptor handleDesc(thread, handleValue);
    handleDesc.SetSetter(handleValue); // Desc is AccessorDescriptor
    handleDesc.SetGetter(handleValue);
    // object is not DictionaryMode and not Element
    JSHandle<JSObject> handleObject = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 3; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), newKey, newKey);
    }
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), handleKey, handleValue);
    ObjectOperator objectOperator1(thread, handleKey);
    objectOperator1.SetAttr(PropertyAttributes(1));

    EXPECT_TRUE(objectOperator1.WriteDataProperty(handleObject, handleDesc));
    auto resultDict = NameDictionary::Cast(handleObject->GetProperties().GetTaggedObject());
    int resultEntry = resultDict->FindEntry(handleKey.GetTaggedValue());
    EXPECT_TRUE(resultDict->GetValue(resultEntry).IsAccessorData());
    EXPECT_EQ(resultDict->GetAttributes(resultEntry).GetValue(), objectOperator1.GetAttr().GetValue());
    // object is DictionaryMode and not Element
    JSObject::DeleteProperty(thread, (handleObject), handleKey);
    JSHandle<JSTaggedValue> handleSetter(factory->NewJSNativePointer(reinterpret_cast<void *>(TestDefinedSetter)));
    JSHandle<AccessorData> handleAccessorData = factory->NewAccessorData();
    handleDesc.SetSetter(handleSetter);
    ObjectOperator objectOperator2(thread, handleKey);
    objectOperator2.SetAttr(PropertyAttributes(handleDesc));
    objectOperator2.SetValue(handleAccessorData.GetTaggedValue());
    EXPECT_TRUE(objectOperator2.WriteDataProperty(handleObject, handleDesc));
    JSHandle<AccessorData> resultAccessorData(thread, objectOperator2.GetValue());
    EXPECT_EQ(resultAccessorData->GetGetter().GetInt(), 2);
    EXPECT_TRUE(resultAccessorData->GetSetter().IsJSNativePointer());
}

HWTEST_F_L0(ObjectOperatorTest, Property_Add_001)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(thread, JSTaggedValue(2));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(3));
    int32_t elementIndex = 2;
    PropertyAttributes handleAttr(elementIndex);
    // object is JSArray and Element
    JSHandle<JSArray> handleArr = factory->NewJSArray();
    handleArr->SetArrayLength(thread, (elementIndex - 1));
    JSHandle<JSTaggedValue> handleArrObj(thread, handleArr.GetTaggedValue());
    ObjectOperator objectOperator1(thread, handleArrObj, elementIndex);
    EXPECT_TRUE(objectOperator1.AddProperty(JSHandle<JSObject>(handleArrObj), handleValue, handleAttr));
    EXPECT_EQ(handleArr->GetArrayLength(), 3U); // (elementIndex - 1) + 2
    // object is DictionaryElement and Element
    JSHandle<JSObject> handleObject = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 3; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), newKey, newKey);
    }
    JSObject::DeleteProperty(thread, (handleObject), handleKey); // Delete key2
    ObjectOperator objectOperator2(thread, JSHandle<JSTaggedValue>(handleObject), elementIndex);
    EXPECT_TRUE(objectOperator2.AddProperty(handleObject, handleValue, handleAttr));
    auto resultDict = NumberDictionary::Cast(handleObject->GetElements().GetTaggedObject());
    int resultEntry = resultDict->FindEntry(JSTaggedValue(static_cast<uint32_t>(elementIndex)));
    EXPECT_EQ(resultDict->GetKey(resultEntry).GetInt(), elementIndex);
    EXPECT_EQ(resultDict->GetValue(resultEntry).GetInt(), 3);
}

HWTEST_F_L0(ObjectOperatorTest, Property_Add_002)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleString(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(3));
    int32_t elementIndex = 4;
    PropertyAttributes handleDefaultAttr(elementIndex);
    PropertyAttributes handleAttr(elementIndex);
    handleDefaultAttr.SetDefaultAttributes();
    // object is not DictionaryMode and DefaultAttr
    JSHandle<JSObject> handleObject1 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 3; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject1), newKey, newKey);
    }
    ObjectOperator objectOperator(thread, JSHandle<JSTaggedValue>(handleObject1), elementIndex);
    EXPECT_TRUE(objectOperator.AddProperty(handleObject1, handleValue, handleDefaultAttr));
    TaggedArray *resultArray = TaggedArray::Cast(handleObject1->GetElements().GetTaggedObject());
    EXPECT_EQ(resultArray->Get(elementIndex).GetInt(), 3);
    EXPECT_EQ(resultArray->GetLength(), 7U);
    // object is not DictionaryMode and not DefaultAttr
    JSHandle<JSObject> handleObject2 = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 4; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject2), newKey, newKey);
    }
    EXPECT_TRUE(objectOperator.AddProperty(handleObject2, handleString, handleAttr));
    auto resultDict = NumberDictionary::Cast(handleObject2->GetElements().GetTaggedObject());
    int resultEntry = resultDict->FindEntry(JSTaggedValue(static_cast<uint32_t>(elementIndex)));
    EXPECT_EQ(resultDict->GetKey(resultEntry).GetInt(), elementIndex);
    EXPECT_TRUE(resultDict->GetValue(resultEntry).IsString());
}

HWTEST_F_L0(ObjectOperatorTest, Property_Add_003)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(4));
    int32_t handleAttrOffset = 4;
    PropertyAttributes handleAttr(handleAttrOffset);
    handleAttr.SetOffset(handleAttrOffset);
    // object is JSGlobalObject and not Element
    JSHandle<JSTaggedValue> globalObj = env->GetJSGlobalObject();
    JSHandle<JSObject> handleGlobalObject(globalObj); // no properties
    ObjectOperator objectOperator(thread, handleGlobalObject, handleKey);
    EXPECT_TRUE(objectOperator.AddProperty(handleGlobalObject, handleValue, handleAttr));
    EXPECT_EQ(objectOperator.GetAttr().GetBoxType(), PropertyBoxType::CONSTANT);
    EXPECT_EQ(objectOperator.FastGetValue()->GetInt(), 4);
    EXPECT_EQ(objectOperator.GetIndex(), 0U);
    EXPECT_TRUE(objectOperator.IsFastMode());
    EXPECT_FALSE(objectOperator.IsTransition());
}

HWTEST_F_L0(ObjectOperatorTest, Property_Add_004)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleKey1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(4));
    JSHandle<JSTaggedValue> handledUndefinedVal(thread, JSTaggedValue::Undefined());
    int32_t handleAttrOffset = 4;
    PropertyAttributes handleAttr(handleAttrOffset);
    handleAttr.SetOffset(handleAttrOffset);
    // object is not DictionaryMode and not Element
    JSHandle<JSObject> handleObject = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 4; i++) {
        JSHandle<JSTaggedValue> newKey(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), newKey, newKey);
    }
    EXPECT_EQ(handleObject->GetJSHClass()->GetInlinedProperties(), 4U);
    ObjectOperator objectOperator(thread, handleObject, handleKey);
    EXPECT_TRUE(objectOperator.AddProperty(handleObject, handleValue, handleAttr));
    EXPECT_EQ(objectOperator.GetAttr().GetPropertyMetaData(), 4);
    EXPECT_EQ(objectOperator.GetValue().GetInt(), 4);
    EXPECT_EQ(objectOperator.GetIndex(), 0U); // 0 = 4 - 4
    EXPECT_TRUE(objectOperator.IsFastMode());
    EXPECT_TRUE(objectOperator.IsTransition());
    // object is DictionaryMode and not Element
    JSObject::DeleteProperty(thread, (handleObject), handleKey);
    EXPECT_TRUE(objectOperator.AddProperty(handleObject, handledUndefinedVal, handleAttr));
    EXPECT_EQ(objectOperator.GetAttr().GetPropertyMetaData(), 4);
    EXPECT_TRUE(objectOperator.GetValue().IsUndefined());
    EXPECT_EQ(objectOperator.GetIndex(), 0U);
    EXPECT_FALSE(objectOperator.IsFastMode());
    EXPECT_FALSE(objectOperator.IsTransition());
}

HWTEST_F_L0(ObjectOperatorTest, Property_DeleteElement1)
{
    uint32_t index = 1; // key value
    uint32_t index2 = 102400;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSTaggedValue> handleKey0(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> handleKey1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> handleKey(thread, JSTaggedValue(102400));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(2));
    PropertyAttributes handleAttr(index);

    // object is not DictionaryMode
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> handleObject1 =
        factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject1), handleKey0, handleKey0);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject1), handleKey1, handleKey1);
    TaggedArray *handleElements = TaggedArray::Cast(handleObject1->GetElements().GetTaggedObject());
    EXPECT_EQ(handleElements->Get(index).GetInt(), 1);

    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject1), handleKey, handleKey);
    ObjectOperator objectOperator1(thread, JSHandle<JSTaggedValue>(handleObject1), index2);
    objectOperator1.DeletePropertyInHolder();

    TaggedArray *resultElements = TaggedArray::Cast(handleObject1->GetElements().GetTaggedObject());
    EXPECT_EQ(resultElements->Get(index).GetInt(), 1);
    auto resultDict1 = NumberDictionary::Cast(handleObject1->GetElements().GetTaggedObject());
    EXPECT_TRUE(resultDict1->IsDictionaryMode());
    EXPECT_TRUE(JSObject::GetProperty(thread, handleObject1, handleKey).GetValue()->IsUndefined());
    // object is DictionaryMode
    JSHandle<JSObject> handleObject2 =
        factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject2), handleKey0, handleKey0);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject2), handleKey1, handleKey1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject2), handleKey, handleKey);
    
    JSObject::DeleteProperty(thread, (handleObject2), handleKey1);
    ObjectOperator objectOperator2(thread, JSHandle<JSTaggedValue>(handleObject2), index - 1);
    objectOperator2.DeletePropertyInHolder();
    auto resultDict2 = NumberDictionary::Cast(handleObject2->GetElements().GetTaggedObject());
    EXPECT_TRUE(resultDict2->GetKey(index - 1U).IsUndefined());
    EXPECT_TRUE(resultDict2->GetValue(index - 1U).IsUndefined());
    EXPECT_TRUE(JSObject::GetProperty(thread, handleObject2, handleKey0).GetValue()->IsUndefined());
    EXPECT_TRUE(JSObject::GetProperty(thread, handleObject2, handleKey1).GetValue()->IsUndefined());
}

HWTEST_F_L0(ObjectOperatorTest, Property_DeleteElement2)
{
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> globalObj = env->GetJSGlobalObject();
    JSHandle<JSObject> handleGlobalObject(globalObj);
    JSHandle<NumberDictionary> handleDict = NumberDictionary::Create(thread, 4);
    handleGlobalObject->SetElements(thread, handleDict.GetTaggedValue());
    handleGlobalObject->GetClass()->SetIsDictionaryElement(true);
    for (int i = 0; i < 10; i++) {
        JSHandle<JSTaggedValue> handleKey(thread, JSTaggedValue(i));
        JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(i));
        JSObject::SetProperty(thread, globalObj, handleKey, handleValue);
        JSObject::DeleteProperty(thread, handleGlobalObject, handleKey);
    }
    auto resultDict = NumberDictionary::Cast(handleGlobalObject->GetElements().GetTaggedObject());
    EXPECT_EQ(resultDict->Size(), 4);
}

HWTEST_F_L0(ObjectOperatorTest, Property_DeleteProperty)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSTaggedValue> handleKey(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleKey0(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> handleKey1(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(33));
    // object is JSGlobalObject
    JSHandle<JSTaggedValue> globalObj = env->GetJSGlobalObject();
    JSHandle<JSObject> handleGlobalObject(globalObj);
    JSMutableHandle<GlobalDictionary> globalDict(thread, handleGlobalObject->GetProperties());
    JSHandle<GlobalDictionary> handleDict = GlobalDictionary::Create(thread, 4);
    globalDict.Update(handleDict.GetTaggedValue());
    JSHandle<PropertyBox> cellHandle = factory->NewPropertyBox(handleKey);
    cellHandle->SetValue(thread, handleValue.GetTaggedValue());
    JSHandle<GlobalDictionary> handleProperties = GlobalDictionary::PutIfAbsent(
        thread, globalDict, handleKey, JSHandle<JSTaggedValue>(cellHandle), PropertyAttributes(12));
    handleGlobalObject->SetProperties(thread, handleProperties.GetTaggedValue());
    ObjectOperator objectOperator1(thread, handleGlobalObject, handleKey);

    objectOperator1.DeletePropertyInHolder();
    auto resultDict = GlobalDictionary::Cast(handleGlobalObject->GetProperties().GetTaggedObject());
    // key not found
    EXPECT_EQ(resultDict->FindEntry(handleKey.GetTaggedValue()), -1);
    EXPECT_EQ(resultDict->GetAttributes(objectOperator1.GetIndex()).GetValue(), 0U);
    // object is not DictionaryMode
    JSHandle<JSObject> handleObject =
        factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), handleKey, handleKey1);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), handleKey0, handleKey0);
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), handleKey1, handleKey1);
    ObjectOperator objectOperator2(thread, handleObject, handleKey);
    objectOperator2.DeletePropertyInHolder();
    auto resultDict1 = NameDictionary::Cast(handleObject->GetProperties().GetTaggedObject());
    // key not found
    EXPECT_EQ(resultDict1->FindEntry(handleKey.GetTaggedValue()), -1);
}

HWTEST_F_L0(ObjectOperatorTest, Define_SetterAndGettetr)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<AccessorData> handleAccessorData = factory->NewAccessorData();
    JSHandle<JSTaggedValue> handleValue(thread, JSTaggedValue(0));
    JSHandle<JSTaggedValue> handleValue1(thread, JSTaggedValue(2));
    JSHandle<EcmaString> handleKey(factory->NewFromASCII("value"));
    JSHandle<JSTaggedValue> handleKey1(factory->NewFromASCII("key"));
    JSHandle<JSTaggedValue> handleKey2(factory->NewFromASCII("value1"));
    // object is not DictionaryMode
    JSHandle<JSTaggedValue> objFunc(thread, JSObjectTestCreate(thread));
    JSHandle<JSObject> handleObject =
        factory->NewJSObjectByConstructor(JSHandle<JSFunction>(objFunc), objFunc);
    for (int i = 0; i < 10; i++) {
        JSHandle<JSTaggedValue> newValue(thread, JSTaggedValue(i));
        JSHandle<EcmaString> newString =
            factory->ConcatFromString(handleKey, JSTaggedValue::ToString(thread, newValue));
        JSHandle<JSTaggedValue> newKey(thread, newString.GetTaggedValue());
        JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handleObject), newKey, newValue);
    }
    // object is not Element
    ObjectOperator objectOperator(thread, handleObject, handleKey1);
    objectOperator.SetIndex(1);
    objectOperator.SetValue(handleAccessorData.GetTaggedValue());
    PropertyDescriptor handleDesc(thread, handleValue);
    handleDesc.SetSetter(handleValue);
    handleDesc.SetGetter(handleValue);
    objectOperator.SetAttr(PropertyAttributes(handleDesc));
    objectOperator.DefineSetter(handleValue1);
    objectOperator.DefineGetter(handleValue);

    JSHandle<JSObject> resultObj1(objectOperator.GetReceiver());
    TaggedArray *properties = TaggedArray::Cast(resultObj1->GetProperties().GetTaggedObject());
    JSHandle<AccessorData> resultAccessorData1(thread, properties->Get(objectOperator.GetIndex()));
    EXPECT_EQ(resultAccessorData1->GetGetter().GetInt(), 0);
    EXPECT_EQ(resultAccessorData1->GetSetter().GetInt(), 2);
    // object is DictionaryMode
    JSObject::DeleteProperty(thread, handleObject, handleKey2);
    objectOperator.DefineSetter(handleValue);
    objectOperator.DefineGetter(handleValue1);
    JSHandle<JSObject> resultObj2(objectOperator.GetReceiver());
    auto resultDict = NameDictionary::Cast(resultObj2->GetProperties().GetTaggedObject());
    JSHandle<AccessorData> resultAccessorData2(thread, resultDict->GetValue(objectOperator.GetIndex()));
    EXPECT_EQ(resultAccessorData2->GetGetter().GetInt(), 2);
    EXPECT_EQ(resultAccessorData2->GetSetter().GetInt(), 0);
}
} // namespace panda::test