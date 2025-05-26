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

#include "common_interfaces/thread/thread_holder-inl.h"

#include "common_components/base_runtime/hooks.h"
#include "common_components/common_runtime/src/mutator/mutator.h"
#include "common_interfaces/base_runtime.h"
#include "common_interfaces/thread/base_thread.h"
#include "common_interfaces/thread/thread_holder_manager.h"

namespace panda {
thread_local ThreadHolder *currentThreadHolder = nullptr;

ThreadHolder *ThreadHolder::CreateAndRegisterNewThreadHolder(void *vm)
{
    if (ThreadLocal::IsArkProcessor()) {
        LOG_COMMON(FATAL) << "CreateAndRegisterNewThreadHolder fail";
        return nullptr;
    }
    Mutator* mutator = panda::Mutator::NewMutator();
    CHECK_CC(mutator != nullptr);
    mutator->SetEcmaVMPtr(vm);
    ThreadHolder *holder = mutator->GetThreadHolder();
    BaseRuntime::GetInstance()->GetThreadHolderManager().RegisterThreadHolder(holder);
    return holder;
}

void ThreadHolder::DestroyThreadHolder(ThreadHolder *holder)
{
    holder->TransferToNative();
    BaseRuntime::GetInstance()->GetThreadHolderManager().UnregisterThreadHolder(holder);
}

ThreadHolder *ThreadHolder::GetCurrent()
{
    return currentThreadHolder;
}

void ThreadHolder::SetCurrent(ThreadHolder *holder)
{
    currentThreadHolder = holder;
}

void ThreadHolder::RegisterJSThread(JSThread *jsThread)
{
    DCHECK_CC(!IsInRunningState());
    TransferToRunning();
    DCHECK_CC(jsThread_ == nullptr);
    jsThread_ = jsThread;
    TransferToNative();
}

void ThreadHolder::UnregisterJSThread(JSThread *jsThread)
{
    DCHECK_CC(!IsInRunningState());
    TransferToRunning();
    DCHECK_CC(jsThread_ == jsThread);
    jsThread_ = nullptr;
    if (coroutines_.empty()) {
        ThreadHolder::DestroyThreadHolder(this);
    }
}

void ThreadHolder::RegisterCoroutine(Coroutine *coroutine)
{
    DCHECK_CC(!IsInRunningState());
    TransferToRunning();
    DCHECK_CC(coroutines_.find(coroutine) == coroutines_.end());
    coroutines_.insert(coroutine);
    TransferToNative();
}

void ThreadHolder::UnregisterCoroutine(Coroutine *coroutine)
{
    DCHECK_CC(!IsInRunningState());
    TransferToRunning();
    DCHECK_CC(coroutines_.find(coroutine) != coroutines_.end());
    coroutines_.erase(coroutine);
    if (coroutines_.empty() && jsThread_ == nullptr) {
        ThreadHolder::DestroyThreadHolder(this);
    }
}

bool ThreadHolder::TryBindMutator()
{
    if (ThreadLocal::IsArkProcessor() || ThreadLocal::GetMutator() != nullptr) {
        return false;
    }

    BaseRuntime::GetInstance()->GetThreadHolderManager().BindMutator(this);
    return true;
}

void ThreadHolder::BindMutator()
{
    if (!TryBindMutator()) {
        LOG_COMMON(FATAL) << "BindMutator fail";
        return;
    }
}

void ThreadHolder::UnbindMutator()
{
    BaseRuntime::GetInstance()->GetThreadHolderManager().UnbindMutator(this);
}

void ThreadHolder::WaitSuspension()
{
    mutatorBase_->HandleSuspensionRequest();
}

void ThreadHolder::VisitAllThreads(CommonRootVisitor visitor)
{
    if (jsThread_ != nullptr) {
        VisitJSThread(jsThread_, visitor);
    }
    for (auto *coroutine : coroutines_) {
        // Depending on the integrated so
    }
}

ThreadHolder::TryBindMutatorScope::TryBindMutatorScope(ThreadHolder *holder) : holder_(nullptr)
{
    if (holder->TryBindMutator()) {
        holder_ = holder;
    }
}

ThreadHolder::TryBindMutatorScope::~TryBindMutatorScope()
{
    if (holder_ != nullptr) {
        holder_->UnbindMutator();
        holder_ = nullptr;
    }
}
} // namespace panda
