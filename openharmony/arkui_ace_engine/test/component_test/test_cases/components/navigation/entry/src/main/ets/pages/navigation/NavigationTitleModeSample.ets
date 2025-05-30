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
import navigationTitleModeTest from '../MyTest/NavigationTitleModeTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct NavigationTitleModeSample {
  aboutToAppear(): void {
    loadComponentTestEntry(navigationTitleModeTest)
  }

  @State titleMode: NavigationTitleMode = NavigationTitleMode.Free
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10]

  build() {
    Column() {
      Navigation() {
        TextInput({ placeholder: 'search...' })
          .width('90%')
          .height(40)
          .backgroundColor('#FFFFFF')
          .margin({ top: 8 })

        List({ space: 12, initialIndex: 0 }) {
          ForEach(this.arr, (item: number) => {
            ListItem() {
              Text('' + item)
                .width('90%')
                .height(72)
                .backgroundColor('#FFFFFF')
                .borderRadius(24)
                .fontSize(16)
                .fontWeight(500)
                .textAlign(TextAlign.Center)
            }
          }, (item: number) => item.toString())
        }
        .height(324)
        .width('60%')
        .margin({ top: 12, left: '10%' })
      }
      .title('hello')
      .height(400)
      .titleMode(this.titleMode)
      .id('navigationTitleMode')

      Button('NavigationTitleMode1')
        .onClick(() => {
          this.titleMode = NavigationTitleMode.Full
        })
      Button('NavigationTitleMode2')
        .onClick(() => {
          this.titleMode = NavigationTitleMode.Mini
        })
    }
  }
}