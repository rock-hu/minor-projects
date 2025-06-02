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

#ifndef COMMON_COMPONENTS_TASKPOOL_TASKPOOL_H
#define COMMON_COMPONENTS_TASKPOOL_TASKPOOL_H

#include <memory>
#include <mutex>

#include "common_components/taskpool/runner.h"
#include "common_interfaces/base/common.h"
#include "libpandabase/macros.h"

namespace panda {
class PUBLIC_API Taskpool {
public:
    PUBLIC_API static Taskpool *GetCurrentTaskpool();

    Taskpool() = default;
    PUBLIC_API ~Taskpool()
    {
        std::lock_guard<std::mutex> guard(mutex_);
        runner_->TerminateThread();
        isInitialized_ = 0;
    }

    NO_COPY_SEMANTIC(Taskpool);
    NO_MOVE_SEMANTIC(Taskpool);

    void Initialize(int threadNum = DEFAULT_TASKPOOL_THREAD_NUM,
        std::function<void(os::thread::native_handle_type)> prologueHook = nullptr,
        const std::function<void(os::thread::native_handle_type)> epilogueHook = nullptr);
    void Destroy(int32_t id);

    void PostTask(std::unique_ptr<Task> task) const
    {
        ASSERT(isInitialized_ > 0);
        if (isInitialized_ > 0) {
            runner_->PostTask(std::move(task));
        }
    }

    void PostDelayedTask(std::unique_ptr<Task> task, uint64_t delayMilliseconds) const
    {
        ASSERT(isInitialized_ > 0);
        if (isInitialized_ > 0) {
            runner_->PostDelayedTask(std::move(task), delayMilliseconds);
        }
    }

    // Terminate a task of a specified type
    void TerminateTask(int32_t id, TaskType type = TaskType::ALL);

    uint32_t GetTotalThreadNum() const
    {
        return runner_->GetTotalThreadNum();
    }

    bool IsInThreadPool(std::thread::id id) const
    {
        return runner_->IsInThreadPool(id);
    }

    void SetThreadPriority(PriorityMode mode)
    {
        runner_->SetQosPriority(mode);
    }

    void ForEachTask(const std::function<void(Task*)> &f);

private:
    virtual uint32_t TheMostSuitableThreadNum(uint32_t threadNum) const;

    std::unique_ptr<Runner> runner_;
    volatile int isInitialized_ = 0;
    std::mutex mutex_;
};
}  // namespace panda
#endif  // COMMON_COMPONENTS_TASKPOOL_TASKPOOL_H
