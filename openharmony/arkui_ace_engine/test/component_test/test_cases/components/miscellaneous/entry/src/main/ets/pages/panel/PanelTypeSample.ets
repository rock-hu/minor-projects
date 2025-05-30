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
import panelTypeTest from '../MyTest/PanelTypeTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct PanelTypeSample {
  @State panelTypes: PanelType = PanelType.CUSTOM

  aboutToAppear(): void {
    loadComponentTestEntry(panelTypeTest)
  }

  build() {
    Column() {
      Panel(true) {
        Column() {
          Text('Today Calendar')
          Divider()
        }
      }
      .type(this.panelTypes)
      .height(120)
      .width(120)
      .id('panelType1')

      Panel(true) {
        Column() {
          Text('Today Calendar')
          Divider()
        }
      }
      .type(PanelType.Minibar)
      .height(120)
      .width(120)
      .id('panelType2')

      Button('PanelType1')
        .onClick(() => {
          this.panelTypes = PanelType.Foldable
        })
      Button('PanelType2')
        .onClick(() => {
          this.panelTypes = PanelType.Temporary
        })
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}