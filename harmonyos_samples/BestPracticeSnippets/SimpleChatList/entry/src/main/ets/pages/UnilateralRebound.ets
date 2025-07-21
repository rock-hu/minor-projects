@Entry
@Component
struct UnilateralRebound {
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  private scroller: Scroller = new Scroller();
  private isTop: boolean = true;
  @StorageProp('topRectHeight') topRectHeight: number = 0;

  build() {
    Column() {
      // [Start UnilateralRebound]
      List({ space: 20, initialIndex: 0, scroller: this.scroller }) {
        // [StartExclude UnilateralRebound]
        ForEach(this.arr, (item: number) => {
          ListItem() {
            Text('' + item)
              .width('100%')
              .height(100)
              .fontSize(16)
              .textAlign(TextAlign.Center)
              .borderRadius(16)
              .backgroundColor(0xDCDCDC)
          }
          .borderRadius(16)
          .backgroundColor(0xDCDCDC)
        }, (item: string) => item)
        // [EndExclude UnilateralRebound]
      }
      .width('90%')
      .scrollBar(BarState.Off)
      .onDidScroll(() => {
        const y = this.scroller.currentOffset().yOffset;
        this.isTop = y <= 0;
      })
      // [Start UnilateralRebound2]
      .edgeEffect(this.isTop ? EdgeEffect.Spring : EdgeEffect.None)
      // [End UnilateralRebound]
      .onScrollIndex((firstIndex: number) => {
        this.isTop = firstIndex === 0;
        console.info('firstIndex:' + firstIndex + ',' + this.isTop)
      })

      // [End UnilateralRebound2]
    }
    .width('100%')
    .height('100%')
    .padding({ top: this.getUIContext().px2vp(this.topRectHeight) })
  }
}