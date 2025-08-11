// [Start quick_start]
import { CommonConstants } from '../common/constants/CommonConstants';
import { MyDataSource } from '../model/MyDataSource'

@Entry
@Component
struct ForceShowOnTopLeftPage {
  // [StartExclude data_init]
  private data: MyDataSource = new MyDataSource();
  @StorageProp(CommonConstants.AS_KEY_STATUS_BAR_HEIGHT) statusBarHeight: number = 0;

  aboutToAppear(): void {
    for(let i = 0; i < 80; i++) {
      this.data.pushData(i);
    }
  }
  // [EndExclude data_init]
  build() {
    Column({ space: 0}) {
      Row() {
        Text($r('app.string.force_left_show'))
          .width('100%')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .margin({ top: '18vp', left: '16vp', bottom: '16vp'})
      }
      WaterFlow() {
        // 1、Add the layout content in the top left corner of WaterFlow.
        FlowItem() {
          Column() {
            ForEach(this.data.getTopMastData(5), (item: number) => {
              Text(`Top Hello ${item}`).fontSize(22)
            })
          }
          .margin({
            top: 4,
            bottom: 4
          })
        }
        .width('100%')
        .alignSelf(ItemAlign.End)
        .backgroundColor(Color.White)
        .borderRadius(8)

        // 2、Add WaterFlow data.
        LazyForEach(this.data, (item: number, index: number) => {
          FlowItem() {
            Row() {
              Text(`Hello ${item}`).fontSize(20)
            }
          }
          .width('100%')
          .height(30 + Math.random() * 30)
          .backgroundColor(Color.White)
          .borderRadius(8)
        }, (item: number) => item.toString())
      }
      .cachedCount(5)
      .columnsTemplate('1fr 1fr')
      .backgroundColor('#efefef')
      .columnsGap(10)
      .rowsGap(5)
      .margin({
        left: '16vp',
        right: '16vp'
      })
    }
    .backgroundColor('#efefef')
    .padding({
      top: this.statusBarHeight
    })
  }
}
// [End quick_start]