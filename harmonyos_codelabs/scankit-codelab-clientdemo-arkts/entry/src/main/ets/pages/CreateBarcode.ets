// CreateBarcode.ets Generate a barcode image.
import { image } from '@kit.ImageKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { scanCore, generateBarcode } from '@kit.ScanKit';
import { buffer } from '@kit.ArkTS';

const TAG: string = 'ScanKit createBarcode';

@Extend(Text)
function demoText() {
  .width(60)
  .height(35)
  .fontSize(14)
  .fontColor(0x000000)
  .textAlign(TextAlign.Start)
}

@Extend(TextInput)
function demoTextInput() {
  .caretColor(Color.Blue)
  .fontSize(14)
  .fontWeight(FontWeight.Bold)
  .height(35)
  .width('60%')
}

@Extend(TextArea)
function demoTextArea() {
  .caretColor(Color.Black)
  .fontSize(14)
  .fontColor(Color.Black)
  .fontWeight(FontWeight.Bold)
  .width('80%')
}

@Extend(Select)
function demoSelect() {
  .font({ size: 14 })
  .selectedOptionFont({ size: 14 })
  .optionFont({ size: 14 })
  .width('60%')
  .height(30)
  .backgroundColor('#eeeeee')
}

@Extend(Row)
function demoRow() {
  .margin({ bottom: 15 })
  .justifyContent(FlexAlign.Center)
}

@Extend(Button)
function genButton() {
  .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
  .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
  .align(Alignment.Center)
  .type(ButtonType.Capsule)
  .margin({ bottom: 12 })
  .width('90%')
  .height(40)
}

@Entry
@Component
struct CreateBarcodePage {
  @State pixelMap: image.PixelMap | undefined = undefined
  @State mContext: string = '123' // Default content of the QR code.
  @State mScanType: number = scanCore.ScanType.QR_CODE // Default barcode type, which is QR code.
  @State mWidth: string = '800' // Default barcode width, in px.
  @State mHeight: string = '800' // Default barcode height, in px.
  @State mMargin: number = 1 // Default barcode margin, in px.
  @State mLevel: generateBarcode.ErrorCorrectionLevel =
    generateBarcode.ErrorCorrectionLevel.LEVEL_H // Default error correction level of the barcode image.
  @State mBackgroundColor: number = 0xffffff // Default background color of the barcode image.
  @State mPixelMapColor: number = 0x000000 // Default color of the barcode image.

