/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include "ecmascript/js_typed_array.h"

#include "ecmascript/base/typed_array_helper-inl.h"
#include "ecmascript/ecma_string-inl.h"
#include "ecmascript/shared_objects/js_sendable_arraybuffer.h"

namespace panda::ecmascript {
using TypedArrayHelper = base::TypedArrayHelper;
using BuiltinsArrayBuffer = builtins::BuiltinsArrayBuffer;
using BuiltinsSendableArrayBuffer = builtins::BuiltinsSendableArrayBuffer;

JSHandle<JSTaggedValue> JSTypedArray::ToPropKey(JSThread *thread, const JSHandle<JSTaggedValue> &key)
{
    if (key->IsSymbol()) {
        return key;
    }
    return JSHandle<JSTaggedValue>(JSTaggedValue::ToString(thread, key));
}
// 9.4.5.1 [[GetOwnProperty]] ( P )
bool JSTypedArray::GetOwnProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                  const JSHandle<JSTaggedValue> &key, PropertyDescriptor &desc)
{
    // 1. Assert : IsPropertyKey(P) is true.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 2. Assert: O is an Object that has a [[ViewedArrayBuffer]] internal slot.
    // 3. If Type(P) is String, then
    //   a. Let numericIndex be CanonicalNumericIndexString(P).
    //   b. Assert: numericIndex is not an abrupt completion.
    //   c. If numericIndex is not undefined, then
    //     i. Let value be IntegerIndexedElementGet (O, numericIndex).
    //     ii. ReturnIfAbrupt(value).
    //     iii. If value is undefined, return undefined.
    //     iv. Return a PropertyDescriptor{ [[Value]]: value, [[Enumerable]]: true, [[Writable]]: true,
    //         [[Configurable]]: false }.
    if (key->IsString() || key->IsNumber()) {
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        if (!numericIndex.IsUndefined()) {
            JSHandle<JSTaggedValue> value =
                JSTypedArray::IntegerIndexedElementGet(thread, typedarray, numericIndex).GetValue();
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
            if (value->IsUndefined()) {
                return false;
            }
            desc.SetValue(value);
            desc.SetEnumerable(true);
            desc.SetWritable(true);
            desc.SetConfigurable(true);
            return true;
        }
    }
    // 4. Return OrdinaryGetOwnProperty(O, P).
    return JSObject::OrdinaryGetOwnProperty(thread, JSHandle<JSObject>(typedarray), key, desc);
}

// 9.4.5.2 [[HasProperty]] ( P )
bool JSTypedArray::HasProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                               const JSHandle<JSTaggedValue> &key)
{
    // 1. Assert: IsPropertyKey(P) is true.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 2. Assert: O is an Object that has a [[ViewedArrayBuffer]] internal slot.
    // 3. If Type(P) is String, then
    //   a. Let numericIndex be CanonicalNumericIndexString(P).
    //   b. Assert: numericIndex is not an abrupt completion.
    //   c. If numericIndex is not undefined, then
    //     i. Let buffer be the value of O’s [[ViewedArrayBuffer]] internal slot.
    //     ii. If IsDetachedBuffer(buffer) is true, throw a TypeError exception.
    //     iii. If IsInteger(numericIndex) is false, return false
    //     iv. If numericIndex = −0, return false.
    //     v. If numericIndex < 0, return false.
    //     vi. If numericIndex ≥ the value of O’s [[ArrayLength]] internal slot, return false.
    //     vii. Return true.
    JSHandle<JSTypedArray> typedarrayObj(typedarray);
    if (key->IsString() || key->IsNumber()) {
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        if (!numericIndex.IsUndefined()) {
            JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
            if (IsArrayBufferDetached(thread, buffer)) {
                THROW_TYPE_ERROR_AND_RETURN(thread, "Is Detached Buffer", false);
            }
            if (!numericIndex.IsInteger()) {
                return false;
            }
            JSHandle<JSTaggedValue> numericIndexHandle(thread, numericIndex);
            JSTaggedNumber numericIndexNumber = JSTaggedValue::ToNumber(thread, numericIndexHandle);
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
            double tNegZero = -0.0;
            auto eZero = JSTaggedNumber(tNegZero);
            JSHandle<JSTaggedValue> zero(thread, JSTaggedValue(0));
            if (JSTaggedNumber::SameValue(numericIndexNumber, eZero)) {
                return false;
            }

            if (JSTaggedValue::Less(thread, numericIndexHandle, zero)) {
                return false;
            }
            uint32_t arrLen = typedarrayObj->GetArrayLength();
            JSHandle<JSTaggedValue> arrLenHandle(thread, JSTaggedValue(arrLen));
            return JSTaggedValue::Less(thread, numericIndexHandle, arrLenHandle);
        }
    }
    // 4. Return OrdinaryHasProperty(O, P).
    PropertyDescriptor desc(thread);
    if (JSObject::OrdinaryGetOwnProperty(thread, JSHandle<JSObject>::Cast(typedarrayObj), key, desc)) {
        return true;
    }
    JSTaggedValue parent = JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>::Cast(typedarrayObj));
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    if (!parent.IsNull()) {
        return JSTaggedValue::HasProperty(thread, JSHandle<JSTaggedValue>(thread, parent), key);
    }
    return false;
}

