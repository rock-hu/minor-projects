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

#include "ecmascript/builtins/builtins_map.h"
#include "ecmascript/interpreter/interpreter.h"
#include "ecmascript/js_primitive_ref.h"
#include "ecmascript/object_fast_operator-inl.h"

namespace panda::ecmascript::builtins {
// 19.1.1.1 Object ( [ value ] )
JSTaggedValue BuiltinsObject::ObjectConstructor(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Constructor);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    auto ecmaVm = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = ecmaVm->GetGlobalEnv();

    // 1.If NewTarget is neither undefined nor the active function, then
    //    a.Return OrdinaryCreateFromConstructor(NewTarget, "%ObjectPrototype%").
    JSHandle<JSTaggedValue> constructor = GetConstructor(argv);
    JSHandle<JSTaggedValue> newTarget = GetNewTarget(argv);
    if (!newTarget->IsUndefined() && !(newTarget.GetTaggedValue() == constructor.GetTaggedValue())) {
        JSHandle<JSObject> obj =
            ecmaVm->GetFactory()->NewJSObjectByConstructor(JSHandle<JSFunction>(constructor), newTarget);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        return obj.GetTaggedValue();
    }

    // 2.If value is null, undefined or not supplied, return ObjectCreate(%ObjectPrototype%).
    JSHandle<JSTaggedValue> value = GetCallArg(argv, 0);
    if (value->IsNull() || value->IsUndefined()) {
        JSHandle<JSObject> obj = ecmaVm->GetFactory()->OrdinaryNewJSObjectCreate(env->GetObjectFunctionPrototype());
        return obj.GetTaggedValue();
    }

    // 3.Return ToObject(value).
    return JSTaggedValue::ToObject(thread, value).GetTaggedValue();
}

JSTaggedValue BuiltinsObject::AssignTaggedValue(JSThread *thread, const JSHandle<JSTaggedValue> &source,
                                                const JSHandle<JSObject> &toAssign)
{
    JSHandle<JSObject> from = JSTaggedValue::ToObject(thread, source);
    JSHandle<TaggedArray> keys = JSTaggedValue::GetOwnPropertyKeys(thread, JSHandle<JSTaggedValue>::Cast(from));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    uint32_t keysLen = keys->GetLength();
    for (uint32_t j = 0; j < keysLen; j++) {
        PropertyDescriptor desc(thread);
        key.Update(keys->Get(thread, j));
        bool success = JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(from), key, desc);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

        if (success && desc.IsEnumerable()) {
            JSTaggedValue value = desc.GetValue().GetTaggedValue();
            if (value.IsUndefined() || JSHandle<JSTaggedValue>::Cast(from)->IsJSProxy()) {
                value = ObjectFastOperator::FastGetPropertyByValue(thread, from.GetTaggedValue(),
                                                                   key.GetTaggedValue());
            }
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

            ObjectFastOperator::FastSetPropertyByValue(thread, toAssign.GetTaggedValue(), key.GetTaggedValue(),
                                                       value);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        }
    }
    return JSTaggedValue::Undefined();
}

// 19.1.2.1 Object.assign ( target, ...sources )
JSTaggedValue BuiltinsObject::Assign(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Assign);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    uint32_t numArgs = argv->GetArgsNumber();
    // 1.Let to be ToObject(target).
    JSHandle<JSTaggedValue> target = GetCallArg(argv, 0);
    JSHandle<JSObject> toAssign = JSTaggedValue::ToObject(thread, target);
    // 2.ReturnIfAbrupt(to).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.If only one argument was passed, return to.
    // 4.Let sources be the List of argument values starting with the second argument.
    // 5.For each element nextSource of sources, in ascending index order
    //   a.If nextSource is undefined or null, let keys be an empty List.
    //   b.Else,
    //     i.Let from be ToObject(nextSource).
    //     ii.Let keys be from.[[OwnPropertyKeys]]().
    //     iii.ReturnIfAbrupt(keys).
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    for (uint32_t i = 1; i < numArgs; i++) {
        JSHandle<JSTaggedValue> source = GetCallArg(argv, i);
        if (!source->IsNull() && !source->IsUndefined()) {
            JSHandle<JSObject> from = JSTaggedValue::ToObject(thread, source);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

            JSHandle<TaggedArray> keys = JSTaggedValue::GetOwnPropertyKeys(thread, JSHandle<JSTaggedValue>::Cast(from));
            // ReturnIfAbrupt(keys)
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

            // c.Repeat for each element nextKey of keys in List order,
            //    i.Let desc be from.[[GetOwnProperty]](nextKey).
            //    ii.ReturnIfAbrupt(desc).
            //    iii.if desc is not undefined and desc.[[Enumerable]] is true, then
            //      1.Let propValue be Get(from, nextKey).
            //      2.ReturnIfAbrupt(propValue).
            //      3.Let status be Set(to, nextKey, propValue, true).
            //      4.ReturnIfAbrupt(status).
            uint32_t keysLen = keys->GetLength();
            for (uint32_t j = 0; j < keysLen; j++) {
                PropertyDescriptor desc(thread);
                key.Update(keys->Get(thread, j));
                bool success = JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(from), key, desc);
                // ReturnIfAbrupt(desc)
                RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

                if (success && desc.IsEnumerable()) {
                    JSTaggedValue value = desc.GetValue().GetTaggedValue();
                    if (value.IsUndefined() || JSHandle<JSTaggedValue>::Cast(from)->IsJSProxy()) {
                        value = ObjectFastOperator::FastGetPropertyByValue(thread, from.GetTaggedValue(),
                                                                           key.GetTaggedValue());
                    }
                    // ReturnIfAbrupt(prop_value)
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

                    ObjectFastOperator::FastSetPropertyByValue(thread, toAssign.GetTaggedValue(), key.GetTaggedValue(),
                                                               value);
                    //  ReturnIfAbrupt(status)
                    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
                }
            }
        }
    }

    // 6.Return to.
    return toAssign.GetTaggedValue();
}

