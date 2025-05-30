// [Start non_same_layer_rendering]
import { promptAction } from '@kit.ArkUI';
import { PRODUCT_DATA } from '../mock/GoodsMock';
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct NonSameLayerRendering {
  @State searchWidth: number = 0;
  @State searchHeight: number = 0;
  @State isWebInit: boolean = false;
  browserTabController: WebviewController = new webview.WebviewController(); // WebviewController controller

  build() {
    Stack() {
      Web({ src: $rawfile('nativeembed_view.html'), controller: this.browserTabController })
        .backgroundColor('#F1F3F5')
        .onPageEnd(() => {
          this.browserTabController.runJavaScript(
            'getEmbedSize()',
            (error, result) => {
              if (result) {
                interface EmbedSize {
                  width: number,
                  height: number
                }
                let embedSize = JSON.parse(result) as EmbedSize;
                this.searchWidth = embedSize.width;
                this.searchHeight = embedSize.height;
                this.isWebInit = true;
              }
            });
        })
      if (this.isWebInit){
        Column() {
          // Because it needs to be displayed according to the actual size of the Web, it needs to wait for the width and height to be obtained after the Web is initialized, and then it needs to be layered on the Web
          SearchComponent({ searchWidth: this.searchWidth, searchHeight: this.searchHeight })
        }
        .zIndex(10)
      }
    }
  }
}

/**
 * Set the data class of the item
 */
class ProductDataModel {
  id: number;
  uri: ResourceStr;
  title: ResourceStr;
  price: ResourceStr;

  constructor(id: number, uri: ResourceStr, title: ResourceStr, price: ResourceStr) {
    this.id = id;
    this.uri = uri;
    this.title = title;
    this.price = price;
  }
}

// [Start search_component]
@Component
struct SearchComponent {
  @Prop searchWidth: number;
  @Prop searchHeight: number;

  build() {
    Column({ space: 8 }) {
      Text('商城')
        .fontSize(16)
      Row() {
        Image($r('app.media.nativeembed_search_icon'))
          .width(14)
          .margin({ left: 14 })
        Text('搜索相关宝贝')
          .fontSize(14)
          .opacity(0.6)
          .fontColor('#000000')
          .margin({ left: 14})
      }
      .width('100%')
      .margin(4)
      .height(36)
      .backgroundColor(Color.White)
      .borderRadius(18)
      .onClick(() => {
        this.getUIContext().getPromptAction().showToast({
          message: '仅演示，可自行实现业务功能'
        });
      })
      Grid() {
        ForEach(PRODUCT_DATA, (item: ProductDataModel, index: number) => {
          GridItem() {
            Column({ space: 8 }) {
              Image(item.uri)
                .width(100)
                .height(100)
              Row({ space: 8 }) {
                Text(item.title)
                  .fontSize(12)
                Text(item.price)
                  .fontSize(12)
              }
            }
            .backgroundColor(Color.White)
            .alignItems(HorizontalAlign.Center)
            .justifyContent(FlexAlign.Center)
            .width('100%')
            .borderRadius(12)
            .padding({ bottom: 12 })
          }
        }, (item: ProductDataModel) => `${item.id}`)
      }
      .columnsTemplate('1fr 1fr')
      .rowsGap(8)
      .columnsGap(8)
      .width('100%')
      .height('90%')
      .backgroundColor('#F1F3F5')
    }
    .padding(10)
    .width(this.searchWidth)
    .height(this.searchHeight)
  }
}
// [End search_component]
// [End non_same_layer_rendering]