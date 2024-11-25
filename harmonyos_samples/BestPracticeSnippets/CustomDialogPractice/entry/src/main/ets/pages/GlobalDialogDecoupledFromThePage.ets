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

import { buildText } from '../uitls/DialogComponent';
import { PromptAction } from '@kit.ArkUI';

@Component
export struct GlobalDialogDecoupledFromThePage {
  promptAction: PromptAction = this.getUIContext().getPromptAction();
  @State message: string = 'This is a dialog content.';
  @Consume('NavPathStack') pageStack: NavPathStack;

  @Builder
  customDialogComponent() {
    buildText({ message: this.message })
  }

  build() {
    NavDestination() {
      Column() {
        Row() {
          Button('OPEN')
            .fontSize(16)
            .width('100%')
            .borderRadius(20)
            .margin({ bottom: 16 })
            .backgroundColor('#0A59F7')
            .onClick(() => {
              this.promptAction.openCustomDialog({
                builder: () => {
                  this.customDialogComponent()
                },
                alignment: DialogAlignment.Center,
                maskColor: 'rgba(0, 0, 0, 0.2)',
              }).then((dialogId: number) => {
                AppStorage.setOrCreate('dialogId', dialogId);
              })
            })
        }
        .width('100%')
        .alignItems(VerticalAlign.Center)
      }
      .width('100%')
      .height('100%')
      .padding({
        left: 16,
        right: 16
      })
      .justifyContent(FlexAlign.End)
    }
  }
}