// Runtime Semantics
JSTaggedValue BuiltinsObject::ObjectDefineProperties(JSThread *thread, const JSHandle<JSTaggedValue> &obj,
                                                     const JSHandle<JSTaggedValue> &prop)
{
    BUILTINS_API_TRACE(thread, Object, DefineProperties);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 1.If Type(O) is not Object, throw a TypeError exception.
    if (!obj->IsECMAObject()) {
        // throw a TypeError exception
        THROW_TYPE_ERROR_AND_RETURN(thread, "is not an object", JSTaggedValue::Exception());
    }

    // 2.Let props be ToObject(Properties).
    JSHandle<JSObject> props = JSTaggedValue::ToObject(thread, prop);

    // 3.ReturnIfAbrupt(props).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4.Let keys be props.[[OwnPropertyKeys]]().
    JSHandle<TaggedArray> handleKeys = JSTaggedValue::GetOwnPropertyKeys(thread, JSHandle<JSTaggedValue>::Cast(props));

    // 5.ReturnIfAbrupt(keys).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 6.Let descriptors be an empty List.
    // new an empty array and append
    uint32_t length = handleKeys->GetLength();
    [[maybe_unused]] JSHandle<TaggedArray> descriptors =
        factory->NewTaggedArray(2 * length);  // 2: 2 means two element list

    // 7.Repeat for each element nextKey of keys in List order,
    //   a.Let propDesc be props.[[GetOwnProperty]](nextKey).
    //   b.ReturnIfAbrupt(propDesc).
    //   c.If propDesc is not undefined and propDesc.[[Enumerable]] is true, then
    //     i.Let descObj be Get( props, nextKey).
    //     ii.ReturnIfAbrupt(descObj).
    //     iii.Let desc be ToPropertyDescriptor(descObj).
    //     iv.ReturnIfAbrupt(desc).
    //     v.Append the pair (a two element List) consisting of nextKey and desc to the end of descriptors.

    std::vector<PropertyDescriptor> desArr;
    for (uint32_t i = 0; i < length; i++) {
        PropertyDescriptor propDesc(thread);
        JSHandle<JSTaggedValue> handleKey(thread, handleKeys->Get(thread, i));

        bool success = JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(props), handleKey, propDesc);
        // ReturnIfAbrupt(propDesc)
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

        if (success && propDesc.IsEnumerable()) {
            JSHandle<JSTaggedValue> descObj =
                JSTaggedValue::GetProperty(thread, JSHandle<JSTaggedValue>::Cast(props), handleKey).GetValue();
            // ReturnIfAbrupt(descObj)
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

            PropertyDescriptor desc(thread);
            JSObject::ToPropertyDescriptor(thread, descObj, desc);
            // ReturnIfAbrupt(desc)
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
            desc.SetKey(handleKey);
            desArr.emplace_back(desc);
        }
    }
    uint32_t desLength = desArr.size();
    for (uint32_t i = 0; i < desLength; i++) {
        // 8.For each pair from descriptors in list order,
        //   a.Let P be the first element of pair.
        //   b.Let desc be the second element of pair.
        //   c.Let status be DefinePropertyOrThrow(O,P, desc).
        //   d.ReturnIfAbrupt(status).
        [[maybe_unused]] bool setSuccess =
            JSTaggedValue::DefinePropertyOrThrow(thread, obj, desArr[i].GetKey(), desArr[i]);

        // ReturnIfAbrupt(status)
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    }
    // 9.Return O.
    return obj.GetTaggedValue();
}

// 19.1.2.2 Object.create ( O [ , Properties ] )
JSTaggedValue BuiltinsObject::Create(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Create);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1.If Type(O) is neither Object nor Null, throw a TypeError exception.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject() && !obj->IsNull()) {
        // throw a TypeError exception
        THROW_TYPE_ERROR_AND_RETURN(thread, "Create: O is neither Object nor Null", JSTaggedValue::Exception());
    }

    if (obj->IsJSShared()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, GET_MESSAGE_STRING(CreateObjectWithSendableProto),
                                    JSTaggedValue::Exception());
    }

    JSHandle<JSTaggedValue> properties = GetCallArg(argv, 1);

    // 2.Let obj be ObjectCreate(O).
    JSHandle<JSObject> objCreate = thread->GetEcmaVM()->GetFactory()->OrdinaryNewJSObjectCreate(obj);

    // 3.If the argument Properties is present and not undefined, then
    //   a.Return ObjectDefineProperties(obj, Properties).
    if (!properties->IsUndefined()) {
        return ObjectDefineProperties(thread, JSHandle<JSTaggedValue>::Cast(objCreate), properties);
    }

    // 4.Return obj.
    return objCreate.GetTaggedValue();
}

