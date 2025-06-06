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
import { WaterFlowDataSource } from '../MyTest/WaterFlowDataSource'
import waterFlowNestedScrollTest from '../MyTest/WaterFlowNestedScrollTest';
import { loadComponentTestEntry } from '@ohos.arkui.componentTest';

@Entry
@Component
struct WaterFlowNestedScrollSample {
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD]
  @State nestedScrollType: NestedScrollOptions =
    { scrollForward: NestedScrollMode.SELF_FIRST, scrollBackward: NestedScrollMode.SELF_FIRST }
  dataSource: WaterFlowDataSource = new WaterFlowDataSource()

  aboutToAppear(): void {
    loadComponentTestEntry(waterFlowNestedScrollTest);
  }

  build() {
    Column() {
      WaterFlow() {
        FlowItem() {
          Text("1").fontSize(12)
        }.width('100%').height('160').backgroundColor(this.colors[0])

        FlowItem() {
          Text("2").fontSize(12)
        }.width('100%').height('160').backgroundColor(this.colors[1])

        FlowItem() {
          Text("3").fontSize(12)
        }.width('100%').height('160').backgroundColor(this.colors[2])

        FlowItem() {
          Text("4").fontSize(12)
        }.width('100%').height('160').backgroundColor(this.colors[3])
      }
      .nestedScroll({ scrollForward: NestedScrollMode.SELF_ONLY, scrollBackward: NestedScrollMode.SELF_ONLY })
      .width('100%')
      .height('100')
      .margin({ bottom: "10" })
      .id('waterFlowNestedScroll1')

      WaterFlow() {
        FlowItem() {
          Text("1").fontSize(12)
        }.width('100%').height('40').backgroundColor(this.colors[0])

        FlowItem() {
          Text("2").fontSize(12)
        }.width('100%').height('40').backgroundColor(this.colors[1])

        FlowItem() {
          Text("3").fontSize(12)
        }.width('100%').height('40').backgroundColor(this.colors[2])

        FlowItem() {
          Text("4").fontSize(12)
        }.width('100%').height('40').backgroundColor(this.colors[3])
      }
      .nestedScroll(this.nestedScrollType)
      .width('100%')
      .height('100')
      .id('waterFlowNestedScroll2')

      Button('WaterFlowNestedScroll1')
        .onClick(() => {
          this.nestedScrollType =
            { scrollForward: NestedScrollMode.PARENT_FIRST, scrollBackward: NestedScrollMode.PARENT_FIRST }
        })
      Button('WaterFlowNestedScroll2')
        .onClick(() => {
          this.nestedScrollType =
            { scrollForward: NestedScrollMode.PARALLEL, scrollBackward: NestedScrollMode.PARALLEL }
        })
    }
  }
}