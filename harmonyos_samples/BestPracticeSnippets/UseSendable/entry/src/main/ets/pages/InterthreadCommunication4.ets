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
* 最佳实践: 应用并发设计 线程间通信 线程间不可变数据共享
*/
// [Start globalconfig]
import { worker } from '@kit.ArkTS';
import { freezeObj } from './freezeObj';
import { hilog } from '@kit.PerformanceAnalysisKit';
// [StartExclude globalconfig]
const DOMAIN = 0x0000;
const TAG = 'ConcurrencyCapabilitySelection2';
const FORMAT = '%{public}s';
// [EndExclude globalconfig]
@Sendable
export class GlobalConfig {
  // Some configuration properties and methods
  init() {
    // Initialization-related logic
    freezeObj(this) // Freeze the current object after initialization is completed.
  }
}

try {
  let globalConfig = new GlobalConfig();
  globalConfig.init();
  const workerInstance = new worker.ThreadWorker('entry/ets/workers/Worker.ets`', { name: 'Worker1' });
  workerInstance.postMessage(globalConfig);
} catch (err) {
  hilog.error(DOMAIN, TAG, FORMAT, `postMessage failed. Cause code: ${err.code},message: ${err.message}`);
}
// [End globalconfig]