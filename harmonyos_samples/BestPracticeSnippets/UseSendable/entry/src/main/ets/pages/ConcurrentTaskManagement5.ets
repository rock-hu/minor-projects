/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
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

/*
* 最佳实践: 应用并发设计 并发任务管理 任务延时调度
*/
// [Start quick_start]
import { taskpool } from '@kit.ArkTS';

@Concurrent
function concurrentTask(num: number): number {
  console.log('Add the task that needs to be executed with delay');
  return num;
}

// create a task
let task: taskpool.Task = new taskpool.Task(concurrentTask, 100);
// Delayed execution of task
taskpool.executeDelayed(3000, task, taskpool.Priority.HIGH).then((value: Object) => {
  // Processing delayed task results
  console.log('taskpool result: ' + value);
});
// [End quick_start]