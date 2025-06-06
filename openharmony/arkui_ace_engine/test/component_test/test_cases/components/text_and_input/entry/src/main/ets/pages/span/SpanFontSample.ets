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
import spanFontTest from '../MyTest/SpanFontTest'
import font from '@ohos.font';

@Entry
@Component
struct SpanFontSample {
  @State fontType: Font = {
    family: 'Arial',
    size: 30,
    weight: FontWeight.Lighter,
    style: FontStyle.Normal
  }
  @State weightType: FontWeight[] = [FontWeight.Normal, FontWeight.Medium, FontWeight.Bold, 30]
  @State index: number = 0

  aboutToAppear() {
    font.registerFont({
      familyName: 'TESTFONTRawFile',
      familySrc: $rawfile('TESTFONT.TTF')
    })
    loadComponentTestEntry(spanFontTest)
  }

  build() {
    Column() {
      Text() {
        Span('span')
          .font(this.fontType)
          .id('spanFont1')
        Span('span')
          .font({ size: $r('app.string.length'), weight: FontWeight.Bolder })
          .id('spanFont2')
        Span('span')
          .font({ weight: "500" })
          .id('spanFont3')
        Span('span')
          .font({ weight: this.weightType[this.index] })
          .id('spanFont4')
      }

      Button('SpanFont1')
        .onClick(() => {
          this.fontType = {
            family: 'TESTFONTRawFile',
            size: '30px',
            weight: FontWeight.Regular,
            style: FontStyle.Italic
          }
        })
      Button('SpanFont2')
        .onClick(() => {
          this.index++
          this.index %= 4
        })
    }
  }
}