// 9.4.5.3 [[DefineOwnProperty]] ( P, Desc )
bool JSTypedArray::DefineOwnProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                     const JSHandle<JSTaggedValue> &key, const PropertyDescriptor &desc)
{
    // 1. Assert: IsPropertyKey(P) is true.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 2. Assert: O is an Object that has a [[ViewedArrayBuffer]] internal slot.
    // 3. If Type(P) is String, then
    //   a. Let numericIndex be CanonicalNumericIndexString (P).
    //   b. Assert: numericIndex is not an abrupt completion.
    //   c. If numericIndex is not undefined, then
    JSHandle<JSTypedArray> typedarrayObj(typedarray);
    if (key->IsString() || key->IsNumber()) {
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        if (!numericIndex.IsUndefined()) {
            // i. If IsInteger(numericIndex) is false, return false
            // ii. Let intIndex be numericIndex.
            // iii. If intIndex = −0, return false.
            // iv. If intIndex < 0, return false.
            // v. Let length be the value of O’s [[ArrayLength]] internal slot.
            // vi. If intIndex ≥ length, return false.
            // vii. If IsAccessorDescriptor(Desc) is true, return false.
            // viii. If Desc has a [[Configurable]] field and if Desc.[[Configurable]] is true, return false.
            // ix. If Desc has an [[Enumerable]] field and if Desc.[[Enumerable]] is false, return false.
            // x. If Desc has a [[Writable]] field and if Desc.[[Writable]] is false, return false.
            // xi. If Desc has a [[Value]] field, then
            //   1. Let value be Desc.[[Value]].
            //   2. Return IntegerIndexedElementSet (O, intIndex, value).
            // xii. Return true.
            if (!numericIndex.IsInteger()) {
                return false;
            }
            JSHandle<JSTaggedValue> numericIndexHandle(thread, numericIndex);
            JSTaggedNumber numericIndexNumber = JSTaggedValue::ToNumber(thread, numericIndexHandle);
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
            double tNegZero = -0.0;
            auto eZero = JSTaggedNumber(tNegZero);
            JSHandle<JSTaggedValue> zero(thread, JSTaggedValue(0));
            if (JSTaggedNumber::SameValue(numericIndexNumber, eZero)) {
                return false;
            }
            if (JSTaggedValue::Less(thread, numericIndexHandle, zero)) {
                return false;
            }
            uint32_t arrLen = typedarrayObj->GetArrayLength();
            JSHandle<JSTaggedValue> arrLenHandle(thread, JSTaggedValue(arrLen));
            if (!JSTaggedValue::Less(thread, numericIndexHandle, arrLenHandle)) {
                return false;
            }
            if (desc.IsAccessorDescriptor()) {
                return false;
            }
            if (desc.HasConfigurable() && !desc.IsConfigurable()) {
                return false;
            }
            if (desc.HasEnumerable() && !desc.IsEnumerable()) {
                return false;
            }
            if (desc.HasWritable() && !desc.IsWritable()) {
                return false;
            }
            if (desc.HasValue()) {
                JSHandle<JSTaggedValue> value = desc.GetValue();
                return (JSTypedArray::IntegerIndexedElementSet(thread, typedarray, numericIndex, value));
            }
            return true;
        }
    }
    // 4. Return OrdinaryDefineOwnProperty(O, P, Desc).
    return JSObject::OrdinaryDefineOwnProperty(thread, JSHandle<JSObject>::Cast(typedarrayObj), key, desc);
}

// 9.4.5.4 [[Get]] ( P, Receiver )
OperationResult JSTypedArray::GetProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                          const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &receiver)
{
    // 1. Assert : IsPropertyKey(P) is true.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 2. If Type(P) is String and if SameValue(O, Receiver) is true, then
    if ((key->IsString() || key->IsNumber()) && JSTaggedValue::SameValue(thread, typedarray, receiver)) {
        //   a. Let numericIndex be CanonicalNumericIndexString (P).
        //   b. Assert: numericIndex is not an abrupt completion.
        //   c. If numericIndex is not undefined, then
        //     i. Return IntegerIndexedElementGet (O, numericIndex).
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(
            thread, OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
        if (!numericIndex.IsUndefined()) {
            return JSTypedArray::IntegerIndexedElementGet(thread, typedarray, numericIndex);
        }
    }

    // 3. Return the result of calling the default ordinary object [[Get]] internal method (9.1.8) on O
    //   passing P and Receiver as arguments.
    return JSObject::GetProperty(thread, typedarray, key, receiver);
}

// 9.4.5.5 [[Set]] ( P, V, Receiver )
bool JSTypedArray::SetProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                               const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value,
                               const JSHandle<JSTaggedValue> &receiver, bool mayThrow)
{
    // 1. Assert : IsPropertyKey(P) is true.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 2. If Type(P) is String and if SameValue(O, Receiver) is true, then
    if ((key->IsString() || key->IsNumber()) && JSTaggedValue::SameValue(thread, typedarray, receiver)) {
        //   a. Let numericIndex be CanonicalNumericIndexString (P).
        //   b. Assert: numericIndex is not an abrupt completion.
        //   c. If numericIndex is not undefined, then
        //     i. Return IntegerIndexedElementSet (O, numericIndex, V).
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        if (!numericIndex.IsUndefined()) {
            return JSTypedArray::IntegerIndexedElementSet(thread, typedarray, numericIndex, value);
        }
    }
    // 3. Return the result of calling the default ordinary object [[Set]] internal method (9.1.8) on O passing
    // P, V, and Receiver as arguments.
    return JSObject::SetProperty(thread, typedarray, key, value, receiver, mayThrow);
}

