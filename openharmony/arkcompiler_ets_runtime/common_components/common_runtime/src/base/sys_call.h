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

#ifndef ARK_COMMON_SYSCALL_H
#define ARK_COMMON_SYSCALL_H
#include <ctime>
#ifdef __RTOS__
#include <sys/prctl.h>
#include "hmkernel/futex.h"
#elif defined(_WIN64)
#include <pthread.h>
#elif defined(__APPLE__)
#include <pthread.h>
#include <unistd.h>
#else
#include <sys/prctl.h>
#include "linux/futex.h"
#endif

namespace panda {
#if defined(__linux__) || defined(PANDA_TARGET_OHOS)
int Futex(const volatile int* uaddr, int op, int val);
#endif

pid_t GetTid();

int GetPid();

#if defined(__linux__) || defined(PANDA_TARGET_OHOS)
#ifndef PR_SET_VMA
#define PR_SET_VMA 0x53564d41
#define PR_SET_VMA_ANON_NAME 0
#endif

// prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME), can set name for anon VMA.
// Considering the purpose is for debuging, in windows we just simply remove it.
#define ARK_COMMON_PRCTL(base_address, allocated_size, mmtag) \
    (void)prctl(PR_SET_VMA, PR_SET_VMA_ANON_NAME, base_address, allocated_size, mmtag)
#endif
} // namespace panda
#endif // ARK_COMMON_SYSCALL_H
