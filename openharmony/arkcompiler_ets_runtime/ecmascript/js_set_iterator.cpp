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

#include "ecmascript/js_set_iterator.h"

#include "ecmascript/builtins/builtins_errors.h"
#include "ecmascript/js_array.h"
#include "ecmascript/linked_hash_table.h"

namespace panda::ecmascript {
using BuiltinsBase = base::BuiltinsBase;
JSTaggedValue JSSetIterator::Next(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1.If Type(O) is not Object, throw a TypeError exception.
    JSHandle<JSTaggedValue> thisObj(BuiltinsBase::GetThis(argv));
    return NextInternal(thread, thisObj);
}

JSTaggedValue JSSetIterator::NextInternal(JSThread *thread, JSHandle<JSTaggedValue> thisObj)
{
    // 3.If O does not have all of the internal slots of a Set Iterator Instance (23.2.5.3), throw a TypeError
    // exception.
    if (!thisObj->IsJSSetIterator()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "this value is not a set iterator", JSTaggedValue::Exception());
    }
    JSHandle<JSSetIterator> iter(thisObj);
    iter->Update(thread);
    JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());
    // 4.Let s be O.[[IteratedSet]].
    JSHandle<JSTaggedValue> iteratedSet(thread, iter->GetIteratedSet(thread));

    // 5.Let index be O.[[SetNextIndex]].
    int index = static_cast<int>(iter->GetNextIndex());
    IterationKind itemKind = iter->GetIterationKind();
    // 7.If s is undefined, return CreateIterResultObject(undefined, true).
    if (iteratedSet->IsUndefined()) {
        return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
    }
    JSHandle<LinkedHashSet> set(iteratedSet);
    int totalElements = set->NumberOfElements() + set->NumberOfDeletedElements();

    while (index < totalElements) {
        if (!set->GetKey(thread, index).IsHole()) {
            iter->SetNextIndex(index + 1);
            JSHandle<JSTaggedValue> key(thread, set->GetKey(thread, index));
            // If itemKind is value
            if (itemKind == IterationKind::VALUE || itemKind == IterationKind::KEY) {
                return JSIterator::CreateIterResultObject(thread, key, false).GetTaggedValue();
            }
            // If itemKind is key+value, then
            ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
            JSHandle<TaggedArray> array(factory->NewTaggedArray(2));  // 2: key and value pair
            array->Set(thread, 0, key);
            array->Set(thread, 1, key);
            JSHandle<JSTaggedValue> keyAndValue(JSArray::CreateArrayFromList(thread, array));
            return JSIterator::CreateIterResultObject(thread, keyAndValue, false).GetTaggedValue();
        }
        index++;
    }
    // 13.Set O.[[IteratedSet]] to undefined.
    iter->SetIteratedSet(thread, JSTaggedValue::Undefined());
    return JSIterator::CreateIterResultObject(thread, undefinedHandle, true).GetTaggedValue();
}

void JSSetIterator::Update(const JSThread *thread)
{
    DISALLOW_GARBAGE_COLLECTION;
    JSTaggedValue iteratedSet = GetIteratedSet(thread);
    if (iteratedSet.IsUndefined()) {
        return;
    }
    LinkedHashSet *set = LinkedHashSet::Cast(iteratedSet.GetTaggedObject());
    if (set->GetNextTable(thread).IsHole()) {
        return;
    }
    int index = static_cast<int>(GetNextIndex());
    JSTaggedValue nextTable = set->GetNextTable(thread);
    while (!nextTable.IsHole()) {
        index -= set->GetDeletedElementsAt(thread, index);
        set = LinkedHashSet::Cast(nextTable.GetTaggedObject());
        nextTable = set->GetNextTable(thread);
    }
    SetIteratedSet(thread, JSTaggedValue(set));
    SetNextIndex(index);
}

JSHandle<JSTaggedValue> JSSetIterator::CreateSetIterator(JSThread *thread, const JSHandle<JSTaggedValue> &obj,
                                                         IterationKind kind)
{
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    if (!obj->IsJSSet()) {
        JSHandle<JSTaggedValue> undefinedHandle(thread, JSTaggedValue::Undefined());
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", undefinedHandle);
    }
    JSHandle<JSTaggedValue> iter(factory->NewJSSetIterator(JSHandle<JSSet>(obj), kind));
    return iter;
}
}  // namespace panda::ecmascript
