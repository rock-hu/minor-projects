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
import { Constants } from '../constants/Constants';
import { TextInputComponent } from '../view/TextInputComponent';

@Entry
@Component
struct MainPage {
  @State bottomPadding: number = Constants.MAIN_PAGE_INITIAL_PADDING;

  aboutToAppear(): void {
    let event: emitter.InnerEvent = {
      eventId: Constants.AVOID_EVENT_ID
    }
    emitter.on(event, (eventData: emitter.EventData) => {
      if (eventData.data) {
        let avoidHeight: number = eventData.data['avoidHeight'];
        if (avoidHeight === 0) {
          this.bottomPadding = Constants.MAIN_PAGE_INITIAL_PADDING;
        } else {
          this.bottomPadding = avoidHeight;
        }
      }
    })
  }

  build() {
    Navigation() {
      Column() {
        Image($r('app.media.arkui_background'))
          .height($r('app.float.main_page_image_height'))
          .width('100%')
          .borderRadius(Constants.IMAGE_BORDER)
          .padding({ left: $r('app.float.main_page_margin'), right: $r('app.float.main_page_margin') })

        TextInputComponent()
      }
      .height('100%')
      .justifyContent(FlexAlign.End)
      .padding({ bottom: this.bottomPadding })
      .expandSafeArea([SafeAreaType.KEYBOARD, SafeAreaType.SYSTEM])
    }
    .mode(NavigationMode.Stack)
    .titleMode(NavigationTitleMode.Full)
    .title($r('app.string.main_page_title'))
  }
}