// 19.1.2.3 Object.defineProperties ( O, Properties )
JSTaggedValue BuiltinsObject::DefineProperties(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, DefineProperties);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1.Return ObjectDefineProperties(O, Properties).
    return ObjectDefineProperties(thread, GetCallArg(argv, 0), GetCallArg(argv, 1));
}

// 19.1.2.4 Object.defineProperty ( O, P, Attributes )
JSTaggedValue BuiltinsObject::DefineProperty(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, DefineProperty);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1.If Type(O) is not Object, throw a TypeError exception.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject()) {
        // throw a TypeError
        THROW_TYPE_ERROR_AND_RETURN(thread, "DefineProperty: O is not Object", JSTaggedValue::Exception());
    }

    // 2.Let key be ToPropertyKey(P).
    JSHandle<JSTaggedValue> prop = GetCallArg(argv, 1);
    JSHandle<JSTaggedValue> key = JSTaggedValue::ToPropertyKey(thread, prop);

    // 3.ReturnIfAbrupt(key).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    // 4.Let desc be ToPropertyDescriptor(Attributes).
    PropertyDescriptor desc(thread);
    JSObject::ToPropertyDescriptor(thread, GetCallArg(argv, BuiltinsBase::ArgsPosition::THIRD), desc);

    // 5.ReturnIfAbrupt(desc).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 6.Let success be DefinePropertyOrThrow(O,key, desc).
    [[maybe_unused]] bool success = JSTaggedValue::DefinePropertyOrThrow(thread, obj, key, desc);

    // 7.ReturnIfAbrupt(success).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    // 8.Return O.
    return obj.GetTaggedValue();
}

// 19.1.2.5 Object.freeze ( O )
JSTaggedValue BuiltinsObject::Freeze(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Freeze);

    // 1.If Type(O) is not Object, return O.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject()) {
        return obj.GetTaggedValue();
    }

    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    bool status = false;
    // 2.Let status be SetIntegrityLevel( O, "frozen").
    if (obj->IsJSSharedObject() || obj->IsJSSharedFunction() || obj->IsJSSharedAsyncFunction()) {
        status = JSObject::FreezeSharedObject(thread, JSHandle<JSObject>(obj));
    } else if (obj->IsJSSharedArray()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, GET_MESSAGE_STRING(UpdateSendableAttributes), JSTaggedValue::Exception());
    } else {
        status = JSObject::SetIntegrityLevel(thread, JSHandle<JSObject>(obj), IntegrityLevel::FROZEN);
    }

    // 3.ReturnIfAbrupt(status).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4.If status is false, throw a TypeError exception.
    if (!status) {
        // throw a TypeError exception
        THROW_TYPE_ERROR_AND_RETURN(thread, "Freeze: freeze failed", JSTaggedValue::Exception());
    }

    // 5.Return O.
    return obj.GetTaggedValue();
}

// 19.1.2.6 Object.getOwnPropertyDescriptor ( O, P )
JSTaggedValue BuiltinsObject::GetOwnPropertyDescriptor(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, GetOwnPropertyDescriptor);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1.Let obj be ToObject(O).
    JSHandle<JSTaggedValue> func = GetCallArg(argv, 0);
    JSHandle<JSObject> handle = JSTaggedValue::ToObject(thread, func);

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.Let key be ToPropertyKey(P).
    JSHandle<JSTaggedValue> prop = GetCallArg(argv, 1);
    JSHandle<JSTaggedValue> key = JSTaggedValue::ToPropertyKey(thread, prop);

    // 4.ReturnIfAbrupt(key).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5.Let desc be obj.[[GetOwnProperty]](key).
    PropertyDescriptor desc(thread);
    JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(handle), key, desc);

    // 6.ReturnIfAbrupt(desc).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 7.Return FromPropertyDescriptor(desc).
    JSHandle<JSTaggedValue> res = JSObject::FromPropertyDescriptor(thread, desc);
    return res.GetTaggedValue();
}

