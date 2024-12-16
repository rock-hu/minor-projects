// BarcodePage.ets Scan the barcode using the default UI.
import { hilog } from '@kit.PerformanceAnalysisKit';
import { scanCore, scanBarcode } from '@kit.ScanKit';
import { BusinessError } from '@kit.BasicServicesKit';

const TAG: string = 'ScanKit scanBarcode';
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

@Entry
@Component
struct ScanBarcodePage {
  @State inputValue: string = ''
  @State scanType: number = -1

  build() {
    Column() {
      Button($r('app.string.default_view_decode'))
        .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
        .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
        .align(Alignment.Center)
        .type(ButtonType.Capsule)
        .margin({ bottom: 12 })
        .width('90%')
        .height(40)
        .margin({ top: 5, bottom: 5 })
        .onClick(() => {
          // Define scanning parameters.
          let options: scanBarcode.ScanOptions = {
            scanTypes: [scanCore.ScanType.ALL],
            enableMultiMode: true,
            enableAlbum: true
          };
          try {
            scanBarcode.startScanForResult(getContext(this), options).then((result: scanBarcode.ScanResult) => {
              // Receive the correct barcode scanning result.
              this.scanType = result.scanType;
              this.inputValue = result.originalValue;
              hilog.debug(0x0001, TAG, `Succeeded in getting ScanResult by promise with options.`);
            }).catch((error: BusinessError) => {
              if (error.code == scanCore.ScanErrorCode.SCAN_SERVICE_CANCELED) {
                hilog.info(0x0001, TAG, `Disabling the Scanning Service.`);
              } else {
                hilog.error(0x0001, TAG,
                  `Failed to start the scanning service. Code: ${error.code}, message: ${error.message}`);
              }
            });
          } catch (error) {
            hilog.error(0x0001, TAG, `Failed to start scan. Code: ${error.code}, message: ${error.message}`);
          }
        })

      CustomTextBox({ scanType: this.getScanTypeKey(this.scanType), inputValue: this.inputValue })
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
    .alignItems(HorizontalAlign.Center)
  }

  getScanTypeKey(type: number): string {
    return SCAN_TYPE[type] || ''
  }
}

@Component
export struct CustomTextBox {
  @Prop inputValue: string = ''
  @Prop scanType: string = ''

  build() {
    Column() {
      Column() {
        Text(this.scanType)
          .fontSize(20)
          .fontColor(Color.Black)
          .fontWeight(FontWeight.Normal)
          .align(Alignment.Center)
          .width('90%')
          .margin({ top: 5, bottom: 5 })
      }

      Column() {
        Text(this.inputValue)
          .fontSize(20)
          .fontColor(Color.Black)
          .fontWeight(FontWeight.Normal)
          .align(Alignment.Center)
          .width('90%')
          .margin({ top: 5, bottom: 5 })
      }
    }
  }
}
