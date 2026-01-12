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

import { emitter } from '@kit.BasicServicesKit';
import { TextInputComponent } from '../view/TextInputComponent';

// [Start MainPage_start]
// [Start MainPageKeyboard_start]
@Entry
@Component
struct MainPage {
  // [StartExclude MainPageKeyboard_start]
  @State bottomPadding: number = 210;
  // [EndExclude MainPageKeyboard_start]
  @State isKeyboardShown: boolean = false;
  // [StartExclude MainPageKeyboard_start]
  aboutToAppear(): void {
    let event: emitter.InnerEvent = {
      eventId: 1
    }
    emitter.on(event, (eventData: emitter.EventData) => {
      if (eventData.data) {
        let avoidHeight: number = eventData.data['avoidHeight'];
        if (avoidHeight === 0) {
          this.bottomPadding = 210;
        } else {
          this.bottomPadding = avoidHeight;
        }
      }
    })
  }
  // [EndExclude MainPageKeyboard_start]
  build() {
    Navigation() {
      Column() {
        // [StartExclude MainPage_start]
        // [StartExclude MainPageKeyboard_start]
        Image($r('app.media.arkui_background'))
          .height($r('app.float.main_page_image_height'))
          .width('100%')
          .borderRadius(12)
          .padding({ left: $r('app.float.main_page_margin'), right: $r('app.float.main_page_margin') })

        TextInputComponent({ isKeyboardShown: this.isKeyboardShown })
        // [EndExclude MainPageKeyboard_start]
        // [EndExclude MainPage_start]
      }
      // [StartExclude MainPageKeyboard_start]
      .padding({ bottom: this.bottomPadding })
      // [StartExclude MainPage_start]
      .height('100%')
      .justifyContent(FlexAlign.End)
      .expandSafeArea([SafeAreaType.KEYBOARD, SafeAreaType.SYSTEM])
      // [EndExclude MainPageKeyboard_start]
      // [EndExclude MainPage_start]
    }
    .onClick(() => {
      if (this.isKeyboardShown) {
        this.isKeyboardShown = false;
      }
    })
    .mode(NavigationMode.Stack)
    .titleMode(NavigationTitleMode.Full)
    .title($r('app.string.main_page_title'))
  }
}
// [End MainPageKeyboard_start]
// [End MainPage_start]