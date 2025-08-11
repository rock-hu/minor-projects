// [Start Case3]
import { MyDataSource } from './segment1';

@Entry
@Component
struct SetHeightTestPage {
  private data: MyDataSource = new MyDataSource();

  build() {
    Scroll() {
      List() {
        LazyForEach(this.data, (item: string, index: number) => {
          ListItem() {
            Row() {
              Text('item value: ' + item + (index + 1))
                .fontSize(20)
                .margin(10)
            }
          }
        })
      }
      .width('100%')
      .height(500)
    }
  }
}
// [End Case3]