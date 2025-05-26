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

/*
 * 最佳实践：手势事件冲突解决方案
 *
 * 场景三：系统手势和自定义手势之间冲突
 */

// [Start gestures_conflict_scene3]
import { promptAction } from '@kit.ArkUI';

@Entry
@Component
struct GesturesConflictScene3 {
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
          .gesture(
            // [Start long_press_gesture]
            LongPressGesture({ repeat: true })
              .onAction((event: GestureEvent) => {
              })// 长按动作一结束触发
              .onActionEnd(() => {
                this.getUIContext().getPromptAction().showToast({ message: 'Long Press' });
              })
            // [End long_press_gesture]
          )
      }
      .width('100%')
    }
    .height('100%')
  }
}

// [End gestures_conflict_scene3]