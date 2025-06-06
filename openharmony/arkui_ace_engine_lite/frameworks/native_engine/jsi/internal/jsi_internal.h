/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ACELITE_JSI_INTERNAL_H
#define OHOS_ACELITE_JSI_INTERNAL_H

#include "jsi_config.h"
#if (ENABLE_JERRY == 1)
#include "jerryscript.h"
#endif

/**
 * Macros for conversions between jsi-values and jerry-values.
 */
#if (ENABLE_JERRY == 1)
#define AS_JERRY_VALUE(jsiValue) (jerry_value_t)(uintptr_t) jsiValue
#define AS_JLENGTH_VALUE(jsiValue) (jerry_length_t)(uintptr_t) jsiValue
#define AS_JSI_VALUE(jValue) (JSIValue)(uintptr_t) jValue
#endif

/**
 * Key words for native module callbacks.
 */
constexpr char ON_TERMINATE[] = "onTerminate";
constexpr char ON_DESTROY_HANDLER[] = "onDestroyHandler";
constexpr char ON_TERMINATE_HANDLER[] = "onTerminateHandler";

#endif // OHOS_ACELITE_JSI_INTERNAL_H
