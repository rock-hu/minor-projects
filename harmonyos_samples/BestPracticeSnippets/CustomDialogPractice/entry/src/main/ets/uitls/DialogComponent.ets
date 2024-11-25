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

import { promptAction } from "@kit.ArkUI";

// DocsCode 1
@Component
export struct buildText {
  @Prop message: string = '';

  build() {
    Column() {
      Row() {
        Text('Title')
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
      }
      .width('100%')
      .height(56)
      .justifyContent(FlexAlign.Center)

      Text(this.message)
        .fontSize(14)

      Button('CONFIRM')
        .fontSize(16)
        .fontColor('#0A59F7')
        .backgroundColor(Color.White)
        .onClick(() => {
          let dialogId = AppStorage.get<number>('dialogId');
          promptAction.closeCustomDialog(dialogId)
        })
        .width('100%')
        .margin({
          top: 8,
          bottom: 16
        })
    }
    .padding({
      left: 24,
      right: 24
    })
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
    .backgroundColor(Color.White)
    .borderRadius(32)
    .margin({ left: 16, right: 16 })
  }
}

// DocsCode 1