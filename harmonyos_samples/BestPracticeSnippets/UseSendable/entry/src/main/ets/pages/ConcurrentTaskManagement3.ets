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
* 最佳实践: 应用并发设计 并发任务管理 多任务同步等待结果（任务组）
*/
// [Start image_processing]
import { taskpool } from '@kit.ArkTS';

// Define asynchronous tasks
@Concurrent
function imageProcessing(arrayBuffer: ArrayBuffer): ArrayBuffer {
  // Here add business logic, the input is ArrayBuffer, and the output is an ArrayBuffer storing the parsed results.
  let message: ArrayBuffer = arrayBuffer;
  return message;
}

let taskGroup: taskpool.TaskGroup = new taskpool.TaskGroup();
let TASK_POOL_CAPACITY: number = 10;

function histogramStatistic(pixelBuffer: ArrayBuffer): void {
  // Add tasks to the task group
  let byteLengthOfTask: number = pixelBuffer.byteLength;
  for (let i = 0; i < TASK_POOL_CAPACITY; i++) {
    let dataSlice: Object = (i === TASK_POOL_CAPACITY - 1) ? pixelBuffer.slice(i * byteLengthOfTask) :
    pixelBuffer.slice(i * byteLengthOfTask, (i + 1) * byteLengthOfTask);
    let task: taskpool.Task = new taskpool.Task(imageProcessing, dataSlice);
    taskGroup.addTask(task);
  }
  taskpool.execute(taskGroup, taskpool.Priority.HIGH).then((res: Object[]): void | Promise<void> => {
    // Result data processing
  }).catch((error: Error) => {
    console.error(`taskpool excute error: ${error}`);
  })
}
// [End image_processing]