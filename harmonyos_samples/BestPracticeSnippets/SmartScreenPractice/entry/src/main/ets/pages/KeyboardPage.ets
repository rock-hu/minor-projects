/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { hilog } from '@kit.PerformanceAnalysisKit';
import { KeyCode } from '@kit.InputKit';

@Entry
@Component
struct KeyboardPage {
  @Consume('pageInfos') pageInfo: NavPathStack;
  @State message: string = '';

  build() {
    Column() {
      Text(this.message)
        .fontSize(16)
      // [Start Keyboard]
      Button($r('app.string.key_event'))
        .defaultFocus(true)
        .width('100%')
        .constraintSize({
          maxWidth: 448
        })
        // Bind key events.
        .onKeyEvent((event) => {
          hilog.info(0x0000, 'SmartScreenPractice', JSON.stringify(event));
          this.message = JSON.stringify(event);
          // The value of keycode is 2119.
          if (event.keyCode === KeyCode.KEYCODE_NUMPAD_ENTER) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // [StartExclude Keyboard]
          // The value of keycode is 2012.
          else if (event.keyCode === KeyCode.KEYCODE_DPAD_UP) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 2013.
          else if (event.keyCode === KeyCode.KEYCODE_DPAD_DOWN) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 2014.
          else if (event.keyCode === KeyCode.KEYCODE_DPAD_LEFT) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 2015.
          else if (event.keyCode === KeyCode.KEYCODE_DPAD_RIGHT) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 2625.
          else if (event.keyCode === KeyCode.KEYCODE_HELP) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 2067.
          else if (event.keyCode === KeyCode.KEYCODE_MENU) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 16.
          else if (event.keyCode === KeyCode.KEYCODE_VOLUME_UP) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // The value of keycode is 17.
          else if (event.keyCode === KeyCode.KEYCODE_VOLUME_DOWN) {
            hilog.info(0x0000, 'SmartScreenPractice', event.keyText);
          }
          // [StartExclude Keyboard]
        })
      // [End Keyboard]
    }
    .padding(16)
    .justifyContent(FlexAlign.SpaceBetween)
    .width('100%')
    .height('100%')
    .backgroundColor($r('sys.color.container_modal_unfocus_background'))
  }
}

@Builder
export function KeyboardPageBuilder() {
  NavDestination() {
    KeyboardPage()
  }
  .hideTitleBar(true)
}