// s12 10.4.5.6 [[Delete]] ( P )
bool JSTypedArray::DeleteProperty(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                  const JSHandle<JSTaggedValue> &key)
{
    // 1. Assert: IsPropertyKey(P) is true.
    // 2. Assert: O is an Integer-Indexed exotic object.
    ASSERT(JSTaggedValue::IsPropertyKey(key));
    // 3. If Type(P) is String, then
        // a. Let numericIndex be CanonicalNumericIndexString(P).
        // b. If numericIndex is not undefined, then
           // i. If IsValidIntegerIndex(O, numericIndex) is false, return true; else return false.
    if (key->IsString() || key->IsNumber()) {
        JSTaggedValue numericIndex = JSTaggedValue::CanonicalNumericIndexString(thread, key);
        if (!numericIndex.IsUndefined()) {
            if (!IsValidIntegerIndex(thread, typedarray, numericIndex)) {
                return true;
            }
            return false;
        }
    }
    // 4. Return ? OrdinaryDelete(O, P).
    return JSObject::DeleteProperty(thread, JSHandle<JSObject>(typedarray), key);
}

// 9.4.5.6 [[OwnPropertyKeys]] ( )
JSHandle<TaggedArray> JSTypedArray::OwnPropertyKeys(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 1. Let keys be a new empty List.
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    // 3. Let len be the value of O’s [[ArrayLength]] internal slot.
    JSHandle<JSTypedArray> arrayObj(typedarray);
    JSHandle<TaggedArray> objKeys = JSObject::GetOwnPropertyKeys(thread, JSHandle<JSObject>::Cast(arrayObj));
    uint32_t objKeysLen = objKeys->GetLength();
    uint32_t bufferKeysLen = arrayObj->GetArrayLength();
    uint32_t length = objKeysLen + bufferKeysLen;
    JSHandle<TaggedArray> nameList = factory->NewTaggedArray(length);

    // 4. For each integer i starting with 0 such that i < len, in ascending order,
    //   a. Add ToString(i) as the last element of keys.
    uint32_t copyLength = 0;
    JSMutableHandle<JSTaggedValue> tKey(thread, JSTaggedValue::Undefined());
    for (uint32_t k = 0; k < bufferKeysLen; k++) {
        tKey.Update(JSTaggedValue(k));
        JSHandle<JSTaggedValue> sKey(JSTaggedValue::ToString(thread, tKey));
        RETURN_HANDLE_IF_ABRUPT_COMPLETION(TaggedArray, thread);
        nameList->Set(thread, copyLength, sKey.GetTaggedValue());
        copyLength++;
    }

    // 5. For each own property key P of O such that Type(P) is String and P is not an integer index, in
    // property creation order
    //   a. Add P as the last element of keys.
    for (uint32_t i = 0; i < objKeysLen; i++) {
        JSTaggedValue key = objKeys->Get(thread, i);
        if (JSTaggedValue(key).IsString()) {
            nameList->Set(thread, copyLength, key);
            copyLength++;
        }
    }

    // 6. For each own property key P of O such that Type(P) is Symbol, in property creation order
    //   a. Add P as the last element of keys.
    for (uint32_t i = 0; i < objKeysLen; i++) {
        JSTaggedValue key = objKeys->Get(thread, i);
        if (JSTaggedValue(key).IsSymbol()) {
            nameList->Set(thread, copyLength, key);
            copyLength++;
        }
    }

    // 7. Return keys.
    return factory->CopyArray(nameList, length, copyLength);
}

