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
import panelShowCloseIconTest from '../MyTest/PanelShowCloseIconTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct PanelShowCloseIconSample {
  @State panelShowCloseIcon: boolean = true

  aboutToAppear(): void {
    loadComponentTestEntry(panelShowCloseIconTest)
  }

  build() {
    Column() {
      Panel(true) {
        Column() {
          Text('Today Calendar')
          Divider()
          Text('1. afternoon 4:00 The project meeting')
        }
      }
      .showCloseIcon(this.panelShowCloseIcon)
      .width(120)
      .height(120)
      .id('panelShowCloseIcon')

      Button('PanelShowCloseIcon')
        .onClick(() => {
          this.panelShowCloseIcon = !this.panelShowCloseIcon
        })
    }.width('100%').height('100%').backgroundColor(0xDCDCDC).padding({ top: 5 })
  }
}