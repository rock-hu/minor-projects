import { CommonConstants } from '../common/constants/CommonConstants';

@Entry
@Component
struct ElementByOrderPage {
  @State data1: string[] = ['1', '2', '3', '4', '5', '6'];
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7E];
  @StorageProp(CommonConstants.AS_KEY_STATUS_BAR_HEIGHT) statusBarHeight: number = 0;

  getHeight(item: string): number {
    if (item === '2' || item === '3' || item === '5') {
      return 25;
    }
    return 40;
  }

  build() {
    RelativeContainer() {
      ForEach(this.data1, (item: string) => {
        if (item === '1') {
          Text('item' + item +
          this.getUIContext().getHostContext()?.resourceManager.getStringSync($r('app.string.item_height')) + ':' +
          this.getHeight(item).toString())
            .id('item' + item)
            .alignRules({
              top: { anchor: '__container__', align: VerticalAlign.Top },
              left: { anchor: '__container__', align: HorizontalAlign.Start },
            })
            .height(this.getHeight(item))
            .width('50%')
            .backgroundColor(this.colors[Number(item) - 1])
        } else if (item === '2') {
          Text('item' + item +
          this.getUIContext().getHostContext()?.resourceManager.getStringSync($r('app.string.item_height')) + ':' +
          this.getHeight(item).toString())
            .id('item' + item)
            .alignRules({
              top: { anchor: '__container__', align: VerticalAlign.Top },
              right: { anchor: '__container__', align: HorizontalAlign.End }
            })
            .height(this.getHeight(item))
            .width('50%')
            .backgroundColor(this.colors[Number(item) - 1])
        } else {
          Text('item' + item +
          this.getUIContext().getHostContext()?.resourceManager.getStringSync($r('app.string.item_height')) + ':' +
          this.getHeight(item).toString())
            .id('item' + item)
            .alignRules({
              top: { anchor: 'item' + (Number(item) - 2).toString(), align: VerticalAlign.Bottom },
              left: { anchor: 'item' + (Number(item) - 2).toString(), align: HorizontalAlign.Start },
            })
            .height(this.getHeight(item))
            .width('50%')
            .backgroundColor(this.colors[Number(item) - 1])
        }
      }, (item: string, index: number) => index.toString())
    }
    .padding({
      top: this.statusBarHeight
    })
  }
}