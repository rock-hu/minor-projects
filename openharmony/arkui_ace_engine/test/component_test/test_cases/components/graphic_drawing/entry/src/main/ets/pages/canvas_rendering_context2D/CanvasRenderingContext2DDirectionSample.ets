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

// 测试代码路径
import canvasRenderingContext2DDirectionTest from '../MyTest/CanvasRenderingContext2DDirectionTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct CanvasRenderingContext2DDirectionSample {
  private settings: RenderingContextSettings = new RenderingContextSettings(true)
  private context: CanvasRenderingContext2D = new CanvasRenderingContext2D(this.settings)
  private directionCanvasDirection: CanvasDirection = 'inherit'

  aboutToAppear(): void {
    loadComponentTestEntry(canvasRenderingContext2DDirectionTest)
  }

  build() {
    Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Center, justifyContent: FlexAlign.Center }) {
      Canvas(this.context)
        .width('100%')
        .height('100%')
        .backgroundColor('#ffff00')
        .id('canvasDirection')
        .onReady(() => {
          this.context.font = '48px serif'
          this.context.textAlign = 'start'
          this.context.fillText("Hi ltr!", 200, 50)
          this.context.direction = this.directionCanvasDirection
        })
      Button('CanvasDirection1')
        .onClick(() => {
          this.directionCanvasDirection = 'rtl'
          this.context.direction = this.directionCanvasDirection;
          this.context.fillText("Hi rtl!", 200, 100)
        })
      Button('CanvasDirection2')
        .onClick(() => {
          this.directionCanvasDirection = 'ltr'
          this.context.direction = this.directionCanvasDirection;
          this.context.fillText("Hi ltr!", 200, 150)
        })
    }
    .width('100%')
    .height('100%')
  }
}