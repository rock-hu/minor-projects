/*
* Copyright (c) 2023 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License")
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

import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import Logger from '../util/Logger';

export default class PointMatrixAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    Logger.info('PointMatrixAbility', '%{public}s', 'PointMatrixAbility onCreate');
  }

  onDestroy() {
    Logger.info('PointMatrixAbility', '%{public}s', 'PointMatrixAbility onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability
    Logger.info('PointMatrixAbility', '%{public}s', 'Ability onWindowStageCreate');
    windowStage.loadContent('pages/Pinch', (err, data) => {
      if (err.code) {
        Logger.error('PointMatrixAbility', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      Logger.info('PointMatrixAbility', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    Logger.info('PointMatrixAbility', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground() {
    // Ability has brought to foreground
    Logger.info('PointMatrixAbility', '%{public}s', 'Ability onForeground');
  }

  onBackground() {
    // Ability has back to background
    Logger.info('PointMatrixAbility', '%{public}s', 'Ability onBackground');
  }
}
