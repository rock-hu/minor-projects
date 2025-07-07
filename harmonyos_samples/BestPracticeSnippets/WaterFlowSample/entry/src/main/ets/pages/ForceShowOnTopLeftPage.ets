// [Start quick_start]
import { MyDataSource } from '../model/MyDataSource'

@Entry
@Component
struct ForceShowOnTopLeftPage {
  // [StartExclude data_init]
  private data: MyDataSource = new MyDataSource();

  aboutToAppear(): void {
    for(let i = 0; i < 80; i++) {
      this.data.pushData(i);
    }
  }
  // [EndExclude data_init]
  build() {
    WaterFlow() {
      // 1、Add the layout content in the top left corner of WaterFlow.
      FlowItem() {
        Column() {
          ForEach(this.data.getTopMastData(5), (item: number) => {
            Text(`Top Hello ${item}`).fontSize(22)
          })
        }
      }
      .width('100%')
      .backgroundColor(Color.White)

      // 2、Add WaterFlow data.
      LazyForEach(this.data, (item: number, index: number) => {
        FlowItem() {
          Row() {
            Text(`Hello ${item}`).fontSize(20)
          }
        }
        .width('100%')
        .height(30 + Math.random() * 50)
        .backgroundColor(Color.White)
      }, (item: number) => item.toString())
    }
    .columnsTemplate('1fr 1fr')
    .backgroundColor('#efefef')
    .columnsGap(10)
    .rowsGap(10)
  }
}
// [End quick_start]