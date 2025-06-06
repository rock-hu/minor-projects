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
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'
import textAreaCustomKeyboardTest from '../MyTest/TextAreaCustomKeyboardTest'

@Entry
@Component
struct TextAreaCustomKeyboardSample {
  controller: TextAreaController = new TextAreaController()
  @State inputValue: string = ""

  @Builder
  CustomKeyboardBuilder() {
    Column() {
      Button('x').onClick(() => {
        this.controller.stopEditing()
      })
      Grid() {
        ForEach([1, 2, 3, 4, 5, 6, 7, 8, 9, '*', 0, '#'], (item: number | string) => {
          GridItem() {
            Button(item + "")
              .width(110).onClick(() => {
              this.inputValue += item
            })
          }
        })
      }.maxCount(3).columnsGap(10).rowsGap(10).padding(5)
    }.backgroundColor(Color.Gray)
  }

  aboutToAppear(): void {
    loadComponentTestEntry(textAreaCustomKeyboardTest)
  }

  build() {
    Column() {
      Search({ controller: this.controller, value: this.inputValue })
        .customKeyboard(this.CustomKeyboardBuilder()).margin(10).border({ width: 1 })
        .id('textAreaCustomKeyboard')
    }
  }
}