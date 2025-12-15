/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { Constants } from '../constants/Constants';
import { KeyboardController } from '../model/KeyboardController';
import { EnglishKeyboard } from './EnglishKeyboard';
import { NumberKeyboard } from './NumberKeyboard';
import display from '@ohos.display';

// [Start customKeyboard_start1]
// [Start avoidHeight_start]
@Component
export struct CustomKeyboard {
  // [StartExclude avoidHeight_start]
  // [StartExclude customKeyboard_start1]
  @Consume isCustomKeyboardAttach: boolean;
  @Consume inputText: string;
  @Consume keyboardController: KeyboardController;
  @Consume systemKeyboardHeight: number;
  @Consume customKeyboardHeight: number;
  @Consume bottomRectHeight: number;
  // [EndExclude avoidHeight_start]
  // [EndExclude customKeyboard_start1]

  @State isLandscape: boolean = false;
  private onDisplayChange = () => {
    this.updateOrientation();
  };

  aboutToAppear(): void {
    this.updateOrientation();
    try {
      display.on('change', this.onDisplayChange);
    } catch (e) {
      // ignore
    }
  }

  aboutToDisappear(): void {
    try {
      display.off('change', this.onDisplayChange);
    } catch (e) {
      // ignore
    }
  }

  private updateOrientation() {
    try {
      const d = display.getDefaultDisplaySync();
      this.isLandscape = d.width > d.height;
    } catch (e) {
      this.isLandscape = false;
    }
  }

  private getKeyboardHeightVp(): number | Resource {
    try {
      const d = display.getDefaultDisplaySync();
      const ui = this.getUIContext();
      const screenHeightVp = ui.px2vp(d.height);
      const shortSideVp = ui.px2vp(Math.min(d.width, d.height));
      // Approximate judgment for tablet/large screen
      const isLargeScreen = shortSideVp >= 600;
      // Phone: Portrait/Landscape 36% (no orientation switching supported)
      // Tablet: Portrait 22%, Landscape 33% (reduce height to adapt to large screen)
      const ratio = isLargeScreen
          ? (this.isLandscape ? 0.33 : 0.22)
          :  0.36;
  
      return Math.floor(screenHeightVp * ratio);
    } catch (e) {
      return $r('app.float.keyboard_total_height');
    }
  }

  build() {
    Column() {
      // [StartExclude avoidHeight_start]
      // [StartExclude customKeyboard_start1]
      NumberKeyboard()
        .visibility(this.keyboardController.keyboardType === Constants.NUMBER_KEYBOARD ? Visibility.Visible :
        Visibility.None)
        .margin({ top: $r('app.float.keyboard_margin_top') })

      EnglishKeyboard()
        .visibility(this.keyboardController.keyboardType === Constants.ENGLISH_KEYBOARD ? Visibility.Visible :
        Visibility.None)
        .margin({ top: $r('app.float.keyboard_margin_top') })
      // [EndExclude customKeyboard_start1]
      // [EndExclude avoidHeight_start]
    }
    // [StartExclude customKeyboard_start1]
    .onAreaChange((oldValue: Area, newValue: Area) => {
      this.customKeyboardHeight = Number(newValue.height);
      let avoidHeight: number = (this.isCustomKeyboardAttach ? this.customKeyboardHeight : this.systemKeyboardHeight)
        - this.bottomRectHeight;
      this.keyboardController.changeAvoidHeight(avoidHeight);
    })
    // [StartExclude avoidHeight_start]
    .padding({ left: $r('app.float.keyboard_padding'), right: $r('app.float.keyboard_padding') })
    // [EndExclude customKeyboard_start1]
    .height(this.getKeyboardHeightVp())
    // [StartExclude customKeyboard_start1]
    .width('100%')
    .backgroundColor($r('app.color.keyboard_background_color'))
    // [EndExclude customKeyboard_start1]
    // [EndExclude avoidHeight_start]
  }
}
// [End customKeyboard_start1]
// [End avoidHeight_start]

