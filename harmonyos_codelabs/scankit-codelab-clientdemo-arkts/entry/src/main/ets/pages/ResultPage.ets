// ResultPage.ets Display the recognition result and the position for drawing the barcode.
import { hilog } from '@kit.PerformanceAnalysisKit';
import { scanBarcode, scanCore } from '@kit.ScanKit';
import { display } from '@kit.ArkUI';
import { MockSetup } from '@ohos/hamock';
import { UIContextSelf } from '../utils/UIContextSelf';

const TAG: string = 'ScanKit ResultPage';
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
struct ScanResultPage {
  @State params: ParamsType = this.getUIContext().getRouter().getParams() as ParamsType;
  @State cameraHeight: number = 1412;
  @State cameraWidth: number = 720;
  @State layTop: number = 226;
  @State layLeft: number = 0;
  @State screen: number = 1;
  @State rx: number = 0;
  @State ry: number = 0;
  @State rw: number = 0;
  @State rh: number = 0;
  public scroller: Scroller = new Scroller();
  private statusHeight: number = 39;
  private result: Array<scanBarcode.ScanResult> = [];
  private uri: string = '';
  private infoW: number = 0;
  private infoH: number = 0;

  @MockSetup
  mock() {
    this.params = new ParamsType();
    let mockScanResult: scanBarcode.ScanResult = {
      scanType: scanCore.ScanType.QR_CODE,
      originalValue: 'mock',
      scanCodeRect: {
        left: 200,
        right: 300,
        top: 200,
        bottom: 300
      }
    };
    this.params.result = [mockScanResult];
    this.result = [mockScanResult];
  }

  aboutToAppear() {
    UIContextSelf.setUIContextByUIContent(this.getUIContext());
    try {
      let isFold: boolean = display.isFoldable();
      this.setDisplay();
      hilog.info(0x0001, TAG, 'aboutToAppear ResultPage');
      let getParams = this.params as ParamsType;
      // Return the barcode recognition result.
      this.result = getParams.result as Array<scanBarcode.ScanResult>;
      this.uri = getParams.uri as string;
      this.infoW = getParams.infoW as number;
      this.infoH = getParams.infoH as number;
      hilog.info(0x0001, TAG, 'Succeeded in getting Params.');
      hilog.info(0x0001, TAG,
        `Succeeded in getting uri: ${this.uri}, this.infoW: ${this.infoW}, this.infoH: ${this.infoH}`);
      this.calculate();
      if (isFold) {
        display.on('foldStatusChange', async (foldStatus: display.FoldStatus) => {
          // 1: expanded; 2: collapsed
          if (foldStatus === 1 || foldStatus === 2) {
            setTimeout(() => {
              this.setDisplay();
              this.calculate();
            }, 300)
          }
        });
      }
    } catch (error) {
      hilog.error(0x0001, TAG, `Failed to operate display. Code: ${error.code}.`);
    }
  }

  public setDisplay(): void {
    hilog.info(0x0001, TAG, 'setDisplay start');
    let displayClass: display.Display | null = null;
    try {
      displayClass = display.getDefaultDisplaySync();
      hilog.info(0x0001, TAG, `Succeeded in getting displayClass.`);
    } catch (error) {
      hilog.error(0x0001, TAG, `Failed to get displayClass. Code: ${error.code}, message: ${error.message}`);
    }

    if (displayClass !== null) {
      displayClass.height = displayClass.height - UIContextSelf.uiContext.vp2px(this.statusHeight);
      this.cameraHeight = Math.round(UIContextSelf.uiContext.px2lpx(displayClass.height));
      this.cameraWidth = Math.round(UIContextSelf.uiContext.px2lpx(displayClass.width));
      hilog.info(0x0001, TAG, `Succeeded in getting cameraHeight: ${this.cameraHeight}`);
      hilog.info(0x0001, TAG, `Succeeded in getting cameraWidth: ${this.cameraWidth}`);
    }
  }

