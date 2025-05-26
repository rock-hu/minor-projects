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
 * 场景五：多点触控场景下手势冲突
 */

// [Start gestures_conflict_scene5]
@Entry
@Component
struct GesturesConflictScene5 {
  @State message: string = 'Hello World';

  build() {
    Column() {
      Row({ space: 20 }) {
        Button('Button1')
          .width(100)
          .height(40)
          .monopolizeEvents(true)
        Button('Button2')
          .width(200)
          .height(50)
          .onClick(() => {
            console.info('GesturesConflictScene5 Button2 click');
          })
      }
      .margin(20)

      Text(this.message)
        .margin(15)
    }
    .width('100%')
    .onDragStart(() => {
      console.info('GesturesConflictScene5 Drag start.');
    })
    .gesture(
      TapGesture({ count: 1 })
        .onAction(() => {
          console.info('GesturesConflictScene5 TapGesture onAction.');
        }),
    )
  }
}

// [End gestures_conflict_scene5]