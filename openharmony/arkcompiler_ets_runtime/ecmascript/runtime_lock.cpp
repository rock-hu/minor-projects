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

#include "ecmascript/mem/heap-inl.h"
#include "ecmascript/runtime_lock.h"
#include "ecmascript/checkpoint/thread_state_transition.h"

namespace panda::ecmascript {
RuntimeLockHolder::RuntimeLockHolder(JSThread *thread, Mutex &mtx)
    : thread_(thread), mtx_(mtx)
{
    if (mtx_.TryLock()) {
        return;
    }
#ifndef NDEBUG
#ifdef USE_CMC_GC
    BaseRuntime::GetInstance()->GetHeap().RequestGC(GcType::ASYNC);  // Trigger CMC FULL GC
#else
    SharedHeap::GetInstance()->CollectGarbage<TriggerGCType::SHARED_FULL_GC, GCReason::OTHER>(thread_);
#endif
#endif
    ThreadStateTransitionScope<JSThread, ThreadState::WAIT> ts(thread_);
    mtx.Lock();
}
}  // namespace panda::ecmascript