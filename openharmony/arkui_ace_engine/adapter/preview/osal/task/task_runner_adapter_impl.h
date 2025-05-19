/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_TASK_RUNNER_ADAPTER_IMPL_H
#define FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_TASK_RUNNER_ADAPTER_IMPL_H

#include <string>

#include "core/common/task_runner_adapter.h"

namespace OHOS::Ace {
class TaskRunnerAdapterImpl final : public TaskRunnerAdapter {
    DECLARE_ACE_TYPE(TaskRunnerAdapterImpl, TaskRunnerAdapter);

public:
    TaskRunnerAdapterImpl() : TaskRunnerAdapter() {};
    ~TaskRunnerAdapterImpl() = default;
    void PostTask(std::function<void()> task, const std::string& caller,
        PriorityType priorityType = PriorityType::LOW) override;
    void PostTaskForTime(std::function<void()> task, uint32_t targetTime, const std::string& caller) override;
    void PostDelayedTask(std::function<void()> task, uint32_t delay, const std::string& caller,
        PriorityType priorityType = PriorityType::LOW) override;
    bool RunsTasksOnCurrentThread() override;
    void Initialize(bool useCurrentEventRunner = false, const std::string& name = "") override;
};
} // namespace OHOS::Ace
#endif // FOUNDATION_ACE_ADAPTER_PREVIEW_OSAL_TASK_RUNNER_ADAPTER_IMPL_H
