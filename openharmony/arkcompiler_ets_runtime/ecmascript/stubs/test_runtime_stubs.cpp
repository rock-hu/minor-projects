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
#include "ecmascript/object_factory.h"
#include "ecmascript/stubs/runtime_stubs.h"

#include "ecmascript/js_tagged_value.h"
#include "ecmascript/global_env.h"
#include "ecmascript/js_function.h"

namespace panda::ecmascript {
#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#define DEF_RUNTIME_STUBS(name) \
JSTaggedType RuntimeStubs::name(uintptr_t argGlue, uint32_t argc, uintptr_t argv) \

#define RUNTIME_STUBS_HEADER(name)                        \
    auto thread = JSThread::GlueToJSThread(argGlue);      \
    [[maybe_unused]] EcmaHandleScope handleScope(thread)  \

#define CONVERT_ARG_TAGGED_TYPE_CHECKED(name, index) \
    ASSERT((index) < argc);                          \
    JSTaggedType name = *(reinterpret_cast<JSTaggedType *>(argv) + (index))

#define CONVERT_ARG_TAGGED_CHECKED(name, index) \
    ASSERT((index) < argc);                     \
    JSTaggedValue name = JSTaggedValue(*(reinterpret_cast<JSTaggedType *>(argv) + (index)))

#define CONVERT_ARG_HANDLE_CHECKED(type, name, index) \
    ASSERT((index) < argc);                           \
    JSHandle<type> name(&(reinterpret_cast<JSTaggedType *>(argv)[index]))

#define CONVERT_ARG_PTR_CHECKED(type, name, index) \
    ASSERT((index) < argc);                        \
    type name = reinterpret_cast<type>(*(reinterpret_cast<JSTaggedType *>(argv) + (index)))

#ifndef NDEBUG
DEF_RUNTIME_STUBS(DefineAotFunc)
{
    RUNTIME_STUBS_HEADER(DefineAotFunc);
    CONVERT_ARG_TAGGED_CHECKED(funcIndex, 0);
    CONVERT_ARG_TAGGED_CHECKED(numArgs, 1);
    EcmaVM *ecmaVm = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVm->GetFactory();
    JSHandle<GlobalEnv> env = ecmaVm->GetGlobalEnv();
    auto codeEntry = thread->GetFastStubEntry(funcIndex.GetInt());
    JSHandle<Method> method = factory->NewMethodForNativeFunction(reinterpret_cast<void *>(codeEntry));
    method->SetAotCodeBit(true);
    method->SetNativeBit(false);
    method->SetNumArgsWithCallField(numArgs.GetInt());
    method->SetCodeEntryOrLiteral(reinterpret_cast<uintptr_t>(codeEntry));
    JSHandle<JSFunction> jsfunc = factory->NewJSFunction(env, method);
    return jsfunc.GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(GetPrintFunc)
{
    RUNTIME_STUBS_HEADER(GetPrintFunc);
    EcmaVM *ecmaVm = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVm->GetFactory();
    auto env = ecmaVm->GetGlobalEnv();
    JSHandle<JSTaggedValue> globalObject(thread, env->GetGlobalObject());
    JSHandle<JSTaggedValue> printString(thread, factory->NewFromStdString("print").GetTaggedValue());

    return JSObject::GetProperty(thread, globalObject, printString).
        GetValue().GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(GetBindFunc)
{
    RUNTIME_STUBS_HEADER(GetBindFunc);
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, target, 0);
    EcmaVM *ecmaVm = thread->GetEcmaVM();
    ObjectFactory *factory = ecmaVm->GetFactory();
    JSHandle<JSTaggedValue> bindString(thread, factory->NewFromStdString("bind").GetTaggedValue());

    return JSObject::GetProperty(thread, target, bindString).GetValue().GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(DefineProxyFunc)
{
    RUNTIME_STUBS_HEADER(DefineProxyFunc);
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, targetHandle, 0);
    // 1. handler has no "Call"
    JSFunction *function = env->GetObjectFunction().GetObject<JSFunction>();
    JSHandle<JSTaggedValue> hclass(thread, function);
    ASSERT(targetHandle->IsECMAObject());

    JSHandle<JSTaggedValue> handlerHandle(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass), hclass));
    ASSERT(handlerHandle->IsECMAObject());
    ASSERT(targetHandle->IsECMAObject());

    JSHandle<JSProxy> proxyHandle = JSProxy::ProxyCreate(thread, targetHandle, handlerHandle);
    ASSERT(*proxyHandle != nullptr);
    // check taggedvalue
    proxyHandle.GetTaggedValue().D(thread);
    return proxyHandle.GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(DefineProxyHandler)
{
    RUNTIME_STUBS_HEADER(DefineProxyHandler);
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, funcHandle, 0);
    ASSERT(funcHandle->IsECMAObject());
    ObjectFactory *factory = thread->GetEcmaVM()->GetFactory();
    JSHandle<GlobalEnv> env = thread->GetEcmaVM()->GetGlobalEnv();
    JSFunction* function = env->GetObjectFunction().GetObject<JSFunction>();
    JSHandle<JSTaggedValue> hclass(thread, function);

    JSHandle<JSTaggedValue> handlerHandle(factory->NewJSObjectByConstructor(JSHandle<JSFunction>(hclass), hclass));
    ASSERT(handlerHandle->IsECMAObject());
    // 1. handler has "Call"
    JSHandle<JSTaggedValue> funcKey = thread->GlobalConstants()->GetHandledApplyString();
    JSObject::SetProperty(thread, JSHandle<JSTaggedValue>(handlerHandle), funcKey, funcHandle);
    handlerHandle.GetTaggedValue().D(thread);
    return handlerHandle.GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(DefineProxyFunc2)
{
    RUNTIME_STUBS_HEADER(DefineProxyFunc2);
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, targetHandle, 0);
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, handlerHandle, 1);
    // 1. handler has "Call"
    ASSERT(handlerHandle->IsECMAObject());
    ASSERT(targetHandle->IsECMAObject());

    JSHandle<JSProxy> proxyHandle = JSProxy::ProxyCreate(thread, targetHandle, handlerHandle);
    targetHandle.GetTaggedValue().D(thread);
    handlerHandle.GetTaggedValue().D(thread);
    proxyHandle.GetTaggedValue().D(thread);
    ASSERT(*proxyHandle != nullptr);
    return proxyHandle.GetTaggedValue().GetRawData();
}

DEF_RUNTIME_STUBS(DumpTaggedType)
{
    RUNTIME_STUBS_HEADER(DumpTaggedType);
    CONVERT_ARG_HANDLE_CHECKED(JSTaggedValue, value, 0);
    ASSERT(value->IsECMAObject());
    value->D(thread);
    return value.GetTaggedValue().GetRawData();
}
#endif
}  // namespace panda::ecmascript