JSTaggedValue BuiltinsObject::GetOwnPropertyDescriptors(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, GetOwnPropertyDescriptors);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1.Let obj be ToObject(O).
    JSHandle<JSTaggedValue> func = GetCallArg(argv, 0);
    JSHandle<JSObject> handle = JSTaggedValue::ToObject(thread, func);

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Let ownKeys be ? obj.[[OwnPropertyKeys]]().
    JSHandle<TaggedArray> ownKeys =
        JSTaggedValue::GetOwnPropertyKeys(thread, JSHandle<JSTaggedValue>(handle));

    // 4.ReturnIfAbrupt(ownKeys).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5.Let descriptors be OrdinaryObjectCreate(%Object.prototype%).
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> constructor(env->GetObjectFunction());
    JSHandle<JSObject> descriptors = factory->NewJSObjectByConstructor(constructor);

    // 6.For each element key of ownKeys, do
    // a. Let desc be ? obj.[[GetOwnProperty]](key).
    // b. Let descriptor be FromPropertyDescriptor(desc).
    // c. If descriptor is not undefined, perform ! CreateDataPropertyOrThrow(descriptors, key, descriptor).
    uint32_t length = ownKeys->GetLength();
    JSMutableHandle<JSTaggedValue> handleKey(thread, JSTaggedValue::Undefined());
    for (uint32_t i = 0; i < length; ++i) {
        handleKey.Update(ownKeys->Get(thread, i));
        PropertyDescriptor desc(thread);
        JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(handle), handleKey, desc);
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        JSHandle<JSTaggedValue> descriptor = JSObject::FromPropertyDescriptor(thread, desc);
        if (!descriptor->IsUndefined()) {
            JSObject::CreateDataPropertyOrThrow(thread, descriptors, handleKey, descriptor);
            RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
        }
    }

    // 7.Return descriptors.
    return descriptors.GetTaggedValue();
}

// Runtime Semantics
JSTaggedValue BuiltinsObject::GetOwnPropertyKeys(JSThread *thread, const JSHandle<JSTaggedValue> &object,
                                                 const KeyType &type)
{
    BUILTINS_API_TRACE(thread, Object, GetOwnPropertyKeys);
    // 1.Let obj be ToObject(O).
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSObject> obj = JSTaggedValue::ToObject(thread, object);

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.Let keys be obj.[[OwnPropertyKeys]]().
    JSHandle<TaggedArray> handleKeys = JSTaggedValue::GetOwnPropertyKeys(thread, JSHandle<JSTaggedValue>::Cast(obj));

    // 4.ReturnIfAbrupt(keys).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5.Let nameList be a new empty List.
    // new an empty array and append
    uint32_t length = handleKeys->GetLength();
    JSHandle<TaggedArray> nameList = factory->NewTaggedArray(length);

    // 6.Repeat for each element nextKey of keys in List order,
    uint32_t copyLength = 0;
    switch (type) {
        case KeyType::STRING_TYPE: {
            for (uint32_t i = 0; i < length; i++) {
                JSTaggedValue key = handleKeys->Get(thread, i);
                if (key.IsString()) {
                    nameList->Set(thread, copyLength, key);
                    copyLength++;
                }
            }
            break;
        }
        case KeyType::SYMBOL_TYPE: {
            for (uint32_t i = 0; i < length; i++) {
                JSTaggedValue key = handleKeys->Get(thread, i);
                if (key.IsSymbol()) {
                    nameList->Set(thread, copyLength, key);
                    copyLength++;
                }
            }
            break;
        }
        default:
            break;
    }

    // 7.Return CreateArrayFromList(nameList).
    JSHandle<TaggedArray> resultList = factory->CopyArray(nameList, length, copyLength);
    JSHandle<JSArray> resultArray = JSArray::CreateArrayFromList(thread, resultList);
    return resultArray.GetTaggedValue();
}

// 19.1.2.7 Object.getOwnPropertyNames ( O )
JSTaggedValue BuiltinsObject::GetOwnPropertyNames(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, GetOwnPropertyNames);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    KeyType type = KeyType::STRING_TYPE;

    // 1.Return GetOwnPropertyKeys(O, String).
    return GetOwnPropertyKeys(thread, obj, type);
}

// 19.1.2.8 Object.getOwnPropertySymbols ( O )
JSTaggedValue BuiltinsObject::GetOwnPropertySymbols(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, GetOwnPropertySymbols);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    KeyType type = KeyType::SYMBOL_TYPE;

    // 1.Return GetOwnPropertyKeys(O, Symbol).
    return GetOwnPropertyKeys(thread, obj, type);
}

// 19.1.2.9 Object.getPrototypeOf ( O )
JSTaggedValue BuiltinsObject::GetPrototypeOf(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, GetPrototypeOf);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1.Let obj be ToObject(O).
    JSHandle<JSTaggedValue> func = GetCallArg(argv, 0);

    JSHandle<JSObject> obj = JSTaggedValue::ToObject(thread, func);

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.Return obj.[[GetPrototypeOf]]().
    return JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(obj));
}

// 19.1.2.10 Object.is ( value1, value2 )
JSTaggedValue BuiltinsObject::Is(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Object, Is);

    // 1.Return SameValue(value1, value2).
    bool result = JSTaggedValue::SameValue(argv->GetThread(), GetCallArg(argv, 0), GetCallArg(argv, 1));
    return GetTaggedBoolean(result);
}

// 19.1.2.11 Object.isExtensible ( O )
JSTaggedValue BuiltinsObject::IsExtensible(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, IsExtensible);
    // 1.If Type(O) is not Object, return false.
    JSTaggedValue obj = GetCallArg(argv, 0).GetTaggedValue();
    if (!obj.IsHeapObject()) {
        return GetTaggedBoolean(false);
    }
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 2.Return IsExtensible(O).
    return GetTaggedBoolean(obj.IsExtensible(thread));
}

