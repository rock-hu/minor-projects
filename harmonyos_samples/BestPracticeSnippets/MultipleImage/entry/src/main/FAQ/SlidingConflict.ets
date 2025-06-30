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

// [End SlidingConflict]