JSHandle<TaggedArray> JSTypedArray::OwnEnumPropertyKeys(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 1. Let keys be a new empty List.
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    // 3. Let len be the value of O’s [[ArrayLength]] internal slot.
    JSHandle<JSTypedArray> arrayObj(typedarray);
    JSHandle<TaggedArray> objKeys = JSObject::GetOwnEnumPropertyKeys(thread, JSHandle<JSObject>::Cast(arrayObj));
    uint32_t objKeysLen = objKeys->GetLength();
    uint32_t bufferKeysLen = arrayObj->GetArrayLength();
    uint32_t length = objKeysLen + bufferKeysLen;
    JSHandle<TaggedArray> nameList = factory->NewTaggedArray(length);

    // 4. For each integer i starting with 0 such that i < len, in ascending order,
    //   a. Add ToString(i) as the last element of keys.
    uint32_t copyLength = 0;
    for (uint32_t k = 0; k < bufferKeysLen; k++) {
        auto key = base::NumberHelper::IntToEcmaString(thread, k);
        nameList->Set(thread, copyLength, key);
        copyLength++;
    }

    // 5. For each own property key P of O such that Type(P) is String and P is not an integer index, in
    // property creation order
    //   a. Add P as the last element of keys.
    for (uint32_t i = 0; i < objKeysLen; i++) {
        JSTaggedValue key = objKeys->Get(thread, i);
        nameList->Set(thread, copyLength, key);
        copyLength++;
    }

    // 7. Return keys.
    return factory->CopyArray(nameList, length, copyLength);
}

// 9.4.5.7 IntegerIndexedObjectCreate (prototype, internalSlotsList)

// 9.4.5.8 IntegerIndexedElementGet ( O, index )
OperationResult JSTypedArray::IntegerIndexedElementGet(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                                       JSTaggedValue index)
{
    // 1. Assert: Type(index) is Number.
    ASSERT(index.IsNumber());
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedarray->IsTypedArray() || typedarray->IsSharedTypedArray());
    // 3. Let buffer be the value of O’s [[ViewedArrayBuffer]] internal slot.
    JSHandle<JSTypedArray> typedarrayObj(typedarray);
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
    // 4. If IsDetachedBuffer(buffer) is true, throw a TypeError exception.
    if (!buffer.IsSendableArrayBuffer() && BuiltinsArrayBuffer::IsDetachedBuffer(thread, buffer)) {
        if (index == JSTaggedValue(0)) {
            return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
        }
        THROW_TYPE_ERROR_AND_RETURN(thread, "Is Detached Buffer",
                                    OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    }
    if (buffer.IsSendableArrayBuffer() && BuiltinsSendableArrayBuffer::IsDetachedBuffer(thread, buffer)) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Is Detached Buffer",
                                    OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    }
    // 5. If IsInteger(index) is false, return undefined
    if (!index.IsInteger()) {
        return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
    }

    // 6. If index = −0, return undefined.
    // 7. Let length be the value of O’s [[ArrayLength]] internal slot.
    // 8. If index < 0 or index ≥ length, return undefined.
    JSHandle<JSTaggedValue> indexHandle(thread, index);
    JSTaggedNumber indexNumber = JSTaggedValue::ToNumber(thread, indexHandle);
    RETURN_VALUE_IF_ABRUPT_COMPLETION(
        thread, OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    double tNegZero = -0.0;
    auto eZero = JSTaggedNumber(tNegZero);
    JSHandle<JSTaggedValue> zero(thread, JSTaggedValue(0));
    if (JSTaggedNumber::SameValue(indexNumber, eZero)) {
        return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
    }
    uint32_t arrLen = typedarrayObj->GetArrayLength();
    JSHandle<JSTaggedValue> arrLenHandle(thread, JSTaggedValue(arrLen));
    if (JSTaggedValue::Less(thread, indexHandle, zero) || !JSTaggedValue::Less(thread, indexHandle, arrLenHandle)) {
        return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
    }
    // 9. Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedarrayObj->GetByteOffset();
    // 10. Let arrayTypeName be the String value of O’s [[TypedArrayName]] internal slot.
    // 11. Let elementSize be the Number value of the Element Size value specified in Table 49 for
    // arrayTypeName.
    uint32_t elementSize = TypedArrayHelper::GetElementSize(typedarrayObj);
    // 12. Let indexedPosition = (index × elementSize) + offset.
    JSTaggedNumber integerValue = JSTaggedValue::ToInteger(thread, indexHandle);
    RETURN_VALUE_IF_ABRUPT_COMPLETION(
        thread, OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    uint32_t k = static_cast<uint32_t>(integerValue.ToInt32());
    uint32_t byteIndex = k * elementSize + offset;
    // 13. Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(typedarrayObj);
    // 14. Return GetValueFromBuffer(buffer, indexedPosition, elementType).
    JSTaggedValue result = BuiltinsArrayBuffer::GetValueFromBuffer(thread, buffer, byteIndex, elementType, true);
    return OperationResult(thread, result, PropertyMetaData(true));
}

// s12 10.4.5.9 IsValidIntegerIndex ( O, index )
bool JSTypedArray::IsValidIntegerIndex(JSThread *thread, const JSHandle<JSTaggedValue> &typedArray, JSTaggedValue index)
{
    // 1. Assert: O is an Integer-Indexed exotic object.
    ASSERT(typedArray->IsTypedArray() || typedArray->IsSharedTypedArray());
    // 2. If IsDetachedBuffer(O.[[ViewedArrayBuffer]]) is true, return false.
    JSHandle<JSTypedArray> typedarrayObj(typedArray);
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
    if (IsArrayBufferDetached(thread, buffer)) {
        return false;
    }
    // 3. If ! IsIntegralNumber(index) is false, return false.
    if (!index.IsInteger()) {
        return false;
    }
    // 4. If index is -0𝔽, return false.
    double val = index.GetNumber();
    if (val == 0 && std::signbit(val)) {
        return false;
    }

    uint32_t arrLen = typedarrayObj->GetArrayLength();
    // 5. If ℝ(index) < 0 or ℝ(index) ≥ O.[[ArrayLength]], return false.
    if (val < 0 || val >= arrLen) {
        return false;
    }
    // 6. Return true.
    return true;
}

DataViewType JSTypedArray::GetTypeFromName(JSThread *thread, const JSHandle<JSTaggedValue> &typeName)
{
    auto type = thread->GetEcmaVM()->GetDataViewType(typeName.GetTaggedType());
    if (type != -1) {
        return DataViewType(type);
    }
    const GlobalEnvConstants *globalConst = thread->GlobalConstants();
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledInt8ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedInt8ArrayString())) {
        return DataViewType::INT8;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledUint8ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedUint8ArrayString())) {
        return DataViewType::UINT8;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledUint8ClampedArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedUint8ClampedArrayString())) {
        return DataViewType::UINT8_CLAMPED;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledInt16ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedInt16ArrayString())) {
        return DataViewType::INT16;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledUint16ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedUint16ArrayString())) {
        return DataViewType::UINT16;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledInt32ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedInt32ArrayString())) {
        return DataViewType::INT32;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledUint32ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedUint32ArrayString())) {
        return DataViewType::UINT32;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledFloat32ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedFloat32ArrayString())) {
        return DataViewType::FLOAT32;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledFloat64ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedFloat64ArrayString())) {
        return DataViewType::FLOAT64;
    }
    if (JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledBigInt64ArrayString()) ||
        JSTaggedValue::SameValue(thread, typeName, globalConst->GetHandledSharedBigInt64ArrayString())) {
        return DataViewType::BIGINT64;
    }
    return DataViewType::BIGUINT64;
}

