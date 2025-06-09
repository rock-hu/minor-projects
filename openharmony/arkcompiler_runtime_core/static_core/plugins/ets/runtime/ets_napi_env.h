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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_ETS_NAPI_ENV_H
#define PANDA_PLUGINS_ETS_RUNTIME_ETS_NAPI_ENV_H

#include "plugins/ets/runtime/ani/ani.h"
#include "plugins/ets/runtime/mem/ets_reference.h"
#include "plugins/ets/runtime/napi/ets_napi.h"

namespace ark::ets {

class EtsCoroutine;
class PandaEtsVM;
using EtsThrowable = EtsObject;

class PandaEtsNapiEnv : public ani_env, public EtsEnv {  // NOLINT(fuchsia-multiple-inheritance)
public:
    static Expected<PandaEtsNapiEnv *, const char *> Create(EtsCoroutine *coroutine,
                                                            mem::InternalAllocatorPtr allocator);
    PANDA_PUBLIC_API static PandaEtsNapiEnv *GetCurrent();

    PandaEtsNapiEnv(EtsCoroutine *coroutine, PandaUniquePtr<EtsReferenceStorage> referenceStorage);
    ~PandaEtsNapiEnv() = default;

    EtsCoroutine *GetEtsCoroutine() const
    {
        return coroutine_;
    }

    PandaEtsVM *GetEtsVM() const;

    EtsReferenceStorage *GetEtsReferenceStorage() const
    {
        return referenceStorage_.get();
    }

    static PandaEtsNapiEnv *ToPandaEtsEnv(EtsEnv *env)
    {
        return static_cast<PandaEtsNapiEnv *>(env);
    }

    static PandaEtsNapiEnv *FromEtsEnv(EtsEnv *env)
    {
        return static_cast<PandaEtsNapiEnv *>(env);
    }

    static PandaEtsNapiEnv *FromAniEnv(ani_env *env)
    {
        return static_cast<PandaEtsNapiEnv *>(env);
    }

    void SetException(EtsThrowable *thr);
    EtsThrowable *GetThrowable() const;
    bool HasPendingException() const;
    void ClearException();
    void FreeInternalMemory();

    NO_COPY_SEMANTIC(PandaEtsNapiEnv);
    NO_MOVE_SEMANTIC(PandaEtsNapiEnv);

private:
    EtsCoroutine *coroutine_;
    PandaUniquePtr<EtsReferenceStorage> referenceStorage_;
};

using PandaEnv = PandaEtsNapiEnv;

}  // namespace ark::ets

#endif  // PANDA_PLUGINS_ETS_RUNTIME_ETS_NAPI_ENV_H
