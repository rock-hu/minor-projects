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

// [Start WrapCapabilitySample]
@Entry
@Component
struct WrapCapabilitySample {
  @State rate: number = 0.7;
  imageList: Resource [] = [
    $r('app.media.flexWrap1'),
    $r('app.media.flexWrap2'),
    $r('app.media.flexWrap3'),
    $r('app.media.flexWrap4'),
    $r('app.media.flexWrap5'),
    $r('app.media.flexWrap6')
  ];

  @Builder
  slider() {
    Slider({ value: this.rate * 100, min: 50, max: 70 })
      .blockColor(Color.White)
      .width('60%')
      .height(50)
      .onChange((value: number) => {
        this.rate = value / 100;
      })
      .position({ x: '20%', y: '87%' })
  }

  build() {
    Column() {
      Flex({
        alignItems: ItemAlign.Center,
        justifyContent: FlexAlign.Center,
        wrap: FlexWrap.Wrap
      }) {
        ForEach(this.imageList, (item: Resource, index: number) => {
          Image(item)
            .width(192)
            .height(138)
            .padding(10)
        }, (item: Resource, index: number) => JSON.stringify(item) + index)
      }
      .backgroundColor('#FFFFFF')
      .padding(20)
      .width(this.rate * 100 + '%')
      .borderRadius(16)

      this.slider()
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F1F3F5')
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
  }
}

// [End WrapCapabilitySample]