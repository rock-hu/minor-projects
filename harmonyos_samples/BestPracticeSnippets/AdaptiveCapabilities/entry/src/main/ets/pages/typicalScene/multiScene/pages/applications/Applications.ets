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

import { ApplicationsData } from '../../data/applications/Applications'

@Component
export struct Applications {
  private scroller: Scroller = new Scroller()

  @Builder Item(name: string) {
    Column() {
      Image($r('app.media.icon'))
        .width(48)
        .height(48)
        .margin(({ top: 8 }))
      Text(name)
        .width(64)
        .height(30)
        .lineHeight(15)
        .fontSize(12)
        .textAlign(TextAlign.Center)
        .margin({ top: 8 })
        .padding({ bottom: 15 })
    }.width(80).height(102)
  }

  build() {
    Scroll(this.scroller) {
      Column() {
        Flex({ justifyContent: FlexAlign.SpaceEvenly, wrap: FlexWrap.Wrap }) {
          ForEach(ApplicationsData, (item : string) => {
            this.Item(item)
          })
        }.width('80%')
      }
      .width('90%')
      .backgroundColor('#FFFFFF')
      .borderRadius(16)
      .padding({ top: 12 })
    }
  }
}