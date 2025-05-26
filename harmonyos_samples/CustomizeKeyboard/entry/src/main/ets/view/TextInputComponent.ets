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

import { window } from '@kit.ArkUI';
import { KeyboardController } from '../model/KeyboardController';
import { CustomKeyboard } from './CustomKeyboard';
import { TabView } from './TabView';

// [Start TextInputComponent_start]
@Component
export struct TextInputComponent {
  @Provide inputText: string = '';
  // [StartExclude TextInputComponent_start]
  @Provide keyBoardController: KeyboardController = new KeyboardController();
  @Provide isCustomKeyboardAttach: boolean = true;
  @Provide isTabViewShow: boolean = false;
  @Provide systemKeyboardHeight: number = 0;
  @Provide customKeyboardHeight: number = 0;
  @Provide bottomRectHeight: number = 0;
  textInputController: TextInputController = new TextInputController();

  aboutToAppear(): void {
    this.keyBoardController.textInputController = this.textInputController;
    window.getLastWindow(this.getUIContext().getHostContext(), (err, data) => {
      let avoidArea = data.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR);
      this.bottomRectHeight = this.getUIContext().px2vp(avoidArea.bottomRect.height);
      data.on('keyboardHeightChange', (value) => {
        if (value) {
          this.systemKeyboardHeight = this.getUIContext().px2vp(value);
          let avoidHeight: number =
            (this.isCustomKeyboardAttach ? this.customKeyboardHeight : this.systemKeyboardHeight)
              - this.bottomRectHeight;
          this.keyBoardController.changeAvoidHeight(avoidHeight);
        }
      })
    })
  }
  // [EndExclude TextInputComponent_start]
  // [Start customKeyboard_start]
  build() {
    Column() {
      // [Start TextInput_start]
      // [Start switchover_start]
      TextInput({
        placeholder: $r('app.string.textInput_placeholder_custom'),
        text: this.inputText,
        controller: this.textInputController
      })
        // [StartExclude TextInput_start]
        // [StartExclude TextInputComponent_start]
        // [StartExclude customKeyboard_start]
        // [StartExclude switchover_start]
        .margin({
          top: $r('app.float.main_page_margin_top'),
          left: $r('app.float.main_page_margin'),
          right: $r('app.float.main_page_margin')
        })
          // [EndExclude switchover_start]
          // [EndExclude customKeyboard_start]
        .customKeyboard(this.isCustomKeyboardAttach ? this.customKeyboard() : null)
          // [End switchover_start]
          // [StartExclude customKeyboard_start]
          // [EndExclude TextInput_start]
        .onTextSelectionChange((start: number, end: number) => {
          this.keyBoardController.setCaretPosition(start, end);
        })
          // [StartExclude TextInput_start]
        .onBlur(() => {
          this.isTabViewShow = false;
          this.keyBoardController.changeAvoidHeight(0);
        })
        .onFocus(() => {
          this.isTabViewShow = true;
        })
        .onChange((value) => {
          this.inputText = value;
          this.keyBoardController.onChange(value);
        })
        .onCut(() => {
          this.keyBoardController.onCut();
        })
        .onPaste((value) => {
          this.keyBoardController.onPaste(value);
        })
        .height($r('app.float.main_page_textInput_height'))
      TabView()
        .visibility(this.isTabViewShow ? Visibility.Visible : Visibility.Hidden)
      // [EndExclude customKeyboard_start]
      // [EndExclude TextInputComponent_start]
    }
  }
  // [StartExclude TextInputComponent_start]
  @Builder
  customKeyboard() {
    CustomKeyboard()
  }
  // [EndExclude TextInputComponent_start]
}
// [EndExclude TextInput_start]
// [End TextInput_start]
// [End customKeyboard_start]