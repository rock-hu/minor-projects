@Entry
@Component
struct ScrollLeftAndRightToRefresh {
  @State isRefreshing: boolean = false;
  @State arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

  build() {
    // [Start ScrollLeftAndRightToRefresh]
    Column() {
      Refresh({ refreshing: $$this.isRefreshing }) {
        List({ space: 10 }) {
          ForEach(this.arr, (item: number) => {
            ListItem() {
              Text('' + item)
                .width(300)
                .height(80)
                .fontSize(16)
                .textAlign(TextAlign.Center)
                .borderRadius(16)
                .backgroundColor(0xFFFFFF)
                .translate({ x: (80 - 300) / 2 })
                .rotate({
                  x: 0,
                  y: 0,
                  z: 1,
                  centerX: '50%',
                  centerY: '50%',
                  angle: 90
                })
            }
            .width(80)
            .height(300)
          }, (item: string) => item)
        }
        .width(300)
        .height(300)
        .alignListItem(ListItemAlign.Center)
        .scrollBar(BarState.Off)
      }
      .onRefreshing(() => {
        setTimeout(() => {
          this.isRefreshing = false;
        }, 2000)
      })
      .backgroundColor(0xDCDCDC)
      .refreshOffset(64)
      .pullToRefresh(true)
    }
    .justifyContent(FlexAlign.Center)
    .width('100%')
    .height('100%')
    .rotate({
      x: 0,
      y: 0,
      z: 1,
      centerX: '50%',
      centerY: '50%',
      angle: -90
    })

    // [End ScrollLeftAndRightToRefresh]
  }
}