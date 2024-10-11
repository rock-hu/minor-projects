/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "ecmascript/taskpool/runner.h"

#ifdef ENABLE_QOS
#include "qos.h"
#endif

namespace panda::ecmascript {
Runner::Runner(uint32_t threadNum, const std::function<void(os::thread::native_handle_type)> prologueHook,
    const std::function<void(os::thread::native_handle_type)> epilogueHook)
    : totalThreadNum_(threadNum),
    prologueHook_(prologueHook),
    epilogueHook_(epilogueHook)
{
    for (uint32_t i = 0; i < threadNum; i++) {
        // main thread is 0;
        std::unique_ptr<std::thread> thread = std::make_unique<std::thread>([this, i] {this->Run(i + 1);});
        threadPool_.emplace_back(std::move(thread));
    }

    for (uint32_t i = 0; i < runningTask_.size(); i++) {
        runningTask_[i] = nullptr;
    }
}

void Runner::TerminateTask(int32_t id, TaskType type)
{
    taskQueue_.TerminateTask(id, type);
    LockHolder holder(mtx_);
    for (uint32_t i = 0; i < runningTask_.size(); i++) {
        if (runningTask_[i] != nullptr) {
            if (id != ALL_TASK_ID && id != runningTask_[i]->GetId()) {
                continue;
            }
            if (type != TaskType::ALL && type != runningTask_[i]->GetTaskType()) {
                continue;
            }
            runningTask_[i]->Terminated();
        }
    }
}

void Runner::TerminateThread()
{
    TerminateTask(ALL_TASK_ID, TaskType::ALL);
    taskQueue_.Terminate();

    LockHolder holder(mtxPool_);
    uint32_t threadNum = threadPool_.size();
    for (uint32_t i = 0; i < threadNum; i++) {
        threadPool_.at(i)->join();
    }
    threadPool_.clear();
}

void Runner::ForEachTask(const std::function<void(Task*)> &f)
{
    taskQueue_.ForEachTask(f);
    LockHolder holder(mtx_);
    for (uint32_t i = 0; i < runningTask_.size(); i++) {
        if (runningTask_[i] != nullptr) {
            f(runningTask_[i]);
        }
    }
}

void Runner::SetQosPriority([[maybe_unused]] PriorityMode mode)
{
#ifdef ENABLE_QOS
    switch (mode) {
        case PriorityMode::STW: {
            for (uint32_t threadId : gcThreadId_) {
                OHOS::QOS::SetQosForOtherThread(OHOS::QOS::QosLevel::QOS_USER_INTERACTIVE, threadId);
            }
            return;
        }
        case PriorityMode::FOREGROUND: {
            for (uint32_t threadId : gcThreadId_) {
                OHOS::QOS::SetQosForOtherThread(OHOS::QOS::QosLevel::QOS_USER_INITIATED, threadId);
            }
            return;
        }
        case PriorityMode::BACKGROUND: {
            for (uint32_t threadId : gcThreadId_) {
                OHOS::QOS::ResetQosForOtherThread(threadId);
            }
            return;
        }
        default:
            UNREACHABLE();
            break;
    }
#endif
}

void Runner::RecordThreadId()
{
    LockHolder holder(mtx_);
    gcThreadId_.emplace_back(os::thread::GetCurrentThreadId());
}

void Runner::SetRunTask(uint32_t threadId, Task *task)
{
    LockHolder holder(mtx_);
    runningTask_[threadId] = task;
}

void Runner::Run(uint32_t threadId)
{
    os::thread::native_handle_type thread = os::thread::GetNativeHandle();
    os::thread::SetThreadName(thread, "OS_GC_Thread");
    PrologueHook(thread);
    RecordThreadId();
    while (std::unique_ptr<Task> task = taskQueue_.PopTask()) {
        SetRunTask(threadId, task.get());
        task->Run(threadId);
        SetRunTask(threadId, nullptr);
    }
    EpilogueHook(thread);
}
}  // namespace panda::ecmascript
