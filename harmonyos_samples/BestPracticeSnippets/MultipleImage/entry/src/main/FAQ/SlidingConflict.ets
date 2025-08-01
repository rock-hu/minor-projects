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

/**
 * 常见问题:Swiper嵌套Swiper滑动手势冲突如何解决
 */

// [Start SlidingConflict]
@Component
struct MySwiper {
  @State swiperIndex: number = 0;
  @State item: number = 0;

  build() {
    Swiper() {
      ForEach([0, 1, 2], (item2: number) => {
        Text(this.item.toString() + item2.toString())
          .width('100%')
          .height(160)
          .backgroundColor(0xAFEEEE)
          .textAlign(TextAlign.Center)
          .fontSize(30)
      })
    }
    .onGestureRecognizerJudgeBegin((event: BaseGestureEvent, current: GestureRecognizer) => {
      if (current.isBuiltIn() && current.getType() == GestureControl.GestureType.PAN_GESTURE) {
        let panEvent = event as PanGestureEvent;
        if (this.swiperIndex === 0) {
          if (panEvent.velocityX > 0) {
            return GestureJudgeResult.REJECT
          }
        } else if (this.swiperIndex === 2) {
          if (panEvent.velocityX < 0) {
            return GestureJudgeResult.REJECT
          }
        }
      }
      return GestureJudgeResult.CONTINUE;
    })
    .index($$this.swiperIndex)
  }
}
// [End SlidingConflict]

@Entry
@Component
struct SwiperExample {
  build() {
    Column({ space: 5 }) {
      Swiper() {
        ForEach([0, 1, 2], (item: number) => {
          Stack() {
            MySwiper({ item: item })
          }
          .height(220)
        })
      }
      .backgroundColor(0xAFee00)
      .cachedCount(2)
      .loop(false)
    }
    .width('100%')
    .margin({ top: 5 })
  }
}