// [Start quick_start]
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
export struct SectionOptionsUsePage {
  // [StartExclude variable_declare]
  @State minSize: number = 120;
  @State maxSize: number = 260;
  scroller: Scroller = new Scroller();
  // [EndExclude variable_declare]
  dataSource: SectionsWaterFlowDataSource = new SectionsWaterFlowDataSource();
  private itemHeightArray: number[] = [];
  sectionMargin: Margin = {
    top: 8,
    left: 0,
    bottom: 0,
    right: 0
  }
  // 1、Create group information.
  @State sections: WaterFlowSections = new WaterFlowSections();

  // 2、Create the first group.
  oneColumnSection: SectionOptions = {
    itemsCount: 3,
    crossCount: 1,
    columnsGap: 5,
    rowsGap: 10,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return 120;
    }
  }
  // 3、Create the second group.
  twoColumnSection: SectionOptions = {
    itemsCount: 2,
    crossCount: 2,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return 160;
    }
  }
  // 4、Create the third group.
  dataSection: SectionOptions = {
    itemsCount: 20,
    crossCount: 2,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }

  // [StartExclude method_declare]
  getSize() {
    let ret = Math.floor(Math.random() * this.maxSize);
    return (ret > this.minSize ? ret : this.minSize);
  }

  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemHeightArray.push(this.getSize());
    }
  }
  // [EndExclude method_declare]

  aboutToAppear() {
    this.setItemSizeArray();
    this.initSections();
  }

  // 5、Initialise group data.
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

  build() {
    Column({ space: 0 }) {
      // 6、Link the grouping information to WaterFlow.
      WaterFlow({ scroller: this.scroller, sections: this.sections }) {
        // [StartExclude water_flow_data]
        LazyForEach(this.dataSource, (item: number) => {
          FlowItem() {
            ReusableFlowItem({ item: item })
          }
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
    }
  }
}
// [End quick_start]