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
import { display, window } from '@kit.ArkUI';
import { BreakpointConstants } from 'constantscommon';
import { BusinessError } from '@kit.BasicServicesKit';

export default class EntryAbility extends UIAbility {
  private windowObj?: window.Window;

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    AppStorage.setOrCreate('context', this.context);
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
  }

  onDestroy() {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage) {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.getMainWindow().then((data: window.Window) => {
      this.windowObj = data;
      this.updateBreakpoint(this.windowObj.getWindowProperties().windowRect.width);
      this.windowObj.on('windowSizeChange', (windowSize: window.Size) => {
        this.updateBreakpoint(windowSize.width);
      });
      data.setWindowLayoutFullScreen(true).then(() => {
        hilog.info(0x0000, 'EntryAbility', 'Succeeded in setting the window layout to full-screen mode.');
      }).catch((err: BusinessError) => {
        hilog.info(0x0000, 'EntryAbility',
          `Failed to set the window layout to full-screen mode. Cause: code=${err.code}, message=${err.message}`);
      });

      let type = window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR;
      let avoidArea = data.getWindowAvoidArea(type);
      let bottomRectHeight = avoidArea.bottomRect.height;
      AppStorage.setOrCreate('bottomRectHeight', bottomRectHeight);

      type = window.AvoidAreaType.TYPE_SYSTEM;
      avoidArea = data.getWindowAvoidArea(type);
      let topRectHeight = avoidArea.topRect.height;
      AppStorage.setOrCreate('topRectHeight', topRectHeight);
    }).catch((error: BusinessError) => {
      hilog.error(0x0000, 'testTag', 'Failed to getMainWindow. Cause: ',
        JSON.stringify(error.code) + JSON.stringify(error.message));
    })

    windowStage.loadContent('pages/Index', (err) => {
      AppStorage.setOrCreate('uiContext', windowStage.getMainWindowSync().getUIContext());
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content.');
    });
  }

  private updateBreakpoint(windowWidth: number): void{
    try {
      let windowWidthVp = windowWidth / display.getDefaultDisplaySync().densityPixels;
      let curBp: string = '';
      if (windowWidthVp < BreakpointConstants.BREAKPOINT_VALUE_NUMBER[1]) {
        curBp = BreakpointConstants.BREAKPOINT_SM;
      } else if (windowWidthVp < BreakpointConstants.BREAKPOINT_VALUE_NUMBER[2]) {
        curBp = BreakpointConstants.BREAKPOINT_MD;
      } else {
        curBp = BreakpointConstants.BREAKPOINT_LG;
      }
      AppStorage.setOrCreate('currentBreakpoint', curBp);
    } catch (error) {
      hilog.error(0x0000, 'testTag', 'Failed to updateBreakpoint. Cause: %{public}s', JSON.stringify(error) ?? '');
    }
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground() {
    // Ability has brought to foreground
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground() {
    // Ability has back to background
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onBackground');
  }
}