// 19.1.2.12 Object.isFrozen ( O )
JSTaggedValue BuiltinsObject::IsFrozen(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Object, IsFrozen);
    // 1.If Type(O) is not Object, return true.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject()) {
        return GetTaggedBoolean(true);
    }

    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 2.Return TestIntegrityLevel(O, "frozen").
    bool status = JSObject::TestIntegrityLevel(thread, JSHandle<JSObject>(obj), IntegrityLevel::FROZEN);
    return GetTaggedBoolean(status);
}

// 19.1.2.13 Object.isSealed ( O )
JSTaggedValue BuiltinsObject::IsSealed(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    BUILTINS_API_TRACE(argv->GetThread(), Object, IsSealed);
    // 1.If Type(O) is not Object, return true.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject()) {
        return GetTaggedBoolean(true);
    }

    JSThread *thread = argv->GetThread();
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 2.Return TestIntegrityLevel(O, "sealed").
    bool status = JSObject::TestIntegrityLevel(thread, JSHandle<JSObject>(obj), IntegrityLevel::SEALED);
    return GetTaggedBoolean(status);
}

// 19.1.2.14 Object.keys(O)
JSTaggedValue BuiltinsObject::Keys(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Keys);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let obj be ToObject(O).
    JSHandle<JSTaggedValue> msg = GetCallArg(argv, 0);

    JSHandle<JSObject> obj = JSTaggedValue::ToObject(thread, msg);

    // 2. ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Let nameList be EnumerableOwnNames(obj).
    JSHandle<TaggedArray> nameList = JSObject::EnumerableOwnNames(thread, obj);

    // 4. ReturnIfAbrupt(nameList).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Return CreateArrayFromList(nameList).
    JSHandle<JSArray> result = JSArray::CreateArrayFromList(thread, nameList);
    return result.GetTaggedValue();
}

// 20.1.2.22 Object.values(O)
JSTaggedValue BuiltinsObject::Values(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Values);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let obj be ToObject(O).
    JSHandle<JSTaggedValue> msg = GetCallArg(argv, 0);
    JSHandle<JSObject> obj = JSTaggedValue::ToObject(thread, msg);

    // 2. ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Let nameList be ? EnumerableOwnPropertyNames(obj, value).
    JSHandle<TaggedArray> nameList = JSObject::EnumerableOwnPropertyNames(thread, obj, PropertyKind::VALUE);

    // 4. ReturnIfAbrupt(nameList).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Return CreateArrayFromList(nameList).
    JSHandle<JSArray> result = JSArray::CreateArrayFromList(thread, nameList);
    return result.GetTaggedValue();
}

// 19.1.2.15 Object.preventExtensions(O)
JSTaggedValue BuiltinsObject::PreventExtensions(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, PreventExtensions);
    // 1. If Type(O) is not Object, return O.
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (!obj->IsECMAObject()) {
        return obj.GetTaggedValue();
    }
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 2. Let status be O.[[PreventExtensions]]().
    bool status = JSTaggedValue::PreventExtensions(thread, obj);

    // 3. ReturnIfAbrupt(status).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4. If status is false, throw a TypeError exception.
    if (!status) {
        // throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN(thread, "PreventExtensions: preventExtensions failed",
                                    JSTaggedValue::Exception());
    }

    // 5. Return O.
    return obj.GetTaggedValue();
}
// 19.1.2.16 Object.prototype

// 19.1.2.17 Object.seal(O)
JSTaggedValue BuiltinsObject::Seal(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, Seal);

    // 1. If Type(O) is not Object, return O.
    JSHandle<JSTaggedValue> msg = GetCallArg(argv, 0);
    if (!msg->IsECMAObject()) {
        return msg.GetTaggedValue();
    }

    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 2. Let status be SetIntegrityLevel(O, "sealed").
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, msg);
    bool status = JSObject::SetIntegrityLevel(thread, object, IntegrityLevel::SEALED);

    // 3. ReturnIfAbrupt(status).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4. If status is false, throw a TypeError exception.
    if (!status) {
        // throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN(thread, "Seal: seal failed", JSTaggedValue::Exception());
    }

    // 5. Return O.
    return object.GetTaggedValue();
}

// 19.1.2.18 Object.setPrototypeOf(O, proto)
JSTaggedValue BuiltinsObject::SetPrototypeOf(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, SetPrototypeOf);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1. Let O be RequireObjectCoercible(O).
    JSHandle<JSTaggedValue> object = JSTaggedValue::RequireObjectCoercible(thread, GetCallArg(argv, 0));

    // 2. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. If Type(proto) is neither Object nor Null, throw a TypeError exception.
    JSHandle<JSTaggedValue> proto = GetCallArg(argv, 1);
    if (!proto->IsNull() && !proto->IsECMAObject()) {
        // throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN(thread, "SetPrototypeOf: proto is neither Object nor Null",
                                    JSTaggedValue::Exception());
    }

    // 4. If Type(O) is not Object, return O.
    if (!object->IsECMAObject()) {
        return object.GetTaggedValue();
    }

    // 5. Let status be O.[[SetPrototypeOf]](proto).
    bool status = JSTaggedValue::SetPrototype(thread, object, proto);

    // 6. ReturnIfAbrupt(status).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 7. If status is false, throw a TypeError exception.
    if (!status) {
        // throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN(thread, "SetPrototypeOf: prototype set failed", JSTaggedValue::Exception());
    }

    // 8. Return O.
    return object.GetTaggedValue();
}

