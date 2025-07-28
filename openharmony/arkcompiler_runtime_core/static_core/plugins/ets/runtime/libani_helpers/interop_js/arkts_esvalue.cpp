/**
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "arkts_esvalue.h"
#include "plugins/ets/runtime/interop_js/native_api/arkts_esvalue.h"

// NOLINTNEXTLINE(readability-identifier-naming)
extern "C" bool arkts_esvalue_unwrap(ani_env *env, ani_object esValue, void **result)
{
    return ark::ets::interop::js::UnwrapESValue(env, esValue, result);
}