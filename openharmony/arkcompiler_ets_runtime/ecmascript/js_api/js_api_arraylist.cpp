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

#include "ecmascript/js_api/js_api_arraylist.h"

#include "ecmascript/base/array_helper.h"
#include "ecmascript/containers/containers_errors.h"
#include "ecmascript/interpreter/interpreter.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_function.h"

namespace panda::ecmascript {
using ContainerError = containers::ContainerError;
using ErrorFlag = containers::ErrorFlag;
bool JSAPIArrayList::Add(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                         const JSHandle<JSTaggedValue> &value)
{
    uint32_t length = arrayList->GetLength().GetArrayLength();
    JSHandle<TaggedArray> elements = GrowCapacity(thread, arrayList, length + 1);

    ASSERT(!elements->IsDictionaryMode());
    elements->Set(thread, length, value);
    arrayList->SetLength(thread, JSTaggedValue(++length));
    return true;
}

void JSAPIArrayList::Insert(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                            const JSHandle<JSTaggedValue> &value, const int &index)
{
    int length = arrayList->GetLength().GetInt();
    if (index < 0 || index > length) {
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be >= 0 && <= " << length
            << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN(thread, error);
    }
    JSHandle<TaggedArray> elements = GrowCapacity(thread, arrayList, length + 1);
    ASSERT(!elements->IsDictionaryMode());
    for (int i = length - 1; i >= index; --i) {
        elements->Set(thread, i + 1, elements->Get(i));
    }
    elements->Set(thread, index, value);
    arrayList->SetLength(thread, JSTaggedValue(++length));
}

void JSAPIArrayList::Clear(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList)
{
    if (!arrayList.IsEmpty()) {
        int length = arrayList->GetLength().GetInt();
        JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
        ASSERT(!elements->IsDictionaryMode());
        for (int i = 0; i < length; ++i) {
            elements->Set(thread, i, JSTaggedValue::Hole());
        }
        arrayList->SetLength(thread, JSTaggedValue(0));
    }
}

JSHandle<JSAPIArrayList> JSAPIArrayList::Clone(JSThread *thread, const JSHandle<JSAPIArrayList> &obj)
{
    JSHandle<TaggedArray> srcElements(thread, obj->GetElements());
    ASSERT(!srcElements->IsDictionaryMode());

    uint32_t length = obj->GetSize();
    auto factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSAPIArrayList> newArrayList = factory->NewJSAPIArrayList(0);
    newArrayList->SetLength(thread, JSTaggedValue(length));

    JSHandle<TaggedArray> dstElements = factory->NewAndCopyTaggedArray(srcElements, length, length);
    newArrayList->SetElements(thread, dstElements);
    return newArrayList;
}

uint32_t JSAPIArrayList::GetCapacity(JSThread *thread, const JSHandle<JSAPIArrayList> &obj)
{
    JSHandle<TaggedArray> elements(thread, obj->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    uint32_t capacity = elements->GetLength();
    return capacity;
}

void JSAPIArrayList::IncreaseCapacityTo(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                                        int capacity)
{
    JSHandle<TaggedArray> elementData(thread, arrayList->GetElements());
    ASSERT(!elementData->IsDictionaryMode());
    int length = arrayList->GetLength().GetInt();
    int oldElementLength = static_cast<int>(elementData->GetLength());
    if (oldElementLength != capacity && length < capacity) {
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        JSHandle<TaggedArray> newElements = factory->NewAndCopyTaggedArray(elementData,
            static_cast<uint32_t>(capacity), static_cast<uint32_t>(length));
        arrayList->SetElements(thread, newElements);
    }
}

void JSAPIArrayList::TrimToCurrentLength(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList)
{
    uint32_t length = arrayList->GetLength().GetArrayLength();
    uint32_t capacity = JSAPIArrayList::GetCapacity(thread, arrayList);
    JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    if (capacity > length) {
        elements->Trim(thread, length);
    }
}

JSTaggedValue JSAPIArrayList::Get(JSThread *thread, const uint32_t index)
{
    if (GetLength().GetArrayLength() == 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (index >= GetLength().GetArrayLength()) {
        ASSERT(GetLength().GetArrayLength() > 0);
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be >= 0 && <= "
            << (GetLength().GetArrayLength() - 1) << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }

    TaggedArray *elements = TaggedArray::Cast(GetElements().GetTaggedObject());
    return elements->Get(index);
}

bool JSAPIArrayList::IsEmpty(const JSHandle<JSAPIArrayList> &arrayList)
{
    return arrayList->GetSize() == 0;
}

int JSAPIArrayList::GetIndexOf(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                               const JSHandle<JSTaggedValue> &value)
{
    JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    uint32_t length = arrayList->GetLength().GetArrayLength();
    JSTaggedValue targetValue = value.GetTaggedValue();
    for (uint32_t i = 0; i < length; ++i) {
        if (JSTaggedValue::StrictEqual(targetValue, elements->Get(i))) {
            return i;
        }
    }
    return -1;
}

int JSAPIArrayList::GetLastIndexOf(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                                   const JSHandle<JSTaggedValue> &value)
{
    JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    JSTaggedValue targetValue = value.GetTaggedValue();
    int length = arrayList->GetLength().GetInt();
    for (int i = length - 1; i >= 0; --i) {
        if (JSTaggedValue::StrictEqual(targetValue, elements->Get(i))) {
            return i;
        }
    }
    return -1;
}

JSTaggedValue JSAPIArrayList::RemoveByIndex(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList, int index)
{
    int length = arrayList->GetLength().GetInt();
    if (length <= 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (index < 0 || index >= length) {
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be >= 0 && <= " << (length - 1)
            << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }

    TaggedArray *resElements = TaggedArray::Cast(arrayList->GetElements().GetTaggedObject());
    JSTaggedValue oldValue = resElements->Get(index);

    if (index >= 0) {
        JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
        ASSERT(!elements->IsDictionaryMode());
        TaggedArray::RemoveElementByIndex(thread, elements, index, length);
        arrayList->SetLength(thread, JSTaggedValue(length - 1));
    }

    return oldValue;
}

bool JSAPIArrayList::Remove(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                            const JSHandle<JSTaggedValue> &value)
{
    int index = GetIndexOf(thread, arrayList, value);
    uint32_t length = arrayList->GetSize();
    if (index >= 0) {
        JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
        ASSERT(!elements->IsDictionaryMode());
        TaggedArray::RemoveElementByIndex(thread, elements, index, length);
        arrayList->SetLength(thread, JSTaggedValue(length - 1));
        return true;
    }
    return false;
}

JSTaggedValue JSAPIArrayList::RemoveByRange(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                                            const JSHandle<JSTaggedValue> &value1,
                                            const JSHandle<JSTaggedValue> &value2)
{
    int32_t startIndex = JSTaggedValue::ToInt32(thread, value1);
    int32_t endIndex = JSTaggedValue::ToInt32(thread, value2);
    int32_t length = arrayList->GetLength().GetInt();
    if (length <= 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    int32_t size = length > endIndex ? endIndex : length;
    if (startIndex < 0 || startIndex >= size) {
        std::ostringstream oss;
        oss << "The value of \"fromIndex\" is out of range. It must be >= 0 && <= " << (size - 1)
            << ". Received value is: " << startIndex;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (endIndex <= startIndex || endIndex < 0 || endIndex > length) {
        std::ostringstream oss;
        oss << "The value of \"toIndex\" is out of range. It must be >= 0 && <= " << length
            << ". Received value is: " << endIndex;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }

    int32_t toIndex = endIndex >= length ? length : endIndex;

    JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    int32_t numMoved = length - toIndex;

    for (int32_t i = 0; i < numMoved; i++) {
        elements->Set(thread, startIndex + i, elements->Get(static_cast<uint32_t>(endIndex + i)));
    }
    int32_t newLength = length - (endIndex - startIndex);
    arrayList->SetLength(thread, JSTaggedValue(newLength));
    elements->Trim(thread, newLength);
    return JSTaggedValue::Undefined();
}

JSTaggedValue JSAPIArrayList::ReplaceAllElements(JSThread *thread, const JSHandle<JSTaggedValue> &thisHandle,
                                                 const JSHandle<JSTaggedValue> &callbackFn,
                                                 const JSHandle<JSTaggedValue> &thisArg)
{
    JSHandle<JSAPIArrayList> arrayList = JSHandle<JSAPIArrayList>::Cast(thisHandle);
    uint32_t length = arrayList->GetSize();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> kValue(thread, JSTaggedValue::Undefined());
    const int32_t argsLength = 3;
    JSHandle<JSTaggedValue> undefined = thread->GlobalConstants()->GetHandledUndefined();
    for (uint32_t k = 0; k < length; k++) {
        kValue.Update(arrayList->Get(thread, k));
        key.Update(JSTaggedValue(k));
        EcmaRuntimeCallInfo *info =
            EcmaInterpreter::NewRuntimeCallInfo(thread, callbackFn, thisArg, undefined, argsLength);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
        info->SetCallArg(kValue.GetTaggedValue(), key.GetTaggedValue(), thisHandle.GetTaggedValue());
        JSTaggedValue funcResult = JSFunction::Call(info);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, funcResult);

        arrayList->Set(thread, k, funcResult);
    }

    return JSTaggedValue::Undefined();
}

JSTaggedValue JSAPIArrayList::Set(JSThread *thread, const uint32_t index, JSTaggedValue value)
{
    if (GetLength().GetArrayLength() == 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (index >= GetLength().GetArrayLength()) {
        ASSERT(GetLength().GetArrayLength() > 0);
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be >= 0 && <= "
            << (GetLength().GetArrayLength() - 1) << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }

    TaggedArray *elements = TaggedArray::Cast(GetElements().GetTaggedObject());
    elements->Set(thread, index, value);
    return JSTaggedValue::Undefined();
}

JSTaggedValue JSAPIArrayList::SortElements(JSThread *thread, JSHandle<TaggedArray> &elements,
                                           JSHandle<JSTaggedValue> &callbackFnHandle)
{
    JSMutableHandle<JSTaggedValue> presentValue(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> middleValue(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> previousValue(thread, JSTaggedValue::Undefined());
    uint32_t length = elements->GetLength();
    for (uint32_t i = 1; i < length; i++) {
        uint32_t beginIndex = 0;
        uint32_t endIndex = i;
        presentValue.Update(elements->Get(i));
        while (beginIndex < endIndex) {
            uint32_t middleIndex = (beginIndex + endIndex) / 2; // 2 : half
            middleValue.Update(elements->Get(middleIndex));
            double compareResult = base::ArrayHelper::SortCompare(thread, callbackFnHandle,
                                                                  middleValue, presentValue);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
            if (compareResult > 0) {
                endIndex = middleIndex;
            } else {
                beginIndex = middleIndex + 1;
            }
        }
        if (endIndex >= 0 && endIndex < i) {
            for (uint32_t j = i; j > endIndex; j--) {
                previousValue.Update(elements->Get(j - 1));
                elements->Set(thread, j, previousValue.GetTaggedValue());
            }
            elements->Set(thread, endIndex, presentValue.GetTaggedValue());
        }
    }
    return elements.GetTaggedValue();
}

JSTaggedValue JSAPIArrayList::SortElementsWithCopy(JSThread *thread, JSHandle<TaggedArray> &array,
                                                   JSHandle<JSTaggedValue> &callbackFnHandle)
{
    uint32_t length = array->GetLength();
    JSHandle<TaggedArray> elements = thread->GetEcmaVM()->GetFactory()->NewTaggedArray(length);
    elements->Copy(thread, 0, 0, TaggedArray::Cast(array.GetTaggedValue()), length);
    return SortElements(thread, elements, callbackFnHandle);
}

JSTaggedValue JSAPIArrayList::SubArrayList(JSThread *thread, const JSHandle<JSAPIArrayList> &arrayList,
                                           const JSHandle<JSTaggedValue> &value1,
                                           const JSHandle<JSTaggedValue> &value2)
{
    int length = arrayList->GetLength().GetInt();
    int fromIndex = JSTaggedValue::ToInt32(thread, value1);
    int toIndex = JSTaggedValue::ToInt32(thread, value2);
    int32_t size = length > toIndex ? toIndex : length;
    if (length <= 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (fromIndex < 0 || fromIndex >= size) {
        std::ostringstream oss;
        oss << "The value of \"fromIndex\" is out of range. It must be >= 0 && <= " << (size - 1)
            << ". Received value is: " << fromIndex;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }
    if (toIndex <= fromIndex || toIndex < 0 || toIndex > length) {
        std::ostringstream oss;
        oss << "The value of \"toIndex\" is out of range. It must be >= 0 && <= " << length
            << ". Received value is: " << toIndex;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, JSTaggedValue::Exception());
    }

    int endIndex = toIndex >= length - 1 ? length - 1 : toIndex;
    int newLength = toIndex == length ? length - fromIndex : endIndex - fromIndex;
    JSHandle<JSAPIArrayList> subArrayList = thread->GetEcmaVM()->GetFactory()->NewJSAPIArrayList(newLength);
    if (newLength == 0) {
        return subArrayList.GetTaggedValue();
    }
    JSHandle<TaggedArray> elements(thread, arrayList->GetElements());
    ASSERT(!elements->IsDictionaryMode());
    subArrayList->SetLength(thread, JSTaggedValue(newLength));

    for (int i = 0; i < newLength; i++) {
        subArrayList->Set(thread, i, elements->Get(fromIndex + i));
    }

    return subArrayList.GetTaggedValue();
}

JSTaggedValue JSAPIArrayList::ForEach(JSThread *thread, const JSHandle<JSTaggedValue> &thisHandle,
                                      const JSHandle<JSTaggedValue> &callbackFn,
                                      const JSHandle<JSTaggedValue> &thisArg)
{
    JSHandle<JSAPIArrayList> arrayList = JSHandle<JSAPIArrayList>::Cast(thisHandle);
    uint32_t length = arrayList->GetSize();
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    JSMutableHandle<JSTaggedValue> kValue(thread, JSTaggedValue::Undefined());
    const int32_t argsLength = 3;
    JSHandle<JSTaggedValue> undefined = thread->GlobalConstants()->GetHandledUndefined();
    for (uint32_t k = 0; k < length; k++) {
        kValue.Update(arrayList->Get(thread, k));
        key.Update(JSTaggedValue(k));
        EcmaRuntimeCallInfo *info =
            EcmaInterpreter::NewRuntimeCallInfo(thread, callbackFn, thisArg, undefined, argsLength);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, JSTaggedValue::Exception());
        info->SetCallArg(kValue.GetTaggedValue(), key.GetTaggedValue(), thisHandle.GetTaggedValue());
        JSTaggedValue funcResult = JSFunction::Call(info);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, funcResult);
        if (length != arrayList->GetSize()) {
            length = arrayList->GetSize();
        }
    }

    return JSTaggedValue::Undefined();
}

JSHandle<TaggedArray> JSAPIArrayList::GrowCapacity(const JSThread *thread, const JSHandle<JSAPIArrayList> &obj,
                                                   uint32_t capacity)
{
    JSHandle<TaggedArray> oldElements(thread, obj->GetElements());
    ASSERT(!oldElements->IsDictionaryMode());
    uint32_t oldCapacity = oldElements->GetLength();
    if (capacity < oldCapacity) {
        return oldElements;
    }
    uint32_t newCapacity = ComputeCapacity(capacity);
    JSHandle<TaggedArray> newElements =
        thread->GetEcmaVM()->GetFactory()->CopyArray(oldElements, oldCapacity, newCapacity);

    obj->SetElements(thread, newElements);
    return newElements;
}

bool JSAPIArrayList::Has(const JSTaggedValue value) const
{
    TaggedArray *elements = TaggedArray::Cast(GetElements().GetTaggedObject());
    uint32_t length = GetSize();
    if (length == 0) {
        return false;
    }

    for (uint32_t i = 0; i < length; i++) {
        if (JSTaggedValue::SameValue(elements->Get(i), value)) {
            return true;
        }
    }
    return false;
}

JSHandle<TaggedArray> JSAPIArrayList::OwnKeys(JSThread *thread, const JSHandle<JSAPIArrayList> &obj)
{
    return JSObject::GetOwnPropertyKeys(thread, JSHandle<JSObject>::Cast(obj));
}

JSHandle<TaggedArray> JSAPIArrayList::OwnEnumKeys(JSThread *thread, const JSHandle<JSAPIArrayList> &obj)
{
    return JSObject::GetOwnEnumPropertyKeys(thread, JSHandle<JSObject>::Cast(obj));
}

bool JSAPIArrayList::GetOwnProperty(JSThread *thread, const JSHandle<JSAPIArrayList> &obj,
                                    const JSHandle<JSTaggedValue> &key)
{
    uint32_t index = 0;
    if (UNLIKELY(!JSTaggedValue::ToElementIndex(key.GetTaggedValue(), &index))) {
        JSHandle<EcmaString> result = JSTaggedValue::ToString(thread, key.GetTaggedValue());
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
        CString errorMsg =
            "The type of \"index\" can not obtain attributes of no-number type. Received value is: "
            + ConvertToString(*result);
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::TYPE_ERROR, errorMsg.c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, false);
    }

    uint32_t length = obj->GetLength().GetArrayLength();
    if (length == 0) {
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, "Container is empty");
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, false);
    }
    if (index >= length) {
        ASSERT(length > 0);
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be > " << (length - 1)
            << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error, false);
    }

    obj->Get(thread, index);
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, false);
    return true;
}

JSTaggedValue JSAPIArrayList::GetIteratorObj(JSThread *thread, const JSHandle<JSAPIArrayList> &obj)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSAPIArrayListIterator> iter(factory->NewJSAPIArrayListIterator(obj));

