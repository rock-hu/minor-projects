// [Start right_index]
// Import performance dot modules
import hiTraceMeter from '@ohos.hiTraceMeter';

@Component
struct TextItem {
  @State item: string = "";

  build() {
    Text(this.item)
      .fontSize(16)
      .backgroundColor(0xF9CF93)
      .width('100%')
      .height(80)
      .textAlign(TextAlign.Center)
  }

  aboutToAppear() {
    // Finish the task
    hiTraceMeter.finishTrace("useGridLayoutOptions", 1);
  }
}

class MyDataSource implements IDataSource {
  private dataArray: string[] = [];

  public pushData(data: string): void {
    this.dataArray.push(data);
  }

  public totalCount(): number {
    return this.dataArray.length;
  }

  public getData(index: number): string {
    return this.dataArray[index];
  }

  registerDataChangeListener(listener: DataChangeListener): void {
  }

  unregisterDataChangeListener(listener: DataChangeListener): void {
  }
}

@Component
export struct GridExample2 {
  private datasource: MyDataSource = new MyDataSource();
  scroller: Scroller = new Scroller();
  private irregularData: number[] = [];
  layoutOptions: GridLayoutOptions = {
    regularSize: [1, 1],
    irregularIndexes: this.irregularData,
  };

  aboutToAppear() {
    for (let i = 1; i <= 2000; i++) {
      this.datasource.pushData(i + '');
      if ((i - 1) % 4 === 0) {
        this.irregularData.push(i - 1);
      }
    }
  }

  build() {
    Column({ space: 5 }) {
      Text('使用GridLayoutOptions设置GridItem大小').fontColor(0xCCCCCC).fontSize(9).width('90%')
      Grid(this.scroller, this.layoutOptions) {
        LazyForEach(this.datasource, (item: string, index: number) => {
          GridItem() {
            TextItem({ item: item })
          }
        }, (item: string) => item)
      }
      .columnsTemplate('1fr 1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .width('90%')
      .height('40%')

      Button("scrollToIndex:1900").onClick(() => {
        // Start some tasks.
        hiTraceMeter.startTrace("useGridLayoutOptions", 1);
        this.scroller.scrollToIndex(1900);
      })
    }.width('100%')
    .margin({ top: 5 })
  }
}
// [End right_index]