  public calculate() {
    // Calculate the proportion conversion of the width and height of the current image and the width and height of the screen.
    if (this.infoW && this.infoH && this.cameraHeight && this.cameraWidth) {
      if (this.infoW / this.infoH > this.cameraWidth / this.cameraHeight) {
        this.screen = 720 / this.infoW;
        this.layTop = (this.cameraHeight - this.cameraWidth * this.infoH / this.infoW) / 2;
        this.layLeft = 0;
      } else {
        this.layTop = 0;
        this.layLeft = (this.cameraWidth - this.cameraHeight * this.infoW / this.infoH) / 2;
        this.screen = this.cameraHeight / this.infoH;
      }
    }
  }

  @Builder
  CustomTextBox(scanType: string, inputValue: string, left: number, top: number, right: number, bottom: number) {
    Column() {
      Column() {
        Text($r('app.string.code_format'))
        Text(scanType).align(Alignment.Start)
      }
      .width('100%')
      .justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.Start)
      .margin({ top: 10 })

      Column() {
        Text($r('app.string.result_content'))
        Text(inputValue)
      }.width('100%').justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.Start)
      .margin({ top: 10 })

      Column() {

        Text($r('app.string.code_react')).align(Alignment.Start)
        Text('left: ' + left + '  top: ' + top + '  right: ' + right + '  bottom: ' + bottom).align(Alignment.Start)
      }.width('100%').justifyContent(FlexAlign.Start)
      .alignItems(HorizontalAlign.Start)
      .margin({ top: 10 })

    }.margin({ bottom: 5 })
  }

  build() {
    Stack() {
      // Display the barcode image.
      if (this.uri) {
        Image(this.uri)
          .objectFit(ImageFit.Contain)
          .width('100%').height('100%')
      }
      // Check the return data and display position of the QR code.
      ForEach(this.result, (item: scanBarcode.ScanResult, index: number) => {
        Column() {
          if (item.scanCodeRect && index >= 0) {
            Image($rawfile('scan_selected.svg'))
              .width(40)
              .height(40)
              .markAnchor({ x: 20, y: 20 })
              .position({
                x: Math.round(item.scanCodeRect.left * this.screen + this.layLeft) +
                  Math.round((item.scanCodeRect.right - item.scanCodeRect.left) * this.screen) / 2 + 'lpx',
                y: Math.round(item.scanCodeRect.top * this.screen + this.layTop) +
                  Math.round((item.scanCodeRect.bottom - item.scanCodeRect.top) * this.screen) / 2 + 'lpx',
              })
          }
        }.width('100%')
        .height('100%')
      }, (item: scanBarcode.ScanResult, _: number) => item.originalValue)

      Scroll(this.scroller) {
        Column() {
          Image($rawfile('scan_back.svg'))
            .fillColor(Color.Black)
            .width(30)
            .height(30)
            .objectFit(ImageFit.Contain)
            .position({ x: 0, y: 0 })
            .zIndex(10)
            .onClick(async () => {
              UIContextSelf.getRouter().back();
            })
          Column() {
            Text($r('app.string.title_result'))
              .fontSize(18)
              .fontColor($r('sys.color.black'))
              .margin({ top: 20 })
            ForEach(this.result, (item: scanBarcode.ScanResult) => {
              if (item['scanCodeRect']) {
                this.CustomTextBox(this.getScanTypeKey(item['scanType']), item['originalValue'],
                  item['scanCodeRect']['left'],
                  item['scanCodeRect'].top, item['scanCodeRect'].right, item['scanCodeRect'].bottom)
              }
            }, (item: scanBarcode.ScanResult) => item.originalValue)
          }
          .width('80%')
          .height('100%')
        }
      }
      .scrollable(ScrollDirection.Vertical)
      .scrollBar(BarState.On)
      .scrollBarColor(Color.Gray)
      .scrollBarWidth(5)
      .height('100%')
    }
    .width('100%')
    .height('100%')
    .padding({
      top: 40
    })
  }

  public getScanTypeKey(type: number): string {
    return SCAN_TYPE[type] || '';
  }
}
