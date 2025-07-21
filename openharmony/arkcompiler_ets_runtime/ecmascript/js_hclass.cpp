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

#include "ecmascript/js_hclass.h"

#include "ecmascript/dependent_infos.h"
#include "ecmascript/global_env_constants-inl.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_tagged_value.h"
#include "ecmascript/pgo_profiler/pgo_profiler.h"
#include "ecmascript/pgo_profiler/pgo_profiler_layout.h"
#include "ecmascript/ic/proto_change_details.h"
#include "ecmascript/js_function.h"
#include "ecmascript/js_object-inl.h"

namespace panda::ecmascript {
using ProfileType = pgo::ProfileType;

JSHandle<TransitionsDictionary> TransitionsDictionary::PutIfAbsent(const JSThread *thread,
                                                                   const JSHandle<TransitionsDictionary> &dictionary,
                                                                   const JSHandle<JSTaggedValue> &key,
                                                                   const JSHandle<JSTaggedValue> &value,
                                                                   const JSHandle<JSTaggedValue> &metaData)
{
    uint32_t hash =
        static_cast<uint32_t>(TransitionsDictionary::Hash(thread, key.GetTaggedValue(), metaData.GetTaggedValue()));

    /* no need to add key if exist */
    int entry = dictionary->FindEntry(thread, key.GetTaggedValue(), metaData.GetTaggedValue());
    if (entry != -1) {
        if (dictionary->GetValue(thread, entry).IsUndefined()) {
            JSTaggedValue weakValue = JSTaggedValue(value->CreateAndGetWeakRef());
            dictionary->SetValue(thread, entry, weakValue);
        }
        return dictionary;
    }

    // Check whether the dictionary should be extended.
    JSHandle<TransitionsDictionary> newDictionary(HashTableT::GrowHashTable(thread, dictionary));
    // Compute the key object.
    entry = newDictionary->FindInsertIndex(thread, hash);
    JSTaggedValue val = value.GetTaggedValue();
    newDictionary->SetEntry(thread, entry, key.GetTaggedValue(), val, metaData.GetTaggedValue());

    newDictionary->IncreaseEntries(thread);
    return newDictionary;
}

int TransitionsDictionary::FindEntry(const JSThread *thread, const JSTaggedValue &key, const JSTaggedValue &metaData)
{
    size_t size = static_cast<size_t>(Size());
    uint32_t count = 1;
    int32_t hash = TransitionsDictionary::Hash(thread, key, metaData);
    // GrowHashTable will guarantee the hash table is never full.
    for (uint32_t entry = GetFirstPosition(hash, size);; entry = GetNextPosition(entry, count++, size)) {
        JSTaggedValue element = GetKey(thread, entry);
        if (element.IsHole()) {
            continue;
        }
        if (element.IsUndefined()) {
            return -1;
        }

        if (TransitionsDictionary::IsMatch(key, metaData, element, GetAttributes(thread, entry).GetWeakRawValue())) {
            return static_cast<int>(entry);
        }
    }
    return -1;
}

JSHandle<TransitionsDictionary> TransitionsDictionary::Remove(const JSThread *thread,
                                                              const JSHandle<TransitionsDictionary> &table,
                                                              const JSHandle<JSTaggedValue> &key,
                                                              const JSTaggedValue &metaData)
{
    int entry = table->FindEntry(thread, key.GetTaggedValue(), metaData);
    if (entry == -1) {
        return table;
    }

    table->RemoveElement(thread, entry);
    return TransitionsDictionary::Shrink(thread, table);
}

void TransitionsDictionary::Rehash(const JSThread *thread, TransitionsDictionary *newTable)
{
    DISALLOW_GARBAGE_COLLECTION;
    if (newTable == nullptr) {
        return;
    }
    int size = this->Size();
    // Rehash elements to new table
    int entryCount = 0;
    for (int i = 0; i < size; i++) {
        int fromIndex = GetEntryIndex(i);
        JSTaggedValue k = this->GetKey(thread, i);
        JSTaggedValue v = this->GetValue(thread, i);
        if (IsKey(k) && TransitionsDictionary::CheckWeakExist(v)) {
            uint32_t hash =
                static_cast<uint32_t>(TransitionsDictionary::Hash(thread, k, this->GetAttributes(thread, i)));
            int insertionIndex = GetEntryIndex(newTable->FindInsertIndex(thread, hash));
            JSTaggedValue tv = Get(thread, fromIndex);
            newTable->Set(thread, insertionIndex, tv);
            for (int j = 1; j < TransitionsDictionary::ENTRY_SIZE; j++) {
                tv = Get(thread, fromIndex + j);
                newTable->Set(thread, insertionIndex + j, tv);
            }
            entryCount++;
        }
    }
    newTable->SetEntriesCount(thread, entryCount);
    newTable->SetHoleEntriesCount(thread, 0);
}

void JSHClass::InitializeWithDefaultValue(const JSThread *thread, uint32_t size, JSType type, uint32_t inlinedProps)
{
    DISALLOW_GARBAGE_COLLECTION;
    ClearBitField();
    if (IsJSTypeObject(type)) {
        SetObjectSize(size + inlinedProps * JSTaggedValue::TaggedTypeSize());
        SetInlinedPropsStart(size);
    } else {
        SetObjectSize(size);
    }
    SetLayout(thread, JSTaggedValue::Null());
    if (type >= JSType::JS_FUNCTION_FIRST && type <= JSType::JS_FUNCTION_LAST) {
        SetIsJSFunction(true);
    }
    SetPrototype(thread, JSTaggedValue::Null());

    SetObjectType(type);
    SetExtensible(true);
    SetIsPrototype(false);
    SetHasDeleteProperty(false);
    SetIsAllTaggedProp(true);
    SetIsStable(true);
    SetElementsKind(ElementsKind::GENERIC);
    SetTransitions(thread, JSTaggedValue::Undefined());
    SetParent(thread, JSTaggedValue::Undefined());
    SetProtoChangeMarker(thread, JSTaggedValue::Null());
    SetProtoChangeDetails(thread, JSTaggedValue::Null());
    SetEnumCache(thread, JSTaggedValue::Null());
    SetConstructionCounter(0);
    SetDependentInfos(thread, JSTaggedValue::Undefined());
}

bool JSHClass::IsJSTypeShared(JSType type)
{
    bool isShared = false;
    switch (type) {
        case JSType::JS_SHARED_OBJECT:
        case JSType::JS_SHARED_FUNCTION:
        case JSType::JS_SHARED_ASYNC_FUNCTION:
        case JSType::JS_API_BITVECTOR:
        case JSType::JS_SHARED_SET:
        case JSType::JS_SHARED_MAP:
        case JSType::JS_SHARED_ARRAY:
        case JSType::JS_SHARED_TYPED_ARRAY:
        case JSType::JS_SHARED_INT8_ARRAY:
        case JSType::JS_SHARED_UINT8_ARRAY:
        case JSType::JS_SHARED_UINT8_CLAMPED_ARRAY:
        case JSType::JS_SHARED_INT16_ARRAY:
        case JSType::JS_SHARED_UINT16_ARRAY:
        case JSType::JS_SHARED_INT32_ARRAY:
        case JSType::JS_SHARED_UINT32_ARRAY:
        case JSType::JS_SHARED_FLOAT32_ARRAY:
        case JSType::JS_SHARED_FLOAT64_ARRAY:
        case JSType::JS_SHARED_BIGINT64_ARRAY:
        case JSType::JS_SHARED_BIGUINT64_ARRAY:
        case JSType::JS_SENDABLE_ARRAY_BUFFER:
        case JSType::BIGINT:
        case JSType::LINE_STRING:
        case JSType::SLICED_STRING:
        case JSType::TREE_STRING:
            isShared = true;
            break;
        default:
            break;
    }
    return isShared;
}

// class JSHClass
void JSHClass::Initialize(const JSThread *thread, uint32_t size, JSType type, uint32_t inlinedProps)
{
    InitializeWithDefaultValue(thread, size, type, inlinedProps);
    if (IsJSTypeObject(type)) {
        SetLayout(thread, thread->GlobalConstants()->GetEmptyLayoutInfo());
    }
}

// for sharedHeap
void JSHClass::Initialize(const JSThread *thread, uint32_t size, JSType type,
    uint32_t inlinedProps, const JSHandle<JSTaggedValue> &layout)
{
    InitializeWithDefaultValue(thread, size, type, inlinedProps);
    if (IsJSTypeObject(type)) {
        SetLayout(thread, layout);
    }
    if (IsJSTypeShared(type)) {
        SetIsJSShared(true);
    }
}


size_t JSHClass::GetCloneSize(JSHClass* jshclass)
{
    return IsJSTypeObject(jshclass->GetObjectType()) ? jshclass->GetInlinedPropsStartSize() :
                                                       jshclass->GetObjectSize();
}

JSHandle<JSHClass> JSHClass::Clone(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                   bool specificInlinedProps, uint32_t specificNumInlinedProps)
{
    JSType type = jshclass->GetObjectType();
    uint32_t size = JSHClass::GetCloneSize(*jshclass);
    uint32_t numInlinedProps = specificInlinedProps ? specificNumInlinedProps : jshclass->GetInlinedProperties();
    JSHandle<JSHClass> newJsHClass;
    if (jshclass.GetTaggedValue().IsInSharedHeap()) {
        newJsHClass = thread->GetEcmaVM()->GetFactory()->NewSEcmaHClass(size, type, numInlinedProps);
    } else {
        newJsHClass = thread->GetEcmaVM()->GetFactory()->NewEcmaHClass(size, type, numInlinedProps);
    }
    // Copy all
    newJsHClass->Copy(thread, *jshclass);
    newJsHClass->SetTransitions(thread, JSTaggedValue::Undefined());
    newJsHClass->SetParent(thread, JSTaggedValue::Undefined());
    newJsHClass->SetProtoChangeDetails(thread, JSTaggedValue::Null());
    newJsHClass->SetEnumCache(thread, JSTaggedValue::Null());
    // reuse Attributes first.
    newJsHClass->SetLayout(thread, jshclass->GetLayout(thread));

    if (jshclass->IsAOT()) {
        newJsHClass->SetAOT(false);
    }

    return newJsHClass;
}

JSHandle<JSHClass> JSHClass::CloneAndIncInlinedProperties(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                                          uint32_t expectedOfProperties)
{
    uint32_t size = jshclass->IsJSObject() ?
        jshclass->GetInlinedPropsStartSize() : jshclass->GetObjectSize();
    ASSERT((size % JSTaggedValue::TaggedTypeSize()) == 0);
    uint32_t maxFields = PropertyAttributes::MAX_FAST_PROPS_CAPACITY - size / JSTaggedValue::TaggedTypeSize();
    expectedOfProperties = std::min(maxFields, expectedOfProperties);
    uint32_t inlinedProp = std::max(expectedOfProperties, jshclass->GetInlinedProperties());
    return JSHClass::Clone(thread, jshclass, true, inlinedProp);
}

JSHandle<JSHClass> JSHClass::CloneWithElementsKind(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                                   const ElementsKind kind, bool isPrototype)
{
    JSHandle<JSHClass> newHClass = Clone(thread, jshclass);
    newHClass->SetIsPrototype(isPrototype);
    newHClass->SetElementsKind(kind);
    return newHClass;
}

// use for transition to dictionary
JSHandle<JSHClass> JSHClass::CloneWithoutInlinedProperties(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    return Clone(thread, jshclass, true);
}

void JSHClass::TransitionElementsToDictionary(const JSThread *thread, const JSHandle<JSObject> &obj)
{
    // property transition to slow first
    if (!obj->GetJSHClass()->IsDictionaryMode()) {
        JSObject::TransitionToDictionary(thread, obj);
        RETURN_IF_ABRUPT_COMPLETION(thread);
    } else {
        TransitionToDictionary(thread, obj);
    }
    obj->GetJSHClass()->SetIsDictionaryElement(true);
    obj->GetJSHClass()->SetIsStableElements(false);
    obj->GetJSHClass()->SetElementsKind(ElementsKind::DICTIONARY);
}

void JSHClass::OptimizeAsFastElements(const JSThread *thread, JSHandle<JSObject> obj)
{
    if (obj->GetJSHClass()->IsDictionaryMode()) {
        JSObject::OptimizeAsFastProperties(thread, obj);
    } else {
        OptimizeAsFastProperties(thread, obj);
    }
    obj->GetJSHClass()->SetIsDictionaryElement(false);
    obj->GetJSHClass()->SetIsStableElements(true);
    obj->GetJSHClass()->SetElementsKind(ElementsKind::HOLE_TAGGED);
}

void JSHClass::ProcessAotHClassTransition(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                          const JSHandle<JSHClass> newHClass, const JSTaggedValue &key)
{
    if (JSHClass::IsNeedNotifyHclassChangedForAotTransition(thread, jshclass, key)) {
        JSHClass::NotifyHclassChanged(thread, jshclass, newHClass, key);
    } else {
#if ENABLE_NEXT_OPTIMIZATION
        JSHClass::NotifyHClassChangedForAot(thread, jshclass, newHClass, key);
#endif
        JSHClass::RefreshUsers(thread, jshclass, newHClass);
    }
    JSHClass::EnablePHCProtoChangeMarker(thread, newHClass);
}

void JSHClass::AddProperty(const JSThread *thread, const JSHandle<JSObject> &obj, const JSHandle<JSTaggedValue> &key,
                           const PropertyAttributes &attr, const Representation &rep)
{
    JSHandle<JSHClass> jshclass(thread, obj->GetJSHClass());
    auto metadata = JSTaggedValue(attr.GetPropertyMetaData());
    JSHClass *newClass = jshclass->FindTransitions(thread, key.GetTaggedValue(), metadata, rep);
    if (newClass != nullptr) {
        // The transition hclass from AOT, which does not have a prototype, needs to be reset here.
        if (newClass->IsAOT()) {
            newClass->SetPrototype(thread, jshclass->GetPrototype(thread));
        }
        // Because we currently only supports Fast ElementsKind
        RestoreElementsKindToGeneric(newClass);
        obj->SynchronizedTransitionClass(thread, newClass);
#if ECMASCRIPT_ENABLE_IC
        // The transition hclass from AOT, which does not have protochangemarker, needs to be reset here
        JSHandle<JSHClass> newHClass = JSHandle<JSHClass>(thread, newClass);
        if (newClass->IsAOT() && newClass->IsPrototype()) {
            JSHClass::ProcessAotHClassTransition(thread, jshclass, newHClass, key.GetTaggedValue());
        } else {
            if (newClass->IsPrototype()) {
                newHClass->SetProtoChangeDetails(thread, jshclass->GetProtoChangeDetails(thread));
            }
            JSHClass::NotifyHclassChanged(thread, jshclass, newHClass, key.GetTaggedValue());
        }
#endif
        return;
    }
    JSHandle<JSHClass> newJsHClass = JSHClass::Clone(thread, jshclass);
    RestoreElementsKindToGeneric(*newJsHClass);
    AddPropertyToNewHClass(thread, jshclass, newJsHClass, key, attr);
    // update hclass in object.
#if ECMASCRIPT_ENABLE_IC
    JSHClass::NotifyHclassChanged(thread, jshclass, newJsHClass, key.GetTaggedValue());
#endif
    // Because we currently only supports Fast ElementsKind
    obj->SynchronizedTransitionClass(thread, *newJsHClass);
}

void JSHClass::AddPropertyToNewHClassWithoutTransition(const JSThread *thread, JSHandle<JSHClass> &newJsHClass,
                                                       const JSHandle<JSTaggedValue> &key,
                                                       const PropertyAttributes &attr)
{
    ASSERT(!newJsHClass->IsDictionaryMode());
    // Add Property and metaData
    newJsHClass->IncNumberOfProps();
    JSMutableHandle<LayoutInfo> layoutInfoHandle(thread, newJsHClass->GetLayout(thread));
    uint32_t offset = attr.GetOffset();
    ASSERT(layoutInfoHandle->NumberOfElements() == static_cast<int>(offset));
    ASSERT(layoutInfoHandle->GetPropertiesCapacity() > static_cast<int>(offset));
    // Duplicate key has Already been Checked in CreateClassFuncWithProperties.
    ASSERT(!layoutInfoHandle->CheckIsDuplicateKey(thread, offset, key->GetKeyHashCode(thread), key.GetTaggedValue()));
    layoutInfoHandle->AddKey<false>(thread, offset, key.GetTaggedValue(), attr);
}

JSHandle<JSHClass> JSHClass::TransitionExtension(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    JSHandle<JSTaggedValue> key(thread->GlobalConstants()->GetHandledPreventExtensionsString());
    {
        auto *newClass =
            jshclass->FindTransitions(thread, key.GetTaggedValue(), JSTaggedValue(0), Representation::NONE);
        if (newClass != nullptr) {
            newClass->SetPrototype(thread, jshclass->GetPrototype(thread));
            return JSHandle<JSHClass>(thread, newClass);
        }
    }
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 2. new a hclass
    JSHandle<JSHClass> newJsHClass = JSHClass::Clone(thread, jshclass);
    newJsHClass->SetExtensible(false);

    JSTaggedValue attrs = newJsHClass->GetLayout(thread);
    {
        JSMutableHandle<LayoutInfo> layoutInfoHandle(thread, attrs);
        layoutInfoHandle.Update(factory->CopyLayoutInfo(layoutInfoHandle).GetTaggedValue());
        newJsHClass->SetLayout(thread, layoutInfoHandle);
    }

    // 3. Add newClass to old hclass's parent's transitions.
    AddExtensionTransitions(thread, jshclass, newJsHClass, key);
    // parent is the same as jshclass, already copy
    return newJsHClass;
}

JSHandle<JSHClass> JSHClass::TransitionProto(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                             const JSHandle<JSTaggedValue> &proto, bool isChangeProto)
{
    JSHandle<JSTaggedValue> key(thread->GlobalConstants()->GetHandledPrototypeString());

    {
        auto *newClass = jshclass->FindProtoTransitions(thread, key.GetTaggedValue(), proto.GetTaggedValue());
        if (newClass != nullptr) {
            return JSHandle<JSHClass>(thread, newClass);
        }
        newClass = FindTransitionProtoForAOT(thread, jshclass, proto);
        if (newClass != nullptr) {
            return JSHandle<JSHClass>(thread, newClass);
        }
    }

    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    // 2. new a hclass
    JSHandle<JSHClass> newJsHClass = JSHClass::Clone(thread, jshclass);
    newJsHClass->SetPrototype(thread, proto.GetTaggedValue(), isChangeProto);

    JSTaggedValue layout = newJsHClass->GetLayout(thread);
    {
        JSMutableHandle<LayoutInfo> layoutInfoHandle(thread, layout);
        layoutInfoHandle.Update(factory->CopyLayoutInfo(layoutInfoHandle).GetTaggedValue());
        newJsHClass->SetLayout(thread, layoutInfoHandle);
    }

    // 3. Add newJsHClass to old jshclass's parent's transitions.
    AddProtoTransitions(thread, jshclass, newJsHClass, key, proto);

    // parent is the same as jshclass, already copy
    return newJsHClass;
}

// static
JSHClass *JSHClass::FindTransitionProtoForAOT(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                              const JSHandle<JSTaggedValue> &proto)
{
    if (!proto->IsECMAObject()) {
        return nullptr;
    }
    JSHandle<JSHClass> baseIhc(thread, proto->GetTaggedObject()->GetClass());
    if (!jshclass->IsAOT() || !baseIhc->IsAOT()) {
        return nullptr;
    }
    auto transitionTable = thread->GetEcmaVM()->GetFunctionProtoTransitionTable();
    auto transHc = transitionTable->FindTransitionByHClass(thread,
                                                           JSHandle<JSTaggedValue>(jshclass),
                                                           JSHandle<JSTaggedValue>(baseIhc));
    JSHandle<JSTaggedValue> transIhc(thread, transHc.first);
    JSHandle<JSTaggedValue> transPhc(thread, transHc.second);
    if (transIhc->IsUndefined() || transPhc->IsUndefined()) {
        return nullptr;
    }
    ReBuildFunctionInheritanceRelationship(thread, proto, JSHandle<JSTaggedValue>(baseIhc), transIhc, transPhc);
    return JSHClass::Cast(transIhc->GetTaggedObject());
}

// static
void JSHClass::ReBuildFunctionInheritanceRelationship(const JSThread *thread,
                                                      const JSHandle<JSTaggedValue> &proto,
                                                      const JSHandle<JSTaggedValue> &baseIhc,
                                                      const JSHandle<JSTaggedValue> &transIhc,
                                                      const JSHandle<JSTaggedValue> &transPhc)
{
    JSHandle<JSHClass>::Cast(transIhc)->SetProto(thread, proto.GetTaggedValue());
    if (baseIhc.GetTaggedType() == transPhc.GetTaggedType()) {
        return;
    }
    // use transPhc to replace the hclass of proto
    JSHandle<JSHClass> oldPhc(thread, proto->GetTaggedObject()->GetClass());
    proto->GetTaggedObject()->SynchronizedTransitionClass(thread, JSHClass::Cast(transPhc->GetTaggedObject()));
    ASSERT(JSHClass::Cast(transPhc->GetTaggedObject())->IsPrototype());
    // update the prototype of new phc
    JSHClass::Cast(transPhc->GetTaggedObject())->SetPrototype(thread, oldPhc->GetPrototype(thread));
    // enable prototype change marker
    JSTaggedValue phcPrototype = JSHClass::Cast(transPhc->GetTaggedObject())->GetPrototype(thread);
    JSHandle<JSTaggedValue> parentPrototype(thread, phcPrototype);
    ASSERT(parentPrototype->IsECMAObject());
    JSHClass::EnablePHCProtoChangeMarker(thread,
        JSHandle<JSHClass>(thread, parentPrototype->GetTaggedObject()->GetClass()));
    JSHClass::EnableProtoChangeMarker(thread, JSHandle<JSHClass>(transIhc));
}

bool JSHClass::ProtoIsFastJSArray(const JSHandle<GlobalEnv> &env,
    const JSHandle<JSTaggedValue> proto, const JSHandle<JSHClass> hclass)
{
    // Since we currently only support ElementsKind for JSArray initial hclass,
    // if an object's hclass has a non-generic ElementsKind, it must be one of the JSArray initial hclass.
    // if an object's hclass has a Generic ElementsKind, it might be the JSArray initial generic elementskind hclass,
    // which therefore needs further hclass comparison.
    if (proto->IsJSArray()) {
        JSTaggedValue genericArrayHClass = env->GetTaggedElementHOLE_TAGGEDClass();
        if (!Elements::IsGeneric(hclass->GetElementsKind()) || hclass.GetTaggedValue() == genericArrayHClass) {
            return true;
        }
    }
    return false;
}

JSHandle<JSHClass> JSHClass::CloneWithAddProto(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                               const JSHandle<JSTaggedValue> &key,
                                               const JSHandle<JSTaggedValue> &proto)
{
    // 1. new a hclass
    JSHandle<JSHClass> newJsHClass = JSHClass::Clone(thread, jshclass);
    newJsHClass->SetPrototype(thread, proto.GetTaggedValue());

    // 2. Add newJsHClass to old jshclass's parent's transitions.
    AddProtoTransitions(thread, jshclass, newJsHClass, key, proto);
    // parent is the same as jshclass, already copy
    return newJsHClass;
}

JSHandle<JSHClass> JSHClass::TransProtoWithoutLayout(const JSThread *thread, const JSHandle<JSHClass> &jshclass,
                                                     const JSHandle<JSTaggedValue> &proto)
{
    JSHandle<JSTaggedValue> key(thread->GlobalConstants()->GetHandledPrototypeString());

    {
        auto *newClass = jshclass->FindProtoTransitions(thread, key.GetTaggedValue(), proto.GetTaggedValue());
        if (newClass != nullptr) {
            return JSHandle<JSHClass>(thread, newClass);
        }
    }

    return CloneWithAddProto(thread, jshclass, key, proto);
}

void JSHClass::SetPrototype(const JSThread *thread, JSTaggedValue proto, bool isChangeProto)
{
    // Because the heap-space of hclass is non-movable, this function can be non-static.
    JSHandle<JSTaggedValue> protoHandle(thread, proto);
    SetPrototype(thread, protoHandle, isChangeProto);
}

void JSHClass::SetPrototype(const JSThread *thread, const JSHandle<GlobalEnv> &env,
    JSTaggedValue proto, bool isChangeProto)
{
    JSHandle<JSTaggedValue> protoHandle(thread, proto);
    if (protoHandle.GetTaggedValue().IsJSObject()) {
        OptimizePrototypeForIC(thread, env, protoHandle, isChangeProto);
    }
    SetProto(thread, protoHandle);
}

JSHandle<JSHClass> JSHClass::SetPrototypeWithNotification(const JSThread *thread,
                                                          const JSHandle<JSHClass> &hclass,
                                                          const JSHandle<JSTaggedValue> &proto,
                                                          bool isChangeProto)
{
    // `hclass` can become prototype inside `TransitionProto` if `hclass` is HClass of `proto`.
    // In this case we don't need to notify
    auto wasPrototype = hclass->IsPrototype();
    JSHandle<JSHClass> newClass = JSHClass::TransitionProto(thread, hclass, proto, isChangeProto);
    if (wasPrototype) {
        ASSERT(hclass->IsPrototype());
        JSHClass::NotifyHclassChanged(thread, hclass, newClass);
    }
    return newClass;
}

void JSHClass::SetPrototypeTransition(JSThread *thread, const JSHandle<JSObject> &object,
                                      const JSHandle<JSTaggedValue> &proto, bool isChangeProto)
{
    JSHandle<JSHClass> hclass(thread, object->GetJSHClass());
    auto newClass = SetPrototypeWithNotification(thread, hclass, proto, isChangeProto);
    RestoreElementsKindToGeneric(*newClass);
    object->SynchronizedTransitionClass(thread, *newClass);
    if (object->IsJSArray()) {
        thread->GetEcmaVM()->GetGlobalEnv()->NotifyArrayPrototypeChangedGuardians(thread, object);
        newClass->SetIsJSArrayPrototypeModified(true);
    }
    ObjectOperator::UpdateDetectorOnSetPrototype(thread, object.GetTaggedValue());
}

void JSHClass::SetPrototype(const JSThread *thread, const JSHandle<JSTaggedValue> &proto, bool isChangeProto)
{
    // Because the heap-space of hclass is non-movable, this function can be non-static.
    if (proto->IsJSObject()) {
        OptimizePrototypeForIC(thread, thread->GetGlobalEnv(), proto, isChangeProto);
    }
    SetProto(thread, proto);
}

void JSHClass::OptimizePrototypeForIC(const JSThread *thread, const JSHandle<GlobalEnv> &env,
    const JSHandle<JSTaggedValue> &proto, bool isChangeProto)
{
    JSHandle<JSHClass> hclass(thread, proto->GetTaggedObject()->GetClass());
    ASSERT(
        g_isEnableCMCGC || !Region::ObjectAddressToRange(reinterpret_cast<TaggedObject *>(*hclass))->InReadOnlySpace());
    if (!hclass->IsPrototype()) {
        // Situations for clone proto hclass:
        // 1: unshared non-ts hclass
        // 2: no matter whether hclass is ts or not when set function prototype
        if ((!hclass->IsAOT() && !hclass->IsJSShared()) || isChangeProto) {
            // The local IC and on-proto IC are different, because the former don't need to notify the whole
            // prototype-chain or listen the changes of prototype chain, but the latter do. Therefore, when
            // an object becomes a prototype object at the first time, we need to copy its hidden class in
            // order to maintain the previously generated local IC and support the on-proto IC in the future.
            // For example, a local IC adds a new property x for o1 and the o1.hclass1 -> o1.hclass2, when the
            // o1 becomes a prototype object of object o2 and an on-proto IC loading x from o2 will rely on the
            // stability of the prototype-chain o2 -> o1. If directly marking the o1.hclass1 as a prototype hclass,
            // the previous IC of adding property x won't trigger IC-miss and fails to notify the IC on o2.

            // At here, When a JSArray with initial hclass is set as a proto,
            // we substitute its hclass with preserved proto hclass.
            JSHandle<JSHClass> newProtoClass;
            if (ProtoIsFastJSArray(env, proto, hclass)) {
                newProtoClass = JSHandle<JSHClass>(thread, thread->GetArrayInstanceHClass(hclass->GetElementsKind(),
                                                                                          true));
            } else {
                newProtoClass = JSHClass::Clone(thread, hclass);
            }
            JSTaggedValue layout = newProtoClass->GetLayout(thread);
            // If the type of object is JSObject, the layout info value is initialized to the default value,
            // if the value is not JSObject, the layout info value is initialized to null.
            if (!layout.IsNull()) {
                JSMutableHandle<LayoutInfo> layoutInfoHandle(thread, layout);
                layoutInfoHandle.Update(
                    thread->GetEcmaVM()->GetFactory()->CopyLayoutInfo(layoutInfoHandle).GetTaggedValue());
                newProtoClass->SetLayout(thread, layoutInfoHandle);
            }

#if ECMASCRIPT_ENABLE_IC
            // After the hclass is updated, check whether the proto chain status of ic is updated.
            NotifyHclassChanged(thread, hclass, newProtoClass);
#endif
            JSObject::Cast(proto->GetTaggedObject())->SynchronizedTransitionClass(thread, *newProtoClass);
            NotifyHClassNotPrototypeChanged(const_cast<JSThread *>(thread), newProtoClass);
            // still dump for class in this path now
            if (!isChangeProto) {
                thread->GetEcmaVM()->GetPGOProfiler()->UpdateRootProfileTypeSafe(*hclass, *newProtoClass);
            }
        } else {
            // There is no sharing in AOT hclass. Therefore, it is not necessary or possible to clone here.
            NotifyHClassNotPrototypeChanged(const_cast<JSThread *>(thread), hclass);
        }
    }
}

void JSHClass::TransitionToDictionary(const JSThread *thread, const JSHandle<JSObject> &obj)
{
    // 1. new a hclass
    JSHandle<JSHClass> jshclass(thread, obj->GetJSHClass());
    JSHandle<JSHClass> newJsHClass = CloneWithoutInlinedProperties(thread, jshclass);

    {
        // 2. Copy
        newJsHClass->SetNumberOfProps(0);
        newJsHClass->SetIsDictionaryMode(true);
        ASSERT(newJsHClass->GetInlinedProperties() == 0);

        // 3. Add newJsHClass to ?
#if ECMASCRIPT_ENABLE_IC
        JSHClass::NotifyHclassChanged(thread, JSHandle<JSHClass>(thread, obj->GetJSHClass()), newJsHClass);
#endif
        DISALLOW_GARBAGE_COLLECTION;
        RestoreElementsKindToGeneric(*newJsHClass);
        obj->SynchronizedTransitionClass(thread, *newJsHClass);
    }
}

void JSHClass::OptimizeAsFastProperties(const JSThread *thread, const JSHandle<JSObject> &obj,
                                        const std::vector<int> &indexOrder, bool isDictionary)
{
    // 1. new a hclass
    JSHandle<JSHClass> jshclass(thread, obj->GetJSHClass());
    JSHandle<JSHClass> newJsHClass = Clone(thread, jshclass, isDictionary);

    // 2. If it is dictionary, migrate should change layout. otherwise, copy the hclass only.
    if (isDictionary) {
        JSHandle<NameDictionary> properties(thread, obj->GetProperties(thread));
        int numberOfProperties = properties->EntriesCount();
        ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
        JSHandle<LayoutInfo> layoutInfoHandle = factory->CreateLayoutInfo(numberOfProperties);
        int numberOfInlinedProps = static_cast<int>(newJsHClass->GetInlinedProperties());
        for (int i = 0; i < numberOfProperties; i++) {
            JSTaggedValue key = properties->GetKey(thread, indexOrder[i]);
            PropertyAttributes attributes = properties->GetAttributes(thread, indexOrder[i]);
            if (i < numberOfInlinedProps) {
                attributes.SetIsInlinedProps(true);
            } else {
                attributes.SetIsInlinedProps(false);
            }
            attributes.SetOffset(i);
            layoutInfoHandle->AddKey(thread, i, key, attributes);
        }

        {
            DISALLOW_GARBAGE_COLLECTION;
            newJsHClass->SetNumberOfProps(numberOfProperties);
            newJsHClass->SetLayout(thread, layoutInfoHandle);
        }
    }

    {
        DISALLOW_GARBAGE_COLLECTION;
        // 3. Copy
        newJsHClass->SetIsDictionaryMode(false);
        // 4. Add newJsHClass to ?
#if ECMASCRIPT_ENABLE_IC
        JSHClass::NotifyHclassChanged(thread, JSHandle<JSHClass>(thread, obj->GetJSHClass()), newJsHClass);
#endif
        obj->SynchronizedTransitionClass(thread, *newJsHClass);
    }
}

void JSHClass::TransitionForRepChange(const JSThread *thread, const JSHandle<JSObject> &receiver,
    const JSHandle<JSTaggedValue> &key, PropertyAttributes attr)
{
    JSHandle<JSHClass> oldHClass(thread, receiver->GetJSHClass());

    // 1. Create hclass and copy layout
    JSHandle<JSHClass> newHClass = JSHClass::Clone(thread, oldHClass);
    RestoreElementsKindToGeneric(*newHClass);

    JSHandle<LayoutInfo> oldLayout(thread, newHClass->GetLayout(thread));
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<LayoutInfo> newLayout(factory->CopyLayoutInfo(oldLayout));
    newHClass->SetLayout(thread, newLayout);

    // 2. update attr
    auto hclass = JSHClass::Cast(newHClass.GetTaggedValue().GetTaggedObject());
    int entry = JSHClass::FindPropertyEntry(thread, hclass, key.GetTaggedValue());
    ASSERT(entry != -1);
    newLayout->SetNormalAttr(thread, entry, attr);

    // 3. update hclass in object.
#if ECMASCRIPT_ENABLE_IC
    JSHClass::NotifyHclassChanged(thread, oldHClass, newHClass, key.GetTaggedValue());
#endif

    receiver->SynchronizedTransitionClass(thread, *newHClass);
    // 4. Maybe Transition And Maintain subtypeing check
}

bool JSHClass::IsInitialArrayHClassWithElementsKind(const JSThread *thread, const JSHClass *targetHClass,
                                                    const ElementsKind targetKind)
{
    JSHClass *hclass = thread->GetArrayInstanceHClass(targetKind, false);
    JSHClass *hclassWithProto = thread->GetArrayInstanceHClass(targetKind, true);
    return targetHClass == hclass || targetHClass == hclassWithProto;
}

bool JSHClass::TransitToElementsKindUncheck(const JSThread *thread, const JSHandle<JSObject> &obj,
                                            ElementsKind newKind)
{
    ElementsKind current = obj->GetJSHClass()->GetElementsKind();
    // currently we only support initial array hclass
    JSHClass *objHclass = obj->GetClass();
    if (IsInitialArrayHClassWithElementsKind(thread, objHclass, current)) {
        JSHClass *hclass = thread->GetArrayInstanceHClass(newKind, objHclass->IsPrototype());
        obj->SynchronizedTransitionClass(thread, hclass);
#if ECMASCRIPT_ENABLE_IC
        JSHClass::NotifyHclassChanged(thread, JSHandle<JSHClass>(thread, objHclass),
                                      JSHandle<JSHClass>(thread, hclass));
#endif
        return true;
    }
    return false;
}

void JSHClass::TransitToElementsKind(const JSThread *thread, const JSHandle<JSArray> &array,
                                     ElementsKind newKind)
{
    JSTaggedValue elements = array->GetElements(thread);
    if (!elements.IsTaggedArray()) {
        return;
    }
    ElementsKind current = array->GetJSHClass()->GetElementsKind();
    newKind = Elements::MergeElementsKind(newKind, current);
    if (newKind == current) {
        return;
    }

    ASSERT(IsInitialArrayHClassWithElementsKind(thread, array->GetJSHClass(), current));
    TransitToElementsKindUncheck(thread, JSHandle<JSObject>(array), newKind);
}

bool JSHClass::TransitToElementsKind(const JSThread *thread, const JSHandle<JSObject> &object,
                                     const JSHandle<JSTaggedValue> &value, ElementsKind kind)
{
    if (!object->IsJSArray()) {
        return false;
    }
    ElementsKind current = object->GetJSHClass()->GetElementsKind();
    if (Elements::IsGeneric(current)) {
        return false;
    }

    // Merge current kind and new kind
    ElementsKind newKind = Elements::MergeElementsKindNoFix(value.GetTaggedValue(), current, kind);
    if (newKind == current) {
        return false;
    }

    // Currently, we only support fast array elementsKind
    ASSERT(IsInitialArrayHClassWithElementsKind(thread, object->GetJSHClass(), current));
    if (!TransitToElementsKindUncheck(thread, object, newKind)) {
        return false;
    }

    if (thread->IsEnableElementsKind() || thread->IsPGOProfilerEnable()) {
        // Update TrackInfo
        JSHandle<JSArray>(object)->UpdateTrackInfo(thread);
    }

    if (!thread->IsPGOProfilerEnable()) {
        return true;
    }
    JSTaggedValue trackInfoVal = JSHandle<JSArray>(object)->GetTrackInfo(thread);
    if (trackInfoVal.IsHeapObject() && trackInfoVal.IsWeak()) {
        TrackInfo *trackInfo = TrackInfo::Cast(trackInfoVal.GetWeakReferentUnChecked());
        thread->GetEcmaVM()->GetPGOProfiler()->UpdateTrackInfo(JSTaggedValue(trackInfo));
    }
    return true;
}

void JSHClass::UpdateFieldType(const JSThread *thread, JSHClass *hclass, const PropertyAttributes &attr)
{
    DISALLOW_GARBAGE_COLLECTION;
    JSHClass *ownHClass = FindFieldOwnHClass(thread, hclass, attr);
    VisitAndUpdateLayout(thread, ownHClass, attr);
}

JSHClass *JSHClass::FindFieldOwnHClass(const JSThread *thread, JSHClass *hclass, const PropertyAttributes &attr)
{
    uint32_t offset = attr.GetOffset();
    JSTaggedValue parent(hclass);
    JSHClass *curHClass = hclass;
    while (parent.IsJSHClass()) {
        auto parentHClass = JSHClass::Cast(parent.GetTaggedObject());
        if (parentHClass->NumberOfProps() <= offset) {
            break;
        }
        curHClass = parentHClass;
        parent = curHClass->GetParent(thread);
    }
    return curHClass;
}

void JSHClass::VisitAndUpdateLayout(const JSThread *thread, JSHClass *ownHClass, const PropertyAttributes &attr)
{
    uint32_t offset = attr.GetOffset();
    auto targetTrackType = attr.GetTrackType();
    std::queue<JSHClass *> backHClass;
    backHClass.push(ownHClass);
    while (!backHClass.empty()) {
        JSHClass *current = backHClass.front();
        backHClass.pop();

        auto layout = LayoutInfo::Cast(current->GetLayout(thread).GetTaggedObject());
        if (layout->GetAttr(thread, offset).GetTrackType() != targetTrackType) {
            layout->UpdateTrackTypeAttr(thread, offset, attr);
        }

        auto transitions = current->GetTransitions(thread);
        if (transitions.IsUndefined()) {
            continue;
        }
        if (transitions.IsWeak()) {
            auto cache = transitions.GetTaggedWeakRef();
            backHClass.push(JSHClass::Cast(cache));
            continue;
        }

        ASSERT(transitions.IsTaggedArray());
        TransitionsDictionary *dict = TransitionsDictionary::Cast(transitions.GetTaggedObject());
        dict->IterateEntryValue(thread, [&backHClass](JSHClass *cache) { backHClass.push(JSHClass::Cast(cache)); });
    }
}

void JSHClass::VisitTransitionAndUpdateObjSize(const JSThread *thread, JSHClass *ownHClass, uint32_t finalInObjPropsNum)
{
    uint32_t size = ownHClass->GetInlinedPropsStartSize();
    uint32_t objectSize = size + finalInObjPropsNum * JSTaggedValue::TaggedTypeSize();
    std::queue<JSHClass *> backHClass;
    backHClass.push(ownHClass);
    while (!backHClass.empty()) {
        JSHClass *current = backHClass.front();
        backHClass.pop();
        current->SetObjectSize(objectSize);
        auto transitions = current->GetTransitions(thread);
        if (transitions.IsUndefined()) {
            continue;
        }
        if (transitions.IsWeak()) {
            auto cache = transitions.GetTaggedWeakRef();
            backHClass.push(JSHClass::Cast(cache));
            continue;
        }

        ASSERT(transitions.IsTaggedArray());
        TransitionsDictionary *dict = TransitionsDictionary::Cast(transitions.GetTaggedObject());
        dict->IterateEntryValue(thread, [&backHClass](JSHClass *cache) { backHClass.push(JSHClass::Cast(cache)); });
    }
}

uint32_t JSHClass::VisitTransitionAndFindMaxNumOfProps(const JSThread *thread, JSHClass *ownHClass)
{
    std::queue<JSHClass *> backHClass;
    backHClass.push(ownHClass);
    uint32_t maxNumOfProps = 0;
    while (!backHClass.empty()) {
        JSHClass *current = backHClass.front();
        uint32_t numOfProps = current->NumberOfProps();
        if (numOfProps > maxNumOfProps) {
            maxNumOfProps = numOfProps;
        }
        backHClass.pop();

        auto transitions = current->GetTransitions(thread);
        if (transitions.IsUndefined()) {
            continue;
        }
        if (transitions.IsWeak()) {
            auto cache = transitions.GetTaggedWeakRef();
            backHClass.push(JSHClass::Cast(cache));
            continue;
        }

        ASSERT(transitions.IsTaggedArray());
        TransitionsDictionary *dict = TransitionsDictionary::Cast(transitions.GetTaggedObject());
        dict->IterateEntryValue(thread, [&backHClass](JSHClass *cache) { backHClass.push(JSHClass::Cast(cache)); });
    }
    return maxNumOfProps;
}

TransitionResult JSHClass::ConvertOrTransitionWithRep(const JSThread *thread,
    const JSHandle<JSObject> &receiver, const JSHandle<JSTaggedValue> &key, const JSHandle<JSTaggedValue> &value,
    PropertyAttributes &attr)
{
    auto hclass = receiver->GetJSHClass();
    auto layout = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    attr = layout->GetAttr(thread, attr.GetOffset());
    if (thread->IsPGOProfilerEnable() && !hclass->IsJSShared() && attr.UpdateTrackType(value.GetTaggedValue())) {
        UpdateFieldType(thread, hclass, attr);
    }

    Representation oldRep = attr.GetRepresentation();
    if (oldRep == Representation::DOUBLE) {
        if (value->IsInt()) {
            double doubleValue = value->GetInt();
            return {false, false, JSTaggedValue(bit_cast<JSTaggedType>(doubleValue))};
        } else if (value->IsObject()) {
            // Is Object
            attr.SetRepresentation(Representation::TAGGED);
            // Transition
            JSHClass::TransitionForRepChange(thread, receiver, key, attr);
            return {true, true, value.GetTaggedValue()};
        } else {
            // Is TaggedDouble
            return {false, false, JSTaggedValue(bit_cast<JSTaggedType>(value->GetDouble()))};
        }
    } else if (oldRep == Representation::INT) {
        if (value->IsInt()) {
            int intValue = value->GetInt();
            return {false, false, JSTaggedValue(static_cast<JSTaggedType>(intValue))};
        } else {
            attr.SetRepresentation(Representation::TAGGED);
            JSHClass::TransitionForRepChange(thread, receiver, key, attr);
            return {true, true, value.GetTaggedValue()};
        }
    }
    return {true, false, value.GetTaggedValue()};
}

void JSHClass::MergeRepresentation(const JSThread *thread, JSHClass *oldJsHClass, JSHClass *newJsHClass)
{
    JSHandle<LayoutInfo> oldLayout(thread, oldJsHClass->GetLayout(thread));
    JSHandle<LayoutInfo> newLayout(thread, newJsHClass->GetLayout(thread));
    int numberOfProps = static_cast<int>(oldJsHClass->NumberOfProps());
    for (int i = 0; i < numberOfProps; i++) {
        PropertyAttributes oldAttr = oldLayout->GetAttr(thread, i);
        PropertyAttributes newAttr = newLayout->GetAttr(thread, i);
        ASSERT(oldAttr.IsInlinedProps());
        if (oldAttr.GetRepresentation() == newAttr.GetRepresentation()) {
            continue;
        }
        oldAttr.SetRepresentation(newAttr.GetRepresentation());
        oldLayout->SetNormalAttr(thread, i, oldAttr);
    }
}

void JSHClass::NotifyHClassNotPrototypeChanged(JSThread *thread, const JSHandle<JSHClass> &jsHClass)
{
    if (jsHClass->IsPrototype()) {
        return;
    }
    jsHClass->SetIsPrototype(true);
    JSTaggedValue infos = jsHClass->GetDependentInfos(thread);
    if (!infos.IsHeapObject()) {
        return;
    }
    JSHandle<DependentInfos> infosHandle(thread, infos);
    DependentInfos::TriggerLazyDeoptimization(
        infosHandle, thread, DependentInfos::DependentState::IS_PROTOTYPE_CHECK);
}

void JSHClass::NotifyLeafHClassChanged(JSThread *thread, const JSHandle<JSHClass> &jsHClass)
{
    if (!jsHClass->IsStable()) {
        return;
    }
    jsHClass->SetIsStable(false);
    JSTaggedValue infos = jsHClass->GetDependentInfos(thread);
    if (!infos.IsHeapObject()) {
        return;
    }
    JSHandle<DependentInfos> infosHandle(thread, infos);
    DependentInfos::TriggerLazyDeoptimization(
        infosHandle, thread, DependentInfos::DependentState::PROTOTYPE_CHECK);
}

JSHandle<JSTaggedValue> JSHClass::EnableProtoChangeMarker(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    JSTaggedValue proto = jshclass->GetPrototype(thread);
    if (!proto.IsECMAObject()) {
        // Return JSTaggedValue directly. No proto check is needed.
        LOG_ECMA(INFO) << "proto is not ecmaobject: " << proto.GetRawData();
        return JSHandle<JSTaggedValue>(thread, JSTaggedValue::Null());
    }
    JSHandle<JSObject> protoHandle(thread, proto);
    JSHandle<JSHClass> protoClass(thread, protoHandle->GetJSHClass());
    // in AOT's IC mechanism (VTable), when the prototype chain changes, it needs to notify each subclass
    // PHC (prototype-HClass) and its IHC (instance-HClass) from the current PHC along the chain.
    // therefore, when registering, it is also necessary to register IHC into its
    // PHC's Listener to ensure that it can be notified.
    RegisterOnProtoChain(thread, protoClass);

    JSTaggedValue protoChangeMarker = protoClass->GetProtoChangeMarker(thread);
    if (protoChangeMarker.IsProtoChangeMarker()) {
        JSHandle<ProtoChangeMarker> markerHandle(thread, ProtoChangeMarker::Cast(protoChangeMarker.GetTaggedObject()));
        if (!markerHandle->GetHasChanged()) {
            return JSHandle<JSTaggedValue>(markerHandle);
        }
    }
    JSHandle<ProtoChangeMarker> markerHandle = thread->GetEcmaVM()->GetFactory()->NewProtoChangeMarker();
    markerHandle->SetHasChanged(false);
    markerHandle->SetNotFoundHasChanged(false);
    // ShareToLocal is prohibited
    if (!protoClass->IsJSShared()) {
        protoClass->SetProtoChangeMarker(thread, markerHandle.GetTaggedValue());
    }
    return JSHandle<JSTaggedValue>(markerHandle);
}

JSHandle<JSTaggedValue> JSHClass::EnablePHCProtoChangeMarker(const JSThread *thread,
    const JSHandle<JSHClass> &protoClass)
{
    RegisterOnProtoChain(thread, protoClass);

    JSTaggedValue protoChangeMarker = protoClass->GetProtoChangeMarker(thread);
    if (protoChangeMarker.IsProtoChangeMarker()) {
        JSHandle<ProtoChangeMarker> markerHandle(thread, ProtoChangeMarker::Cast(protoChangeMarker.GetTaggedObject()));
        if (!markerHandle->GetHasChanged()) {
            return JSHandle<JSTaggedValue>(markerHandle);
        }
    }
    JSHandle<ProtoChangeMarker> markerHandle = thread->GetEcmaVM()->GetFactory()->NewProtoChangeMarker();
    markerHandle->SetHasChanged(false);
    markerHandle->SetNotFoundHasChanged(false);
    protoClass->SetProtoChangeMarker(thread, markerHandle.GetTaggedValue());
    return JSHandle<JSTaggedValue>(markerHandle);
}

void JSHClass::NotifyHClassChangedForAot(const JSThread *thread, JSHandle<JSHClass> oldHclass,
                                         JSHandle<JSHClass> newHclass, JSTaggedValue addedKey)
{
    if (!oldHclass->IsPrototype()) {
        return;
    }
    // The old hclass is the same as new one
    if (oldHclass.GetTaggedValue() == newHclass.GetTaggedValue()) {
        return;
    }
    JSHClass::NoticeThroughChain<true>(thread, oldHclass, addedKey);
}

void JSHClass::NotifyHclassChanged(const JSThread *thread, JSHandle<JSHClass> oldHclass, JSHandle<JSHClass> newHclass,
                                   JSTaggedValue addedKey)
{
    if (!oldHclass->IsPrototype()) {
        return;
    }
    // The old hclass is the same as new one
    if (oldHclass.GetTaggedValue() == newHclass.GetTaggedValue()) {
        return;
    }
    // For now, at pgo profiling stage, we use ProfileType::Kind to mark a hclass is CHC, PHC or IHC.
    // We can have case like the following:
    //
    //     class C3 {
    //         constructor(a5) {
    //         }
    //     }
    //     class C18 extends C3 {
    //         constructor() {
    //             super(1);
    //             C3.valueOf = 1;
    //         }
    //     }
    //     const v37 = new C18();
    //
    // C3 is profiled as CHC even though its IsPrototype bit is marked as true when 'class C18 extends C3' is executed.
    // Since C3 is marked as CHC and it has ProfileType::Kind::ConstructorId,
    // when generating hclass at aot, its child hclass and itself will not have IsPrototype bit set as true.
    //
    // However, we currently support hclass substitution when executing 'C3.valueOf' for C3's oldHclass at runtime.
    // Therefore, oldHclass's IsPrototype bit is set as true; But for newHclass, it is generated at aot stage,
    // it will not have IsPrototype bit set as true.
    //
    // Good neww is our AOT hclass can not be shared, hence we can set newHclass IsPrototype as true at here.
    if (newHclass->IsAOT() && !newHclass->IsPrototype()) {
        NotifyHClassNotPrototypeChanged(const_cast<JSThread *>(thread), newHclass);
    }
    NotifyLeafHClassChanged(const_cast<JSThread *>(thread), oldHclass);
    JSHClass::NoticeThroughChain<false>(thread, oldHclass, addedKey);
    JSHClass::RefreshUsers(thread, oldHclass, newHclass);
}

void JSHClass::NotifyAccessorChanged(const JSThread *thread, JSHandle<JSHClass> hclass)
{
    DISALLOW_GARBAGE_COLLECTION;
    JSTaggedValue markerValue = hclass->GetProtoChangeMarker(thread);
    if (markerValue.IsProtoChangeMarker()) {
        ProtoChangeMarker *protoChangeMarker = ProtoChangeMarker::Cast(markerValue.GetTaggedObject());
        protoChangeMarker->SetAccessorHasChanged(true);
    }

    JSTaggedValue protoDetailsValue = hclass->GetProtoChangeDetails(thread);
    if (!protoDetailsValue.IsProtoChangeDetails()) {
        return;
    }
    JSTaggedValue listenersValue =
        ProtoChangeDetails::Cast(protoDetailsValue.GetTaggedObject())->GetChangeListener(thread);
    if (!listenersValue.IsTaggedArray()) {
        return;
    }
    ChangeListener *listeners = ChangeListener::Cast(listenersValue.GetTaggedObject());
    JSMutableHandle<JSHClass> hclassTemp(thread, JSTaggedValue::Undefined());
    for (uint32_t i = 0; i < listeners->GetEnd(); i++) {
        JSTaggedValue temp = listeners->Get(thread, i);
        if (temp.IsJSHClass()) {
            hclassTemp.Update(listeners->Get(thread, i));
            NotifyAccessorChanged(thread, hclassTemp);
        }
    }
}

void JSHClass::RegisterOnProtoChain(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    // ShareToLocal is prohibited
    if (jshclass->IsJSShared()) {
        return;
    }
    JSHandle<JSHClass> user = jshclass;
    JSHandle<ProtoChangeDetails> userDetails = GetProtoChangeDetails(thread, user);

    while (true) {
        // Find the prototype chain as far as the hclass has not been registered.
        if (userDetails->GetRegisterIndex() != static_cast<uint32_t>(ProtoChangeDetails::UNREGISTERED)) {
            return;
        }

        JSTaggedValue proto = user->GetPrototype(thread);
        if (!proto.IsHeapObject()) {
            return;
        }
        if (proto.IsJSProxy()) {
            return;
        }
        // ShareToLocal is prohibited
        if (proto.IsJSShared()) {
            return;
        }
        ASSERT(proto.IsECMAObject());
        JSHandle<JSObject> protoHandle(thread, proto);
        JSHandle<ProtoChangeDetails> protoDetails =
            GetProtoChangeDetails(thread, JSHandle<JSHClass>(thread, protoHandle->GetJSHClass()));
        JSTaggedValue listeners = protoDetails->GetChangeListener(thread);
        JSHandle<ChangeListener> listenersHandle;
        if (listeners.IsUndefined()) {
            listenersHandle = JSHandle<ChangeListener>(ChangeListener::Create(thread));
        } else {
            listenersHandle = JSHandle<ChangeListener>(thread, listeners);
        }
        uint32_t registerIndex = 0;
        JSHandle<ChangeListener> newListeners = ChangeListener::Add(thread, listenersHandle, user, &registerIndex);
        userDetails->SetRegisterIndex(registerIndex);
        protoDetails->SetChangeListener(thread, newListeners.GetTaggedValue());
        userDetails = protoDetails;
        user = JSHandle<JSHClass>(thread, protoHandle->GetJSHClass());
    }
}

bool JSHClass::UnregisterOnProtoChain(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    ASSERT(jshclass->IsPrototype());
    if (!jshclass->GetProtoChangeDetails(thread).IsProtoChangeDetails()) {
        return false;
    }
    if (!jshclass->GetPrototype(thread).IsECMAObject()) {
        JSTaggedValue listeners = ProtoChangeDetails::Cast(jshclass->GetProtoChangeDetails(thread).GetTaggedObject())
                                      ->GetChangeListener(thread);
        return !listeners.IsUndefined();
    }
    JSHandle<ProtoChangeDetails> currentDetails = GetProtoChangeDetails(thread, jshclass);
    uint32_t index = currentDetails->GetRegisterIndex();
    if (index == static_cast<uint32_t>(ProtoChangeDetails::UNREGISTERED)) {
        return false;
    }
    JSTaggedValue proto = jshclass->GetPrototype(thread);
    ASSERT(proto.IsECMAObject());
    JSTaggedValue protoDetailsValue =
        JSObject::Cast(proto.GetTaggedObject())->GetJSHClass()->GetProtoChangeDetails(thread);
    if (protoDetailsValue.IsUndefined() || protoDetailsValue.IsNull()) {
        return false;
    }
    ASSERT(protoDetailsValue.IsProtoChangeDetails());
    JSTaggedValue listenersValue =
        ProtoChangeDetails::Cast(protoDetailsValue.GetTaggedObject())->GetChangeListener(thread);
    ASSERT(!listenersValue.IsUndefined());
    JSHandle<ChangeListener> listeners(thread, listenersValue.GetTaggedObject());
    ASSERT(listeners->Get(thread, index) == jshclass.GetTaggedValue());
    listeners->Delete(thread, index);
    return true;
}

JSHandle<ProtoChangeDetails> JSHClass::GetProtoChangeDetails(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    JSTaggedValue protoDetails = jshclass->GetProtoChangeDetails(thread);
    if (protoDetails.IsProtoChangeDetails()) {
        return JSHandle<ProtoChangeDetails>(thread, protoDetails);
    }
    JSHandle<ProtoChangeDetails> protoDetailsHandle = thread->GetEcmaVM()->GetFactory()->NewProtoChangeDetails();
    jshclass->SetProtoChangeDetails(thread, protoDetailsHandle.GetTaggedValue());
    return protoDetailsHandle;
}

JSHandle<ProtoChangeDetails> JSHClass::GetProtoChangeDetails(const JSThread *thread, const JSHandle<JSObject> &obj)
{
    JSHandle<JSHClass> jshclass(thread, obj->GetJSHClass());
    return GetProtoChangeDetails(thread, jshclass);
}

JSHandle<TaggedArray> JSHClass::GetEnumCacheOwnWithOutCheck(const JSThread *thread, const JSHandle<JSHClass> &jshclass)
{
    return JSHandle<TaggedArray>(thread, EnumCache::Cast(jshclass->GetEnumCache(thread))->GetEnumCacheOwn(thread));
}

void JSHClass::RefreshUsers(const JSThread *thread, const JSHandle<JSHClass> &oldHclass,
                            const JSHandle<JSHClass> &newHclass)
{
    ASSERT(oldHclass->IsPrototype());
    ASSERT(newHclass->IsPrototype());
    bool onceRegistered = UnregisterOnProtoChain(thread, oldHclass);

    newHclass->SetProtoChangeDetails(thread, oldHclass->GetProtoChangeDetails(thread));
    oldHclass->SetProtoChangeDetails(thread, JSTaggedValue::Null());
    if (onceRegistered) {
        if (newHclass->GetProtoChangeDetails(thread).IsProtoChangeDetails()) {
            ProtoChangeDetails::Cast(newHclass->GetProtoChangeDetails(thread).GetTaggedObject())
                ->SetRegisterIndex(ProtoChangeDetails::UNREGISTERED);
        }
        RegisterOnProtoChain(thread, newHclass);
    }
}

PropertyLookupResult JSHClass::LookupPropertyInAotHClass(const JSThread *thread, JSHClass *hclass, JSTaggedValue key)
{
    DISALLOW_GARBAGE_COLLECTION;
    ASSERT(hclass->IsAOT());

    PropertyLookupResult result;
    if (hclass->IsDictionaryMode()) {
        // not fuond
        result.SetIsFound(false);
        return result;
    }

    int entry = JSHClass::FindPropertyEntry(thread, hclass, key);
    // found in local
    if (entry != -1) {
        result.SetIsFound(true);
        result.SetIsLocal(true);
        PropertyAttributes attr = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject())->GetAttr(thread, entry);
        if (attr.IsInlinedProps()) {
            result.SetIsInlinedProps(true);
            result.SetOffset(hclass->GetInlinedPropertiesOffset(entry));
        } else {
            result.SetIsInlinedProps(false);
            result.SetOffset(attr.GetOffset() - hclass->GetInlinedProperties());
        }
        if (attr.IsNotHole()) {
            result.SetIsNotHole(true);
        }
        if (attr.IsAccessor()) {
            result.SetIsAccessor(true);
        }
        result.SetRepresentation(attr.GetRepresentation());
        result.SetIsWritable(attr.IsWritable());
        return result;
    }

