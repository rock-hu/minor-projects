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

#include "builtins_set.h"
#include "ecmascript/interpreter/interpreter.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_set.h"
#include "ecmascript/js_set_iterator.h"
#include "ecmascript/linked_hash_table.h"
namespace panda::ecmascript::builtins {
JSTaggedValue BuiltinsSet::SetConstructor(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Constructor);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 1.If NewTarget is undefined, throw a TypeError exception
    JSHandle<JSTaggedValue> newTarget = GetNewTarget(argv);
    if (newTarget->IsUndefined()) {
        // throw type error
        THROW_TYPE_ERROR_AND_RETURN(thread, "new target can't be undefined", JSTaggedValue::Exception());
    }
    // 2.Let set be OrdinaryCreateFromConstructor(NewTarget, "%SetPrototype%", «‍[[SetData]]» ).
    JSHandle<JSTaggedValue> constructor = GetConstructor(argv);
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), newTarget);
    // 3.returnIfAbrupt()
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    JSHandle<JSSet> set = JSHandle<JSSet>::Cast(obj);
    // 3.ReturnIfAbrupt(set).
    // 4.Set set’s [[SetData]] internal slot to a new empty List.
    JSHandle<LinkedHashSet> linkedSet = LinkedHashSet::Create(thread);
    set->SetLinkedSet(thread, linkedSet);

    // add data into set from iterable
    // 5.If iterable is not present, let iterable be undefined.
    // 6.If iterable is either undefined or null, let iter be undefined.
    JSHandle<JSTaggedValue> iterable(GetCallArg(argv, 0));
    // 8.If iter is undefined, return set
    if (iterable->IsUndefined() || iterable->IsNull()) {
        return set.GetTaggedValue();
    }
    // Let adder be Get(set, "add").
    JSHandle<JSTaggedValue> adderKey(thread->GlobalConstants()->GetHandledAddString());
    JSHandle<JSTaggedValue> setHandle(set);
    JSHandle<JSTaggedValue> adder = JSObject::GetProperty(thread, setHandle, adderKey).GetValue();
    // ReturnIfAbrupt(adder).
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, adder.GetTaggedValue());
    // If IsCallable(adder) is false, throw a TypeError exception
    if (!adder->IsCallable()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "adder is not callable", adder.GetTaggedValue());
    }
    // Let iter be GetIterator(iterable).
    JSHandle<JSTaggedValue> iter(JSIterator::GetIterator(thread, iterable));
    // ReturnIfAbrupt(iter).
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, iter.GetTaggedValue());
    // values in iterator_result may be a JSArray, values[0] = key values[1]=value, used valueIndex to get value from
    // jsarray
    JSHandle<JSTaggedValue> valueIndex(thread, JSTaggedValue(1));
    JSHandle<JSTaggedValue> next = JSIterator::IteratorStep(thread, iter);
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, next.GetTaggedValue());
    while (!next->IsFalse()) {
        // Let nextValue be IteratorValue(next).
        JSHandle<JSTaggedValue> nextValue(JSIterator::IteratorValue(thread, next));
        // ReturnIfAbrupt(nextValue).
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, nextValue.GetTaggedValue());
        JSHandle<JSTaggedValue> undefined = thread->GlobalConstants()->GetHandledUndefined();
        EcmaRuntimeCallInfo *info = EcmaInterpreter::NewRuntimeCallInfo(thread, adder, setHandle, undefined, 1);
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, nextValue.GetTaggedValue());
        info->SetCallArg(nextValue.GetTaggedValue());
        if (nextValue->IsArray(thread)) {
            auto prop = JSTaggedValue::GetProperty(thread, nextValue, valueIndex).GetValue();
            info->SetCallArg(prop.GetTaggedValue());
        }
        JSFunction::Call(info);
        // Let status be Call(adder, set, «nextValue.[[value]]»).
        if (thread->HasPendingException()) {
            return JSIterator::IteratorCloseAndReturn(thread, iter);
        }
        // Let next be IteratorStep(iter).
        next = JSIterator::IteratorStep(thread, iter);
        // ReturnIfAbrupt(next).
        RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, next.GetTaggedValue());
    }
    return set.GetTaggedValue();
}

JSTaggedValue BuiltinsSet::Add(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Add);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);

    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }

    JSHandle<JSTaggedValue> value(GetCallArg(argv, 0));
    JSHandle<JSSet> set(self);
    JSSet::Add(thread, set, value);
    return set.GetTaggedValue();
}

