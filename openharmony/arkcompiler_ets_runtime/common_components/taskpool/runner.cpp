/*
 * Copyright (c) 2021-2025 Huawei Device Co., Ltd.
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

#include "common_components/taskpool/runner.h"

#include "libpandabase/os/thread.h"
#ifdef ENABLE_QOS
#include "qos.h"
#endif

#ifdef ENABLE_RSS
#include "res_sched_client.h"
#endif

namespace common {
Runner::Runner(uint32_t threadNum, const std::function<void(native_handle_type)> prologueHook,
    const std::function<void(native_handle_type)> epilogueHook)
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
    std::lock_guard<std::mutex> guard(mtx_);
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

    std::lock_guard<std::mutex> guard(mtxPool_);
    uint32_t threadNum = threadPool_.size();
    for (uint32_t i = 0; i < threadNum; i++) {
        threadPool_.at(i)->join();
    }
    threadPool_.clear();
}

void Runner::ForEachTask(const std::function<void(Task*)> &f)
{
    taskQueue_.ForEachTask(f);
    std::lock_guard<std::mutex> guard(mtx_);
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

void Runner::SetRssPriority([[maybe_unused]] RssPriorityType type)
{
#ifdef ENABLE_RSS
    uint64_t pid = getpid();
    int64_t status = static_cast<int64_t>(type);
    for (uint32_t threadId : gcThreadId_) {
        std::unordered_map<std::string, std::string> payLoad = { { "pid", std::to_string(pid) },
                                                    { "tid", std::to_string(threadId) } };
        OHOS::ResourceSchedule::ResSchedClient::GetInstance()
            .ReportData(OHOS::ResourceSchedule::ResType::RES_TYPE_GC_THREAD_QOS_STATUS_CHANGE,
            status, payLoad);
    }
#endif
}

void Runner::RecordThreadId()
{
    std::lock_guard<std::mutex> guard(mtx_);
    gcThreadId_.emplace_back(panda::os::thread::GetCurrentThreadId());
}

void Runner::SetRunTask(uint32_t threadId, Task *task)
{
    std::lock_guard<std::mutex> guard(mtx_);
    runningTask_[threadId] = task;
}

void Runner::Run(uint32_t threadId)
{
    native_handle_type thread = panda::os::thread::GetNativeHandle();
    panda::os::thread::SetThreadName(thread, "OS_GC_Thread");
    PrologueHook(thread);
    RecordThreadId();
    while (std::unique_ptr<Task> task = taskQueue_.PopTask()) {
        SetRunTask(threadId, task.get());
        task->Run(threadId);
        SetRunTask(threadId, nullptr);
    }
    EpilogueHook(thread);
}
}  // namespace common