    // not found
    result.SetIsFound(false);
    return result;
}

PropertyLookupResult JSHClass::LookupPropertyInPGOHClass(const JSThread *thread, JSHClass *hclass, JSTaggedValue key)
{
    DISALLOW_GARBAGE_COLLECTION;

    PropertyLookupResult result;
    if (hclass == nullptr || hclass->IsDictionaryMode()) {
        result.SetIsFound(false);
        return result;
    }

    int entry = JSHClass::FindPropertyEntry(thread, hclass, key);
    // found in local
    if (entry != -1) {
        result.SetIsFound(true);
        result.SetIsLocal(true);
        PropertyAttributes attr = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject())->GetAttr(thread, entry);
        if (attr.IsInlinedProps()) {
            result.SetIsInlinedProps(true);
            result.SetOffset(hclass->GetInlinedPropertiesOffset(entry));
        } else {
            result.SetIsInlinedProps(false);
            result.SetOffset(attr.GetOffset() - hclass->GetInlinedProperties());
        }

        if (attr.IsNotHole()) {
            result.SetIsNotHole(true);
        }
        if (attr.IsAccessor()) {
            result.SetIsAccessor(true);
        }
        result.SetRepresentation(attr.GetRepresentation());
        result.SetIsWritable(attr.IsWritable());
        return result;
    }

    // not fuond
    result.SetIsFound(false);
    return result;
}

