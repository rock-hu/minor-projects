/*
 * Copyright (c) 2021-2024 Huawei Device Co., Ltd.
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

#include <cctype>
#include "assembly-type.h"
#include "ets_coroutine.h"
#include "handle_scope.h"
#include "include/mem/panda_containers.h"
#include "macros.h"
#include "mem/mem.h"
#include "mem/vm_handle.h"
#include "types/ets_array.h"
#include "types/ets_box_primitive.h"
#include "types/ets_class.h"
#include "types/ets_method.h"
#include "types/ets_object.h"
#include "types/ets_type.h"
#include "types/ets_box_primitive-inl.h"
#include "types/ets_type_comptime_traits.h"
#include "types/ets_typeapi_create_panda_constants.h"
#include "types/ets_typeapi_method.h"
#include "runtime/include/value-inl.h"

namespace ark::ets::intrinsics {

namespace {
// NOTE: requires parent handle scope
EtsObject *TypeAPIMethodInvokeImplementation(EtsCoroutine *coro, EtsMethod *meth, EtsObject *recv, EtsArray *args)
{
    if (meth->IsAbstract()) {
        ASSERT(recv != nullptr);
        meth = recv->GetClass()->ResolveVirtualMethod(meth);
    }
    size_t methArgsCount = meth->GetNumArgs();
    PandaVector<Value> realArgs {methArgsCount};
    size_t firstRealArg = 0;
    if (!meth->IsStatic()) {
        realArgs[0] = Value(recv->GetCoreType());
        firstRealArg = 1;
    }

    size_t argsLength = args->GetLength();
    if (methArgsCount - firstRealArg != argsLength) {
        UNREACHABLE();
    }

    for (size_t i = 0; i < argsLength; i++) {
        // issue #14003
        // SUPPRESS_CSA_NEXTLINE(alpha.core.WasteObjHeader)
        auto arg = args->GetCoreType()->Get<ObjectHeader *>(i);
        auto argType = meth->GetArgType(firstRealArg + i);
        if (argType == EtsType::OBJECT) {
            realArgs[firstRealArg + i] = Value(arg);
            continue;
        }
        EtsPrimitiveTypeEnumToComptimeConstant(argType, [&](auto type) -> void {
            using T = EtsTypeEnumToCppType<decltype(type)::value>;
            realArgs[firstRealArg + i] =
                Value(EtsBoxPrimitive<T>::FromCoreType(EtsObject::FromCoreType(arg))->GetValue());
        });
    }

    ASSERT(meth->GetPandaMethod()->GetNumArgs() == realArgs.size());
    auto res = meth->GetPandaMethod()->Invoke(coro, realArgs.data());
    if (res.IsReference()) {
        return EtsObject::FromCoreType(res.GetAs<ObjectHeader *>());
    }

    if (meth->GetReturnValueType() == EtsType::VOID) {
        // NOTE(kprokopenko): remove reinterpret_cast when void is synced with runtime
        return reinterpret_cast<EtsObject *>(EtsCoroutine::GetCurrent()->GetUndefinedObject());
    }

    ASSERT(res.IsPrimitive());
    // CC-OFFNXT(G.FMT.14-CPP) project code style
    return EtsPrimitiveTypeEnumToComptimeConstant(meth->GetReturnValueType(), [&](auto type) -> EtsObject * {
        using T = EtsTypeEnumToCppType<decltype(type)::value>;
        return EtsBoxPrimitive<T>::Create(coro, res.GetAs<T>());
    });
}
}  // namespace

extern "C" {
EtsObject *TypeAPIMethodInvoke(EtsString *desc, EtsObject *recv, EtsArray *args)
{
    auto coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope {coro};
    VMHandle<EtsObject> recvHandle {coro, recv->GetCoreType()};
    VMHandle<EtsArray> argsHandle {coro, args->GetCoreType()};
    // this method shouldn't trigger gc, because class is loaded,
    // however static analyzer blames this line
    auto meth = EtsMethod::FromTypeDescriptor(desc->GetMutf8());
    return TypeAPIMethodInvokeImplementation(coro, meth, recvHandle.GetPtr(), argsHandle.GetPtr());
}

EtsObject *TypeAPIMethodInvokeConstructor(EtsString *desc, EtsArray *args)
{
    auto coro = EtsCoroutine::GetCurrent();
    [[maybe_unused]] HandleScope<ObjectHeader *> scope {coro};
    VMHandle<EtsArray> argsHandle {coro, args->GetCoreType()};
    VMHandle<EtsString> descHandle {coro, desc->GetCoreType()};

    auto meth = EtsMethod::FromTypeDescriptor(desc->GetMutf8());
    ASSERT(meth->IsConstructor());
    auto klass = meth->GetClass()->GetRuntimeClass();
    auto initedObj = ObjectHeader::Create(coro, klass);
    ASSERT(initedObj->ClassAddr<Class>() == klass);
    VMHandle<EtsObject> ret {coro, initedObj};
    TypeAPIMethodInvokeImplementation(coro, meth, ret.GetPtr(), argsHandle.GetPtr());
    return ret.GetPtr();
}
}

}  // namespace ark::ets::intrinsics
