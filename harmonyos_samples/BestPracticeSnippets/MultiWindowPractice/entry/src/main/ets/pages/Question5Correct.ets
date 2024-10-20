/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { common } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

@Builder
export function Question5CorrectBuilder() {
  Question5Correct()
}

@Component
export struct Question5Correct {
  @State windowStatus: WindowStatusType = window.WindowStatusType.FULL_SCREEN;

  aboutToAppear(): void {
    let windowClass = (getContext(this) as common.UIAbilityContext).windowStage.getMainWindowSync();
    this.windowStatus = windowClass.getWindowStatus();
    windowClass.on('windowStatusChange', data => {
      this.windowStatus = data;
    })
  }

  build() {
    NavDestination() {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center }) {
        Text($r('app.string.login_page'))
          .fontSize(24)
          .fontWeight(FontWeight.Medium)
          .fontColor($r('sys.color.font_primary'))
          .margin({ top: this.windowStatus === window.WindowStatusType.FULL_SCREEN ? 30 : 0 })

        if (this.windowStatus === window.WindowStatusType.FULL_SCREEN) {
          Text($r('app.string.login_more'))
            .fontSize(16)
            .fontColor($r('sys.color.font_secondary'))

          Image($r('app.media.login_pic'))
            .width(200)
            .height(200)
            .margin({
              bottom: 12,
              top: 35
            })
        }

        Column() {
          TextInput({ placeholder: $r('app.string.account') })
            .maxLength(11)
            .type(InputType.Number)
            .inputStyle()

          Line()
            .width('100%')
            .height(0.5)
            .margin({
              left: 12,
              right: 12
            })
            .backgroundColor('#0D182431')

          TextInput({ placeholder: $r('app.string.password') })
            .maxLength(8)
            .type(InputType.Password)
            .inputStyle()
        }
        .margin({ top: 16 })
        .borderRadius(24)

        Button($r('app.string.login'))
          .width('100%')
          .height(40)
          .fontSize(16)
          .fontWeight(FontWeight.Medium)
          .backgroundColor($r('sys.color.brand'))
          .margin({ bottom: 12, top: 36 })
      }
      .height('100%')
      .width('100%')
      .align(Alignment.Center)
      .padding(12)
    }
    .hideTitleBar(true)
    .backgroundColor('#F1F3F5')
    .padding({ top: AppStorage.get<number>('statusBarHeight') ?? 0 })
  }
}

@Extend(TextInput)
function inputStyle() {
  .placeholderColor($r('sys.color.font_secondary'))
  .height(48)
  .fontSize(16)
  .backgroundColor(Color.White)
  .margin({ top: 4 })
  .padding({ left: 12 })
}