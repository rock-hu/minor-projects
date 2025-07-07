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

#include "ecmascript/js_array_iterator.h"

#include "ecmascript/base/typed_array_helper-inl.h"
#include "ecmascript/js_array.h"

namespace panda::ecmascript {
using BuiltinsBase = base::BuiltinsBase;
// 22.1.5.2.1 %ArrayIteratorPrototype%.next ( )
JSTaggedValue JSArrayIterator::Next(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1.Let O be the this value.
    JSHandle<JSTaggedValue> thisObj(BuiltinsBase::GetThis(argv));
    return NextInternal(thread, thisObj);
}

JSTaggedValue JSArrayIterator::NextInternal(JSThread *thread, JSHandle<JSTaggedValue> thisObj)
{
    // 2.If Type(O) is not Object, throw a TypeError exception.
    // 3.If O does not have all of the internal slots of an TaggedArray Iterator Instance (22.1.5.3), throw a TypeError
    // exception.
    if (!thisObj->IsJSArrayIterator()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this value is not an array iterator", JSTaggedValue::Exception());
    }
    JSHandle<JSArrayIterator> iter(thisObj);
    // 4.Let a be O.[[IteratedArrayLike]].
    JSHandle<JSTaggedValue> array(thread, iter->GetIteratedArray(thread));
    JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());
    // 5.If a is undefined, return CreateIterResultObject(undefined, true).
    if (array->IsUndefined()) {
        return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
    }
    // 6.Let index be O.[[ArrayLikeNextIndex]].
    uint32_t index = iter->GetNextIndex();
    // 7.Let itemKind be O.[[ArrayLikeIterationKind]].
    IterationKind itemKind = iter->GetIterationKind();

    uint32_t length;
    // 8. If a has a [[TypedArrayName]] internal slot, then
    //   a. Let len be the value of O’s [[ArrayLength]] internal slot.
    if (array->IsTypedArray()) {
        length = JSHandle<JSTypedArray>::Cast(array)->GetArrayLength();
    } else if (array->IsJSArray()) {
        length = JSHandle<JSArray>(array)->GetArrayLength();
    } else {
        // 9.Else
        JSHandle<JSTaggedValue> lengthKey = thread->GlobalConstants()->GetHandledLengthString();
        auto lengthProperty = JSTaggedValue::GetProperty(thread, array, lengthKey);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        auto lengthValue = JSTaggedValue::ToLength(thread, lengthProperty.GetValue());
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        length = lengthValue.ToUint32();
    }

    // 10.If index ≥ len, then
    if (index >= length) {
        // Set O.[[IteratedArrayLike]] to undefined.
        // Return CreateIterResultObject(undefined, true).
        iter->SetIteratedArray(thread, undefinedHandle);
        return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
    }
    // 11.Set O.[[ArrayLikeNextIndex]] to index + 1.
    iter->SetNextIndex(index + 1);
    // 12.If itemKind is key, return CreateIterResultObject(index, false).
    JSHandle<JSTaggedValue> key(thread, JSTaggedValue(index));
    if (itemKind == IterationKind::KEY) {
        return JSIterator::CreateIterResultObject(thread, key, false).GetTaggedValue();
    }
    JSHandle<JSTaggedValue> value = JSArray::FastGetPropertyByValue(thread, array, index);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    // 15.If itemKind is value, let result be elementValue.
    if (itemKind == IterationKind::VALUE) {
        return JSIterator::CreateIterResultObject(thread, value, false).GetTaggedValue();
    }
    // 16. Else
    ASSERT_PRINT(itemKind == IterationKind::KEY_AND_VALUE, "itemKind is invalid");
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<TaggedArray> resultArray(factory->NewTaggedArray(2));  // 2 means the length of array
    resultArray->Set(thread, 0, key);
    resultArray->Set(thread, 1, value);
    JSHandle<JSTaggedValue> keyAndValue(JSArray::CreateArrayFromList(thread, resultArray));
    return JSIterator::CreateIterResultObject(thread, keyAndValue, false).GetTaggedValue();
}
}  // namespace panda::ecmascript
