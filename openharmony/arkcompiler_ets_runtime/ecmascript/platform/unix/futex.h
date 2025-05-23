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

#ifndef ECMASCRIPT_PLATFORM_UNIX_FUTEX_H
#define ECMASCRIPT_PLATFORM_UNIX_FUTEX_H

#include <unistd.h>
#include <linux/futex.h>
#include <sys/syscall.h>

namespace panda::ecmascript {

inline int futex(volatile int *uaddr, int op, int val, const struct timespec *timeout, volatile int *uaddr2, int val3)
{
    return syscall(SYS_futex, uaddr, op, val, timeout, uaddr2, val3);
}

}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_PLATFORM_MUTEX_H