PropertyLookupResult JSHClass::LookupPropertyInBuiltinPrototypeHClass(const JSThread *thread, JSHClass *hclass,
                                                                      JSTaggedValue key)
{
    DISALLOW_GARBAGE_COLLECTION;
    ASSERT(hclass->IsPrototype());

    PropertyLookupResult result;
    if (hclass->IsDictionaryMode()) {
        // not fuond
        result.SetIsFound(false);
        return result;
    }
    int entry = JSHClass::FindPropertyEntry(thread, hclass, key);
    // When the property is not found, the value of 'entry' is -1.
    // Currently, not all methods on the prototype of 'builtin' have been changed to inlined.
    // Therefore, when a non-inlined method is encountered, it is also considered not found.
    if (entry == -1 || static_cast<uint32_t>(entry) >= hclass->GetInlinedProperties()) {
        result.SetIsFound(false);
        return result;
    }

    result.SetIsFound(true);
    result.SetIsLocal(true);
    PropertyAttributes attr = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject())->GetAttr(thread, entry);
    if (attr.IsInlinedProps()) {
        result.SetIsInlinedProps(true);
        result.SetOffset(hclass->GetInlinedPropertiesOffset(entry));
    } else {
        result.SetIsInlinedProps(false);
        result.SetOffset(attr.GetOffset() - hclass->GetInlinedProperties());
    }
    result.SetIsNotHole(true);
    if (attr.IsAccessor()) {
        result.SetIsAccessor(true);
    }
    result.SetRepresentation(attr.GetRepresentation());
    result.SetIsWritable(attr.IsWritable());
    return result;
}

