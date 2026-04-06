/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
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

import { abilityAccessCtrl, AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

export default class entryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.permissions();
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onCreate');
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'want param:' + JSON.stringify(want) ?? '');
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'launchParam:' + JSON.stringify(launchParam) ?? '');
  }

  onDestroy(): void | Promise<void> {
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability.
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/BillHomePage', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'EntryAbility', `Failed to load the content. Code:${err.code},message: ${err.message}`);
        return;
      }
      hilog.info(0x0000, 'EntryAbility', 'Succeeded in loading the content.');
      try {
        AppStorage.setOrCreate('uiContext', windowStage.getMainWindowSync().getUIContext());
      } catch (err) {
        hilog.error(0x0000, 'EntryAbility', `getMainWindowSync failed. Code:${err.code},message: ${err.message}`);
      }
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources.
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground.
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background.
    hilog.isLoggable(0x0000, 'EntryAbility', hilog.LogLevel.INFO);
    hilog.info(0x0000, 'EntryAbility', '%{public}s', 'Ability onBackground');
  }

  /**
   * Apply for the permission to exchange data between different devices.
   */
  permissions(): void {
    let atManager = abilityAccessCtrl.createAtManager();
    atManager.requestPermissionsFromUser(this.context, ['ohos.permission.DISTRIBUTED_DATASYNC']).then((data) => {
      hilog.info(0x0000, 'EntryAbility', `Data permissions:${data.permissions}`);
    }).catch((err: BusinessError) => {
      hilog.error(0x0000, 'EntryAbility', `request permissions failed, Code:${err.code},message: ${err.message}`);
    });
  }
}