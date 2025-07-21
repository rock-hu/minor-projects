@Entry
@Component
struct ScrollToTheBottom {
  private arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  // [Start Scroller]
  private scroller: Scroller = new Scroller();
  @StorageProp('topRectHeight') topRectHeight: number = 0;

  // [StartExclude Scroller]

  build() {
    Column() {
      // [EndExclude Scroller]
      // [Start initialIndex]
      List({ space: 20, initialIndex: this.arr.length - 1, scroller: this.scroller }) {
        // [End Scroller]
        ForEach(this.arr, (item: number) => {
          ListItem() {
            // [StartExclude initialIndex]
            Text('' + item)
              .width('100%')
              .height(100)
              .fontSize(16)
              .textAlign(TextAlign.Center)
              .borderRadius(16)
              .backgroundColor(0xDCDCDC)
            // [EndExclude initialIndex]
          }
          .borderRadius(16)
          .backgroundColor(0xDCDCDC)
        }, (item: string) => item)
      }
      // [End initialIndex]
      .scrollBar(BarState.Off)
      .friction(0.6)
      .contentEndOffset(60)
      .onAppear(() => {
        // [Start scrollToIndex]
        this.scroller.scrollToIndex(this.arr.length - 1);
        // [End scrollToIndex]
        // [Start scrollEdge]
        this.scroller.scrollEdge(Edge.Bottom);
        // [End scrollEdge]
      })
      .width('90%')
    }
    .width('100%')
    .height('100%')
    .padding({ top: this.getUIContext().px2vp(this.topRectHeight) })
  }
}