JSHandle<JSTaggedValue> JSHClass::ParseKeyFromPGOCString(ObjectFactory* factory,
                                                         const CString& cstring,
                                                         const PGOHandler& handler)
{
    if (handler.GetIsSymbol()) {
        JSHandle<JSSymbol> symbol;
        auto str = cstring.substr(0, 6); // `method` length is 6
        if (str == "method") { // cstring is `method_0ULL` after _ is private id of symbol
            symbol = factory->NewPublicSymbolWithChar("method");
            ASSERT(cstring.size() > 0);
            str = cstring.substr(7, cstring.size() - 1); // `method_` length is 7
            symbol->SetPrivateId(CStringToULL(str));
        } else { // cstring is private id of symbol
            symbol = factory->NewJSSymbol();
            symbol->SetPrivateId(CStringToULL(cstring));
        }
        return JSHandle<JSTaggedValue>(symbol);
    } else {
        return JSHandle<JSTaggedValue>(factory->NewFromStdString(std::string(cstring)));
    }
}

CString JSHClass::GetJSTypeDesc(JSType type)
{
    static const CString JSTYPE_METADATA[int(JSType::TYPE_LAST) + 1] = {
        "INVALID",
        JSTYPE_DECL(JSTYPE_STRING),
    };
    if (int(type) > int(JSType::TYPE_LAST)) {
        return "";
    }
    return JSTYPE_METADATA[int(type)];
}