// static
bool JSTypedArray::FastCopyElementToArray(JSThread *thread, const JSHandle<JSTaggedValue> &typedArray,
                                          JSHandle<TaggedArray> &array)
{
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedArray->IsTypedArray() || typedArray->IsSharedTypedArray());
    // 3. Let buffer be the value of O’s [[ViewedArrayBuffer]] internal slot.
    JSHandle<JSTypedArray> typedarrayObj(typedArray);
    // 4. If IsDetachedBuffer(buffer) is true, throw a TypeError exception.
    JSHandle<JSTaggedValue> bufferHandle =
        JSHandle<JSTaggedValue>(thread, typedarrayObj->GetViewedArrayBufferOrByteArray(thread));
    if (IsArrayBufferDetached(thread, bufferHandle.GetTaggedValue())) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Is Detached Buffer", false);
    }

    // 7. Let length be the value of O’s [[ArrayLength]] internal slot.
    // 8. If index < 0 or index ≥ length, return undefined.
    uint32_t arrLen = typedarrayObj->GetArrayLength();

    // 9. Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedarrayObj->GetByteOffset();
    // 11. Let elementSize be the Number value of the Element Size value specified in Table 49 for arrayTypeName.
    uint32_t elementSize = TypedArrayHelper::GetElementSize(typedarrayObj);
    // 13. Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(typedarrayObj);
    for (uint32_t index = 0; index < arrLen; index++) {
        // 12. Let indexedPosition = (index × elementSize) + offset.
        uint32_t byteIndex = index * elementSize + offset;
        // 14. Return GetValueFromBuffer(buffer, indexedPosition, elementType).
        JSTaggedValue result = BuiltinsArrayBuffer::GetValueFromBuffer(thread, bufferHandle.GetTaggedValue(),
                                                                       byteIndex, elementType, true);
        array->Set(thread, index, result);
    }
    return true;
}

// static
OperationResult JSTypedArray::FastElementGet(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                             uint32_t index)
{
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedarray->IsTypedArray() || typedarray->IsSharedTypedArray());
    // 3. Let buffer be the value of O’s [[ViewedArrayBuffer]] internal slot.
    JSHandle<JSTypedArray> typedarrayObj(typedarray);
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
    // 10.4.5.15 TypedArrayGetElement ( O, index )
    //  1. If IsValidIntegerIndex(O, index) is false, return undefined.
    if (IsArrayBufferDetached(thread, buffer)) {
        return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
    }

    // 7. Let length be the value of O’s [[ArrayLength]] internal slot.
    // 8. If index < 0 or index ≥ length, return undefined.
    uint32_t arrLen = typedarrayObj->GetArrayLength();
    if (index >= arrLen) {
        return OperationResult(thread, JSTaggedValue::Undefined(), PropertyMetaData(true));
    }
    // 9. Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedarrayObj->GetByteOffset();
    // 11. Let elementSize be the Number value of the Element Size value specified in Table 49 for arrayTypeName.
    uint32_t elementSize = TypedArrayHelper::GetElementSize(typedarrayObj);
    // 12. Let indexedPosition = (index × elementSize) + offset.
    uint32_t byteIndex = index * elementSize + offset;
    // 13. Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(typedarrayObj);
    // 14. Return GetValueFromBuffer(buffer, indexedPosition, elementType).
    JSTaggedValue result = BuiltinsArrayBuffer::GetValueFromBuffer(thread, buffer, byteIndex, elementType, true);
    return OperationResult(thread, result, PropertyMetaData(true));
}

