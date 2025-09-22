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
* 最佳实践: 应用并发设计 线程间通信 Sendable实现线程间模块共享（单例模式） （方案二）
*/
// [Start initsingleton]
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { taskpool } from '@kit.ArkTS';
import { Demo } from './demo';
// [StartExclude taskpool]
const DOMAIN = 0x0000;
const TAG = 'InterthreadCommunication3';
const FORMAT = '%{public}s';
// [EndExclude taskpool]
@Concurrent
function initSingleton(): void {
  let demo = Demo.getInstance();
  demo.init();
  // Notify the main thread that initialization is complete
}

async function executeTaskPool(): Promise<void> {
  let task = new taskpool.Task(initSingleton);
  await taskpool.execute(task).catch((err: BusinessError) => {
    hilog.error(DOMAIN, TAG, FORMAT, `taskpool execute error. Cause code: ${err.code},message: ${err.message}`);
  });
}

executeTaskPool();
// [End initsingleton]

@Entry
@Component
struct Index {
  build() {
    Text('Sendable实现线程间模块共享（单例模式）')
  }
}