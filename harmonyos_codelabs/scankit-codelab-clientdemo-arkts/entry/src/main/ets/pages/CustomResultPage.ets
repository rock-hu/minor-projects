// CustomResultPage.ets Display the result of barcode scanning using the custom UI.
import { scanBarcode, scanCore } from '@kit.ScanKit';
import { MockSetup } from '@ohos/hamock';
import { UIContextSelf } from '../utils/UIContextSelf';

@Extend(Text)
function textName() {
  .fontSize(20)
  .fontColor($r('sys.color.black'))
  .fontWeight(FontWeight.Medium)
  .width('100%')
  .margin({ bottom: 5 })
}

@Extend(Text)
function textValue() {
  .fontSize(16)
  .fontWeight(FontWeight.Normal)
  .width('100%')
  .margin({ bottom: 5 })
}

const SCAN_TYPE: Record<number, string> = {
  0: 'FORMAT_UNKNOWN',
  1: 'AZTEC_CODE',
  2: 'CODABAR_CODE',
  3: 'CODE39_CODE',
  4: 'CODE93_CODE',
  5: 'CODE128_CODE',
  6: 'DATAMATRIX_CODE',
  7: 'EAN8_CODE',
  8: 'EAN13_CODE',
  9: 'ITF14_CODE',
  10: 'PDF417_CODE',
  11: 'QR_CODE',
  12: 'UPC_A_CODE',
  13: 'UPC_E_CODE',
  14: 'MULTIFUNCTIONAL_CODE',
  100: 'ONE_D_CODE',
  101: 'TWO_D_CODE'
};

class ParamsType {
  public result: scanBarcode.ScanResult[] = []
  public uri: string = ''
  public infoW: number = 0
  public infoH: number = 0
}

@Entry
@Component
struct CustomResultPage {
  @State params: ParamsType = UIContextSelf.getRouter().getParams() as ParamsType;
  private result: Array<scanBarcode.ScanResult> = [];
  public scroller: Scroller = new Scroller();

  aboutToAppear() {
    this.result = this.params.result as Array<scanBarcode.ScanResult>;
  }

  @MockSetup
  mock() {
    this.params = new ParamsType();
    let mockScanResult: scanBarcode.ScanResult = {
      scanType: scanCore.ScanType.QR_CODE,
      originalValue: 'mock',
      scanCodeRect: {
        left: 10,
        right: 100,
        top: 10,
        bottom: 100
      }
    };
    this.params.result = [mockScanResult];
    this.result = this.params.result as Array<scanBarcode.ScanResult>;
  }

  @Builder
  CustomTextBox(scanType: string, inputValue: string, left: number, top: number, right: number, bottom: number) {
    Column() {
      Column() {
        Text($r('app.string.code_format')).textName()
        Text(scanType).textValue()

      }
      .align(Alignment.Start)
      .margin({ left: 10, top: 20, bottom: 20 })

      Column() {
        Text($r('app.string.result_content')).textName()
          .fontWeight(FontWeight.Medium)
        Text(inputValue).textValue()
      }
      .align(Alignment.Start)
      .margin({ left: 10, bottom: 20 })

      Column() {
        Text($r('app.string.code_react')).textName()
        Text('left: ' + left + '  top: ' + top + '  right: ' + right + '  bottom: ' + bottom).textValue()
      }
      .align(Alignment.Start)
      .margin({ left: 10, bottom: 20 })
    }.margin({ bottom: 5 })
    .backgroundColor('#fffaf8f8')
    .borderRadius(8)
  }

  build() {
    Column() {
      Row() {
        Image($rawfile('scan_back.svg'))
          .fillColor(Color.Black)
          .width(30)
          .height(30)
          .objectFit(ImageFit.Contain)
        Text($r('app.string.title_result'))
          .fontSize(20)
          .fontColor($r('sys.color.black'))
          .margin({ left: 15 })
      }
      .margin({ top: 20 })
      .zIndex(10)
      .position({ x: 15, y: 0 })
      .onClick(async () => {
        UIContextSelf.getRouter().pushUrl({
          url: 'pages/Index'
        }).then(() => {
          UIContextSelf.getRouter().clear();
        });
      })

      Scroll(this.scroller) {
        Column() {
          Column() {
            ForEach(this.result, (item: scanBarcode.ScanResult) => {
              if (item['scanCodeRect']) {
                this.CustomTextBox(this.getScanTypeKey(item['scanType']), item['originalValue'],
                  item['scanCodeRect']['left'],
                  item['scanCodeRect'].top, item['scanCodeRect'].right, item['scanCodeRect'].bottom)
              }
            }, (item: scanBarcode.ScanResult) => item.originalValue)
          }
        }
        .width(330)
      }
      .scrollable(ScrollDirection.Vertical)
      .scrollBar(BarState.On)
      .scrollBarColor(Color.Gray)
      .scrollBarWidth(5)
      .margin({ top: 120 })
    }
    .width('100%')
    .padding({
      top: 40
    })
  }

  public getScanTypeKey(type: number): string {
    return SCAN_TYPE[type] || '';
  }
}
