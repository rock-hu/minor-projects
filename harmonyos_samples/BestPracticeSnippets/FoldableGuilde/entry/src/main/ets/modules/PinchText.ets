@Entry
@Component
export struct PinchText {
  text: string = '亲近大海，领悟海，体会到了海的心声，海自然与你共鸣，无需可以强求，也无需费力搜寻，海，始终都会以潮声与你相亲，已其海色伴你舒适。'
  @State textFontSize: number = 26

  build() {
    Column() {
      Text(this.text).fontSize(this.textFontSize)
    }
    .gesture(PinchGesture({ fingers: 2 }).onActionUpdate((event: GestureEvent) => {
      let tmp = event.scale;
      if (tmp > 1.45) {
        tmp = 1.45;
      } else {
        tmp = 0.75;
      }
      this.textFontSize = 16 * tmp;
    }))
  }
}