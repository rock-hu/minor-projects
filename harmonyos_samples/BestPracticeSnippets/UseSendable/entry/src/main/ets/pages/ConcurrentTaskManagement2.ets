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
* 最佳实践: 应用并发设计 并发任务管理 多任务关联执行（树状依赖）
*/
// [Start updatesab]
import { taskpool } from '@kit.ArkTS';

@Concurrent
function updateSAB(args: Uint32Array) {
  if (args[0] == 0) {
    args[0] = 100;
    return 100;
  } else if (args[0] == 100) {
    args[0] = 200;
    return 200;
  } else if (args[0] == 200) {
    args[0] = 300;
    return 300;
  }
  return 0;
}

let sab = new SharedArrayBuffer(20);
let typedArray = new Uint32Array(sab);
let task1 = new taskpool.Task(updateSAB, typedArray);
let task2 = new taskpool.Task(updateSAB, typedArray);
let task3 = new taskpool.Task(updateSAB, typedArray);
task1.addDependency(task2);
task2.addDependency(task3);
taskpool.execute(task1).then((res: object) => {
  console.info('taskpool:: execute task1 res: ' + res);
})
taskpool.execute(task2).then((res: object) => {
  console.info('taskpool:: execute task2 res: ' + res);
})
taskpool.execute(task3).then((res: object) => {
  console.info('taskpool:: execute task3 res: ' + res);
})
// [End updatesab]