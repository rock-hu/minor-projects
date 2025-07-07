// [Start quick_start]
import { MyDataSource } from '../model/MyDataSource'

@Entry
@Component
struct MergeDoubleWaterFlowPage {
  // [StartExclude data_init]
  @State minSize: number = 80
  @State maxSize: number = 210;
  @State fontSize: number = 24;
  scroller: Scroller = new Scroller();
  private data: MyDataSource = new MyDataSource();
  private itemHeightArray: number[] = [];
  @State sections: WaterFlowSections = new WaterFlowSections();
  sectionMargin: Margin = {
    top: 10,
    left: 5,
    bottom: 10,
    right: 5
  }
  oneColumnSection: SectionOptions = {
    itemsCount: 21,
    crossCount: 2,
    columnsGap: '5vp',
    rowsGap: 10,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }
  twoColumnSection: SectionOptions = {
    itemsCount: 1,
    crossCount: 1,
    columnsGap: '5vp',
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }
  lastSection: SectionOptions = {
    itemsCount: 78,
    crossCount: 2,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
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

  aboutToAppear(): void {
    for(let i = 0; i < 100; i++) {
      this.data.pushData(i);
    }
    this.setItemSizeArray();
    let sectionOptions: SectionOptions[] = [];
    sectionOptions.push(this.oneColumnSection);
    sectionOptions.push(this.twoColumnSection);
    sectionOptions.push(this.lastSection);
    this.sections.splice(0, 0, sectionOptions);
  }
  // [EndExclude data_init]
  build() {
    Column({ space: 2 }) {
      WaterFlow({ scroller: this.scroller, sections: this.sections }) {
        LazyForEach(this.data, (item: number) => {
          FlowItem() {
            if (item === 21) {
              // 1、This is the content for stitching the item.
              Column() {
                Text('------推荐商品------')
                  .align(Alignment.BottomStart)
                  .width('100%')
                  .fontSize(24)
                  .fontWeight(FontWeight.Bold)
                  .height(50)
              }
            } else {
              // 2、Other data within WaterFlow.
              Column() {
                Text('N ' + item)
                  .fontSize(12)
                  .height('16vp')
                Image($rawfile(`sections/${item % 4}.jpg`))
                  .objectFit(ImageFit.Fill)
                  .width('100%')
                  .layoutWeight(1)
              }
            }
          }
          .width('100%')
        }, (item: number) => item.toString())
      }
    }
  }
}
// [End quick_start]