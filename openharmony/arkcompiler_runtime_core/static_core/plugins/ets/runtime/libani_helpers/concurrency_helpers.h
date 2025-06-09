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

#ifndef PANDA_RUNTIME_ETS_CONCURRENCY_HELPERS_H
#define PANDA_RUNTIME_ETS_CONCURRENCY_HELPERS_H

#include "ani.h"

namespace arkts::concurrency_helpers {

enum class WorkStatus {
    OK,
    INVALID_ARGS,
    CANCELED,
    ERROR,
};

class AsyncWork;
using ExecuteCallback = void (*)(ani_env *env, void *data);
using CompleteCallback = void (*)(ani_env *env, WorkStatus status, void *data);

__attribute__((visibility("default"))) WorkStatus CreateAsyncWork(ani_env *env, ExecuteCallback execute,
                                                                  CompleteCallback complete, void *data,
                                                                  AsyncWork **result);
__attribute__((visibility("default"))) WorkStatus QueueAsyncWork(ani_env *env, AsyncWork *work);
__attribute__((visibility("default"))) WorkStatus CancelAsyncWork(ani_env *env, AsyncWork *work);
__attribute__((visibility("default"))) WorkStatus DeleteAsyncWork(ani_env *env, AsyncWork *work);

}  // namespace arkts::concurrency_helpers

#endif  // PANDA_RUNTIME_ETS_CONCURRENCY_HELPERS_H
