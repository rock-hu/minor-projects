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
* 最佳实践: 应用并发设计 并发任务管理 多任务关联执行（串行顺序依赖）
*/
// [Start seq_runner]
import { taskpool } from '@kit.ArkTS';

@Concurrent
function additionDelay(delay: number): void {
  let start: number = new Date().getTime();
  while (new Date().getTime() - start < delay) {
    continue;
  }
}

@Concurrent
function waitForRunner(resString: string): string {
  return resString;
}

async function seqRunner() {
  let result: string = '';
  let task1: taskpool.Task = new taskpool.Task(additionDelay, 300);
  let task2: taskpool.Task = new taskpool.Task(additionDelay, 200);
  let task3: taskpool.Task = new taskpool.Task(additionDelay, 100);
  let task4: taskpool.Task = new taskpool.Task(waitForRunner, 50);

  let runner: taskpool.SequenceRunner = new taskpool.SequenceRunner();
  runner.execute(task1).then(() => {
    result += 'a';
  });
  runner.execute(task2).then(() => {
    result += 'b';
  });
  runner.execute(task3).then(() => {
    result += 'c';
  });
  await runner.execute(task4);
  console.info('SequenceRunner: result is ' + result);
}

// [End seq_runner]