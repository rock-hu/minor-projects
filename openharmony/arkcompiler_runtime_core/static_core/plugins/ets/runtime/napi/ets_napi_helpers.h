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

#ifndef PANDA_PLUGINS_ETS_RUNTIME_NAPI_ETS_NAPI_HELPERS_H
#define PANDA_PLUGINS_ETS_RUNTIME_NAPI_ETS_NAPI_HELPERS_H

namespace ark::ets::napi {
const void *GetEtsNapiEntryPoint();
const void *GetEtsNapiCriticalEntryPoint();
}  // namespace ark::ets::napi

#endif  // PANDA_PLUGINS_ETS_RUNTIME_NAPI_ETS_NAPI_HELPERS_H
