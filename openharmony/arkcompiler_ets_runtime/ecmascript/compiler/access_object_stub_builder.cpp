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
#include "ecmascript/compiler/access_object_stub_builder.h"
#include "ecmascript/compiler/ic_stub_builder.h"
#include "ecmascript/compiler/interpreter_stub-inl.h"
#include "ecmascript/compiler/profiler_stub_builder.h"
#include "ecmascript/compiler/rt_call_signature.h"
#include "ecmascript/compiler/stub_builder-inl.h"
#include "ecmascript/ic/profile_type_info.h"

namespace panda::ecmascript::kungfu {
GateRef AccessObjectStubBuilder::LoadObjByName(GateRef glue, GateRef receiver, GateRef prop, const StringIdInfo &info,
                                               GateRef profileTypeInfo, GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    GateRef value = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.LoadICByName(&result, &tryFastPath, &tryPreDump, &exit, callback);
    Bind(&tryFastPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = GetPropertyByName(glue, receiver, propKey, callback, True());
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result =
            CallRuntime(glue, RTSTUB_ID(LoadICByName), {profileTypeInfo, receiver, propKey, IntToTaggedInt(slotId)});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::LoadPrivatePropertyByName(
    GateRef glue, GateRef receiver, GateRef key, GateRef profileTypeInfo, GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    GateRef value = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.LoadICByName(&result, &tryFastPath, &tryPreDump, &exit, callback);
    Bind(&tryFastPath);
    {
        result = GetPropertyByName(glue, receiver, key, callback, True());
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(LoadICByName), {profileTypeInfo, receiver, key, IntToTaggedInt(slotId)});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

// Used for deprecated bytecodes which will not support ic
GateRef AccessObjectStubBuilder::DeprecatedLoadObjByName(GateRef glue, GateRef receiver, GateRef propKey)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label fastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    BRANCH(TaggedIsHeapObject(receiver), &fastPath, &slowPath);
    Bind(&fastPath);
    {
        result = GetPropertyByName(glue, receiver, propKey, ProfileOperation(), True());
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(LoadICByName),
            { Undefined(), receiver, propKey, IntToTaggedInt(Int32(0xFF)) });  // 0xFF: invalid slot id
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StoreObjByName(GateRef glue, GateRef receiver, GateRef prop, const StringIdInfo &info,
                                                GateRef value, GateRef profileTypeInfo, GateRef slotId,
                                                ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId, callback);
    builder.StoreICByName(&result, &tryFastPath, &tryPreDump, &exit);
    Bind(&tryFastPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = SetPropertyByName(glue, receiver, propKey, value, false, True(), callback);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = CallRuntime(
            glue, RTSTUB_ID(StoreICByName), {profileTypeInfo, receiver, propKey, value, IntToTaggedInt(slotId)});
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StorePrivatePropertyByName(GateRef glue,
                                                            GateRef receiver,
                                                            GateRef key,
                                                            GateRef value,
                                                            GateRef profileTypeInfo,
                                                            GateRef slotId,
                                                            ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId, callback);
    builder.StoreICByName(&result, &tryFastPath, &tryPreDump, &exit);
    Bind(&tryFastPath);
    {
        result = SetPropertyByName(glue, receiver, key, value, false, True(), callback);
        Branch(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(
            glue, RTSTUB_ID(StoreICByName), {profileTypeInfo, receiver, key, value, IntToTaggedInt(slotId)});
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::ResolvePropKey(GateRef glue, GateRef prop, const StringIdInfo &info)
{
    if (jsFunc_ != Circuit::NullGate()) {
        GateRef constpool = GetConstPoolFromFunction(jsFunc_);
        return GetStringFromConstPool(glue, constpool, ChangeIntPtrToInt32(prop));
    }
    if (!info.IsValid()) {
        return prop;
    }
    ASSERT(info.IsValid());
    InterpreterToolsStubBuilder builder(GetCallSignature(), GetEnvironment());
    GateRef stringId = builder.GetStringId(info);
    return GetStringFromConstPool(glue, info.GetConstantPool(), stringId);
}

GateRef AccessObjectStubBuilder::LoadObjByValue(GateRef glue, GateRef receiver, GateRef key, GateRef profileTypeInfo,
                                                GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    GateRef value = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId, key);
    builder.LoadICByValue(&result, &tryFastPath, &tryPreDump, &exit, callback);
    Bind(&tryFastPath);
    {
        result = GetPropertyByValue(glue, receiver, key, callback);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(LoadICByValue), {profileTypeInfo, receiver, key, IntToTaggedInt(slotId)});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

// Used for deprecated bytecodes which will not support ic
GateRef AccessObjectStubBuilder::DeprecatedLoadObjByValue(GateRef glue, GateRef receiver, GateRef key)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label fastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    BRANCH(TaggedIsHeapObject(receiver), &fastPath, &slowPath);
    Bind(&fastPath);
    {
        result = GetPropertyByValue(glue, receiver, key, ProfileOperation());
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(LoadICByValue),
            { Undefined(), receiver, key, IntToTaggedInt(Int32(0xFF)) });  // 0xFF: invalied slot id
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StoreObjByValue(GateRef glue, GateRef receiver, GateRef key, GateRef value,
                                                 GateRef profileTypeInfo, GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId, key, callback);
    builder.StoreICByValue(&result, &tryFastPath, &tryPreDump, &exit);
    Bind(&tryFastPath);
    {
        result = SetPropertyByValue(glue, receiver, key, value, false, callback);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(
            glue, RTSTUB_ID(StoreICByValue), {profileTypeInfo, receiver, key, value, IntToTaggedInt(slotId)});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StoreOwnByIndex(GateRef glue, GateRef receiver, GateRef index, GateRef value,
                                                 GateRef profileTypeInfo, GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);
    Label tryPreDump(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId, IntToTaggedPtr(index), callback);
    builder.StoreICByValue(&result, &tryFastPath, &tryPreDump, &exit);
    Bind(&tryFastPath);
    {
        Label isHeapObject(env);
        BRANCH(TaggedIsHeapObject(receiver), &isHeapObject, &slowPath);
        Bind(&isHeapObject);
        Label notClassConstructor(env);
        BRANCH(IsClassConstructor(receiver), &slowPath, &notClassConstructor);
        Bind(&notClassConstructor);
        Label notClassPrototype(env);
        BRANCH(IsClassPrototype(receiver), &slowPath, &notClassPrototype);
        Bind(&notClassPrototype);
        result = SetPropertyByIndex(glue, receiver, index, value, true);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&tryPreDump);
    {
        callback.TryPreDump();
        Jump(&slowPath);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue,
                             RTSTUB_ID(StoreOwnICByValue),
                             {profileTypeInfo, receiver, IntToTaggedInt(index), value, IntToTaggedInt(slotId)});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::TryLoadGlobalByName(GateRef glue, GateRef prop, const StringIdInfo &info,
                                                     GateRef profileTypeInfo, GateRef slotId,
                                                     ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    GateRef receiver = 0;
    GateRef value = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.TryLoadGlobalICByName(&result, &tryFastPath, &slowPath, &exit);
    Bind(&tryFastPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        GateRef record = LdGlobalRecord(glue, propKey);
        Label foundInRecord(env);
        Label notFoundInRecord(env);
        BRANCH(TaggedIsUndefined(record), &notFoundInRecord, &foundInRecord);
        Bind(&foundInRecord);
        {
            result = Load(VariableType::JS_ANY(), record, IntPtr(PropertyBox::VALUE_OFFSET));
            Jump(&exit);
        }
        Bind(&notFoundInRecord);
        {
            GateRef globalObject = GetGlobalObject(glue);
            result = GetGlobalOwnProperty(glue, globalObject, propKey, callback);
            BRANCH(TaggedIsHole(*result), &slowPath, &exit);
        }
    }
    Bind(&slowPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = CallRuntime(glue, RTSTUB_ID(TryLdGlobalICByName),
                             { profileTypeInfo, propKey, IntToTaggedInt(slotId) });
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::TryStoreGlobalByName(GateRef glue, GateRef prop, const StringIdInfo &info,
                                                      GateRef value, GateRef profileTypeInfo, GateRef slotId,
                                                      ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Undefined());
    GateRef receiver = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.TryStoreGlobalICByName(&result, &tryFastPath, &slowPath, &exit);
    Bind(&tryFastPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        GateRef record = LdGlobalRecord(glue, propKey);
        Label foundInRecord(env);
        Label notFoundInRecord(env);
        BRANCH(TaggedIsUndefined(record), &notFoundInRecord, &foundInRecord);
        Bind(&foundInRecord);
        {
            result = CallRuntime(glue, RTSTUB_ID(TryUpdateGlobalRecord), { propKey, value });
            Jump(&exit);
        }
        Bind(&notFoundInRecord);
        {
            GateRef globalObject = GetGlobalObject(glue);
            result = GetGlobalOwnProperty(glue, globalObject, propKey, callback);
            Label isFoundInGlobal(env);
            Label notFoundInGlobal(env);
            BRANCH(TaggedIsHole(*result), &notFoundInGlobal, &isFoundInGlobal);
            Bind(&isFoundInGlobal);
            {
                result = CallRuntime(glue, RTSTUB_ID(StGlobalVar), { propKey, value });
                Jump(&exit);
            }
            Bind(&notFoundInGlobal);
            {
                result = CallRuntime(glue, RTSTUB_ID(ThrowReferenceError), { propKey });
                Jump(&exit);
            }
        }
    }
    Bind(&slowPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        GateRef globalObject = GetGlobalObject(glue);
        result = CallRuntime(glue, RTSTUB_ID(StoreMiss),
                             { profileTypeInfo, globalObject, propKey, value, IntToTaggedInt(slotId),
                               IntToTaggedInt(Int32(static_cast<int>(ICKind::NamedGlobalTryStoreIC))) });
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::LoadGlobalVar(GateRef glue, GateRef prop, const StringIdInfo &info,
                                               GateRef profileTypeInfo, GateRef slotId, ProfileOperation callback)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    GateRef receiver = 0;
    GateRef value = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.TryLoadGlobalICByName(&result, &tryFastPath, &slowPath, &exit);
    Bind(&tryFastPath);
    {
        GateRef globalObject = GetGlobalObject(glue);
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = GetGlobalOwnProperty(glue, globalObject, propKey, callback);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        GateRef globalObject = GetGlobalObject(glue);
        GateRef propKey = ResolvePropKey(glue, prop, info);
        result = CallRuntime(glue, RTSTUB_ID(LdGlobalICVar),
                             { globalObject, propKey, profileTypeInfo, IntToTaggedInt(slotId) });
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StoreGlobalVar(GateRef glue, GateRef prop, const StringIdInfo &info,
                                                GateRef value, GateRef profileTypeInfo, GateRef slotId)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    Label exit(env);
    Label tryFastPath(env);
    Label slowPath(env);

    DEFVARIABLE(result, VariableType::JS_ANY(), Undefined());
    GateRef receiver = 0;
    ICStubBuilder builder(this);
    builder.SetParameters(glue, receiver, profileTypeInfo, value, slotId);
    builder.TryStoreGlobalICByName(&result, &tryFastPath, &slowPath, &exit);
    Bind(&tryFastPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        // IR later
        result = CallRuntime(glue, RTSTUB_ID(StGlobalVar), { propKey, value });
        Jump(&exit);
    }
    Bind(&slowPath);
    {
        GateRef propKey = ResolvePropKey(glue, prop, info);
        GateRef globalObject = GetGlobalObject(glue);
        result = CallRuntime(glue, RTSTUB_ID(StoreMiss),
                             { profileTypeInfo, globalObject, propKey, value, IntToTaggedInt(slotId),
                               IntToTaggedInt(Int32(static_cast<int>(ICKind::NamedGlobalStoreIC))) });
        Jump(&exit);
    }

    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StOwnByIndex(GateRef glue, GateRef receiver, GateRef index, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label isHeapObject(env);
    Label slowPath(env);
    Label exit(env);
    BRANCH(TaggedIsHeapObject(receiver), &isHeapObject, &slowPath);
    Bind(&isHeapObject);
    Label notClassConstructor(env);
    BRANCH(IsClassConstructor(receiver), &slowPath, &notClassConstructor);
    Bind(&notClassConstructor);
    Label notClassPrototype(env);
    BRANCH(IsClassPrototype(receiver), &slowPath, &notClassPrototype);
    Bind(&notClassPrototype);
    {
        result = SetPropertyByIndex(glue, receiver, TruncInt64ToInt32(index), value, true);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(StOwnByIndex), {receiver, IntToTaggedInt(index), value });
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StOwnByValue(GateRef glue, GateRef receiver, GateRef key, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label isHeapObject(env);
    Label slowPath(env);
    Label exit(env);
    BRANCH(TaggedIsHeapObject(receiver), &isHeapObject, &slowPath);
    Bind(&isHeapObject);
    Label notClassConstructor(env);
    BRANCH(IsClassConstructor(receiver), &slowPath, &notClassConstructor);
    Bind(&notClassConstructor);
    Label notClassPrototype(env);
    BRANCH(IsClassPrototype(receiver), &slowPath, &notClassPrototype);
    Bind(&notClassPrototype);
    {
        result = SetPropertyByValue(glue, receiver, key, value, true);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(StOwnByValue), { receiver, key, value });
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StOwnByName(GateRef glue, GateRef receiver, GateRef key, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label isJSObject(env);
    Label slowPath(env);
    Label exit(env);
    BRANCH(IsJSObject(receiver), &isJSObject, &slowPath);
    Bind(&isJSObject);
    Label notClassConstructor(env);
    BRANCH(IsClassConstructor(receiver), &slowPath, &notClassConstructor);
    Bind(&notClassConstructor);
    Label notClassPrototype(env);
    BRANCH(IsClassPrototype(receiver), &slowPath, &notClassPrototype);
    Bind(&notClassPrototype);
    {
        result = SetPropertyByName(glue, receiver, key, value, true, True());
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(StOwnByName), { receiver, key, value });
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StOwnByValueWithNameSet(GateRef glue, GateRef receiver, GateRef key, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label isHeapObject(env);
    Label slowPath(env);
    Label notClassConstructor(env);
    Label notClassPrototype(env);
    Label notHole(env);
    Label exit(env);
    BRANCH(TaggedIsHeapObject(receiver), &isHeapObject, &slowPath);
    Bind(&isHeapObject);
    {
        BRANCH(IsClassConstructor(receiver), &slowPath, &notClassConstructor);
        Bind(&notClassConstructor);
        {
            BRANCH(IsClassPrototype(receiver), &slowPath, &notClassPrototype);
            Bind(&notClassPrototype);
            {
                result = SetPropertyByValue(glue, receiver, key, value, true, ProfileOperation(), true);
                BRANCH(TaggedIsHole(*result), &slowPath, &notHole);
                Bind(&notHole);
                {
                    Label notexception(env);
                    BRANCH(TaggedIsException(*result), &exit, &notexception);
                    Bind(&notexception);
                    CallRuntime(glue, RTSTUB_ID(SetFunctionNameNoPrefix), { value, key });
                    Jump(&exit);
                }
            }
        }
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(StOwnByValueWithNameSet), { receiver, key, value });
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StOwnByNameWithNameSet(GateRef glue, GateRef receiver, GateRef key, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label isJSObject(env);
    Label notJSObject(env);
    Label notClassConstructor(env);
    Label notClassPrototype(env);
    Label notHole(env);
    Label exit(env);
    BRANCH(IsJSObject(receiver), &isJSObject, &notJSObject);
    Bind(&isJSObject);
    {
        BRANCH(IsClassConstructor(receiver), &notJSObject, &notClassConstructor);
        Bind(&notClassConstructor);
        {
            BRANCH(IsClassPrototype(receiver), &notJSObject, &notClassPrototype);
            Bind(&notClassPrototype);
            {
                result = SetPropertyByName(glue, receiver, key, value, true, True(), ProfileOperation(), false, true);
                BRANCH(TaggedIsHole(*result), &notJSObject, &notHole);
                Bind(&notHole);
                {
                    Label notException(env);
                    BRANCH(TaggedIsException(*result), &exit, &notException);
                    Bind(&notException);
                    CallRuntime(glue, RTSTUB_ID(SetFunctionNameNoPrefix), {value, key});
                    Jump(&exit);
                }
            }
        }
    }
    Bind(&notJSObject);
    {
        result = CallRuntime(glue, RTSTUB_ID(StOwnByNameWithNameSet), { receiver, key, value });
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::StObjByIndex(GateRef glue, GateRef receiver, GateRef index, GateRef value)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(result, VariableType::JS_ANY(), Hole());
    Label exit(env);
    Label fastPath(env);
    Label slowPath(env);
    BRANCH(TaggedIsHeapObject(receiver), &fastPath, &slowPath);
    Bind(&fastPath);
    {
        result = SetPropertyByIndex(glue, receiver, TruncInt64ToInt32(index), value, false);
        BRANCH(TaggedIsHole(*result), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        result = CallRuntime(glue, RTSTUB_ID(StObjByIndex), {receiver, IntToTaggedInt(index), value});
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *result;
    env->SubCfgExit();
    return ret;
}

GateRef AccessObjectStubBuilder::LdObjByIndex(GateRef glue, GateRef receiver, GateRef index)
{
    auto env = GetEnvironment();
    Label entry(env);
    env->SubCfgEntry(&entry);
    DEFVARIABLE(varAcc, VariableType::JS_ANY(), Hole());
    Label fastPath(env);
    Label slowPath(env);
    Label exit(env);
    BRANCH(TaggedIsHeapObject(receiver), &fastPath, &slowPath);
    Bind(&fastPath);
    {
        varAcc = GetPropertyByIndex(glue, receiver, TruncInt64ToInt32(index), ProfileOperation());
        BRANCH(TaggedIsHole(*varAcc), &slowPath, &exit);
    }
    Bind(&slowPath);
    {
        GateRef undefined = Undefined();
        auto args = { receiver, IntToTaggedInt(index), TaggedFalse(), undefined };
        varAcc = CallRuntime(glue, RTSTUB_ID(LdObjByIndex), args);
        Jump(&exit);
    }
    Bind(&exit);
    auto ret = *varAcc;
    env->SubCfgExit();
    return ret;
}
}  // namespace panda::ecmascript::kungfu
