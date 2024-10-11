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

#ifndef ECMASCRIPT_TASKPOOL_TASK_QUEUE_H
#define ECMASCRIPT_TASKPOOL_TASK_QUEUE_H

#include <algorithm>
#include <atomic>
#include <deque>
#include <memory>
#include <functional>

#include "ecmascript/taskpool/task.h"
#include "ecmascript/platform/mutex.h"

namespace panda::ecmascript {
class TaskQueue {
public:
    TaskQueue() = default;
    ~TaskQueue() = default;

    NO_COPY_SEMANTIC(TaskQueue);
    NO_MOVE_SEMANTIC(TaskQueue);

    void PostTask(std::unique_ptr<Task> task);
    std::unique_ptr<Task> PopTask();

    void Terminate();
    void TerminateTask(int32_t id, TaskType type);
    void ForEachTask(const std::function<void(Task*)> &f);

private:
    std::deque<std::unique_ptr<Task>> tasks_;

    std::atomic_bool terminate_ = false;
    Mutex mtx_;
    ConditionVariable cv_;
};
}  // namespace panda::ecmascript
#endif  // ECMASCRIPT_TASKPOOL_TASK_QUEUE_H
