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
//测试代码路径
import imageDraggableTest from '../MyTest/ImageDraggableTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct ImageDraggableSample {
  @State dragGable: boolean = true

  aboutToAppear(): void {
    loadComponentTestEntry(imageDraggableTest)
  }

  build() {
    Column() {
      Row() {
        Button('ImageDraggable1')
          .onClick(() => {
            this.dragGable = !this.dragGable
          })
      }

      Image($r('app.media.app_icon'))
        .width(110)
        .height(110)
        .margin(15)
        .draggable(this.dragGable)
        .id('imageDraggable')
    }
  }
}