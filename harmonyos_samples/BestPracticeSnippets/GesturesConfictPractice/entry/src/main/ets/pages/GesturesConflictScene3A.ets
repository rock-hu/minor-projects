/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

@Entry
@Component
struct GesturesConflictScene3A {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        Image($r('app.media.startIcon'))
          .margin({ top: 100 })
          .width(360)
          .height(360)
            // [Start gesture_action]
          .priorityGesture(
            LongPressGesture({ repeat: true })
              .onAction((event: GestureEvent) => {
              })
              .onActionEnd(() => {
                this.getUIContext().getPromptAction().showToast({ message: 'Long Press' });
              })
            )
            // [End gesture_action]
      }
      .width('100%')
    }
    .height('100%')
  }
}