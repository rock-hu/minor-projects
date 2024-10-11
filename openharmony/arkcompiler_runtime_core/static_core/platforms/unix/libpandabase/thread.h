/**
 * Copyright (c) 2022-2024 Huawei Device Co., Ltd.
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

#ifndef PANDA_LIBPANDABASE_OS_UNIX_THREAD_H_
#define PANDA_LIBPANDABASE_OS_UNIX_THREAD_H_

#include "libpandabase/macros.h"

WEAK_FOR_LTO_START

namespace ark::os::thread {
constexpr int LOWEST_PRIORITY = 19;

PANDA_PUBLIC_API int GetPriority(int threadId);
PANDA_PUBLIC_API int SetPriority(int threadId, int prio);
}  // namespace ark::os::thread

WEAK_FOR_LTO_END

#endif  // PANDA_LIBPANDABASE_OS_UNIX_THREAD_H_