  build() {

    Flex({
      direction: FlexDirection.Row,
      wrap: FlexWrap.Wrap,
      alignItems: ItemAlign.Center,
      justifyContent: FlexAlign.Center
    }) {
      Column() {
      }.height(30)

      Row() {
        // Set the QR code content.
        Text($r('app.string.barcode_content')).demoText()
        TextArea({ placeholder: this.mContext })
          .demoTextArea()
          .onChange((value: string) => {
            this.mContext = value;
          })
      }.width('100%').demoRow()

      // Set the QR code width.
      Row() {
        Text($r('app.string.barcode_width')).demoText()
        TextInput({ placeholder: '800' }).demoTextInput()
          .type(InputType.Number)
          .onChange((value: string) => {
            this.mWidth = value;
          })
      }.width('50%').demoRow()

      // Set the QR code height.
      Row() {
        Text($r('app.string.barcode_height')).demoText()
        TextInput({ placeholder: '800' }).demoTextInput()
          .type(InputType.Number)
          .onChange((value: string) => {
            this.mHeight = value;
          })
      }.width('50%').demoRow()

      // Set the QR code type.
      Row() {
        Text($r('app.string.barcode_type')).demoText()
        Column() {
          Select(ScanConstant.codeTypeItems)
            .value('QR_CODE')
            .selected(10)
            .demoSelect()
            .onSelect((index: number, value?: string) => {
              hilog.info(0x0001, TAG, `Succeeded in selecting scanType, index is ${index}`);
              this.mScanType = getTypeNum(value);
            })
        }
      }.width('50%').demoRow()

      // Set the QR code margin.
      Row() {
        Text($r('app.string.barcode_margin')).demoText()
        TextInput({ placeholder: '1' }).demoTextInput()
          .type(InputType.Number)
          .onChange((value: string) => {
            this.mMargin = Number.parseInt(value);
          })
      }.width('50%').demoRow()

      // Set the QR code color.
      Row() {
        Text($r('app.string.barcode_color')).demoText()
        Column() {
          Select(ScanConstant.colorItems)
            .selected(0)
            .value('Black')
            .width(100)
            .demoSelect()
            .onSelect((index: number, value?: string) => {
              hilog.info(0x0001, TAG, `Succeeded in selecting pixelMapColor, index is ${index}`);
              this.mPixelMapColor = getColorType(value);
            })
        }
      }.width('50%').demoRow()

      // Set the QR code background color.
      Row() {
        Text($r('app.string.barcode_backgroundcolor')).demoText()
        Column() {
          Select(ScanConstant.colorItems)
            .selected(7)
            .value('White')
            .width(80)
            .demoSelect()
            .onSelect((index: number, value?: string) => {
              hilog.info(0x0001, TAG, `Succeeded in selecting backgroundColor, index is ${index}`);
              this.mBackgroundColor = getColorType(value);
            })
            .align(Alignment.Center)
        }
      }.width('50%').demoRow()

      // Set the QR code error correction level.
      Row() {
        Text($r('app.string.barcode_error_level')).demoText()
        Column() {
          Select(ScanConstant.errorLevelItems)
            .selected(3)
            .value('LEVEL_H')
            .width('60%')
            .demoSelect()
            .onSelect((index: number, value?: string) => {
              hilog.info(0x0001, TAG, `Succeeded in selecting errorLevel, index is ${index}`);
              this.mLevel = getLevelType(value);
            })
        }
      }.width('55%').demoRow()

      Row() {
      }.width('45%').demoRow()

      // Generate a barcode image from text using a promise.
      Button($r('app.string.generate_barcode')).genButton()
        .onClick(() => {
          this.pixelMap = undefined;
          let content = this.mContext;
          let options: generateBarcode.CreateOptions = {
            scanType: this.mScanType,
            width: Number(this.mWidth),
            height: Number(this.mHeight),
            margin: Number(this.mMargin),
            level: this.mLevel,
            backgroundColor: this.mBackgroundColor,
            pixelMapColor: this.mPixelMapColor,
          }
          try {
            generateBarcode.createBarcode(content, options).then((result: image.PixelMap) => {
              this.pixelMap = result;
            }).catch((error: BusinessError) => {
              showError(error);
              hilog.error(0x0001, TAG,
                `Failed to get pixelMap by promise with options. Code: ${error.code}, message: ${error.message}`);
            })
          } catch (error) {
            showError(error);
            hilog.error(0x0001, TAG, `Failed to createBarCode. Code: ${error.code}, message: ${error.message}`);
          }
        })

      // Generate a barcode image from a byte array using a promise.
      Button($r('app.string.generate_trip_barcode')).genButton()
        .onClick(() => {
          this.pixelMap = undefined;
          let content = this.mContext;
          let pattern = /^[0-9a-fA-F]+$/;
          if (content && !pattern.test(content)) {
            promptAction.showToast({
              message: $r('app.string.trip_barcode_tip'),
              duration: 2000
            });
            return;
          }
          if (content.length % 2 !== 0) {
            content = '0' + content;
          }
          let contentBuffer: ArrayBuffer = buffer.from(content, 'hex').buffer;
          let options: generateBarcode.CreateOptions = {
            scanType: this.mScanType,
            height: Number(this.mHeight),
            width: Number(this.mWidth),
            margin: Number(this.mMargin),
            level: this.mLevel,
            backgroundColor: this.mBackgroundColor,
            pixelMapColor: this.mPixelMapColor,
          }

          try {
            generateBarcode.createBarcode(contentBuffer, options).then((result: image.PixelMap) => {
              this.pixelMap = result;
              hilog.info(0x0001, TAG, 'Succeeded in creating barCode.');
            }).catch((error: BusinessError) => {
              showError(error);
              hilog.error(0x0001, TAG,
                `catch Failed to create barCode. Code: ${error.code}, message: ${error.message}`);
            })
          } catch (error) {
            showError(error);
            hilog.error(0x0001, TAG, `000 Failed to create barCode. Code: ${error.code}, message: ${error.message}`);
          }
        })

      // Barcode generated. The image size is (300, 300).
      Column() {
        if (this.pixelMap) {
          Image(this.pixelMap).width(300).height(300).objectFit(ImageFit.Contain)
        }
      }.width(300).height(300).margin(10)

    }
    .width('100%')
    .height('100%')
    .backgroundColor('#eeeeee')
    .padding({
      left: 10,
      top: 40,
      right: 10,
      bottom: 10
    })
  }
}

