// [Start grid_component_load_slow]
// Import performance dot modules
import { hiTraceMeter } from '@kit.PerformanceAnalysisKit';

@Component
struct TextItem {
  @State item: string = '';

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
    hiTraceMeter.finishTrace('useColumnStartColumnEnd', 1);
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

@Entry
@Component
struct GridExample {
  private datasource: MyDataSource = new MyDataSource();
  scroller: Scroller = new Scroller();

  aboutToAppear() {
    for (let i = 1; i <= 2000; i++) {
      this.datasource.pushData(i + '');
    }
  }

  build() {
    Column({ space: 5 }) {
      Text('使用columnStart,columnEnd设置GridItem大小').fontColor(0xCCCCCC).fontSize(9).width('90%')
      Grid(this.scroller) {
        LazyForEach(this.datasource, (item: string, index: number) => {
          if ((index % 4) === 0) {
            GridItem() {
              TextItem({ item: item })
            }
            .columnStart(0).columnEnd(2)
          } else {
            GridItem() {
              TextItem({ item: item })
            }
          }
        }, (item: string) => item)
      }
      .columnsTemplate('1fr 1fr 1fr')
      .columnsGap(10)
      .rowsGap(10)
      .width('90%')
      .height('40%')

      Button('scrollToIndex:1900').onClick(() => {
        // Start some tasks.
        hiTraceMeter.startTrace('useColumnStartColumnEnd', 1);
        this.scroller.scrollToIndex(1900);
      })
    }.width('100%')
    .margin({ top: 5 })
  }
}
// [End grid_component_load_slow]