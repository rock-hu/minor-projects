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

@Component
export struct CustomKeyboard {
  @Consume isCustomKeyboardAttach: boolean;
  @Consume inputText: string;
  @Consume keyBoardController: KeyboardController;
  @Consume systemKeyboardHeight: number;
  @Consume customKeyboardHeight: number;
  @Consume bottomRectHeight: number;

  build() {
    Column() {
      NumberKeyboard()
        .visibility(this.keyBoardController.keyBoardType === Constants.NUMBER_KEYBOARD ? Visibility.Visible :
        Visibility.None)
        .margin({ top: $r('app.float.keyboard_margin_top') })

      EnglishKeyboard()
        .visibility(this.keyBoardController.keyBoardType === Constants.ENGLISH_KEYBOARD ? Visibility.Visible :
        Visibility.None)
        .margin({ top: $r('app.float.keyboard_margin_top') })
    }
    .onAreaChange((oldValue: Area, newValue: Area) => {
      this.customKeyboardHeight = Number(newValue.height);
      let avoidHeight: number = (this.isCustomKeyboardAttach ? this.customKeyboardHeight : this.systemKeyboardHeight)
        - this.bottomRectHeight;
      this.keyBoardController.changeAvoidHeight(avoidHeight);
    })
    .padding({ left: $r('app.float.keyboard_padding'), right: $r('app.float.keyboard_padding') })
    .height($r('app.float.keyboard_total_height'))
    .width('100%')
    .backgroundColor($r('app.color.keyboard_background_color'))
  }
}

