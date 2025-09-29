/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { UIAbility } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

export default class EntryAbility extends UIAbility {
  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/ToDoListPage', (err: BusinessError, data) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      // Immersive Adaptation
      this.immersionFuc(windowStage);
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content. Data: %{public}s', JSON.stringify(data) ?? '');
    });
  }

  /**
   * Page immersion.
   */
  immersionFuc(windowStage: window.WindowStage): void {
    try {
      let windowClass: window.Window = windowStage.getMainWindowSync();
      windowClass.setWindowLayoutFullScreen(true).catch((err: BusinessError) => {
        hilog.error(0x0000, 'testTag', '%{public}s',
          `SetWindowLayoutFullScreen failed. Cause code: ${err.code}, message: ${err.message}`);
      });
      let navigationBarArea: window.AvoidArea =
        windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR);
      let area: window.AvoidArea = windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
      AppStorage.setOrCreate<number>('naviIndicatorHeight',
        windowClass.getUIContext().px2vp(navigationBarArea.bottomRect.height));
      AppStorage.setOrCreate<number>('statusBarHeight', windowClass.getUIContext().px2vp(area.topRect.height));
      AppStorage.setOrCreate<window.Window>('windowClass', windowClass);
    } catch (err) {
      hilog.error(0x0000, 'testTag', '%{public}s',
        `immersionFuc failed, error code=${err.code}, message=${err.message}`);
    }
  }
}