JSHandle<JSHClass> JSHClass::CreateRootHClassFromPGO(const JSThread* thread,
                                                     const HClassLayoutDesc* desc,
                                                     uint32_t maxNum)
{
    auto rootDesc = reinterpret_cast<const pgo::RootHClassLayoutDesc *>(desc);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    uint32_t numOfProps = rootDesc->NumOfProps();
    uint32_t index = 0;
    JSType type = rootDesc->GetObjectType();
    size_t size = rootDesc->GetObjectSize();
    JSHandle<JSHClass> hclass = factory->NewEcmaHClass(size, type, maxNum);
    // Dictionary?
    JSHandle<LayoutInfo> layout = factory->CreateLayoutInfo(maxNum, MemSpaceType::SEMI_SPACE, GrowMode::KEEP);
    rootDesc->IterateProps([thread, factory, &index, hclass, layout](const pgo::PropertyDesc& propDesc) {
        auto& cstring = propDesc.first;
        auto& handler = propDesc.second;
        JSHandle<JSTaggedValue> key = ParseKeyFromPGOCString(factory, cstring, handler);
        PropertyAttributes attributes = PropertyAttributes::Default();
        if (handler.SetAttribute(thread, attributes)) {
            hclass->SetIsAllTaggedProp(false);
        }
        attributes.SetIsInlinedProps(true);
        attributes.SetOffset(index);
        layout->AddKey(thread, index, key.GetTaggedValue(), attributes);
        index++;
    });
    hclass->SetLayout(thread, layout);
    hclass->SetNumberOfProps(numOfProps);
    hclass->SetAOT(true);
    return hclass;
}

