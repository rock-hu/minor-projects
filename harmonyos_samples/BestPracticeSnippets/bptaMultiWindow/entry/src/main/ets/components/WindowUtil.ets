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
* 最佳实践: 一多窗口适配开发实践
*/

// [Start windowutil]
import { BusinessError } from '@kit.BasicServicesKit';
import { window } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';


@Observed
export class WindowUtil {
  private mainWindowClass?: window.Window;

  // [Start maximize]
  maximize(): void {
    if (this.mainWindowClass!.getWindowStatus() === window.WindowStatusType.FLOATING) {
      this.mainWindowClass!.maximize()
        .then(() => {
          hilog.info(0x0000, 'testTag', '%{public}s', `Succeed in maximizing the window.`);
        })
        .catch((err: BusinessError) => {
          hilog.error(0x0000, 'testTag', `Failed to maximize the window. Code: ${err.code}, message: ${err.message}`,
            JSON.stringify(err) ?? '');
        });
    }
  }
  // [End maximize]

  // [Start recover]
  recover(): void {
    if (this.mainWindowClass!.getWindowStatus() === window.WindowStatusType.FULL_SCREEN) {
      this.mainWindowClass!.recover()
        .then(() => {
          hilog.info(0x0000, 'testTag', '%{public}s', `Succeed in recovering the window.`);
        })
        .catch((err: BusinessError) => {
          hilog.error(0x0000, 'testTag', `Failed to recover the window. Code: ${err.code}, message: ${err.message}`,
            JSON.stringify(err) ?? '');
        });
    }
  }
  // [End recover]
}
// [Start windowutil]