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

#include "ecmascript/builtins/builtins_weak_map.h"

#include "ecmascript/builtins/builtins_map.h"
#include "ecmascript/js_tagged_value-inl.h"
#include "ecmascript/js_weak_container.h"
#include "ecmascript/linked_hash_table.h"
namespace panda::ecmascript::builtins {
JSTaggedValue BuiltinsWeakMap::WeakMapConstructor(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), WeakMap, Constructor);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 1.If NewTarget is undefined, throw a TypeError exception
    JSHandle<JSTaggedValue> newTarget = GetNewTarget(argv);
    if (newTarget->IsUndefined()) {
        // throw type error
        THROW_TYPE_ERROR_AND_RETURN(thread, "new target can't be undefined", JSTaggedValue::Exception());
    }
    // 2.Let WeakMap be OrdinaryCreateFromConstructor(NewTarget, "%WeakMapPrototype%", «‍[[WeakMapData]]» ).
    JSHandle<JSTaggedValue> constructor = GetConstructor(argv);
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), newTarget);
    // 3.returnIfAbrupt()
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    JSHandle<JSWeakMap> weakMap = JSHandle<JSWeakMap>::Cast(obj);

    // 4.Set weakmap’s [[WeakMapData]] internal slot to a new empty List.
    JSHandle<LinkedHashMap> linkedMap = LinkedHashMap::Create(thread);
    weakMap->SetLinkedMap(thread, linkedMap);
    // add data into set from iterable
    // 5.If iterable is not present, let iterable be undefined.
    // 6.If iterable is either undefined or null, let iter be undefined.
    JSHandle<JSTaggedValue> iterable = GetCallArg(argv, 0);
    // 8.If iter is undefined, return set
    if (iterable->IsUndefined() || iterable->IsNull()) {
        return weakMap.GetTaggedValue();
    }
    if (!iterable->IsECMAObject()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "iterable is not object", JSTaggedValue::Exception());
    }
    // Let adder be Get(weakMap, "set").
    JSHandle<JSTaggedValue> adderKey = thread->GlobalConstants()->GetHandledSetString();
    JSHandle<JSTaggedValue> adder =
        JSObject::GetProperty(thread, JSHandle<JSTaggedValue>(weakMap), adderKey).GetValue();
    // ReturnIfAbrupt(adder).
    RETURN_VALUE_IF_ABRUPT_COMPLETION(thread, adder.GetTaggedValue());
    // If IsCallable(adder) is false, throw a TypeError exception
    if (!adder->IsCallable()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "adder is not callable", adder.GetTaggedValue());
    }
    return BuiltinsMap::AddEntriesFromIterable(thread, obj, iterable, adder, factory);
}

JSTaggedValue BuiltinsWeakMap::Delete(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), WeakMap, Delete);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[WeakMapData]] internal slot, throw a TypeError exception.
    if (!self->IsJSWeakMap()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSWeakMap.", JSTaggedValue::Exception());
    }

    JSHandle<JSWeakMap> weakMap(self);
    JSHandle<JSTaggedValue> key = GetCallArg(argv, 0);
    // 5.If CanBeHeldWeakly(key) is false, return false.
    if (!JSTaggedValue::CanBeHeldWeakly(thread, key)) {
        return GetTaggedBoolean(false);
    }
    return GetTaggedBoolean(JSWeakMap::Delete(thread, weakMap, key));
}

JSTaggedValue BuiltinsWeakMap::Has(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), WeakMap, Has);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self(GetThis(argv));
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[WeakMapData]] internal slot, throw a TypeError exception.
    if (!self->IsJSWeakMap()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSWeakMap.", JSTaggedValue::Exception());
    }
    JSWeakMap *jsWeakMap = JSWeakMap::Cast(self.GetTaggedValue().GetTaggedObject());
    JSHandle<JSTaggedValue> key = GetCallArg(argv, 0);
    // 5.If CanBeHeldWeakly(key) is false, return false.
    if (!JSTaggedValue::CanBeHeldWeakly(thread, key)) {
        return GetTaggedBoolean(false);
    }
    return GetTaggedBoolean(jsWeakMap->Has(thread, key.GetTaggedValue()));
}

JSTaggedValue BuiltinsWeakMap::Get(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), WeakMap, Get);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self(GetThis(argv));
    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[WeakMapData]] internal slot, throw a TypeError exception.
    if (!self->IsJSWeakMap()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSWeakMap.", JSTaggedValue::Exception());
    }
    JSWeakMap *jsWeakMap = JSWeakMap::Cast(self.GetTaggedValue().GetTaggedObject());
    JSHandle<JSTaggedValue> key = GetCallArg(argv, 0);
    // 4.If CanBeHeldWeakly(key) is false, return undefined.
    if (!JSTaggedValue::CanBeHeldWeakly(thread, key)) {
        return JSTaggedValue::Undefined();
    }
    return jsWeakMap->Get(thread, key.GetTaggedValue());
}

JSTaggedValue BuiltinsWeakMap::Set(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), WeakMap, Set);
    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> self = GetThis(argv);

    // 2.If Type(S) is not Object, throw a TypeError exception.
    // 3.If S does not have a [[WeakMapData]] internal slot, throw a TypeError exception.
    if (!self->IsJSWeakMap()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "obj is not JSWeakMap.", JSTaggedValue::Exception());
    }

    JSHandle<JSTaggedValue> key = GetCallArg(argv, 0);
    // 4.If CanBeHeldWeakly(key) is false, throw a TypeError exception.
    if (!JSTaggedValue::CanBeHeldWeakly(thread, key)) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "invalid value used as weak map key.", JSTaggedValue::Exception());
    }

    JSHandle<JSTaggedValue> value = GetCallArg(argv, 1);

    JSHandle<JSWeakMap> map(self);
    JSWeakMap::Set(thread, map, key, value);
    return map.GetTaggedValue();
}
}  // namespace panda::ecmascript::builtins