JSHandle<JSHClass> JSHClass::CreateRootHClassWithCached(const JSThread* thread,
                                                        const HClassLayoutDesc* desc,
                                                        uint32_t literalLength,
                                                        uint32_t maxPropsNum)
{
    auto rootDesc = reinterpret_cast<const pgo::RootHClassLayoutDesc *>(desc);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    uint32_t index = 0;
    ASSERT(rootDesc->GetObjectSize() == JSObject::SIZE);
    ASSERT(rootDesc->GetObjectType() == JSType::JS_OBJECT);
    JSHandle<JSHClass> hclass = factory->GetObjectLiteralRootHClass(literalLength, maxPropsNum);
    JSHandle<LayoutInfo> layout = factory->CreateLayoutInfo(literalLength, MemSpaceType::SEMI_SPACE, GrowMode::KEEP);
    hclass->SetPrototype(thread, JSTaggedValue::Null());
    hclass->SetLayout(thread, layout);
    hclass->SetAOT(true);
    rootDesc->IterateProps([thread, factory, &index, &hclass, &maxPropsNum](const pgo::PropertyDesc& propDesc) {
        auto& cstring = propDesc.first;
        auto& handler = propDesc.second;
        JSHandle<JSTaggedValue> key = ParseKeyFromPGOCString(factory, cstring, handler);
        PropertyAttributes attributes = PropertyAttributes::Default();
        if (handler.SetAttribute(thread, attributes)) {
            hclass->SetIsAllTaggedProp(false);
        }
        attributes.SetIsInlinedProps(true);
        attributes.SetOffset(index++);
        auto rep = attributes.GetRepresentation();

        JSHandle<JSHClass> child = SetPropertyOfObjHClass(thread, hclass, key, attributes, rep, true, maxPropsNum);
        child->SetParent(thread, hclass);
        child->SetPrototype(thread, JSTaggedValue::Null());
        child->SetAOT(true);
        hclass = child;
    });
    return hclass;
}

