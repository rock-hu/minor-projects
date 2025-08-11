// [Start quick_start]
import { CommonConstants } from "../common/constants/CommonConstants";
import { SectionsWaterFlowDataSource } from "../model/SectionsWaterFlowDataSource";
// [StartExclude flow_item_layout]
@Reusable
struct ReusableFlowItem {
  @State item: number = 0;

  aboutToReuse(params: Record<string, number>) {
    this.item = params.item;
  }

  build() {
    RelativeContainer() {
      Image($rawfile(`sections/${this.item % 4}.jpg`))
        .objectFit(ImageFit.Cover)
        .width('100%')
        .layoutWeight(1)
        .borderRadius(8)
        .alignRules({
          top: { anchor: '__container__', align: VerticalAlign.Top },
          bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
          left: { anchor: '__container__', align: HorizontalAlign.Start },
          right: { anchor: '__container__', align: HorizontalAlign.End }
        })
        .id('image1')

      Stack() {
      }
      .linearGradient({
        angle: 0,
        colors: [['#80000000', 0.0],
          ['#03000000', 1.0]]
      })
      .width('100%')
      .height(32)
      .borderRadius(8)
      .hitTestBehavior(HitTestMode.None)
      .alignRules({
        bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
        left: { anchor: '__container__', align: HorizontalAlign.Start },
        right: { anchor: '__container__', align: HorizontalAlign.End }
      })
      .id('mask1')

      Text('NO. ' + (this.item + 1))
        .fontSize(12)
        .fontColor(Color.White)
        .alignRules({
          bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
          left: { anchor: '__container__', align: HorizontalAlign.Start }
        })
        .margin({
          left: 8,
          bottom: 4
        })
        .id('text1')
    }
    .width('100%')
    .borderRadius(8)
    .backgroundColor(Color.Gray)
  }
}
// [EndExclude flow_item_layout]
@Entry
@Component
struct DataLoadMorePage {
  @State sections: WaterFlowSections = new WaterFlowSections();
  dataSource: SectionsWaterFlowDataSource = new SectionsWaterFlowDataSource();
  // [StartExclude variable_declare]
  @State isRefreshing: boolean = false;
  @State currentItem: number = -1;
  @State minSize: number = 80;
  @State maxSize: number = 180;
  @StorageProp(CommonConstants.AS_KEY_STATUS_BAR_HEIGHT) statusBarHeight: number = 0;
  scroller: Scroller = new Scroller();
  private itemHeightArray: number[] = [];
  sectionMargin: Margin = {
    top: 8,
    left: 16,
    bottom: 0,
    right: 16
  }
  oneColumnSection: SectionOptions = {
    itemsCount: 3,
    crossCount: 1,
    columnsGap: 5,
    rowsGap: 10,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return 170;
    }
  }
  twoColumnSection: SectionOptions = {
    itemsCount: 2,
    crossCount: 2,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return 100;
    }
  }
  dataSection: SectionOptions = {
    itemsCount: 20,
    crossCount: 2,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }

  @Builder
  headerRefresh() {
    Column() {
      LoadingProgress()
        .color(Color.Black)
        .opacity(0.6)
        .width(36)
        .height(36)
    }
    .justifyContent(FlexAlign.Center)
  }

  refresh() {
    this.currentItem = -1;
    setTimeout(() => {
      // Add new data.
      this.dataSource.dataArray = [];
      let value = Math.floor(Math.random() * 100);
      for (let i = 0; i < 100; i++) {
        this.dataSource.dataArray.push(i + value);
        this.dataSource.notifyDataAdd(i);
      }
      // Update sections itemsCount.
      this.oneColumnSection.itemsCount = 3;
      this.oneColumnSection.crossCount = 1;
      this.twoColumnSection.itemsCount = 2;
      this.twoColumnSection.crossCount = 2;
      this.dataSection.itemsCount = 95;
      this.dataSection.crossCount = 2;
      this.sections.update(0, this.oneColumnSection);
      this.sections.update(1, this.twoColumnSection);
      this.sections.update(2, this.dataSection);
      this.isRefreshing = false;
    }, 2000);
  }

  getSize() {
    let ret = Math.floor(Math.random() * this.maxSize);
    return (ret > this.minSize ? ret : this.minSize);
  }

  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemHeightArray.push(this.getSize());
    }
  }

  initSections() {
    let sectionOptions: SectionOptions[] = [];
    let count = 0;
    let oneOrTwo = 0;
    let dataCount = this.dataSource.totalCount();
    while (count < dataCount) {
      if (dataCount - count < 96) {
        this.dataSection.itemsCount = dataCount - count;
        sectionOptions.push(this.dataSection);
        break;
      }
      if (oneOrTwo++ % 2 === 0) {
        sectionOptions.push(this.oneColumnSection);
        count += this.oneColumnSection.itemsCount;
      } else {
        sectionOptions.push(this.twoColumnSection);
        count += this.twoColumnSection.itemsCount;
      }
    }
    this.sections.splice(0, 0, sectionOptions);
  }

  removeItem(item: number): void {
    let index = this.dataSource.indexOf(item);
    this.dataSource.deleteItem(index);
    const sections: Array<SectionOptions> = this.sections.values();
    let newSection: SectionOptions;
    let tmpIndex = 0;
    let sectionIndex = 0;
    for(let i = 0; i < sections.length; i++) {
      tmpIndex += sections[i].itemsCount;
      if (index < tmpIndex) {
        sectionIndex = i;
        break;
      }
    }
    newSection = sections[sectionIndex];
    newSection.itemsCount -= 1;
    if (newSection.crossCount && newSection.crossCount > newSection.itemsCount) {
      newSection.crossCount = newSection.itemsCount;
    }
    this.sections.update(sectionIndex, newSection);
  }

  aboutToAppear() {
    this.setItemSizeArray();
    this.initSections();
  }
  // [EndExclude variable_declare]
  build() {
    Column({ space: 0 }) {
      Refresh({ refreshing: $$this.isRefreshing, builder: this.headerRefresh()}) {
        WaterFlow({ scroller: this.scroller, sections: this.sections }) {
          // [StartExclude water_flow_data]
          LazyForEach(this.dataSource, (item: number) => {
            FlowItem() {
              Stack() {
                Row(){
                  Button('Delete')
                    .fontColor(Color.Red)
                    .backgroundColor(Color.White)
                    .onClick(() => {
                      this.getUIContext().animateTo({ duration: 300 }, () => {
                        this.removeItem(item);
                      });
                    })
                }
                .width('100%')
                .height('100%')
                .borderRadius(8)
                .justifyContent(FlexAlign.Center)
                .zIndex(1)
                .visibility(this.currentItem === item ? Visibility.Visible : Visibility.Hidden)
                .backgroundColor('#33000000')
                ReusableFlowItem({ item: item })
              }
            }
            .transition({ type: TransitionType.Delete, opacity: 0 })
            .priorityGesture(LongPressGesture()
              .onAction(() => {
                this.currentItem = item;
              }))
            .width('100%')
            .borderRadius(8)
            .backgroundColor(Color.Gray)
          }, (item: string) => item)
          // [EndExclude water_flow_data]
        }
        .cachedCount(12)
        .columnsGap(8)
        .rowsGap(8)
        .width('100%')
        .height('100%')
        .layoutWeight(1)
        // For better experience, pre load data.
        .onScrollIndex((first: number, last: number) => {
          // 1、Obtain the total amount of data in the waterfall flow.
          let totalCount = this.dataSource.totalCount();
          // 2、If the index of the last visible area is greater than the total amount of data, it triggers loading more.
          if (last + 20 >= totalCount) {
            // 3、Re-add 20 data entries to the waterfall.
            for (let i = 0; i < 20; i++) {
              this.dataSource.addLastItem();
            }
            // 4、Update the itemsCount in the group and refresh the group information.
            this.dataSection.itemsCount += 20;
            this.sections.update(2, this.dataSection);
          }
        })
      }
      // [StartExclude refresh]
      .refreshOffset(56)
      .onRefreshing(() => {
        this.refresh();
      })
      // [EndExclude refresh]
    }
    .padding({
      top: this.statusBarHeight
    })
  }
}
// [End quick_start]