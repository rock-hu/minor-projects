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

#ifndef PANDA_LIBPANDABASE_OS_UNIX_SIGHOOK_H_
#define PANDA_LIBPANDABASE_OS_UNIX_SIGHOOK_H_

#include <csignal>
#include <cstdint>
#include "libpandabase/macros.h"

namespace ark {

#if PANDA_TARGET_MACOS && !defined _NSIG
#define _NSIG NSIG
#endif

static constexpr uint64_t SIGHOOK_ALLOW_NORETURN = 0x1UL;

struct SighookAction {
    bool (*scSigaction)(int, siginfo_t *, void *);
    sigset_t scMask;
    uint64_t scFlags;
};

extern "C" void RegisterHookHandler(int signal, const SighookAction *sa);
extern "C" void RemoveHookHandler(int signal, bool (*action)(int, siginfo_t *, void *));
extern "C" void CheckOldHookHandler(int signal);
PANDA_PUBLIC_API void ClearSignalHooksHandlersArray();

// running on mobile, actually use sigchain, here provide sigchain stub to make sure complier success
// the real used function is in libsigchain.a
struct SigchainAction {
    bool (*scSigaction)(int, siginfo_t *, void *);
    sigset_t scMask;
    uint64_t scFlags;
};

extern "C" PANDA_PUBLIC_API void AddSpecialSignalHandlerFn(int signal, SigchainAction *sa);
extern "C" PANDA_PUBLIC_API void RemoveSpecialSignalHandlerFn(int signal, bool (*fn)(int, siginfo_t *, void *));
extern "C" void EnsureFrontOfChain(int signal);

}  // namespace ark

#endif  // PANDA_LIBPANDABASE_OS_UNIX_SIGHOOK_H_