JSHandle<JSHClass> JSHClass::CreateChildHClassFromPGO(const JSThread* thread,
                                                      const JSHandle<JSHClass>& parent,
                                                      const HClassLayoutDesc* desc)
{
    pgo::PropertyDesc propDesc = reinterpret_cast<const pgo::ChildHClassLayoutDesc *>(desc)->GetPropertyDesc();
    auto& cstring = propDesc.first;
    auto& handler = propDesc.second;
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    uint32_t numOfProps = parent->NumberOfProps();

    JSHandle<JSHClass> newJsHClass = JSHClass::Clone(thread, parent);
    newJsHClass->SetAOT(true);
    ASSERT(newJsHClass->GetInlinedProperties() >= (numOfProps + 1));
    uint32_t offset = numOfProps;
    {
        JSMutableHandle<LayoutInfo> layoutInfoHandle(thread, newJsHClass->GetLayout(thread));
        if (layoutInfoHandle->NumberOfElements() != static_cast<int>(offset)) {
            layoutInfoHandle.Update(factory->CopyAndReSort(layoutInfoHandle, offset, offset + 1));
            newJsHClass->SetLayout(thread, layoutInfoHandle);
        } else if (layoutInfoHandle->GetPropertiesCapacity() <= static_cast<int>(offset)) {  // need to Grow
            layoutInfoHandle.Update(
                factory->ExtendLayoutInfo(layoutInfoHandle, offset));
            newJsHClass->SetLayout(thread, layoutInfoHandle);
        }
        JSHandle<JSTaggedValue> key = ParseKeyFromPGOCString(factory, cstring, handler);
        PropertyAttributes attributes = PropertyAttributes::Default();
        if (handler.SetAttribute(thread, attributes)) {
            newJsHClass->SetIsAllTaggedProp(false);
        }
        attributes.SetOffset(offset);
        attributes.SetIsInlinedProps(true);
        layoutInfoHandle->AddKey(thread, offset, key.GetTaggedValue(), attributes);
        newJsHClass->IncNumberOfProps();
        AddTransitions(thread, parent, newJsHClass, key, attributes);
        JSHClass::NotifyHclassChanged(thread, parent, newJsHClass, key.GetTaggedValue());
    }

    return newJsHClass;
}

bool JSHClass::DumpRootHClassByPGO(const JSThread *thread, const JSHClass *hclass, HClassLayoutDesc *desc)
{
    DISALLOW_GARBAGE_COLLECTION;
    if (hclass->IsDictionaryMode()) {
        return false;
    }

    LayoutInfo *layout = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    int element = static_cast<int>(hclass->NumberOfProps());
    for (int i = 0; i < element; i++) {
        layout->DumpFieldIndexByPGO(thread, i, desc);
    }
    return true;
}

bool JSHClass::DumpChildHClassByPGO(const JSThread *thread, const JSHClass *hclass, HClassLayoutDesc *desc)
{
    DISALLOW_GARBAGE_COLLECTION;
    if (hclass->IsDictionaryMode()) {
        return false;
    }
    if (hclass->PropsIsEmpty()) {
        return false;
    }
    uint32_t last = hclass->LastPropIndex();
    LayoutInfo *layoutInfo = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    layoutInfo->DumpFieldIndexByPGO(thread, last, desc);
    return true;
}

bool JSHClass::UpdateChildLayoutDescByPGO(const JSThread *thread, const JSHClass *hclass, HClassLayoutDesc *childDesc)
{
    DISALLOW_GARBAGE_COLLECTION;
    if (hclass->IsDictionaryMode()) {
        return false;
    }
    if (hclass->PropsIsEmpty()) {
        return false;
    }
    uint32_t last = hclass->LastPropIndex();
    LayoutInfo *layoutInfo = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    return layoutInfo->UpdateFieldIndexByPGO(thread, last, childDesc);
}

bool JSHClass::UpdateRootLayoutDescByPGO(const JSThread *thread, const JSHClass *hclass, HClassLayoutDesc *desc)
{
    DISALLOW_GARBAGE_COLLECTION;
    if (hclass->IsDictionaryMode()) {
        return false;
    }

    auto rootDesc = reinterpret_cast<const pgo::RootHClassLayoutDesc *>(desc);
    int rootPropLen = static_cast<int>(rootDesc->NumOfProps());
    LayoutInfo *layout = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    for (int i = 0; i < rootPropLen; i++) {
        layout->UpdateFieldIndexByPGO(thread, i, desc);
    }
    return true;
}