    return iter.GetTaggedValue();
}

OperationResult JSAPIArrayList::GetProperty(JSThread *thread, const JSHandle<JSAPIArrayList> &obj,
                                            const JSHandle<JSTaggedValue> &key)
{
    int length = obj->GetLength().GetInt();
    JSHandle<JSTaggedValue> indexKey = key;
    if (indexKey->IsDouble()) {
        // Math.floor(1) will produce TaggedDouble, we need to cast into TaggedInt
        // For integer which is greater than INT32_MAX, it will remain TaggedDouble
        indexKey = JSHandle<JSTaggedValue>(thread, JSTaggedValue::TryCastDoubleToInt32(indexKey->GetDouble()));
    }
    if (!indexKey->IsInt()) {
        CString errorMsg = "The type of \"index\" must be small integer.";
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::TYPE_ERROR, errorMsg.c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error,
                                         OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    }

    int index = indexKey->GetInt();
    if (index < 0 || index >= length) {
        std::ostringstream oss;
        oss << "The value of \"index\" is out of range. It must be >= 0 && <= " << (length - 1)
            << ". Received value is: " << index;
        JSTaggedValue error = ContainerError::BusinessError(thread, ErrorFlag::RANGE_ERROR, oss.str().c_str());
        THROW_NEW_ERROR_AND_RETURN_VALUE(thread, error,
                                         OperationResult(thread, JSTaggedValue::Exception(), PropertyMetaData(false)));
    }

    return OperationResult(thread, obj->Get(thread, index), PropertyMetaData(false));
}

bool JSAPIArrayList::SetProperty(JSThread *thread, const JSHandle<JSAPIArrayList> &obj,
                                 const JSHandle<JSTaggedValue> &key,
                                 const JSHandle<JSTaggedValue> &value)
{
    int length = obj->GetLength().GetInt();
    int index = static_cast<int>(key->GetNumber());
    if (index < 0 || index >= length) {
        return false;
    }

    obj->Set(thread, index, value.GetTaggedValue());
    return true;
}
}  // namespace panda::ecmascript