// 9.4.5.9 IntegerIndexedElementSet ( O, index, value )
bool JSTypedArray::IntegerIndexedElementSet(JSThread *thread, const JSHandle<JSTaggedValue> &typedarray,
                                            JSTaggedValue index, const JSHandle<JSTaggedValue> &value)
{
    // 1. Assert: Type(index) is Number.
    ASSERT(index.IsNumber());
    // 2. Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedarray->IsTypedArray() || typedarray->IsSharedTypedArray());
    // 3. If O.[[ContentType]] is BigInt, let numValue be ? ToBigInt(value).
    JSHandle<JSTaggedValue> numValueHandle;
    ContentType contentType = JSHandle<JSTypedArray>::Cast(typedarray)->GetContentType();
    if (UNLIKELY(contentType == ContentType::BigInt)) {
        numValueHandle = JSHandle<JSTaggedValue>(thread, JSTaggedValue::ToBigInt(thread, value));
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    } else {
        numValueHandle = JSHandle<JSTaggedValue>(thread, JSTaggedValue::ToNumber(thread, value));
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    }

    JSHandle<JSTypedArray> typedarrayObj(typedarray);
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
    JSHandle<JSTaggedValue> indexHandle(thread, index);
    // 5. If ! IsValidIntegerIndex(O, index) is true, then
    if (IsValidIntegerIndex(thread, typedarray, index)) {
        // 6. Let offset be the value of O’s [[ByteOffset]] internal slot.
        uint32_t offset = typedarrayObj->GetByteOffset();
        // 7. Let arrayTypeName be the String value of O’s [[TypedArrayName]] internal slot.
        // 8. Let elementSize be the Number value of the Element Size value specified in Table 49 for
        // arrayTypeName.
        uint32_t elementSize = TypedArrayHelper::GetElementSize(typedarrayObj);
        // 9. Let indexedPosition = (index × elementSize) + offset.
        JSTaggedNumber integerValue = JSTaggedValue::ToInteger(thread, indexHandle);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        uint32_t k = integerValue.ToUint32();
        uint32_t byteIndex = k * elementSize + offset;
        // 10. Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
        DataViewType elementType = TypedArrayHelper::GetType(typedarrayObj);
        // 11. Perform SetValueInBuffer(buffer, indexedPosition, elementType, numValue).
        BuiltinsArrayBuffer::SetValueInBuffer(thread, buffer, byteIndex, elementType, numValueHandle, true);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    }
    return true;
}

// only use in TypeArray fast set property
JSTaggedNumber JSTypedArray::NonEcmaObjectToNumber(JSThread *thread, const JSTaggedValue tagged)
{
    ASSERT_PRINT(!tagged.IsECMAObject(), "tagged must not be EcmaObject");
    if (tagged.IsInt() || tagged.IsDouble()) {
        return JSTaggedNumber(tagged);
    }
    if (tagged.IsString()) {
        return JSTaggedValue::StringToDouble(thread, tagged);
    }
    switch (tagged.GetRawData()) {
        case JSTaggedValue::VALUE_UNDEFINED:
        case JSTaggedValue::VALUE_HOLE: {
            return JSTaggedNumber(base::NAN_VALUE);
        }
        case JSTaggedValue::VALUE_TRUE: {
            return JSTaggedNumber(1);
        }
        case JSTaggedValue::VALUE_FALSE:
        case JSTaggedValue::VALUE_NULL: {
            return JSTaggedNumber(0);
        }
        default: {
            break;
        }
    }
    if (tagged.IsSymbol()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Cannot convert a Symbol value to a number", JSTaggedNumber::Exception());
    }
    if (tagged.IsBigInt()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "Cannot convert a BigInt value to a number", JSTaggedNumber::Exception());
    }
    THROW_TYPE_ERROR_AND_RETURN(thread, "Cannot convert a Unknown value to a number", JSTaggedNumber::Exception());
}

