/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 // * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
// 测试代码路径
import scrollEnableScrollInteractionTest from '../MyTest/ScrollEnableScrollInteractionTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct ScrollEnableScrollInteractionSample {
  @State scrollEnableScrollInteraction: boolean = true
  scroller: Scroller = new Scroller()

  aboutToAppear(): void {
    loadComponentTestEntry(scrollEnableScrollInteractionTest)
  }

  build() {
    Column() {
      Scroll(this.scroller) {
        Column() {
          Text('ScrollEnableScrollInteraction1')
            .width('90%')
            .height(150)
            .margin({ top: 10 })
          Text('ScrollEnableScrollInteraction2')
            .width('90%')
            .height(150)
            .margin({ top: 10 })
          Text('ScrollEnableScrollInteraction3')
            .width('90%')
            .height(150)
            .margin({ top: 10 })
        }.width('100%')
      }
      .enableScrollInteraction(this.scrollEnableScrollInteraction)
      .id('scrollEnableScrollInteraction')
      .margin({ top: 20, left: 20 })
      .width('100%')
      .height('50%')
      .backgroundColor(0xDCDCDC)

      Button('ScrollEnableScrollInteraction').onClick(() => {
        this.scrollEnableScrollInteraction = !this.scrollEnableScrollInteraction
      })
    }
  }
}