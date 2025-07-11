/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

@Entry
@Component
struct IndentationSample {
  build() {
    Row() {
      GridRow() {
        ForEach([0, 1, 2, 4], (item:number, index) => {
          GridCol({
            span: { sm: 10, md: 8, lg: 8 },
            offset: {
              sm: index === 0 ? 1 : 2,
              md: index === 0 ? 2 : 4,
              lg: index === 0 ? 2 : 4
            }
          }) {
            Column() {
              ItemContent()
            }
          }
        })
      }
    }
    .height('100%')
    .alignItems((VerticalAlign.Center))
    .justifyContent(FlexAlign.Center)
    .backgroundColor('#F1F3f5')
  }
}

@Component
struct ItemContent {
  build() {
    Column() {
      Row() {
        Row() {
        }
        .width(28)
        .height(28)
        .borderRadius(14)
        .margin({ right: 15 })
        .backgroundColor('#E4E6E8')

        Row() {
        }
        .width('30%')
        .height(20)
        .borderRadius(4)
        .backgroundColor('#E4E6E8')
      }.width('100%').height(28)

      Row() {
      }
      .width('100%')
      .height(68)
      .borderRadius(16)
      .margin({ top: 12 })
      .backgroundColor('#E4E6E8')
    }
    .height(128)
    .borderRadius(24)
    .backgroundColor('#FFFFFF')
    .padding({ top: 12, bottom: 12, left: 18, right: 18 })
    .margin({ bottom: 12 })
  }
}