// 19.1.3.1 Object.prototype.constructor

// 19.1.3.2 Object.prototype.hasOwnProperty(V)
JSTaggedValue BuiltinsObject::HasOwnProperty(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, HasOwnProperty);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> thisValue = GetThis(argv);
    JSHandle<JSTaggedValue> prop = GetCallArg(argv, 0);
    return HasOwnPropertyInternal(thread, thisValue, prop);
}

JSTaggedValue BuiltinsObject::HasOwnPropertyInternal(JSThread *thread, JSHandle<JSTaggedValue> thisValue,
                                                     JSHandle<JSTaggedValue> prop)
{
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    std::pair<JSTaggedValue, bool> result = ObjectFastOperator::HasOwnProperty(thread, thisValue.GetTaggedValue(),
        prop.GetTaggedValue());
    if (!result.first.IsHole()) {
        return GetTaggedBoolean(true);
    } else if (result.second) {
        return GetTaggedBoolean(false);
    }

    // 1. Let P be ToPropertyKey(V).
    JSHandle<JSTaggedValue> property = JSTaggedValue::ToPropertyKey(thread, prop);

    // 2. ReturnIfAbrupt(P).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Let O be ToObject(this value).
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, thisValue);

    // 4. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Return HasOwnProperty(O, P).
    bool res = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(object), property);
    return GetTaggedBoolean(res);
}

// 19.1.3.3 Object.prototype.isPrototypeOf(V)
JSTaggedValue BuiltinsObject::IsPrototypeOf(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, IsPrototypeOf);
    // 1. If Type(V) is not Object, return false.
    JSHandle<JSTaggedValue> msg = GetCallArg(argv, 0);
    if (!msg->IsECMAObject()) {
        return GetTaggedBoolean(false);
    }
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 2. Let O be ToObject(this value).
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, GetThis(argv));
    // 3. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 4. Repeat
    //    a. Let V be V.[[GetPrototypeOf]]().
    //    b. If V is null, return false
    //    c. If SameValue(O, V) is true, return true.
    JSMutableHandle<JSTaggedValue> msgValueHandle(thread, msg.GetTaggedValue());
    while (!msgValueHandle->IsNull()) {
        msgValueHandle.Update(JSTaggedValue::GetPrototype(thread, msgValueHandle));
        RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

        if (JSTaggedValue::SameValue(thread, object.GetTaggedValue(), msgValueHandle.GetTaggedValue())) {
            return GetTaggedBoolean(true);
        }
    }
    return GetTaggedBoolean(false);
}

// 19.1.3.4 Object.prototype.propertyIsEnumerable(V)
JSTaggedValue BuiltinsObject::PropertyIsEnumerable(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    // 1. Let P be ToPropertyKey(V).
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, PropertyIsEnumerable);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    JSHandle<JSTaggedValue> msg = GetCallArg(argv, 0);
    JSHandle<JSTaggedValue> property = JSTaggedValue::ToPropertyKey(thread, msg);

    // 2. ReturnIfAbrupt(P).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. Let O be ToObject(this value).
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, GetThis(argv));
    // 4. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Let desc be O.[[GetOwnProperty]](P).
    PropertyDescriptor desc(thread);
    JSTaggedValue::GetOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(object), property, desc);

    // 6. ReturnIfAbrupt(desc).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 7. If desc is undefined, return false.
    if (desc.IsEmpty()) {
        return GetTaggedBoolean(false);
    }

    // 8. Return the value of desc.[[Enumerable]].
    return GetTaggedBoolean(desc.IsEnumerable());
}

// 19.1.3.5 Object.prototype.toLocaleString([reserved1[, reserved2]])
JSTaggedValue BuiltinsObject::ToLocaleString(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ToLocaleString);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1. Let O be the this value.
    JSHandle<JSTaggedValue> object = GetThis(argv);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 2. Return Invoke(O, "toString").
    JSHandle<JSTaggedValue> calleeKey = thread->GlobalConstants()->GetHandledToStringString();
    const uint32_t argsLength = argv->GetArgsNumber();
    JSHandle<JSTaggedValue> undefined = thread->GlobalConstants()->GetHandledUndefined();
    EcmaRuntimeCallInfo *info = EcmaInterpreter::NewRuntimeCallInfo(thread, undefined, object, undefined, argsLength);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    info->SetCallArg(argsLength, 0, argv, 0);
    return JSFunction::Invoke(info, calleeKey);
}