// Error message.
function showError(businessError: BusinessError): void {
  try {
    promptAction.showToast({
      message: `Error Code: '${businessError.code}', message: ${businessError.message}`,
      duration: 2000
    });
  } catch (error) {
    if (error) {
      hilog.error(0x0001, TAG, `Failed to ShowToast. Code: ${error.code}, message: ${error.message}`);
    }
  }
}

export default class ScanConstant {
  static readonly colorItems: Array<SelectOption> = [
    {
      value: 'Black'
    },
    {
      value: 'Blue'
    },
    {
      value: 'Brown'
    },
    {
      value: 'Gray'
    },
    {
      value: 'Green'
    },
    {
      value: 'Orange'
    },
    {
      value: 'Red'
    },
    {
      value: 'White'
    },
    {
      value: 'Yellow'
    }];
  static readonly errorLevelItems: Array<SelectOption> = [
    {
      value: 'LEVEL_L'
    },
    {
      value: 'LEVEL_M'
    },
    {
      value: 'LEVEL_Q'
    },
    {
      value: 'LEVEL_H'
    }];
  static readonly codeTypeItems: Array<SelectOption> = [
    {
      value: 'AZTEC_CODE'
    },
    {
      value: 'CODABAR_CODE'
    },
    {
      value: 'CODE93_CODE'
    },
    {
      value: 'CODE39_CODE'
    },
    {
      value: 'CODE128_CODE'
    },
    {
      value: 'DATAMATRIX_CODE'
    },
    {
      value: 'EAN8_CODE'
    },
    {
      value: 'EAN13_CODE'
    },
    {
      value: 'ITF14_CODE'
    },
    {
      value: 'PDF417_CODE'
    },
    {
      value: 'QR_CODE'
    },
    {
      value: 'UPC_A_CODE'
    },
    {
      value: 'UPC_E_CODE'
    }];
}

function getLevelType(type?: string): generateBarcode.ErrorCorrectionLevel {
  const LEVEL_TYPE: Record<string, number> = {
    'LEVEL_L': generateBarcode.ErrorCorrectionLevel.LEVEL_L,
    'LEVEL_M': generateBarcode.ErrorCorrectionLevel.LEVEL_M,
    'LEVEL_Q': generateBarcode.ErrorCorrectionLevel.LEVEL_Q,
    'LEVEL_H': generateBarcode.ErrorCorrectionLevel.LEVEL_H
  }
  return type ? LEVEL_TYPE[type] || LEVEL_TYPE['LEVEL_H'] : LEVEL_TYPE['LEVEL_H'];
}

function getColorType(type?: string): number {
  const COLOR_MAP: Record<string, number> = {
    'Black': 0x000000,
    'Blue': 0x0000ff,
    'Brown': 0xa52a2a,
    'Gray': 0x808080,
    'Green': 0x008000,
    'Orange': 0xffa500,
    'Red': 0xff0000,
    'White': 0xffffff,
    'Yellow': 0xffff00,
    'Default': 0x1e1e1e
  };
  return type ? COLOR_MAP[type] || COLOR_MAP['Black'] : COLOR_MAP['Black'];
}

function getTypeNum(type?: string): scanCore.ScanType {
  const SCAN_TYPE: Record<string, number> = {
    'AZTEC_CODE': scanCore.ScanType.AZTEC_CODE,
    'CODABAR_CODE': scanCore.ScanType.CODABAR_CODE,
    'CODE39_CODE': scanCore.ScanType.CODE39_CODE,
    'CODE93_CODE': scanCore.ScanType.CODE93_CODE,
    'CODE128_CODE': scanCore.ScanType.CODE128_CODE,
    'DATAMATRIX_CODE': scanCore.ScanType.DATAMATRIX_CODE,
    'EAN8_CODE': scanCore.ScanType.EAN8_CODE,
    'EAN13_CODE': scanCore.ScanType.EAN13_CODE,
    'ITF14_CODE': scanCore.ScanType.ITF14_CODE,
    'PDF417_CODE': scanCore.ScanType.PDF417_CODE,
    'QR_CODE': scanCore.ScanType.QR_CODE,
    'UPC_A_CODE': scanCore.ScanType.UPC_A_CODE,
    'UPC_E_CODE': scanCore.ScanType.UPC_E_CODE
  }
  return type ? SCAN_TYPE[type] || SCAN_TYPE['QR_CODE'] : SCAN_TYPE['QR_CODE'];
}