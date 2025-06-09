/**
 * Copyright (c) 2022-2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "plugins/ets/runtime/ets_coroutine.h"
#include "runtime/include/value.h"
#include "macros.h"
#include "mem/refstorage/reference.h"
#include "runtime/include/object_header.h"
#include "plugins/ets/runtime/types/ets_promise.h"
#include "plugins/ets/runtime/types/ets_job.h"
#include "plugins/ets/runtime/ets_vm.h"
#include "runtime/include/panda_vm.h"
#include "plugins/ets/runtime/ets_class_linker_extension.h"
#include "plugins/ets/runtime/types/ets_object.h"
#include "plugins/ets/runtime/types/ets_box_primitive-inl.h"
#include "intrinsics.h"

namespace ark::ets {

// ExternalIfaceTable contains std::function, which cannot be trivially constructed even for nullptr
// NOLINTNEXTLINE(fuchsia-statically-constructed-objects)
ExternalIfaceTable EtsCoroutine::emptyExternalIfaceTable_ = ExternalIfaceTable();

EtsCoroutine::EtsCoroutine(ThreadId id, mem::InternalAllocatorPtr allocator, PandaVM *vm, PandaString name,
                           CoroutineContext *context, std::optional<EntrypointInfo> &&epInfo, Type type)
    : Coroutine(id, allocator, vm, ark::panda_file::SourceLang::ETS, std::move(name), context, std::move(epInfo), type)
{
    ASSERT(vm != nullptr);
}

PandaEtsVM *EtsCoroutine::GetPandaVM() const
{
    return static_cast<PandaEtsVM *>(GetVM());
}

CoroutineManager *EtsCoroutine::GetCoroutineManager() const
{
    return GetPandaVM()->GetCoroutineManager();
}

void EtsCoroutine::Initialize()
{
    auto allocator = GetVM()->GetHeapManager()->GetInternalAllocator();
    auto etsNapiEnv = PandaEtsNapiEnv::Create(this, allocator);
    if (!etsNapiEnv) {
        LOG(FATAL, RUNTIME) << "Cannot create PandaEtsNapiEnv: " << etsNapiEnv.Error();
    }
    etsNapiEnv_ = etsNapiEnv.Value();
    // Main EtsCoroutine is Initialized before class linker and promise_class_ptr_ will be set after creating the class
    if (Runtime::GetCurrent()->IsInitialized()) {
        promiseClassPtr_ = PlatformTypes(GetPandaVM())->corePromise->GetRuntimeClass();
        SetupNullValue(GetPandaVM()->GetNullValue());
        GetLocalStorage().Set<EtsCoroutine::DataIdx::ETS_PLATFORM_TYPES_PTR>(
            ToUintPtr(GetPandaVM()->GetClassLinker()->GetEtsClassLinkerExtension()->GetPlatformTypes()));
        // NOTE (electronick, #15938): Refactor the managed class-related pseudo TLS fields
        // initialization in MT ManagedThread ctor and EtsCoroutine::Initialize
        auto *linkExt = GetPandaVM()->GetClassLinker()->GetEtsClassLinkerExtension();
        SetStringClassPtr(linkExt->GetClassRoot(ClassRoot::STRING));
        SetArrayU16ClassPtr(linkExt->GetClassRoot(ClassRoot::ARRAY_U16));
        SetArrayU8ClassPtr(linkExt->GetClassRoot(ClassRoot::ARRAY_U8));
    }
    ASSERT(promiseClassPtr_ != nullptr || !HasManagedEntrypoint());

    Coroutine::Initialize();
}

void EtsCoroutine::CleanUp()
{
    Coroutine::CleanUp();
    // add the required local storage entries cleanup here!
}

void EtsCoroutine::FreeInternalMemory()
{
    etsNapiEnv_->FreeInternalMemory();
    ManagedThread::FreeInternalMemory();
}

void EtsCoroutine::RequestCompletion(Value returnValue)
{
    auto *completionObjRef = GetCompletionEvent()->ReleaseReturnValueObject();
    if (completionObjRef == nullptr) {
        Coroutine::RequestCompletion(returnValue);
        return;
    }
    auto *storage = GetVM()->GetGlobalObjectStorage();
    auto *completionObj = EtsObject::FromCoreType(storage->Get(completionObjRef));

    if (completionObj->IsInstanceOf(PlatformTypes(this)->corePromise)) {
        RequestPromiseCompletion(completionObjRef, returnValue);
    } else if (completionObj->IsInstanceOf(PlatformTypes(this)->coreJob)) {
        RequestJobCompletion(completionObjRef, returnValue);
    } else {
        UNREACHABLE();
    }
}

void EtsCoroutine::RequestJobCompletion(mem::Reference *jobRef, Value returnValue)
{
    auto *storage = GetVM()->GetGlobalObjectStorage();
    auto *job = EtsJob::FromCoreType(storage->Get(jobRef));
    storage->Remove(jobRef);
    if (job == nullptr) {
        LOG(DEBUG, COROUTINES)
            << "Coroutine \"" << GetName()
            << "\" has completed, but the associated job has been already collected by the GC. Exception thrown: "
            << HasPendingException();
        Coroutine::RequestCompletion(returnValue);
        return;
    }
    [[maybe_unused]] EtsHandleScope scope(this);
    EtsHandle<EtsJob> hjob(this, job);
    EtsObject *retObject = nullptr;
    if (!HasPendingException()) {
        panda_file::Type returnType = GetReturnType();
        retObject = GetReturnValueAsObject(returnType, returnValue);
        if (retObject != nullptr) {
            LOG_IF(returnType.IsVoid(), DEBUG, COROUTINES) << "Coroutine \"" << GetName() << "\" has completed";
            LOG_IF(returnType.IsPrimitive(), DEBUG, COROUTINES)
                << "Coroutine \"" << GetName() << "\" has completed with return value 0x" << std::hex
                << returnValue.GetAs<uint64_t>();
            LOG_IF(returnType.IsReference(), DEBUG, COROUTINES)
                << "Coroutine \"" << GetName() << "\" has completed with return value = ObjectPtr<"
                << returnValue.GetAs<ObjectHeader *>() << ">";
        }
    }
    if (HasPendingException()) {
        // An exception may occur while boxin primitive return value in GetReturnValueAsObject
        auto *exc = GetException();
        ClearException();
        LOG(INFO, COROUTINES) << "Coroutine \"" << GetName()
                              << "\" completed with an exception: " << exc->ClassAddr<Class>()->GetName();
        EtsJob::EtsJobFail(hjob.GetPtr(), EtsObject::FromCoreType(exc));
        return;
    }
    EtsJob::EtsJobFinish(hjob.GetPtr(), retObject);
}

void EtsCoroutine::RequestPromiseCompletion(mem::Reference *promiseRef, Value returnValue)
{
    auto *storage = GetVM()->GetGlobalObjectStorage();
    auto *promise = EtsPromise::FromCoreType(storage->Get(promiseRef));
    storage->Remove(promiseRef);
    if (promise == nullptr) {
        LOG(DEBUG, COROUTINES)
            << "Coroutine " << GetName()
            << " has completed, but the associated promise has been already collected by the GC. Exception thrown: "
            << HasPendingException();
        Coroutine::RequestCompletion(returnValue);
        return;
    }
    [[maybe_unused]] EtsHandleScope scope(this);
    EtsHandle<EtsPromise> hpromise(this, promise);
    EtsObject *retObject = nullptr;
    if (!HasPendingException()) {
        panda_file::Type returnType = GetReturnType();
        retObject = GetReturnValueAsObject(returnType, returnValue);
        if (retObject != nullptr) {
            LOG_IF(returnType.IsVoid(), DEBUG, COROUTINES) << "Coroutine " << GetName() << " has completed";
            LOG_IF(returnType.IsPrimitive(), DEBUG, COROUTINES)
                << "Coroutine " << GetName() << " has completed with return value 0x" << std::hex
                << returnValue.GetAs<uint64_t>();
            LOG_IF(returnType.IsReference(), DEBUG, COROUTINES)
                << "Coroutine " << GetName() << " has completed with return value = ObjectPtr<"
                << returnValue.GetAs<ObjectHeader *>() << ">";
        }
    }
    if (HasPendingException()) {
        // An exception may occur while boxin primitive return value in GetReturnValueAsObject
        auto *exc = GetException();
        ClearException();
        LOG(INFO, COROUTINES) << "Coroutine " << GetName()
                              << " completed with an exception: " << exc->ClassAddr<Class>()->GetName();
        intrinsics::EtsPromiseReject(hpromise.GetPtr(), EtsObject::FromCoreType(exc), ToEtsBoolean(false));
        return;
    }
    if (retObject != nullptr && retObject->IsInstanceOf(PlatformTypes(this)->corePromise)) {
        retObject = GetValueFromPromiseSync(EtsPromise::FromEtsObject(retObject));
        if (retObject == nullptr) {
            LOG(INFO, COROUTINES) << "Coroutine " << GetName() << " completion by a promise retval went wrong";
        }
    }
    intrinsics::EtsPromiseResolve(hpromise.GetPtr(), retObject, ToEtsBoolean(false));
}

EtsObject *EtsCoroutine::GetValueFromPromiseSync(EtsPromise *promise)
{
    return intrinsics::EtsAwaitPromise(promise);
}

panda_file::Type EtsCoroutine::GetReturnType()
{
    Method *entrypoint = GetManagedEntrypoint();
    ASSERT(entrypoint != nullptr);
    return entrypoint->GetReturnType();
}

// The result will be used to resolve a promise, so this function perfoms a "box" operation on ark::Value
EtsObject *EtsCoroutine::GetReturnValueAsObject(panda_file::Type returnType, Value returnValue)
{
    switch (returnType.GetId()) {
        case panda_file::Type::TypeId::VOID:
            return nullptr;  // a representation of ets "undefined"
        case panda_file::Type::TypeId::U1:
            return EtsBoxPrimitive<EtsBoolean>::Create(this, returnValue.GetAs<EtsBoolean>());
        case panda_file::Type::TypeId::I8:
            return EtsBoxPrimitive<EtsByte>::Create(this, returnValue.GetAs<EtsByte>());
        case panda_file::Type::TypeId::I16:
            return EtsBoxPrimitive<EtsShort>::Create(this, returnValue.GetAs<EtsShort>());
        case panda_file::Type::TypeId::U16:
            return EtsBoxPrimitive<EtsChar>::Create(this, returnValue.GetAs<EtsChar>());
        case panda_file::Type::TypeId::I32:
            return EtsBoxPrimitive<EtsInt>::Create(this, returnValue.GetAs<EtsInt>());
        case panda_file::Type::TypeId::F32:
            return EtsBoxPrimitive<EtsFloat>::Create(this, returnValue.GetAs<EtsFloat>());
        case panda_file::Type::TypeId::F64:
            return EtsBoxPrimitive<EtsDouble>::Create(this, returnValue.GetAs<EtsDouble>());
        case panda_file::Type::TypeId::I64:
            return EtsBoxPrimitive<EtsLong>::Create(this, returnValue.GetAs<EtsLong>());
        case panda_file::Type::TypeId::REFERENCE:
            return EtsObject::FromCoreType(returnValue.GetAs<ObjectHeader *>());
        default:
            LOG(FATAL, COROUTINES) << "Unsupported return type: " << returnType;
            break;
    }
    return nullptr;
}

ExternalIfaceTable *EtsCoroutine::GetExternalIfaceTable()
{
    auto *worker = GetWorker();
    auto *table = worker->GetLocalStorage().Get<CoroutineWorker::DataIdx::EXTERNAL_IFACES, ExternalIfaceTable *>();
    if (table != nullptr) {
        return table;
    }
    return &emptyExternalIfaceTable_;
}

void EtsCoroutine::OnHostWorkerChanged()
{
    // update the interop context pointer
    auto *worker = GetWorker();
    auto *ptr = worker->GetLocalStorage().Get<CoroutineWorker::DataIdx::INTEROP_CTX_PTR, void *>();
    GetLocalStorage().Set<DataIdx::INTEROP_CTX_PTR>(ptr);
}

}  // namespace ark::ets