JSTaggedValue BuiltinsObject::GetBuiltinObjectToString(JSThread *thread, const JSHandle<JSObject> &object)
{
    BUILTINS_API_TRACE(thread, Object, GetBuiltinObjectToString);
    // 4. Let isArray be IsArray(O).
    bool isArray = object.GetTaggedValue().IsArray(thread);
    // 5. ReturnIfAbrupt(isArray).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    if (isArray) {
        // 6. If isArray is true, return "[object Array]".
        return thread->GlobalConstants()->GetArrayToString();
    } else if (object->IsJSPrimitiveRef()) {
        // 7. Else, if O is an exotic String object, return "[object String]".
        JSPrimitiveRef *primitiveRef = JSPrimitiveRef::Cast(*object);
        if (primitiveRef->IsString(thread)) {
            return thread->GlobalConstants()->GetStringToString();
        } else if (primitiveRef->IsBoolean(thread)) {
            // 11. Else, if O has a [[BooleanData]] internal slot, return "[object Boolean]".
            return thread->GlobalConstants()->GetBooleanToString();
        } else if (primitiveRef->IsNumber(thread)) {
            // 12. Else, if O has a [[NumberData]] internal slot, return "[object Number]".
            return thread->GlobalConstants()->GetNumberToString();
        }
    } else if (object->IsArguments()) {
        // if O has a [[ArgumentsData]] internal slot, return "[object Arguments]".
        return thread->GlobalConstants()->GetArgumentsToString();
    } else if (object->IsCallable()) {
        // if O has a [[CallableData]] internal slot, return "[object Function]".
        return thread->GlobalConstants()->GetFunctionToString();
    } else if (object->IsJSError()) {
        // if O has a [[ErrorData]] internal slot, return "[object Error]".
        return thread->GlobalConstants()->GetErrorToString();
    } else if (object->IsDate()) {
        // if O has a [[DateData]] internal slot, return "[object Date]".
        return thread->GlobalConstants()->GetDateToString();
    } else if (object->IsJSRegExp()) {
        // if O has a [[RegExpData]] internal slot, return "[object JSRegExp]".
        return thread->GlobalConstants()->GetRegExpToString();
    }
    // 15. Else, return "[Object Object]".
    return thread->GlobalConstants()->GetObjectToString();
}

// 19.1.3.6 Object.prototype.toString()
JSTaggedValue BuiltinsObject::ToString(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ToString);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1. If the this value is undefined, return "[object Undefined]".

    JSHandle<JSTaggedValue> msg = GetThis(argv);
    if (msg->IsUndefined()) {
        return thread->GlobalConstants()->GetUndefinedToString();
    }
    // 2. If the this value is null, return "[object Null]".
    if (msg->IsNull()) {
        return thread->GlobalConstants()->GetNullToString();
    }

    // 3. Let O be ToObject(this value).
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, GetThis(argv));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 16. Let tag be Get (O, @@toStringTag).
    auto ecmaVm = thread->GetEcmaVM();
    JSHandle<GlobalEnv> env = ecmaVm->GetGlobalEnv();
    auto factory = ecmaVm->GetFactory();

    JSHandle<JSTaggedValue> tag = JSTaggedValue::GetProperty(thread, msg, env->GetToStringTagSymbol()).GetValue();

    // 17. ReturnIfAbrupt(tag).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 18. If Type(tag) is not String, return builtin object to string.
    if (!tag->IsString()) {
        return GetBuiltinObjectToString(thread, object);
    }

    // 19. Return the String that is the result of concatenating "[object ", tag, and "]".
    JSHandle<EcmaString> leftString(factory->NewFromASCII("[object "));
    JSHandle<EcmaString> rightString(factory->NewFromASCII("]"));

    JSHandle<EcmaString> newLeftStringHandle =
        factory->ConcatFromString(leftString, JSTaggedValue::ToString(thread, tag));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    auto result = factory->ConcatFromString(newLeftStringHandle, rightString);
    return result.GetTaggedValue();
}

// 19.1.3.7 Object.prototype.valueOf()
JSTaggedValue BuiltinsObject::ValueOf(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ValueOf);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Return ToObject(this value).
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, GetThis(argv));
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    return object.GetTaggedValue();
}
// B.2.2.1 Object.prototype.__proto__
JSTaggedValue BuiltinsObject::ProtoGetter(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ProtoGetter);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1.Let obj be ToObject(this value).
    JSHandle<JSObject> obj = JSTaggedValue::ToObject(thread, GetThis(argv));

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.Return obj.[[GetPrototypeOf]]().
    return JSTaggedValue::GetPrototype(thread, JSHandle<JSTaggedValue>(obj));
}

JSTaggedValue BuiltinsObject::ProtoSetter(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ProtoSetter);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    // 1. Let O be RequireObjectCoercible(this value).
    JSHandle<JSTaggedValue> obj = JSTaggedValue::RequireObjectCoercible(thread, GetThis(argv));

    // 2. ReturnIfAbrupt(O).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3. If Type(proto) is neither Object nor Null, return undefined..
    JSHandle<JSTaggedValue> proto = GetCallArg(argv, 0);
    if (!proto->IsNull() && !proto->IsECMAObject()) {
        return JSTaggedValue::Undefined();
    }

    // 4. If Type(O) is not Object, return undefined.
    if (!obj->IsECMAObject()) {
        return JSTaggedValue::Undefined();
    }

    // 5. Let status be O.[[SetPrototypeOf]](proto).
    bool status = JSTaggedValue::SetPrototype(thread, obj, proto, true);

    // 6. ReturnIfAbrupt(status).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 7. If status is false, throw a TypeError exception.
    if (!status) {
        // throw a TypeError exception.
        THROW_TYPE_ERROR_AND_RETURN(thread, "ProtoSetter: proto set failed", JSTaggedValue::Exception());
    }

    // 8. Return O.
    return JSTaggedValue::Undefined();
}

