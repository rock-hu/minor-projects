/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "backend/debugger_executor.h"

#include "ecmascript/debugger/js_debugger_manager.h"
#include "tooling/base/pt_types.h"

namespace panda::ecmascript::tooling {
void DebuggerExecutor::Initialize(const EcmaVM *vm)
{
    [[maybe_unused]] EcmaHandleScope handleScope(vm->GetJSThread());
    Local<ObjectRef> globalObj = JSNApi::GetGlobalObject(vm);
    globalObj->Set(vm, StringRef::NewFromUtf8(vm, "debuggerSetValue"), FunctionRef::New(
        const_cast<panda::EcmaVM*>(vm), DebuggerExecutor::DebuggerSetValue));
    globalObj->Set(vm, StringRef::NewFromUtf8(vm, "debuggerGetValue"), FunctionRef::New(
        const_cast<panda::EcmaVM*>(vm), DebuggerExecutor::DebuggerGetValue));
}

Local<JSValueRef> DebuggerExecutor::DebuggerGetValue(JsiRuntimeCallInfo *runtimeCallInfo)
{
    EcmaVM *vm = runtimeCallInfo->GetVM();
    uint32_t argc = runtimeCallInfo->GetArgsNumber();
    if (argc != NUM_ARGS) {
        return JSValueRef::Undefined(vm);
    }
    Local<JSValueRef> name = runtimeCallInfo->GetCallArgRef(0);
    if (!name->IsString(vm)) {
        return JSValueRef::Undefined(vm);
    }
    Local<JSValueRef> isThrow = runtimeCallInfo->GetCallArgRef(1);

    auto &frameHandler = vm->GetJsDebuggerManager()->GetEvalFrameHandler();
    ASSERT(frameHandler);

    Local<JSValueRef> value = GetValue(vm, frameHandler.get(), Local<StringRef>(name));
    if (!value.IsEmpty()) {
        return value;
    }

    if (!isThrow->ToBoolean(vm)->Value()) {
        DebuggerApi::ClearException(vm);
        return JSValueRef::Undefined(vm);
    }

    std::string varName = Local<StringRef>(name)->ToString(vm);
    ThrowException(vm, varName + " is not defined");
    return Local<JSValueRef>();
}

Local<JSValueRef> DebuggerExecutor::DebuggerSetValue(JsiRuntimeCallInfo *runtimeCallInfo)
{
    EcmaVM *vm = runtimeCallInfo->GetVM();
    uint32_t argc = runtimeCallInfo->GetArgsNumber();
    if (argc != NUM_ARGS) {
        return JSValueRef::Undefined(vm);
    }
    Local<JSValueRef> name = runtimeCallInfo->GetCallArgRef(0);
    if (!name->IsString(vm)) {
        return JSValueRef::Undefined(vm);
    }
    Local<JSValueRef> value = runtimeCallInfo->GetCallArgRef(1);

    auto &frameHandler = vm->GetJsDebuggerManager()->GetEvalFrameHandler();
    ASSERT(frameHandler);

    if (SetValue(vm, frameHandler.get(), Local<StringRef>(name), value)) {
        return value;
    }

    std::string varName = StringRef::Cast(*name)->ToString(vm);
    ThrowException(vm, varName + " is not defined");
    return Local<JSValueRef>();
}

Local<JSValueRef> DebuggerExecutor::GetValue(const EcmaVM *vm, const FrameHandler *frameHandler, Local<StringRef> name)
{
    Local<JSValueRef> value;
    value = GetLocalValue(vm, frameHandler, name);
    if (!value.IsEmpty()) {
        return value;
    }
    value = GetLexicalValue(vm, frameHandler, name);
    if (!value.IsEmpty()) {
        return value;
    }
    value = GetModuleValue(vm, frameHandler, name);
    if (!value.IsEmpty()) {
        return value;
    }
    value = GetGlobalValue(vm, name);
    if (!value.IsEmpty()) {
        return value;
    }

    return Local<JSValueRef>();
}

bool DebuggerExecutor::SetValue(const EcmaVM *vm, FrameHandler *frameHandler,
                                Local<StringRef> name, Local<JSValueRef> value)
{
    if (SetLocalValue(vm, frameHandler, name, value)) {
        return true;
    }
    if (SetLexicalValue(vm, frameHandler, name, value)) {
        return true;
    }
    if (SetModuleValue(vm, frameHandler, name, value)) {
        return true;
    }
    if (SetGlobalValue(vm, name, value)) {
        return true;
    }

    return false;
}

void DebuggerExecutor::ThrowException(const EcmaVM *vm, const std::string &error)
{
    Local<StringRef> msg = StringRef::NewFromUtf8(vm, error.c_str());
    Local<JSValueRef> exception = Exception::ReferenceError(vm, msg);
    JSNApi::ThrowException(vm, exception);
}

Local<JSValueRef> DebuggerExecutor::GetLocalValue(const EcmaVM *vm, const FrameHandler *frameHandler,
                                                  Local<StringRef> name)
{
    Local<JSValueRef> result;

    int32_t index = DebuggerApi::GetVregIndex(frameHandler, name->ToString(vm));
    if (index == -1) {
        return result;
    }

    result = DebuggerApi::GetVRegValue(vm, frameHandler, index);
    return result;
}

bool DebuggerExecutor::SetLocalValue(const EcmaVM *vm, FrameHandler *frameHandler,
                                     Local<StringRef> name, Local<JSValueRef> value)
{
    std::string varName = name->ToString(vm);
    int32_t index = DebuggerApi::GetVregIndex(frameHandler, varName);
    if (index == -1) {
        return false;
    }

    DebuggerApi::SetVRegValue(frameHandler, index, value);
    vm->GetJsDebuggerManager()->NotifyScopeUpdated(varName, value, Scope::Type::Local());
    return true;
}

Local<JSValueRef> DebuggerExecutor::GetLexicalValue(const EcmaVM *vm, const FrameHandler *frameHandler,
                                                    Local<StringRef> name)
{
    Local<JSValueRef> result;

    auto [level, slot] = DebuggerApi::GetLevelSlot(frameHandler, name->ToString(vm));
    if (level == -1) {
        return result;
    }

    result = DebuggerApi::GetProperties(vm, frameHandler, level, slot);
    return result;
}

bool DebuggerExecutor::SetLexicalValue(const EcmaVM *vm, const FrameHandler *frameHandler,
                                       Local<StringRef> name, Local<JSValueRef> value)
{
    std::string varName = name->ToString(vm);
    auto [level, slot] = DebuggerApi::GetLevelSlot(frameHandler, varName);
    if (level == -1) {
        return false;
    }

    DebuggerApi::SetProperties(vm, frameHandler, level, slot, value);
    vm->GetJsDebuggerManager()->NotifyScopeUpdated(varName, value, Scope::Type::Closure());
    return true;
}

Local<JSValueRef> DebuggerExecutor::GetGlobalValue(const EcmaVM *vm, Local<StringRef> name)
{
    return DebuggerApi::GetGlobalValue(vm, name);
}

bool DebuggerExecutor::SetGlobalValue(const EcmaVM *vm, Local<StringRef> name, Local<JSValueRef> value)
{
    std::string varName = name->ToString(vm);
    vm->GetJsDebuggerManager()->NotifyScopeUpdated(varName, value, Scope::Type::Global());
    return DebuggerApi::SetGlobalValue(vm, name, value);
}

Local<JSValueRef> DebuggerExecutor::GetModuleValue(const EcmaVM *vm, const FrameHandler *frameHandler,
                                                   Local<StringRef> name)
{
    Local<JSValueRef> result;
    std::string varName = name->ToString(vm);
    Method *method = DebuggerApi::GetMethod(frameHandler);
    const JSPandaFile *jsPandaFile = method->GetJSPandaFile();
    if (jsPandaFile != nullptr && (jsPandaFile->IsBundlePack() || !jsPandaFile->IsNewVersion())) {
        return result;
    }
    JSThread *thread = vm->GetJSThread();
    JSHandle<JSTaggedValue> currentModule(thread, DebuggerApi::GetCurrentModule(vm));
    if (currentModule->IsSourceTextModule()) {
        result = DebuggerApi::GetModuleValue(vm, currentModule, varName);
    }
    return result;
}

bool DebuggerExecutor::SetModuleValue(const EcmaVM *vm, const FrameHandler *frameHandler,
                                      Local<StringRef> name, Local<JSValueRef> value)
{
    std::string varName = name->ToString(vm);
    Method *method = DebuggerApi::GetMethod(frameHandler);
    const JSPandaFile *jsPandaFile = method->GetJSPandaFile();
    if (jsPandaFile != nullptr && (jsPandaFile->IsBundlePack() || !jsPandaFile->IsNewVersion())) {
        return false;
    }
    JSThread *thread = vm->GetJSThread();
    JSHandle<JSTaggedValue> currentModule(thread, DebuggerApi::GetCurrentModule(vm));
    bool result = false;
    if (currentModule->IsSourceTextModule()) {
        result = DebuggerApi::SetModuleValue(vm, currentModule, varName, value);
        vm->GetJsDebuggerManager()->NotifyScopeUpdated(varName, value, Scope::Type::Module());
    }
    return result;
}
}  // namespace panda::ecmascript::tooling
