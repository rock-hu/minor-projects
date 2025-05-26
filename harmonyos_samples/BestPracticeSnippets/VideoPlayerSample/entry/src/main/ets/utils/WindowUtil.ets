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
import { CommonConstants, Logger } from '@ohos/MediaService';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
const TAG: string = '[WindowUtil]';

export class WindowUtil {
  private static instance: WindowUtil;
  private windowStage?: window.WindowStage;
  private mainWindowClass?: window.Window;

  public static getInstance() {
    if (!WindowUtil.instance) {
      WindowUtil.instance = new WindowUtil();
    }
    return WindowUtil.instance;
  }

  // [Start set_stage1]
  public setWindowStage(windowStage: window.WindowStage): void {
    let uiContext = windowStage.getMainWindowSync().getUIContext();
    this.windowStage = windowStage;
    this.windowStage.getMainWindow((err, windowClass: window.Window) => {
      // [StartExclude set_stage1]
      if (err.code) {
        Logger.error(TAG, `Failed to obtain the main window. Code:${err.code}, message:${err.message}`);
        return;
      }
      // [EndExclude set_stage1]
      this.mainWindowClass = windowClass;
      const properties = windowClass.getWindowProperties(); // 获取窗口信息
      // [StartExclude set_stage1]
      // Get status bar height.
      let area: window.AvoidArea = windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM);
      let naviBarArea: window.AvoidArea =
        windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR);
      // [EndExclude set_stage1]
      AppStorage.setOrCreate('deviceWidth', properties.windowRect.width); // 设置窗口宽度
      AppStorage.setOrCreate('deviceHeight', properties.windowRect.height); // 设置窗口高度
      // [StartExclude set_stage1]
      AppStorage.setOrCreate('statusBarHeight', uiContext.px2vp(area.topRect.height));
      AppStorage.setOrCreate('navBarHeight', uiContext.px2vp(naviBarArea.bottomRect.height));
      // [EndExclude set_stage1]
    });
  }
  // [End set_stage1]

  // [Start set_main]
  // Select the corresponding parameters according to the rotation strategy of the application itself.
  setMainWindowOrientation(orientation: window.Orientation, callback?: Function): void {
    // [StartExclude set_main]
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    // Setting orientation.
    // [EndExclude set_main]
    this.mainWindowClass.setPreferredOrientation(orientation).then(() => {
      callback?.();
    }).catch((err: BusinessError) => {
      hilog.error(0x0001, TAG, `Failed to set the ${orientation} of main window. Code:${err.code}, message:${err.message}`);
    });
  }
  // [End set_main]

  disableWindowSystemBar(): void {
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    // Set the status bar and navigation bar to be invisible in full-screen mode.
    this.mainWindowClass.setWindowSystemBarEnable([]);
  }

  enableWindowSystemBar(): void {
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    this.mainWindowClass.setWindowSystemBarEnable(['status', 'navigation']);
  }

  setLandscapeMultiWindow(enable: boolean) {
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    if (enable) {
      this.mainWindowClass?.enableLandscapeMultiWindow();
    } else {
      this.mainWindowClass?.disableLandscapeMultiWindow();
    }
  }
  // [Start size_change]
  // [Start about_appear]
  registerOnWindowSizeChange(callback?: (size: window.Size) => void) {
    // [StartExclude about_appear]
    // [StartExclude size_change]
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    // [StartExclude about_appear]
    // [EndExclude size_change]
    this.mainWindowClass.on('windowSizeChange', (size) => {
      //Because the real width of the application window is the screen width in the mobile phone, there is no need to change the width.
      AppStorage.setOrCreate('deviceHeight', size.height);
      callback?.(size);
    });
  }
  // [End about_appear]
  // [End size_change]

  registerOffWindowSizeChange() {
    if (this.mainWindowClass === undefined) {
      Logger.error(`MainWindowClass is undefined`);
      return;
    }
    this.mainWindowClass.off('windowSizeChange');
  }
}