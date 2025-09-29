/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { window } from '@kit.ArkUI';
import { CommonConstants } from 'basic/Index';
import { AccountComponent } from './AccountComponent';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const DOMAIN = 0x0000;
const TAG = 'AccountPage';

@Component
export struct AccountPage {
  @StorageProp('avoidArea') topHeight: number = 0;

  onPageShow() {
    window.getLastWindow(this.getUIContext().getHostContext()).then((windowStage: window.Window) => {
      try {
        let area = windowStage.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
        let topHeight = this.getUIContext().px2vp(area.topRect.height);
        if (topHeight > 0) {
          windowStage.setWindowLayoutFullScreen(true).catch((err: BusinessError) => {
            hilog.error(DOMAIN, TAG, `Failed to set the window layout to full-screen mode. Cause code: ${err.code}, message: ${err.message}`)
          });
        }
      } catch (error) {
        hilog.error(DOMAIN, TAG, `Failed to set the window avoid area. Cause code: ${error.code}, message: ${error.message}`)
      }
    }).catch((error: BusinessError) => {
      hilog.error(DOMAIN, TAG, `Failed to obtain the last window. Cause code: ${error.code}, message: ${error.message}`)
    })
  }

  onPageHide() {
    window.getLastWindow(this.getUIContext().getHostContext()).then((windowStage: window.Window) => {
      if (this.topHeight > 0) {
        windowStage.setWindowLayoutFullScreen(false).catch((error: BusinessError) => {
          hilog.error(DOMAIN, TAG, `Failed to set the window layout to full-screen mode. Cause code: ${error.code}, message: ${error.message}`)
        });
      }
    }).catch((error: BusinessError) => {
      hilog.error(DOMAIN, TAG, `Failed to obtain the last window. Cause code: ${error.code}, message: ${error.message}`)
    })
  }

  build() {
    Row() {
      AccountComponent()
    }
    .height(CommonConstants.FULL_HEIGHT_PERCENT)

  }
}