/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

import { DisplayMetrics } from './types';
import window from '@ohos.window';
import { RNOHLogger } from './RNOHLogger';
import display from '@ohos.display';
import { RNOHError } from "./RNOHError"
import UIContext from '@ohos.arkui.UIContext';

const defaultDisplayMetrics: DisplayMetrics = {
  windowPhysicalPixels: {
    width: 0,
    height: 0,
    scale: 1,
    fontScale: 1,
    densityDpi: 480,
  },
  screenPhysicalPixels: {
    width: 0,
    height: 0,
    scale: 1,
    fontScale: 1,
    densityDpi: 480
  },
} as const;

/**
 * @internal
 */
export class DisplayMetricsManager {
  private displayMetrics: DisplayMetrics = defaultDisplayMetrics;
  private logger: RNOHLogger
  private mainWindow: window.Window

  constructor(logger: RNOHLogger) {
    this.logger = logger.clone("DisplayMetricsManager");
  }

  public setMainWindow(mainWindow: window.Window) {
    this.mainWindow = mainWindow;
  }

  public getFoldStatus():display.FoldStatus{
    return display.getFoldStatus()
  }

  public getIsSplitScreenMode():boolean{
    return AppStorage.get("isSplitScreenMode") ?? false
  }

  public setFontSizeScale(fontSizeScale: number){
    this.displayMetrics.screenPhysicalPixels.fontScale = fontSizeScale;
    this.displayMetrics.windowPhysicalPixels.fontScale = fontSizeScale;
    this.updateDisplayMetrics()
  }

  public getFontSizeScale():number{
    return this.displayMetrics.windowPhysicalPixels.fontScale
  }

  public updateWindowSize(windowSize: window.Size | window.Rect) {
    this.displayMetrics.windowPhysicalPixels.height = windowSize.height;
    this.displayMetrics.windowPhysicalPixels.width = windowSize.width;
    this.updateDisplayMetrics()
  }

  public updateDisplayMetrics() {
    try {
      const windowDisplayId = this.mainWindow.getWindowProperties().displayId;
      if (windowDisplayId == undefined) {
        throw new Error("windowDisplayId is undefined!");
      }
      const displayInstance = display.getDisplayByIdSync(windowDisplayId);
      this.displayMetrics = {
        screenPhysicalPixels: {
          width: displayInstance.width,
          height: displayInstance.height,
          scale: displayInstance.densityPixels,
          fontScale: this.displayMetrics.screenPhysicalPixels.fontScale,
          densityDpi: displayInstance.densityDPI,
        },
        windowPhysicalPixels: {
          width: this.displayMetrics.windowPhysicalPixels.width,
          height: this.displayMetrics.windowPhysicalPixels.height,
          scale: displayInstance.densityPixels,
          fontScale: this.displayMetrics.windowPhysicalPixels.fontScale,
          densityDpi: displayInstance.densityDPI,
        }
      };
    }
    catch (err) {
      this.logger.error('Failed to update display size ' + JSON.stringify(err));
    }
  }

  public getDisplayMetrics(): DisplayMetrics {
    if (!this.displayMetrics) {
      throw new RNOHError({ whatHappened: "DisplayMetrics are undefined", howCanItBeFixed: [] });
    }
    return this.displayMetrics;
  }
}