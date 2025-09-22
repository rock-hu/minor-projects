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
* 最佳实践: 应用并发设计 并发能力选择 耗时任务并发执行场景
*/
// [Start taskpool]
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { taskpool } from '@kit.ArkTS';
// [StartExclude taskpool]
const DOMAIN = 0x0000;
const TAG = 'ConcurrencyCapabilitySelection2';
const FORMAT = '%{public}s';
// [EndExclude taskpool]
@Concurrent
async function foo(a: number, b: number) {
  return a + b;
}

taskpool.execute(foo, 1, 2).then((ret: Object) => { // Result processing
  hilog.info(DOMAIN, TAG, FORMAT, 'Return:' + ret);
}).catch((err: BusinessError) => {
  hilog.error(DOMAIN, TAG, FORMAT, `taskpool execute error. Cause code: ${err.code},message: ${err.message}`);
});
// [End taskpool]