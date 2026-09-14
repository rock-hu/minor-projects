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

import { display } from '@kit.ArkUI';
import { window } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { CommonConstants as Const } from '../constants/CommonConstants';
import Logger from './Logger';

const TAG = 'DeviceScreen'

export class DeviceScreen {
  /**
   * Get the device size.
   *
   * @returns promise of window.
   */
  public static getDeviceSize(context: common.UIAbilityContext): Promise<null | window.Window> {
    let lastWindow = window.getLastWindow(context).catch((err: Error) => {
      Logger.error(TAG, `getDeviceSize failed, error = ${JSON.stringify(err)}`);
      return null;
    })
    return lastWindow
  }

  /**
   * Get the screen height.
   *
   * @returns screen height.
   */
  public static getDeviceHeight() {
    let height = 0;
    try {
      let displayObject = display.getDefaultDisplaySync();
      let screenPixelHeight = displayObject.height;
      let screenDensityDPI = displayObject.densityDPI;
      return screenPixelHeight * (Const.DEVICE_DPI / screenDensityDPI);
    } catch (err) {
      Logger.error(TAG, `get device height failed, error = ${JSON.stringify(err)}`);
      return height;
    }


  }
}