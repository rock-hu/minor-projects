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
* 最佳实践: 应用并发设计 并发任务管理 多任务优先级调度
*/
// [Start execcolorinfo]
import { taskpool } from '@kit.ArkTS';

function exec(bufferArray: ArrayBuffer): void {
  taskpool.execute(execColorInfo, bufferArray, taskpool.Priority.HIGH);
}

@Concurrent
async function execColorInfo(bufferArray: ArrayBuffer): Promise<ArrayBuffer> {
  if (!bufferArray) {
    return new ArrayBuffer(0);
  }
  const newBufferArr = bufferArray;
  let colorInfo = new Uint8Array(newBufferArr);
  let PIXEL_STEP = 2;
  for (let i = 0; i < colorInfo?.length; i += PIXEL_STEP) {
    // data processing
  }
  return newBufferArr;
}
// [End execcolorinfo]