/**
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "runtime/include/locks.h"
#include "libpandabase/macros.h"
#include "libpandabase/utils/logger.h"
#include "include/thread.h"

#include <memory>
#if defined(USE_ADDRESS_SANITIZER)
#include <sanitizer/lsan_interface.h>
#endif

namespace ark {

static bool g_isInitialized = false;

os::memory::Mutex *Locks::customTlsLock_ = nullptr;
os::memory::Mutex *Locks::userSuspensionLock_ = nullptr;

void Locks::Initialize()
{
    if (!g_isInitialized) {
        Locks::customTlsLock_ = new os::memory::Mutex();
        Locks::userSuspensionLock_ = new os::memory::Mutex();
        g_isInitialized = true;
    }
}

MutatorLock *Locks::NewMutatorLock()
{
    auto *lock = new MutatorLock();
#if defined(USE_ADDRESS_SANITIZER)
    // We intentionally don't delete mutuator lock because it is used
    // by managed thread when it enters to termination loop. This
    // moment is unpredictable so there is no good time to delete the lock.
    // So we ignore such memory leaks by calling __lsan_ignore_object.
    __lsan_ignore_object(lock);
#endif
    return lock;
}

#ifndef NDEBUG

void MutatorLock::ReadLock()
{
#ifndef ARK_HYBRID
    ASSERT(!HasLock());
#endif
    LockT::ReadLock();
    LOG(DEBUG, RUNTIME) << "MutatorLock::ReadLock";
    Thread::GetCurrent()->SetLockState(RDLOCK);
}

void MutatorLock::WriteLock()
{
#ifndef ARK_HYBRID
    ASSERT(!HasLock());
#endif
    LockT::WriteLock();
    LOG(DEBUG, RUNTIME) << "MutatorLock::WriteLock";
    Thread::GetCurrent()->SetLockState(WRLOCK);
}

bool MutatorLock::TryReadLock()
{
    bool ret = LockT::TryReadLock();
    LOG(DEBUG, RUNTIME) << "MutatorLock::TryReadLock";
    if (ret) {
        Thread::GetCurrent()->SetLockState(RDLOCK);
    }
    return ret;
}

bool MutatorLock::TryWriteLock()
{
    bool ret = LockT::TryWriteLock();
    LOG(DEBUG, RUNTIME) << "MutatorLock::TryWriteLock";
    if (ret) {
        Thread::GetCurrent()->SetLockState(WRLOCK);
    }
    return ret;
}

void MutatorLock::Unlock()
{
#ifndef ARK_HYBRID
    ASSERT(HasLock());
#endif
    LockT::Unlock();
    LOG(DEBUG, RUNTIME) << "MutatorLock::Unlock";
    Thread::GetCurrent()->SetLockState(UNLOCKED);
}

MutatorLock::MutatorLockState MutatorLock::GetState() const
{
    return Thread::GetCurrent()->GetLockState();
}

bool MutatorLock::HasLock() const
{
    auto state = Thread::GetCurrent()->GetLockState();
    return state == RDLOCK || state == WRLOCK;
}
#endif  // !NDEBUG

}  // namespace ark
