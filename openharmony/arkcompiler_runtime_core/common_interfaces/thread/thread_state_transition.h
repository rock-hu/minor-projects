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

#ifndef COMMON_INTERFACES_THREAD_THREAD_STATE_TRANSITION_H
#define COMMON_INTERFACES_THREAD_THREAD_STATE_TRANSITION_H

#include "thread/thread_holder-inl.h"
#include "thread/thread_holder_manager.h"

namespace panda {

// Fixme: T is a temporary mock impl to adapt the current JSThread callback when transfer to RUNNING,
// actually it should always be ThreadHolder, and need to be removed later.
template <typename T, ThreadState newState>
class ThreadStateTransitionScope final {
    static_assert(std::is_base_of_v<ThreadHolder, T>);

public:
    explicit ThreadStateTransitionScope(T *self) : self_(self)
    {
        DCHECK_CC(self_ != nullptr);
        if constexpr (newState == ThreadState::RUNNING) {
            hasSwitchState_ = self_->TransferToRunningIfInNative();
        } else {
            hasSwitchState_ = self_->TransferToNativeIfInRunning();
        }
    }

    ~ThreadStateTransitionScope()
    {
        if (hasSwitchState_) {
            if constexpr (newState == ThreadState::RUNNING) {
                self_->TransferToNative();
            } else {
                self_->TransferToRunning();
            }
        }
    }

private:
    T *self_;
    ThreadState oldState_;
    bool hasSwitchState_ {false};
    NO_COPY_SEMANTIC_CC(ThreadStateTransitionScope);
};

class ThreadSuspensionScope final {
public:
    explicit ThreadSuspensionScope(ThreadHolder *self) : scope_(self)
    {
        DCHECK_CC(!self->IsInRunningState());
    }

    ~ThreadSuspensionScope() = default;

private:
    ThreadStateTransitionScope<ThreadHolder, ThreadState::IS_SUSPENDED> scope_;
    NO_COPY_SEMANTIC_CC(ThreadSuspensionScope);
};

class ThreadNativeScope final {
public:
    explicit ThreadNativeScope(ThreadHolder *self) : scope_(self)
    {
        DCHECK_CC(!self->IsInRunningState());
    }

    ~ThreadNativeScope() = default;

private:
    ThreadStateTransitionScope<ThreadHolder, ThreadState::NATIVE> scope_;
    NO_COPY_SEMANTIC_CC(ThreadNativeScope);
};

// Fixme: T is a temporary mock impl to adapt the current JSThread callback when transfer to RUNNING,
// actually it should always be ThreadHolder, and need to be removed later.
template <typename T>
class ThreadManagedScope final {
    static_assert(std::is_base_of_v<ThreadHolder, T>);

public:
    explicit ThreadManagedScope(T *self) : scope_(self) {}

    ~ThreadManagedScope() = default;

private:
    ThreadStateTransitionScope<T, ThreadState::RUNNING> scope_;
    NO_COPY_SEMANTIC_CC(ThreadManagedScope);
};

// Fixme: T is a temporary mock impl to adapt the current JSThread callback when transfer to RUNNING,
// actually it should always be ThreadHolder, and need to be removed later.
template <typename T>
class SuspendAllScope final {
    static_assert(std::is_base_of_v<ThreadHolder, T>);

public:
    explicit SuspendAllScope(T *self) : self_(self), scope_(self)
    {
#if defined(TODO_MACRO)
        TRACE_GC(GCStats::Scope::ScopeId::SuspendAll, SharedHeap::GetInstance()->GetEcmaGCStats());
        ECMA_BYTRACE_NAME(HITRACE_LEVEL_MAX, HITRACE_TAG_ARK, "SuspendAll", "");
#endif
        BaseRuntime::GetInstance()->GetThreadHolderManager().SuspendAll(self_);
    }

    ~SuspendAllScope()
    {
#if defined(TODO_MACRO)
        TRACE_GC(GCStats::Scope::ScopeId::ResumeAll, SharedHeap::GetInstance()->GetEcmaGCStats());
        ECMA_BYTRACE_NAME(HITRACE_LEVEL_MAX, HITRACE_TAG_ARK, "ResumeAll", "");
#endif
        BaseRuntime::GetInstance()->GetThreadHolderManager().ResumeAll(self_);
    }

private:
    T *self_;
    ThreadStateTransitionScope<T, ThreadState::IS_SUSPENDED> scope_;
    NO_COPY_SEMANTIC_CC(SuspendAllScope);
};
}  // namespace panda
#endif  // COMMON_INTERFACES_THREAD_THREAD_STATE_TRANSITION_H
