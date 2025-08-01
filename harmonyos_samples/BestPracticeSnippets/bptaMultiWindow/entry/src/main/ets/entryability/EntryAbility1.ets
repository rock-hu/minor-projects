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

// [Start setdefaultorientation]
import { hilog } from "@kit.PerformanceAnalysisKit";
import { BusinessError } from "@kit.BasicServicesKit";
import { window } from "@kit.ArkUI";
import { UIAbility } from "@kit.AbilityKit";

export default class EntryAbility extends UIAbility {
  windowObj?: window.Window;
  uiContext?: UIContext;
  onWindowSizeChange: (data: window.Size) => void = (data: window.Size) => {
    this.setDefaultOrientation();
  }

  setDefaultOrientation(): void {
    let windowRect: window.Rect = this.windowObj!.getWindowProperties().windowRect;
    let windowWidthVp: number = this.uiContext!.px2vp(windowRect.width);
    let windowHeightVp: number = this.uiContext!.px2vp(windowRect.height);

    // If this condition is met, the window can be rotated.
    if (Math.min(windowWidthVp, windowHeightVp) > 348) {
      this.windowObj?.setPreferredOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
    }
    // Otherwise, the window is displayed in portrait mode.
    else {
      this.windowObj?.setPreferredOrientation(window.Orientation.PORTRAIT);
    }
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onWindowStageCreate');
    windowStage.getMainWindow().then((windowObj) => {
      this.windowObj = windowObj;
    }).catch((err: BusinessError) => {
      hilog.error(0x0000, 'testTag', `Failed to obtain the main window. Cause code: ${err.code}, message: ${err.message}`);
    });
    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(0x0000, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err) ?? '');
        return;
      }
      this.uiContext = this.windowObj!.getUIContext();
      this.setDefaultOrientation();
      this.windowObj!.on('windowSizeChange', this.onWindowSizeChange);
      hilog.info(0x0000, 'testTag', 'Succeeded in loading the content.');
    });
  }
}
// [End setdefaultorientation]