JSTaggedValue BuiltinsSet::Clear(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Clear);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);

    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }
    JSHandle<JSSet> set(self);
    JSSet::Clear(thread, set);
    return JSTaggedValue::Undefined();
}

JSTaggedValue BuiltinsSet::Delete(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Delete);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }

    JSHandle<JSSet> set(self);
    JSHandle<JSTaggedValue> value = GetCallArg(argv, 0);
    bool flag = JSSet::Delete(thread, set, value);
    return GetTaggedBoolean(flag);
}

JSTaggedValue BuiltinsSet::Has(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Has);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }
    JSSet* jsSet = JSSet::Cast(self.GetTaggedValue().GetTaggedObject());
    JSHandle<JSTaggedValue> value = GetCallArg(argv, 0);
    bool flag = jsSet->Has(thread, value.GetTaggedValue());
    return GetTaggedBoolean(flag);
}

JSTaggedValue BuiltinsSet::ForEach(EcmaRuntimeCallInfo *argv)
{
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Set, ForEach);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }
    JSHandle<JSSet> set(self);

    // 4.If IsCallable(callbackfn) is false, throw a TypeError exception.
    JSHandle<JSTaggedValue> func(GetCallArg(argv, 0));
    if (!func->IsCallable()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "callbackfn is not callable", JSTaggedValue::Exception());
    }

    // 5.If thisArg was supplied, let T be thisArg; else let T be undefined.
    JSHandle<JSTaggedValue> thisArg = GetCallArg(argv, 1);

    // 6.Let entries be the List that is the value of S’s [[SetData]] internal slot.
    JSMutableHandle<LinkedHashSet> hashSet(thread, set->GetLinkedSet(thread));
    const uint32_t argsLength = 3;
    int index = 0;
    int totalElements = hashSet->NumberOfElements() + hashSet->NumberOfDeletedElements();
    JSHandle<JSTaggedValue> undefined = thread->GlobalConstants()->GetHandledUndefined();
    // 7.Repeat for each e that is an element of entries, in original insertion order
    while (index < totalElements) {
        JSHandle<JSTaggedValue> key(thread, hashSet->GetKey(thread, index++));
        // a. If e is not empty, then
        if (!key->IsHole()) {
            EcmaRuntimeCallInfo *info = EcmaInterpreter::NewRuntimeCallInfo(
                thread, func, thisArg, undefined, argsLength);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
            info->SetCallArg(key.GetTaggedValue(), key.GetTaggedValue(), set.GetTaggedValue());
            // i. Let funcResult be Call(callbackfn, T, «e, e, S»).
            JSTaggedValue ret = JSFunction::Call(info);  // 3: three args
            // ii. ReturnIfAbrupt(funcResult).
            RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, ret);
            // Maybe add or delete
            JSTaggedValue nextTable = hashSet->GetNextTable(thread);
            while (!nextTable.IsHole()) {
                index -= hashSet->GetDeletedElementsAt(thread, index);
                hashSet.Update(nextTable);
                nextTable = hashSet->GetNextTable(thread);
            }
            totalElements = hashSet->NumberOfElements() + hashSet->NumberOfDeletedElements();
        }
    }
    return JSTaggedValue::Undefined();
}

JSTaggedValue BuiltinsSet::Species(EcmaRuntimeCallInfo *argv)
{
    return GetThis(argv).GetTaggedValue();
}

JSTaggedValue BuiltinsSet::GetSize(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, GetSize);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self(GetThis(argv));
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[SetData]] internal slot, throw a TypeError exception.
    if (!self->IsJSSet()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSSet", JSTaggedValue::Exception());
    }
    JSSet* jsSet = JSSet::Cast(self.GetTaggedValue().GetTaggedObject());
    uint32_t count = jsSet->GetSize(thread);
    return JSTaggedValue(count);
}

JSTaggedValue BuiltinsSet::Entries(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Entries);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    JSHandle<JSTaggedValue> iter = JSSetIterator::CreateSetIterator(thread, self, IterationKind::KEY_AND_VALUE);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return iter.GetTaggedValue();
}

JSTaggedValue BuiltinsSet::Values(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Set, Values);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    JSHandle<JSTaggedValue> iter = JSSetIterator::CreateSetIterator(thread, self, IterationKind::VALUE);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return iter.GetTaggedValue();
}
}  // namespace panda::ecmascript::builtins
