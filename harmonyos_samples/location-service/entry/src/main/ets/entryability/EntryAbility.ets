/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { UIAbility, AbilityConstant, Want, abilityAccessCtrl } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from '../utils/Logger';

const TAG: string = 'EntryAbility';

export default class EntryAbility extends UIAbility {
  async onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    Logger.info(TAG, `[Demo] EntryAbility onCreate`);
    let atManager = abilityAccessCtrl.createAtManager();
    try {
      atManager.requestPermissionsFromUser(this.context,
        ['ohos.permission.INTERNET', 'ohos.permission.LOCATION', 'ohos.permission.APPROXIMATELY_LOCATION'])
        .then((data) => {
          Logger.info(TAG, `data: ${JSON.stringify(data)}`);
        })
        .catch((err: BusinessError) => {
          Logger.error(TAG, `err: ${JSON.stringify(err)}`);
        })
    } catch (err) {
      Logger.error(TAG, `catch err->${JSON.stringify(err)}`);
    }
  }

  onDestroy() {
    Logger.info(TAG, `[Demo] EntryAbility onDestroy`);
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    Logger.info(TAG, `[Demo] EntryAbility onWindowStageCreate`);

    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        Logger.error(TAG, `Failed to load the content. Cause: ${JSON.stringify(err)}`);
        return
      }
      Logger.info(`Succeeded in loading the content. Data: ${JSON.stringify(data)}`);
    })
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    Logger.info(TAG, `[Demo] EntryAbility onWindowStageDestroy`);
  }

  onForeground() {
    // Ability has brought to foreground
    Logger.info(TAG, `[Demo] EntryAbility onForeground`);
  }

  onBackground() {
    // Ability has back to background
    Logger.info(TAG, `[Demo] EntryAbility onBackground`);
  }
}