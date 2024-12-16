// DetectBarcode.ets Use the picker to select an image for barcode recognition.
import { image } from '@kit.ImageKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { fileIo } from '@kit.CoreFileKit';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { router, promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { scanCore, detectBarcode, scanBarcode } from '@kit.ScanKit';

const TAG = 'ScanKit detectBarcode';

@Entry
@Component
struct DetectBarcodePage {
  build() {
    Column() {
      Button($r('app.string.bitmap_api_decode'))
        .backgroundColor($r('sys.color.ohos_id_color_button_normal'))
        .fontColor($r('sys.color.ohos_id_color_text_primary_activated'))
        .align(Alignment.Center)
        .type(ButtonType.Capsule)
        .margin({ bottom: 12 })
        .width('90%')
        .height(40)
        .onClick(() => {
          // Define scanning options for image-based barcode recognition.
          let options: scanBarcode.ScanOptions = {
            scanTypes: [scanCore.ScanType.ALL],
            enableMultiMode: false // Set this parameter to false to recognize only one barcode, and to true to enable scanning of multiple barcodes.
          }
          try {
            // Open the picker to select an image from the gallery.
            let photoOption = new photoAccessHelper.PhotoSelectOptions();
            photoOption.MIMEType = photoAccessHelper.PhotoViewMIMETypes.IMAGE_TYPE;
            photoOption.maxSelectNumber = 1;
            let photoPicker = new photoAccessHelper.PhotoViewPicker();
            photoPicker.select(photoOption).then((photoResult) => {
              let infoW: number = 0;
              let infoH: number = 0;
              try {
                let file = fileIo.openSync(photoResult.photoUris[0], fileIo.OpenMode.READ_ONLY);
                const imageSourceApi = image.createImageSource(file.fd);
                try {
                  fileIo.closeSync(file);
                } catch (error) {
                  hilog.error(0x0001, TAG, `Failed to closeSync. Code: ${error.code}.`);
                }
                imageSourceApi.getImageInfo(0, (error: BusinessError, imageInfo) => {
                  try {
                    imageSourceApi.release();
                  } catch (error) {
                    hilog.error(0x0001, TAG,
                      `Failed to release imageSourceApi. Code: ${error.code}, message: ${error.message}`);
                    return;
                  }
                  if (imageInfo == undefined) {
                    hilog.error(0x0001, TAG,
                      `Failed to get imageInfo by callbak. Code: ${error.code}, message: ${error.message}`);
                    return;
                  }
                  infoW = imageInfo.size.width;
                  infoH = imageInfo.size.height;
                });
              } catch (error) {
                hilog.error(0x0001, TAG, `Failed to get imageInfo. Code: ${error.code}, message: ${error.message}`);
                return;
              }
              try {
                // Define the barcode scanning parameter inputImage. The uri parameter indicates the image selected using the picker.
                let inputImage: detectBarcode.InputImage = { uri: photoResult.photoUris[0] }
                // Call the image-based barcode recognition API that uses a promise.
                detectBarcode.decode(inputImage, options).then((result: Array<scanBarcode.ScanResult>) => {
                  hilog.info(0x0001, TAG, `Succeeded in getting ScanResult by promise with options.`);
                  if (result && result.length > 0) {
                    router.pushUrl({
                      url: 'pages/ResultPage',
                      params: {
                        result: result,
                        uri: photoResult.photoUris[0],
                        infoW: infoW,
                        infoH: infoH,
                        options: options
                      }
                    })
                  } else {
                    promptAction.showToast({
                      message: $r('app.string.Scan_diaLog_not_found'),
                      duration: 2000
                    });
                  }
                }).catch((error: BusinessError) => {
                  if (photoResult.photoUris[0]) {
                    promptAction.showToast({
                      message: $r('app.string.Scan_diaLog_not_found'),
                      duration: 2000
                    });
                    hilog.error(0x0001, TAG,
                      `Failed to get ScanResult by promise with options. Code: ${error.code}, message: ${error.message}`);
                  }
                });
              } catch (error) {
                hilog.error(0x0001, TAG, `Failed to decode. Code: ${error.code}, message: ${error.message}`);
              }
            })
          } catch (error) {
            hilog.error(0x0001, TAG, `Failed to select photo. Code: ${error.code}, message: ${error.message}`);
          }
        });
    }
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
  }
}