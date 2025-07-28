/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
//
// Created on 2025/5/20.
//
// Node APIs are not fully supported. To solve the compilation error of the interface cannot be found,
// please include "napi/native_api.h".

/**
 * 最佳实践：CppCrash问题案例分析
 */

#include <cassert>
#include <js_native_api_types.h>

// [Start TriggerCrash6]
static napi_value TriggerCrash(napi_env env, napi_callback_info info)
{
#if 0  //If the value is 0, an error will be reported. If it is 1, it is normal
    void *pc = malloc(1024);
#else
    void *pc = nullptr;
#endif
    assert(pc != nullptr);
    return 0;
}
// [End TriggerCrash6]