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

import blendModeTest_three from '../MyTest/BlendModeTest_three';
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct BlendModeSample_three {
  scrollArr: BlendMode[] = [BlendMode.XOR, BlendMode.PLUS, BlendMode.MODULATE, BlendMode.SCREEN,
    BlendMode.OVERLAY, BlendMode.DARKEN]
  index: number = 0
  @State scroll: BlendMode = this.scrollArr[this.index]

  aboutToAppear(): void {
    loadComponentTestEntry(blendModeTest_three)
  }

  build() {
    Column() {
      Row() {
        Circle()
          .width(200)
          .height(200)
          .fill(Color.Green)
          .position({ x: 50, y: 50 })
        Circle()
          .width(200)
          .height(200)
          .fill(Color.Blue)
          .position({ x: 150, y: 50 })
      }
      .id('rowBlendMode3')
      .blendMode(this.scroll, BlendApplyType.OFFSCREEN)
      .alignItems(VerticalAlign.Center)
      .height(300)
      .width('100%')

      Button('ButtonBlendMode3').onClick(() => {
        this.index++
        this.scroll = this.scrollArr[this.index % 6]
      })
    }
    .height('100%')
    .width('100%')
  }
}
