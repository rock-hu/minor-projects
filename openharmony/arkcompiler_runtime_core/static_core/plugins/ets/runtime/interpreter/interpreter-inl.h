/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#ifndef PLUGINS_ETS_INTERPRETER_INTERPRETER_INL_H
#define PLUGINS_ETS_INTERPRETER_INTERPRETER_INL_H

#include "runtime/coroutines/coroutine_manager.h"
#include "runtime/interpreter/interpreter-inl.h"
#include "runtime/mem/internal_allocator.h"
#include "plugins/ets/runtime/ets_class_linker_extension.h"
#include "plugins/ets/runtime/ets_coroutine.h"
#include "plugins/ets/runtime/ets_exceptions.h"
#include "plugins/ets/runtime/ets_language_context.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "plugins/ets/runtime/ets_handle_scope.h"
#include "plugins/ets/runtime/ets_handle.h"
#include "plugins/ets/runtime/types/ets_promise.h"
#include "plugins/ets/runtime/types/ets_string.h"
#include "plugins/ets/runtime/ets_stubs-inl.h"
#include "plugins/ets/runtime/ets_stubs.h"
#include "runtime/mem/refstorage/global_object_storage.h"
#include "runtime/include/class_linker-inl.h"

namespace ark::ets {
template <class RuntimeIfaceT, bool IS_DYNAMIC, bool IS_DEBUG, bool IS_PROFILE_ENABLED>
class InstructionHandler : public interpreter::InstructionHandler<RuntimeIfaceT, IS_DYNAMIC, IS_DEBUG> {
public:
    ALWAYS_INLINE inline explicit InstructionHandler(interpreter::InstructionHandlerState *state)
        : interpreter::InstructionHandler<RuntimeIfaceT, IS_DYNAMIC, IS_DEBUG>(state)
    {
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunchShort()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch.short v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", " << std::hex << "0x" << id;
        HandleLaunch<FORMAT, false>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunch()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 2>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 3>() << ", " << std::hex << "0x" << id;
        HandleLaunch<FORMAT, false>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunchRange()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch.range v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", " << std::hex << "0x"
                   << id;
        HandleLaunch<FORMAT, true>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunchVirtShort()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch.virt.short v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", " << std::hex << "0x" << id;
        HandleLaunchVirt<FORMAT, false>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunchVirt()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch.virt v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 2>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 3>() << ", " << std::hex << "0x" << id;
        HandleLaunchVirt<FORMAT, false>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLaunchVirtRange()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        LOG_INST() << "launch.virt.range v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", " << std::hex << "0x"
                   << id;
        HandleLaunchVirt<FORMAT, true>(id);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLdobjName()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.ldobj.name v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<true>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                this->LoadPrimitiveField(obj, field);
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::I32, true>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<true>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLdobjNameWide()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.ldobj.name.64 v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<true>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                this->LoadPrimitiveField(obj, field);
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::I64, true>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<true>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLdobjNameObj()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.ldobj.name.obj v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<true>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                ASSERT(field->GetType().IsReference());
                this->GetAccAsVReg().SetReference(
                    obj->GetFieldObject<RuntimeIfaceT::NEED_READ_BARRIER>(this->GetThread(), *field));
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::REFERENCE, true>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<true>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsStobjName()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.stobj.name v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<false>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                 this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                this->StorePrimitiveField(obj, field);
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::I32, false>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<false>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsStobjNameWide()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.stobj.name.64 v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<false>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                 this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                this->StorePrimitiveField(obj, field);
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::I64, false>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<false>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsStobjNameObj()
    {
        uint16_t vs = this->GetInst().template GetVReg<FORMAT>();
        auto id = this->GetInst().template GetId<FORMAT>();

        LOG_INST() << "ets.stobj.name v" << vs << ", " << std::hex << "0x" << id;

        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
        } else {
            auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
            auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
            auto caller = this->GetFrame()->GetMethod();
            auto rawField = classLinker->GetField(*caller, caller->GetClass()->ResolveFieldIndex(id.AsIndex()), false);
            InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
            Field *field = GetFieldByName<false>(cache->GetEntry(this->GetInst().GetAddress()), caller, rawField,
                                                 this->GetInst().GetAddress(), klass);
            if (field != nullptr) {
                ASSERT(field->GetType().IsReference());
                obj->SetFieldObject<RuntimeIfaceT::NEED_WRITE_BARRIER>(this->GetThread(), *field,
                                                                       this->GetAcc().GetReference());
                this->template MoveToNextInst<FORMAT, true>();
                return;
            }
            ark::Method *method = GetAccessorByName<panda_file::Type::TypeId::REFERENCE, false>(
                cache->GetEntry(this->GetInst().GetAddress()), caller, rawField, this->GetInst().GetAddress(), klass);
            if (method != nullptr) {
                this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, false, false, false,
                                          false>(method);
                return;
            }
            LookUpException<false>(klass, rawField);
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT, bool IS_RANGE = false>
    ALWAYS_INLINE void HandleEtsMethodCallName()
    {
        auto id = this->GetInst().template GetId<FORMAT>();
        uint16_t vs = this->GetInst().template GetVReg<FORMAT, 0>();
        ObjectHeader *obj = this->GetFrame()->GetVReg(vs).GetReference();
        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
            return;
        }
        auto klass = static_cast<ark::Class *>(obj->ClassAddr<ark::BaseClass>());
        auto *classLinker = Runtime::GetCurrent()->GetClassLinker();
        auto caller = this->GetFrame()->GetMethod();
        auto rawMethod = classLinker->GetMethod(*caller, caller->GetClass()->ResolveMethodIndex(id.AsIndex()));
        if (UNLIKELY(rawMethod == nullptr)) {
            HandlePendingException();
            this->MoveToExceptionHandler();
            return;
        }
        InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
        ETSStubCacheInfo cacheInfo {cache->GetEntry(this->GetInst().GetAddress()), caller,
                                    this->GetInst().GetAddress()};
        ark::Method *method = GetMethodByName(GetCoro(), cacheInfo, rawMethod, klass);
        if (UNLIKELY(method == nullptr)) {
            LookUpException(klass, rawMethod);
            this->MoveToExceptionHandler();
            return;
        }
        if (method->IsAbstract()) {
            RuntimeIfaceT::ThrowAbstractMethodError(method);
            this->MoveToExceptionHandler();
            return;
        }
        this->template HandleCall<ark::interpreter::FrameHelperDefault, FORMAT, false, IS_RANGE, false, false, false>(
            method);
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsCallNameShort()
    {
        LOG_INST() << "ets.call.name.short v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", " << std::hex << "0x"
                   << this->GetInst().template GetId<FORMAT>();

        HandleEtsMethodCallName<FORMAT>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsCallName()
    {
        LOG_INST() << "ets.call.name v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 1>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 2>() << ", v"
                   << this->GetInst().template GetVReg<FORMAT, 3>() << ", " << std::hex << "0x"
                   << this->GetInst().template GetId<FORMAT>();

        HandleEtsMethodCallName<FORMAT>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsCallNameRange()
    {
        LOG_INST() << "ets.call.name.range v" << this->GetInst().template GetVReg<FORMAT, 0>() << ", " << std::hex
                   << "0x" << this->GetInst().template GetId<FORMAT>();

        HandleEtsMethodCallName<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsLdnullvalue()
    {
        LOG_INST() << "ets.ldnullvalue";

        this->GetAccAsVReg().SetReference(GetCoro()->GetNullValue());
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsMovnullvalue()
    {
        uint16_t vd = this->GetInst().template GetVReg<FORMAT>();
        LOG_INST() << "ets.movnullvalue v" << vd;

        this->GetFrameHandler().GetVReg(vd).SetReference(GetCoro()->GetNullValue());
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsIsnullvalue()
    {
        LOG_INST() << "ets.isnullvalue";

        ObjectHeader *obj = this->GetAcc().GetReference();
        this->GetAccAsVReg().SetPrimitive(obj == GetCoro()->GetNullValue());
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsEquals()
    {
        uint16_t v1 = this->GetInst().template GetVReg<FORMAT, 0>();
        uint16_t v2 = this->GetInst().template GetVReg<FORMAT, 1>();

        LOG_INST() << "ets.equals v" << v1 << ", v" << v2;

        ObjectHeader *obj1 = this->GetFrame()->GetVReg(v1).GetReference();
        ObjectHeader *obj2 = this->GetFrame()->GetVReg(v2).GetReference();

        bool res = EtsReferenceEquals(GetCoro(), EtsObject::FromCoreType(obj1), EtsObject::FromCoreType(obj2));
        this->GetAccAsVReg().SetPrimitive(res);
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsStrictequals()
    {
        uint16_t v1 = this->GetInst().template GetVReg<FORMAT, 0>();
        uint16_t v2 = this->GetInst().template GetVReg<FORMAT, 1>();

        LOG_INST() << "ets.strictequals v" << v1 << ", v" << v2;

        ObjectHeader *obj1 = this->GetFrame()->GetVReg(v1).GetReference();
        ObjectHeader *obj2 = this->GetFrame()->GetVReg(v2).GetReference();

        bool res = EtsReferenceEquals<true>(GetCoro(), EtsObject::FromCoreType(obj1), EtsObject::FromCoreType(obj2));
        this->GetAccAsVReg().SetPrimitive(res);
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsTypeof()
    {
        uint16_t v1 = this->GetInst().template GetVReg<FORMAT, 0>();

        LOG_INST() << "ets.typeof v" << v1;

        ObjectHeader *obj = this->GetFrame()->GetVReg(v1).GetReference();

        EtsString *res = EtsReferenceTypeof(GetCoro(), EtsObject::FromCoreType(obj));
        this->GetAccAsVReg().SetReference(res->AsObjectHeader());
        this->template MoveToNextInst<FORMAT, true>();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE void HandleEtsIstrue()
    {
        uint16_t v = this->GetInst().template GetVReg<FORMAT, 0>();

        LOG_INST() << "ets.istrue v" << v;

        ObjectHeader *obj = this->GetFrame()->GetVReg(v).GetReference();

        bool res = EtsIstrue(GetCoro(), EtsObject::FromCoreType(obj));
        this->GetAccAsVReg().SetPrimitive(res);
        this->template MoveToNextInst<FORMAT, true>();
    }

private:
    ALWAYS_INLINE bool IsNull(ObjectHeader *obj)
    {
        return obj == nullptr;
    }

    ALWAYS_INLINE bool IsUndefined(ObjectHeader *obj)
    {
        return obj == GetCoro()->GetUndefinedObject();
    }

    ALWAYS_INLINE bool IsNullish(ObjectHeader *obj)
    {
        return IsNull(obj) || IsUndefined(obj);
    }

    ALWAYS_INLINE EtsCoroutine *GetCoro() const
    {
        return EtsCoroutine::CastFromThread(this->GetThread());
    }

    template <BytecodeInstruction::Format FORMAT, bool IS_RANGE>
    ALWAYS_INLINE void HandleLaunchVirt(BytecodeId methodId)
    {
        auto *method = ResolveMethod(methodId);
        if (LIKELY(method != nullptr)) {
            ObjectHeader *obj = this->GetCallerObject<FORMAT>();
            if (UNLIKELY(obj == nullptr)) {
                return;
            }
            auto *cls = obj->ClassAddr<Class>();
            ASSERT(cls != nullptr);
            method = cls->ResolveVirtualMethod(method);
            ASSERT(method != nullptr);
            HandleLaunch<FORMAT, IS_RANGE>(method);
        } else {
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT, bool IS_RANGE>
    ALWAYS_INLINE void HandleLaunch(BytecodeId methodId)
    {
        auto *method = ResolveMethod(methodId);
        if (LIKELY(method != nullptr)) {
            HandleLaunch<FORMAT, IS_RANGE>(method);
        } else {
            this->MoveToExceptionHandler();
        }
    }

    template <BytecodeInstruction::Format FORMAT, bool IS_RANGE>
    ALWAYS_INLINE void HandleLaunch(Method *method)
    {
        if (UNLIKELY(!method->Verify())) {
            RuntimeIfaceT::ThrowVerificationException(method->GetFullName());
            this->MoveToExceptionHandler();
            return;
        }

        // this promise is going to be resolved on coro completion
        EtsCoroutine *coroutine = EtsCoroutine::GetCurrent();
        EtsPromise *promise = EtsPromise::Create(coroutine);
        if (UNLIKELY(promise == nullptr)) {
            this->MoveToExceptionHandler();
            return;
        }
        PandaEtsVM *etsVm = coroutine->GetPandaVM();
        auto *coroManager = coroutine->GetCoroutineManager();
        auto promiseRef = etsVm->GetGlobalObjectStorage()->Add(promise, mem::Reference::ObjectType::GLOBAL);
        auto evt = Runtime::GetCurrent()->GetInternalAllocator()->New<CompletionEvent>(promiseRef, coroManager);

        // create the coro and put it to the ready queue
        [[maybe_unused]] EtsHandleScope scope(coroutine);
        EtsHandle<EtsPromise> promiseHandle(coroutine, promise);

        // since transferring arguments from frame registers (which are local roots for GC) to a C++ vector
        // introduces the potential risk of pointer invalidation in case GC moves the referenced objects,
        // we would like to do this transfer below all potential GC invocation points (e.g. Promise::Create)
        size_t numArgs = method->GetNumArgs();
        PandaVector<Value> args(numArgs);
        FillArgs<FORMAT, IS_RANGE>(args);

        bool launchResult =
            coroutine->GetCoroutineManager()->Launch(evt, method, std::move(args), CoroutineLaunchMode::DEFAULT);
        if (UNLIKELY(!launchResult)) {
            // OOM
            Runtime::GetCurrent()->GetInternalAllocator()->Delete(evt);
            this->MoveToExceptionHandler();
            return;
        }

        this->GetAccAsVReg().SetReference(promiseHandle.GetPtr());
        this->GetFrame()->SetAcc(this->GetAcc());
        this->template MoveToNextInst<FORMAT, false>();
    }

    template <BytecodeInstruction::Format FORMAT, bool IS_RANGE>
    ALWAYS_INLINE void FillArgs(PandaVector<Value> &args)
    {
        if (args.empty()) {
            return;
        }

        auto curFrameHandler = this->template GetFrameHandler<false>();
        if constexpr (IS_RANGE) {
            uint16_t startReg = this->GetInst().template GetVReg<FORMAT, 0>();
            for (size_t i = 0; i < args.size(); ++i) {
                args[i] = Value::FromVReg(curFrameHandler.GetVReg(startReg + i));
            }
        } else {
            // launch.short of launch
            args[0] = Value::FromVReg(curFrameHandler.GetVReg(this->GetInst().template GetVReg<FORMAT, 0U>()));
            if (args.size() > 1U) {
                args[1] = Value::FromVReg(curFrameHandler.GetVReg(this->GetInst().template GetVReg<FORMAT, 1U>()));
            }
            if constexpr (FORMAT == BytecodeInstruction::Format::PREF_V4_V4_V4_V4_ID16) {
                if (args.size() > 2U) {
                    args[2] = Value::FromVReg(curFrameHandler.GetVReg(this->GetInst().template GetVReg<FORMAT, 2U>()));
                }
                if (args.size() > 3U) {
                    args[3] = Value::FromVReg(curFrameHandler.GetVReg(this->GetInst().template GetVReg<FORMAT, 3U>()));
                }
            }
        }
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE ObjectHeader *GetObjHelper()
    {
        uint16_t objVreg = this->GetInst().template GetVReg<FORMAT, 0>();
        return this->GetFrame()->GetVReg(objVreg).GetReference();
    }

    template <BytecodeInstruction::Format FORMAT>
    ALWAYS_INLINE ObjectHeader *GetCallerObject()
    {
        ObjectHeader *obj = GetObjHelper<FORMAT>();

        if (UNLIKELY(obj == nullptr)) {
            RuntimeIfaceT::ThrowNullPointerException();
            this->MoveToExceptionHandler();
            return nullptr;
        }
        return obj;
    }

    ALWAYS_INLINE Method *ResolveMethod(BytecodeId id)
    {
        this->UpdateBytecodeOffset();

        InterpreterCache *cache = this->GetThread()->GetInterpreterCache();
        auto *res = cache->template Get<Method>(this->GetInst().GetAddress(), this->GetFrame()->GetMethod());
        if (res != nullptr) {
            return res;
        }

        this->GetFrame()->SetAcc(this->GetAcc());
        auto *method = RuntimeIfaceT::ResolveMethod(this->GetThread(), *this->GetFrame()->GetMethod(), id);
        this->GetAcc() = this->GetFrame()->GetAcc();
        if (UNLIKELY(method == nullptr)) {
            ASSERT(this->GetThread()->HasPendingException());
            return nullptr;
        }

        cache->Set(this->GetInst().GetAddress(), method, this->GetFrame()->GetMethod());
        return method;
    }
};
}  // namespace ark::ets
#endif  // PLUGINS_ETS_INTERPRETER_INTERPRETER_INL_H
