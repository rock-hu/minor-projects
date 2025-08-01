@Entry
@Component
struct PullUpLoading {
  @State arr: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
  @State isRefreshing: boolean = false;
  @State refreshOffset: number = 0;
  @State refreshState: RefreshStatus = RefreshStatus.Inactive;
  @StorageProp('topRectHeight') topRectHeight: number = 0;

  // [Start PullToRefresh]
  @Builder
  refreshBuilder() {
    Stack({ alignContent: Alignment.Bottom }) {
      if (this.refreshState != RefreshStatus.Inactive && this.refreshState != RefreshStatus.Done) {
        Progress({ value: this.refreshOffset, total: 64, type: ProgressType.Ring })
          .width(32)
          .height(32)
          .style({ status: this.isRefreshing ? ProgressStatus.LOADING : ProgressStatus.PROGRESSING })
          .margin(10)
      }
    }
    .width('100%')
    .height('100%')
  }

  build() {
    Refresh({ refreshing: $$this.isRefreshing, builder: this.refreshBuilder }) {
      // [StartExclude PullToRefresh]
      // [Start PullUpLoading]
      List({ space: 20 }) {
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
        }, (item: string) => item)
        ListItem() {
          Row() {
            LoadingProgress().height(32).width(48)
            Text('加载中')
          }
        }
        .width('100%')
        .height(64)
      }
      .width('90%')
      .onScrollIndex((start: number, end: number) => {
        if (end > this.arr.length) {
          setTimeout(() => {
            for (let i = 0; i < 5; i++) {
              this.arr.push(this.arr.length);
            }
          })
        }
      })
      // [End PullUpLoading]
      .onAreaChange((oldValue: Area, newValue: Area) => {
        this.refreshOffset = newValue.position.y as number;
      })
      .scrollBar(BarState.Off)
      .divider({ strokeWidth: '1px' })
    }
    .width('100%')
    .height('100%')
    .padding({ top: this.getUIContext().px2vp(this.topRectHeight) })
    .onStateChange((state: RefreshStatus) => {
      this.refreshState = state;
    })
    // [EndExclude PullToRefresh]
    .onRefreshing(() => {
      setTimeout(() => {
        this.isRefreshing = false;
      }, 2000)
    })

    // [End PullToRefresh]
  }
}