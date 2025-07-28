/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "libpandabase/taskmanager/task_manager.h"

namespace ark::taskmanager {

TaskManager *TaskManager::inst_ = nullptr;

/*static*/
void TaskManager::Start(size_t workerCount, TaskTimeStatsType statsType)
{
    ASSERT_PRINT(inst_ == nullptr, "Try to create task manager while it exists");
    inst_ = new TaskManager(workerCount, statsType);
}

/*static*/
bool TaskManager::IsUsed()
{
    return inst_ != nullptr;
}

/*static*/
void TaskManager::Finish()
{
    ASSERT_PRINT(inst_ != nullptr, "Try to destroy task manager while it doesn't exists");
    delete inst_;
    inst_ = nullptr;
}

/*static*/
TaskQueueInterface *TaskManager::GetTaskQueue(QueueId id)
{
    ASSERT(inst_ != nullptr);
    return inst_->queueSet_.GetQueue(id);
}

/*static*/
void TaskManager::SetWorkersCount(size_t count)
{
    inst_->scheduler_.SetCountOfWorkers(count);
}

/*static*/
size_t TaskManager::GetWorkersCount()
{
    return inst_->scheduler_.GetCountOfWorkers();
}

TaskManager::TaskManager(size_t workerCount, TaskTimeStatsType statsType)
    : waitList_(), queueSet_(&waitList_, statsType), scheduler_(workerCount, &waitList_, &queueSet_)
{
    queueSet_.SetCallbacks([] { TaskManager::inst_->scheduler_.SignalWorkers(); });
}

}  // namespace ark::taskmanager