JSTaggedValue JSTypedArray::FastGetPropertyByIndex(JSThread *thread, const JSTaggedValue typedarray, uint32_t index,
                                                   JSType jsType)
{
    // Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedarray.IsTypedArray() || typedarray.IsSharedTypedArray());
    // Let buffer be the value of O’s [[ViewedArrayBuffer]] internal slot.
    JSTypedArray *typedarrayObj = JSTypedArray::Cast(typedarray.GetTaggedObject());
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);
    if (IsArrayBufferDetached(thread, buffer)) {
        return JSTaggedValue::Undefined();
    }

    // Let length be the value of O’s [[ArrayLength]] internal slot.
    // If arrLen < 0 or index ≥ length, return undefined.
    uint32_t arrLen = typedarrayObj->GetArrayLength();
    if (index >= arrLen) {
        return JSTaggedValue::Undefined();
    }
    // Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedarrayObj->GetByteOffset();
    // Let elementSize be the Number value of the Element Size value specified in Table 49 for arrayTypeName.
    uint32_t elementSize = TypedArrayHelper::GetElementSize(jsType);
    // Let indexedPosition = (index × elementSize) + offset.
    uint32_t byteIndex = index * elementSize + offset;
    // Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(jsType);
    // Return GetValueFromBuffer(buffer, indexedPosition, elementType).
    return BuiltinsArrayBuffer::GetValueFromBuffer(thread, buffer, byteIndex, elementType, true);
}

JSTaggedValue JSTypedArray::FastSetPropertyByIndex(JSThread *thread, const JSTaggedValue typedarray, uint32_t index,
                                                   JSTaggedValue value, JSType jsType)
{
    // Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedarray.IsTypedArray() || typedarray.IsSharedTypedArray());
    // If O.[[ContentType]] is BigInt, let numValue be ? ToBigInt(value).
    JSTypedArray *typedarrayObj = JSTypedArray::Cast(typedarray.GetTaggedObject());
    if (UNLIKELY(typedarrayObj->GetContentType() == ContentType::BigInt || value.IsECMAObject())) {
        return JSTaggedValue::Hole();
    }
    JSTaggedNumber numValue = JSTypedArray::NonEcmaObjectToNumber(thread, value);
    // ReturnIfAbrupt(numValue).
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());

    DISALLOW_GARBAGE_COLLECTION;
    JSTaggedValue buffer = typedarrayObj->GetViewedArrayBufferOrByteArray(thread);

    // If ℝ(index) < 0 or ℝ(index) ≥ O.[[ArrayLength]], return false.
    uint32_t arrLen = typedarrayObj->GetArrayLength();
    if (index >= arrLen) {
        return JSTaggedValue::Undefined();
    }
    // Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedarrayObj->GetByteOffset();
    // Let arrayTypeName be the String value of O’s [[TypedArrayName]]
    // Let elementSize be the Number value of the Element Size value specified in Table 49 for
    // arrayTypeName.
    uint32_t elementSize = TypedArrayHelper::GetElementSize(jsType);
    // Let indexedPosition = (index × elementSize) + offset.
    uint32_t byteIndex = index * elementSize + offset;
    // Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(jsType);
    // Perform SetValueInBuffer(buffer, indexedPosition, elementType, numValue).
    return BuiltinsArrayBuffer::FastSetValueInBuffer(thread,
        buffer, byteIndex, elementType, numValue.GetNumber(), true);
}

JSTaggedValue JSTypedArray::GetOffHeapBuffer(JSThread *thread, JSHandle<JSTypedArray> &typedArray)
{
    JSTaggedValue arrBuf = typedArray->GetViewedArrayBufferOrByteArray(thread);
    if (arrBuf.IsArrayBuffer() || arrBuf.IsSharedArrayBuffer()) {
        return arrBuf;
    }

    ByteArray *byteArray = ByteArray::Cast(arrBuf.GetTaggedObject());
    int32_t length = static_cast<int32_t>(byteArray->GetArrayLength() * byteArray->GetByteLength());
    JSHandle<JSArrayBuffer> arrayBuffer = thread->GetEcmaVM()->GetFactory()->NewJSArrayBuffer(length);

    if (length > 0) {
        void *fromBuf = reinterpret_cast<void *>(ToUintPtr(
            ByteArray::Cast(typedArray->GetViewedArrayBufferOrByteArray(thread).GetTaggedObject())->GetData()));
        JSTaggedValue data = arrayBuffer->GetArrayBufferData(thread);
        void *toBuf = reinterpret_cast<void *>(
            ToUintPtr(JSNativePointer::Cast(data.GetTaggedObject())->GetExternalPointer()));
        JSArrayBuffer::CopyDataPointBytes(toBuf, fromBuf, 0, length);
    }
    typedArray->SetViewedArrayBufferOrByteArray(thread, arrayBuffer.GetTaggedValue());
    JSHandle<JSTaggedValue> typeName(thread, typedArray->GetTypedArrayName(thread));
    DataViewType arrayType = JSTypedArray::GetTypeFromName(thread, typeName);
    JSHandle<JSHClass> notOnHeapHclass = TypedArrayHelper::GetNotOnHeapHclassFromType(
        thread, typedArray, arrayType);
#if ECMASCRIPT_ENABLE_IC
    JSHClass::NotifyHclassChanged(thread, JSHandle<JSHClass>(thread, typedArray->GetJSHClass()), notOnHeapHclass);
#endif
    TaggedObject::Cast(*typedArray)->SynchronizedTransitionClass(thread, *notOnHeapHclass); // onHeap->notOnHeap

    return arrayBuffer.GetTaggedValue();
}