JSTaggedValue BuiltinsObject::CreateRealm(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, CreateRealm);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<JSRealm> realm = factory->NewJSRealm();
    return realm.GetTaggedValue();
}

JSTaggedValue BuiltinsObject::Entries(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, ToString);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let obj be ? ToObject(O).
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    if (obj->IsJSUint8Array() || obj->IsJSUint16Array()) {
        THROW_RANGE_ERROR_AND_RETURN(thread, "Object entries is not supported IsJSUint8Array or IsJSUint16Array",
                                     JSTaggedValue::Exception());
    }
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, obj);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    // 2. Let nameList be ? EnumerableOwnPropertyNames(obj, key+value).
    JSHandle<TaggedArray> nameList = JSObject::EnumerableOwnPropertyNames(thread, object, PropertyKind::KEY_VALUE);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);
    // 3. Return CreateArrayFromList(nameList).
    return JSArray::CreateArrayFromList(thread, nameList).GetTaggedValue();
}

JSTaggedValue BuiltinsObject::FromEntries(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, FromEntries);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    JSHandle<JSTaggedValue> iterable = GetCallArg(argv, 0);
    // 1. Perform ? RequireObjectCoercible(iterable).
    if (iterable->IsUndefined() || iterable->IsNull()) {
        THROW_TYPE_ERROR_AND_RETURN(thread, "iterable is undefined or null", JSTaggedValue::Exception());
    }

    // 2. Let obj be ! OrdinaryObjectCreate(%Object.prototype%).
    // 3. Assert: obj is an extensible ordinary object with no own properties.
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSHandle<JSFunction> constructor(env->GetObjectFunction());
    JSHandle<JSObject> obj = factory->NewJSObjectByConstructor(constructor);

    // 4. Let stepsDefine be the algorithm steps defined in CreateDataPropertyOnObject Functions.
    // 5. Let lengthDefine be the number of non-optional parameters of the function definition in
    //    CreateDataPropertyOnObject Functions.
    // 6. Let adder be ! CreateBuiltinFunction(stepsDefine, lengthDefine, "", « »).
    JSHandle<Method> method(thread,
        thread->GetEcmaVM()->GetMethodByIndex(MethodIndex::BUILTINS_OBJECT_CREATE_DATA_PROPERTY_ON_OBJECT_FUNCTIONS));
    JSHandle<JSFunction> addrFunc = factory->NewJSFunction(env, method);

    JSHandle<JSTaggedValue> adder(thread, addrFunc.GetTaggedValue());

    // 7. Return ? AddEntriesFromIterable(obj, iterable, adder).
    return BuiltinsMap::AddEntriesFromIterable(thread, obj, iterable, adder, factory);
}

JSTaggedValue BuiltinsObject::CreateDataPropertyOnObjectFunctions(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, CreateDataPropertyOnObjectFunctions);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let O be the this value.
    JSHandle<JSTaggedValue> thisHandle = GetThis(argv);
    JSHandle<JSObject> thisObjHandle = JSHandle<JSObject>::Cast(thisHandle);

    // 2. Assert: Type(O) is Object.
    // 3. Assert: O is an extensible ordinary object.
    ASSERT(thisHandle->IsHeapObject());

    JSHandle<JSTaggedValue> key = GetCallArg(argv, 0);
    JSHandle<JSTaggedValue> value = GetCallArg(argv, 1);

    // 4. Let propertyKey be ? ToPropertyKey(key).
    JSHandle<JSTaggedValue> propertyKey = JSTaggedValue::ToPropertyKey(thread, key);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Perform ! CreateDataPropertyOrThrow(O, propertyKey, value).
    JSObject::CreateDataPropertyOrThrow(thread, thisObjHandle, propertyKey, value);
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 6. Return undefined.
    return JSTaggedValue::Undefined();
}

JSTaggedValue BuiltinsObject::HasOwn(EcmaRuntimeCallInfo *argv)
{
    ASSERT(argv);
    JSThread *thread = argv->GetThread();
    BUILTINS_API_TRACE(thread, Object, HasOwn);
    [[maybe_unused]] EcmaHandleScope handleScope(thread);

    // 1. Let obj be ? ToObject(O).
    JSHandle<JSTaggedValue> obj = GetCallArg(argv, 0);
    JSHandle<JSObject> object = JSTaggedValue::ToObject(thread, obj);

    // 2.ReturnIfAbrupt(obj).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 3.Let key be ToPropertyKey(P).
    JSHandle<JSTaggedValue> prop = GetCallArg(argv, 1);
    JSHandle<JSTaggedValue> key = JSTaggedValue::ToPropertyKey(thread, prop);

    // 4. ReturnIfAbrupt(4).
    RETURN_EXCEPTION_IF_ABRUPT_COMPLETION(thread);

    // 5. Return HasOwnProperty(O, P).
    bool res = JSTaggedValue::HasOwnProperty(thread, JSHandle<JSTaggedValue>::Cast(object), key);
    return GetTaggedBoolean(res);
}
}  // namespace panda::ecmascript::builtins
