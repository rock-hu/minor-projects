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

#ifndef NAPI_ARK_INTEROP_LOADER_H
#define NAPI_ARK_INTEROP_LOADER_H

#include "ark_interop_macro.h"
#include "ecmascript/napi/include/jsnapi_expo.h"

COV_EXPORT panda::Local<panda::JSValueRef> LoadCJModule(panda::EcmaVM* vm, const char* libName, void* napiEnv);

#endif // NAPI_ARK_INTEROP_LOADER_H