JSTaggedValue JSSharedTypedArray::GetSharedOffHeapBuffer(JSThread *thread, JSHandle<JSSharedTypedArray> typedArray)
{
    JSTaggedValue arrBuf = typedArray->GetViewedArrayBufferOrByteArray(thread);
    if (arrBuf.IsSendableArrayBuffer()) {
        return arrBuf;
    }

    ByteArray *byteArray = ByteArray::Cast(arrBuf.GetTaggedObject());
    int32_t length = static_cast<int32_t>(byteArray->GetArrayLength() * byteArray->GetByteLength());
    JSHandle<JSSendableArrayBuffer> arrayBuffer = thread->GetEcmaVM()->GetFactory()->NewJSSendableArrayBuffer(length);

    if (length > 0) {
        void *fromBuf = reinterpret_cast<void *>(ToUintPtr(
            ByteArray::Cast(typedArray->GetViewedArrayBufferOrByteArray(thread).GetTaggedObject())->GetData()));
        JSTaggedValue data = arrayBuffer->GetArrayBufferData(thread);
        void *toBuf = reinterpret_cast<void *>(
            ToUintPtr(JSNativePointer::Cast(data.GetTaggedObject())->GetExternalPointer()));
        JSSendableArrayBuffer::CopyDataPointBytes(toBuf, fromBuf, 0, length);
    }
    typedArray->SetViewedArrayBufferOrByteArray(thread, arrayBuffer.GetTaggedValue());
    JSHandle<JSTaggedValue> typeName(thread, typedArray->GetTypedArrayName(thread));
    DataViewType arrayType = JSTypedArray::GetTypeFromName(thread, typeName);
    JSHandle<JSHClass> notOnHeapHclass = TypedArrayHelper::GetSharedNotOnHeapHclassFromType(
        thread, typedArray, arrayType);
#if ECMASCRIPT_ENABLE_IC
    JSHClass::NotifyHclassChanged(thread, JSHandle<JSHClass>(thread, typedArray->GetJSHClass()), notOnHeapHclass);
#endif
    TaggedObject::Cast(*typedArray)->SynchronizedTransitionClass(thread, *notOnHeapHclass); // onHeap->notOnHeap

    return arrayBuffer.GetTaggedValue();
}

bool JSTypedArray::FastTypedArrayFill(JSThread *thread, const JSHandle<JSTaggedValue> &typedArray,
                                      const JSHandle<JSTaggedValue> &value, uint32_t start, uint32_t end)
{
    // Assert: O is an Object that has [[ViewedArrayBuffer]], [[ArrayLength]], [[ByteOffset]], and
    // [[TypedArrayName]] internal slots.
    ASSERT(typedArray->IsTypedArray() || typedArray->IsSharedTypedArray());
    // If O.[[ContentType]] is BigInt, let numValue be ? ToBigInt(value).
    JSHandle<JSTypedArray> typedArrayObj = JSHandle<JSTypedArray>::Cast(typedArray);
    if (UNLIKELY(typedArrayObj->GetContentType() == ContentType::BigInt || value->IsECMAObject())) {
        return false;
    }
    JSTaggedNumber numValue = JSTypedArray::NonEcmaObjectToNumber(thread, value.GetTaggedValue());
    // ReturnIfAbrupt(numValue).
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, true);
    JSTaggedValue buffer = typedArrayObj->GetViewedArrayBufferOrByteArray(thread);
    // If ℝ(index) < 0 or ℝ(index) ≥ O.[[ArrayLength]], return false.
    uint32_t arrLen = typedArrayObj->GetArrayLength();
    // Let offset be the value of O’s [[ByteOffset]] internal slot.
    uint32_t offset = typedArrayObj->GetByteOffset();
    // Let arrayTypeName be the String value of O’s [[TypedArrayName]]
    // Let elementSize be the Number value of the Element Size value specified in Table 49 for arrayTypeName.
    JSType jsType = typedArrayObj->GetClass()->GetObjectType();
    uint32_t elementSize = TypedArrayHelper::GetElementSize(jsType);
    // Let elementType be the String value of the Element Type value in Table 49 for arrayTypeName.
    DataViewType elementType = TypedArrayHelper::GetType(jsType);
    uint64_t byteBeginOffset = start * elementSize + offset;
    uint64_t byteEndOffset = std::min(end, arrLen) * elementSize + offset;
    if (byteBeginOffset < byteEndOffset) {
        BuiltinsArrayBuffer::TryFastSetValueInBuffer(thread, buffer,
            byteBeginOffset, byteEndOffset, elementType, numValue.GetNumber(), true);
    }
    return true;
}

inline bool JSTypedArray::IsArrayBufferDetached(JSThread *thread, JSTaggedValue buffer)
{
    if (!buffer.IsSendableArrayBuffer() && BuiltinsArrayBuffer::IsDetachedBuffer(thread, buffer)) {
        return true;
    }
    if (buffer.IsSendableArrayBuffer() && BuiltinsSendableArrayBuffer::IsDetachedBuffer(thread, buffer)) {
        return true;
    }
    return false;
}
}  // namespace panda::ecmascript
