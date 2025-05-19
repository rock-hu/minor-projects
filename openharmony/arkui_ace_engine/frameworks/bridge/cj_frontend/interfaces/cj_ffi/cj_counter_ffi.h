/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACE_FRAMEWORK_CJ_COUNTER_FFI_H
#define OHOS_ACE_FRAMEWORK_CJ_COUNTER_FFI_H

#include <cstdint>
#include <string>

#include "bridge/cj_frontend/interfaces/cj_ffi/cj_common_ffi.h"
#include "bridge/cj_frontend/interfaces/cj_ffi/cj_macro.h"

extern "C" {
CJ_EXPORT void FfiOHOSAceFrameworkCounterCreate();
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetWidth(double value, int32_t unit);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetHeight(double value, int32_t unit);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetSize(double width, int32_t widthUnit, double height, int32_t heightUnit);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetControlWidth(double value, int32_t unit);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetStateChange(bool state);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetBackgroundColor(uint32_t color);
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetOnInc(void (*callback)());
CJ_EXPORT void FfiOHOSAceFrameworkCounterSetOnDec(void (*callback)());
CJ_EXPORT void FfiOHOSAceFrameworkCounterEnableDec(bool enable);
CJ_EXPORT void FfiOHOSAceFrameworkCounterEnableInc(bool enable);
}

#endif // OHOS_ACE_FRAMEWORK_CJ_COUNTER_FFI_H