std::pair<bool, CString> JSHClass::DumpToString(const JSThread *thread, JSTaggedType hclassVal)
{
    DISALLOW_GARBAGE_COLLECTION;
    auto hclass = JSHClass::Cast(JSTaggedValue(hclassVal).GetTaggedObject());
    bool isInvalid = false;
    if (hclass->IsDictionaryMode()) {
        return std::make_pair(isInvalid, "");
    }

    CString result;
    LayoutInfo *layout = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject());
    int element = static_cast<int>(hclass->NumberOfProps());
    for (int i = 0; i < element; i++) {
        auto key = layout->GetKey(thread, i);
        if (key.IsString()) {
            uint64_t value = EcmaStringAccessor(key).GetHashcode(thread);
            value <<= sizeof(uint32_t) * BITS_PER_BYTE;
            auto attr = layout->GetAttr(thread, i);
            auto defaultAttr = PropertyAttributes(attr.GetPropertyMetaData());
            defaultAttr.SetTrackType(attr.GetTrackType());
            value += defaultAttr.GetValue();
            result += ToCString(value);
        } else if (key.IsSymbol()) {
            auto symbol = JSSymbol::Cast(key);
            if (symbol->HasId()) {
                result += JSSymbol::Cast(key)->GetPrivateId();
                auto attr = layout->GetAttr(thread, i);
                result += static_cast<int32_t>(attr.GetTrackType());
                result += attr.GetPropertyMetaData();
            } else {
                isInvalid = true;
                result = "";
                break;
            }
        } else {
            LOG_ECMA(FATAL) << "JSHClass::DumpToString UNREACHABLE";
        }
    }
    return std::make_pair(isInvalid, result);
}

PropertyLookupResult JSHClass::LookupPropertyInBuiltinHClass(const JSThread *thread, JSHClass *hclass,
                                                             JSTaggedValue key)
{
    DISALLOW_GARBAGE_COLLECTION;

    PropertyLookupResult result;
    if (hclass->IsDictionaryMode()) {
        result.SetIsFound(false);
        return result;
    }

    int entry = JSHClass::FindPropertyEntry(thread, hclass, key);
    // found in local
    if (entry != -1) {
        result.SetIsFound(true);
        result.SetIsLocal(true);
        PropertyAttributes attr = LayoutInfo::Cast(hclass->GetLayout(thread).GetTaggedObject())->GetAttr(thread, entry);
        if (attr.IsInlinedProps()) {
            result.SetIsInlinedProps(true);
            result.SetOffset(hclass->GetInlinedPropertiesOffset(entry));
        } else {
            result.SetIsInlinedProps(false);
            result.SetOffset(attr.GetOffset() - hclass->GetInlinedProperties());
        }

        if (attr.IsNotHole()) {
            result.SetIsNotHole(true);
        }
        if (attr.IsAccessor()) {
            result.SetIsAccessor(true);
        }
        result.SetRepresentation(attr.GetRepresentation());
        result.SetIsWritable(attr.IsWritable());
        return result;
    }

    // not fuond
    result.SetIsFound(false);
    return result;
}

JSHandle<JSHClass> JSHClass::CreateSHClass(JSThread *thread,
                                           const std::vector<PropertyDescriptor> &descs,
                                           const JSHClass *parentHClass,
                                           bool isFunction)
{
    EcmaVM *vm = thread->GetEcmaVM();
    ObjectFactory *factory = vm->GetFactory();

    uint32_t length = descs.size();
    uint32_t maxInline = isFunction ? JSSharedFunction::MAX_INLINE : JSSharedObject::MAX_INLINE;

    if (parentHClass) {
        if (parentHClass->IsDictionaryMode()) {
            auto dict = reinterpret_cast<NameDictionary *>(parentHClass->GetLayout(thread).GetTaggedObject());
            length += static_cast<uint32_t>(dict->EntriesCount());
        } else {
            length += parentHClass->NumberOfProps();
        }
    }

    JSHandle<JSHClass> hclass =
        isFunction ? factory->NewSEcmaHClass(JSSharedFunction::SIZE, JSType::JS_SHARED_FUNCTION, length)
                   : factory->NewSEcmaHClass(JSSharedObject::SIZE, JSType::JS_SHARED_OBJECT, length);
    if (LIKELY(length <= maxInline)) {
        CreateSInlinedLayout(thread, descs, hclass, parentHClass);
    } else {
        CreateSDictLayout(thread, descs, hclass, parentHClass);
    }

    return hclass;
}

JSHandle<JSHClass> JSHClass::CreateSConstructorHClass(JSThread *thread, const std::vector<PropertyDescriptor> &descs)
{
    auto hclass = CreateSHClass(thread, descs, nullptr, true);
    hclass->SetClassConstructor(true);
    hclass->SetConstructor(true);
    return hclass;
}

JSHandle<JSHClass> JSHClass::CreateSPrototypeHClass(JSThread *thread, const std::vector<PropertyDescriptor> &descs)
{
    auto hclass = CreateSHClass(thread, descs);
    hclass->SetClassPrototype(true);
    hclass->SetIsPrototype(true);
    return hclass;
}

void JSHClass::CreateSInlinedLayout(JSThread *thread,
                                    const std::vector<PropertyDescriptor> &descs,
                                    const JSHandle<JSHClass> &hclass,
                                    const JSHClass *parentHClass)
{
    EcmaVM *vm = thread->GetEcmaVM();
    ObjectFactory *factory = vm->GetFactory();

    uint32_t parentLength{0};
    if (parentHClass) {
        parentLength = parentHClass->NumberOfProps();
    }
    auto length = descs.size();
    auto layout = factory->CreateSLayoutInfo(length + parentLength);

    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    for (uint32_t i = 0; i < length; ++i) {
        key.Update(descs[i].GetKey());
        PropertyAttributes attr =
            PropertyAttributes::Default(descs[i].IsWritable(), descs[i].IsEnumerable(), descs[i].IsConfigurable());
        if (UNLIKELY(descs[i].GetValue()->IsAccessor())) {
            attr.SetIsAccessor(true);
        }
        attr.SetIsInlinedProps(true);
        attr.SetRepresentation(Representation::TAGGED);
        attr.SetSharedFieldType(descs[i].GetSharedFieldType());
        attr.SetOffset(i);
        layout->AddKey(thread, i, key.GetTaggedValue(), attr);
    }

    auto index = length;
    if (parentHClass) {
        JSHandle<LayoutInfo> old(thread, parentHClass->GetLayout(thread));
        for (uint32_t i = 0; i < parentLength; i++) {
            key.Update(old->GetKey(thread, i));
            auto entry = layout->FindElementWithCache(thread, *hclass, key.GetTaggedValue(), index);
            if (entry != -1) {
                continue;
            }
            auto attr = PropertyAttributes(old->GetAttr(thread, i));
            attr.SetOffset(index);
            layout->AddKey(thread, index, old->GetKey(thread, i), attr);
            ++index;
        }
    }

    hclass->SetLayout(thread, layout);
    hclass->SetNumberOfProps(index);
    auto inlinedPropsLength = hclass->GetInlinedProperties();
    if (inlinedPropsLength > index) {
        uint32_t duplicatedSize = (inlinedPropsLength - index) * JSTaggedValue::TaggedTypeSize();
        hclass->SetObjectSize(hclass->GetObjectSize() - duplicatedSize);
    }
}

void JSHClass::CreateSDictLayout(JSThread *thread,
                                 const std::vector<PropertyDescriptor> &descs,
                                 const JSHandle<JSHClass> &hclass,
                                 const JSHClass *parentHClass)
{
    uint32_t parentLength{0};
    if (parentHClass) {
        if (parentHClass->IsDictionaryMode()) {
            parentLength = static_cast<uint32_t>(
                reinterpret_cast<NameDictionary *>(parentHClass->GetLayout(thread).GetTaggedObject())->EntriesCount());
        } else {
            parentLength = parentHClass->NumberOfProps();
        }
    }
    auto length = descs.size();
    JSMutableHandle<NameDictionary> dict(
        thread,
        NameDictionary::CreateInSharedHeap(thread, NameDictionary::ComputeHashTableSize(length + parentLength)));
    JSMutableHandle<JSTaggedValue> key(thread, JSTaggedValue::Undefined());
    auto globalConst = const_cast<GlobalEnvConstants *>(thread->GlobalConstants());
    JSHandle<JSTaggedValue> value = globalConst->GetHandledUndefined();

    for (uint32_t i = 0; i < length; ++i) {
        key.Update(descs[i].GetKey());
        PropertyAttributes attr =
            PropertyAttributes::Default(descs[i].IsWritable(), descs[i].IsEnumerable(), descs[i].IsConfigurable());
        attr.SetSharedFieldType(descs[i].GetSharedFieldType());
        attr.SetBoxType(PropertyBoxType::UNDEFINED);
        JSHandle<NameDictionary> newDict = NameDictionary::Put(thread, dict, key, value, attr);
        dict.Update(newDict);
    }

    if (parentHClass) {
        if (parentHClass->IsDictionaryMode()) {
            JSHandle<NameDictionary> old(thread, parentHClass->GetLayout(thread));
            std::vector<int> indexOrder = old->GetEnumerationOrder(thread);
            for (uint32_t i = 0; i < parentLength; i++) {
                key.Update(old->GetKey(thread, indexOrder[i]));
                JSHandle<NameDictionary> newDict = NameDictionary::Put(
                    thread, dict, key, value, PropertyAttributes(old->GetAttributes(thread, indexOrder[i])));
                dict.Update(newDict);
            }
        } else {
            JSHandle<LayoutInfo> old(thread, parentHClass->GetLayout(thread));
            for (uint32_t i = 0; i < parentLength; i++) {
                key.Update(old->GetKey(thread, i));
                JSHandle<NameDictionary> newDict =
                    NameDictionary::Put(thread, dict, key, value, PropertyAttributes(old->GetAttr(thread, i)));
                dict.Update(newDict);
            }
        }
    }

    hclass->SetLayout(thread, dict);
    hclass->SetNumberOfProps(0);
    hclass->SetIsDictionaryMode(true);
}

bool JSHClass::IsNeedNotifyHclassChangedForAotTransition(const JSThread *thread, const JSHandle<JSHClass> &hclass,
                                                         JSTaggedValue key)
{
    JSMutableHandle<JSObject> protoHandle(thread, hclass->GetPrototype(thread));
    while (protoHandle.GetTaggedValue().IsHeapObject()) {
        JSHClass *protoHclass = protoHandle->GetJSHClass();
        if (JSHClass::FindPropertyEntry(thread, protoHclass, key) != -1) {
            return true;
        }
        protoHandle.Update(protoHclass->GetPrototype(thread));
    }
    return false;
}
}  // namespace panda::ecmascript
