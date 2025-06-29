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

import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';

export default class EntryAbility extends UIAbility {
  windowStage: window.WindowStage | undefined = undefined;
  tag: string = 'EntryAbility';
  domain: number = 0x0000;
  want: Want | undefined = undefined;
  launchParam: AbilityConstant.LaunchParam | undefined = undefined;
  windowStageEventFunc: (data: window.WindowStageEventType) => void = (data: window.WindowStageEventType): void => {
    hilog.info(
      this.domain,
      'Succeeded in enabling the listener for window stage event changes. Data: %{public}',
      JSON.stringify(data) ?? ''
    );
  }

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.want = want;
    this.launchParam = launchParam;
    hilog.info(this.domain, this.tag, 'Ability is onCreate.');
  }

  onDestroy() {
    hilog.info(this.domain, this.tag, 'Ability is onDestroy.');
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    this.windowStage = windowStage;

    // Set WindowStage event subscription (onFocus/onBlur or visible/invisible).
    try {
      windowStage.on('windowStageEvent', this.windowStageEventFunc);
    } catch (exception) {
      hilog.error(
        this.domain,
        'Failed to enable the listener for window stage event changes. Cause: %{public}',
        JSON.stringify(exception) ?? ''
      );
    }

    // Main window is created, set main page for this ability
    // Set the UI page to be loaded.
    windowStage.loadContent('pages/LifeCyclePage', (err, data) => {
      if (err.code) {
        hilog.error(this.domain, 'testTag', 'Failed to load the content. Cause: %{public}s',
          JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(this.domain, this.tag, 'Succeeded in loading the content. Data: %{public}s',
        JSON.stringify(data) ?? '');
    });
  }

  onWindowStageDestroy() {
    // Releasing UI Resources
    // Unregisters the WindowStage event for getting/out of focus in onWindowStageDestroy()
    try {
      this.windowStage?.off('windowStageEvent');
    } catch (exception) {
      hilog.error(this.domain, 'Failed to disable the listener for window stage event changes. Cause: %{public}s',
        JSON.stringify(exception));
    }
  }

  onForeground() {
    // Ability has brought to foreground
    hilog.info(this.domain, this.tag, 'Ability is onForeground.');
  }

  onBackground() {
    // Ability has back to background
    hilog.info(this.domain, this.tag, 'Ability is onBackground.');
  }
}
