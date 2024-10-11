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

#ifndef PANDA_LIBPANDABASE_UTILS_TIME_H_
#define PANDA_LIBPANDABASE_UTILS_TIME_H_

#include <cstdint>

#include "libpandabase/macros.h"

WEAK_FOR_LTO_START

namespace ark::time {

/// Return current time in milliseconds
PANDA_PUBLIC_API uint64_t GetCurrentTimeInMillis(bool needSystem = false);

/// Return current time in microseconds
PANDA_PUBLIC_API uint64_t GetCurrentTimeInMicros(bool needSystem = false);

/// Return current time in nanoseconds
PANDA_PUBLIC_API uint64_t GetCurrentTimeInNanos(bool needSystem = false);

}  // namespace ark::time

WEAK_FOR_LTO_END

#endif  // PANDA_LIBPANDABASE_UTILS_TIME_H_
