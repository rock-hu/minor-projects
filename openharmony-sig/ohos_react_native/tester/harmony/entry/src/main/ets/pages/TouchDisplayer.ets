/**
 * Copyright (c) 2024 Huawei Technologies Co., Ltd.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE-MIT file in the root directory of this source tree.
 */

@Component
export struct TouchDisplayer {
  @State currentTouches: TouchObject[] = []
  @State touchIndicatorOpacity: number = 0
  @BuilderParam buildChildren: () => void
  build() {
    Stack() {
      this.buildChildren()
      ForEach(this.currentTouches, (activeTouch: TouchObject) => {
        Stack() {
        }
        .width(64)
        .height(64)
        .backgroundColor("blue")
        .borderWidth(2)
        .borderColor("white")
        .opacity(this.touchIndicatorOpacity)
        .position({ x: activeTouch.x - 32, y: activeTouch.y - 32 })
        .borderRadius(1000)
        .hitTestBehavior(HitTestMode.Transparent)
      })
    }
    .width("100%")
    .height("100%")
    .hitTestBehavior(HitTestMode.Transparent)
    .onTouch(e => {
      this.currentTouches = e.touches
      this.touchIndicatorOpacity = 0.5
      animateTo({
        duration: 500,
        curve: Curve.Linear,
      }, () => {
        this.touchIndicatorOpacity = 0
      })
    })
  }
}