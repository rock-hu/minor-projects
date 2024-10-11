/**
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

#include "plugins/ets/runtime/napi/ets_napi_internal.h"

namespace ark::ets::napi {

extern "C" ets_int ETS_GetDefaultVMInitArgs(EtsVMInitArgs *vmArgs)
{
    return _internal_ETS_GetDefaultVMInitArgs(vmArgs);
}

extern "C" ets_int ETS_GetCreatedVMs(EtsVM **vmBuf, ets_size bufLen, ets_size *nVms)
{
    return _internal_ETS_GetCreatedVMs(vmBuf, bufLen, nVms);
}

extern "C" ETS_EXPORT ets_int ETS_CreateVM(EtsVM **pVm, EtsEnv **pEnv, EtsVMInitArgs *vmArgs)
{
    return _internal_ETS_CreateVM(pVm, pEnv, vmArgs);
}
}  // namespace ark::ets::napi
