import { fileIo as fs, picker } from '@kit.CoreFileKit';
import { router } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG = '[PDFView_LOG_TAG]';

function getCurrentDateFormatted(): string {
  const now = new Date();
  const year = now.getFullYear();
  const month = (now.getMonth() + 1).toString().padStart(2, '0');
  const date = now.getDate().toString().padStart(2, '0');
  return `${year}-${month}-${date}`;
}

async function copyResourceFileAndJump(): Promise<void> {
  try {

    let context = getContext() as common.UIAbilityContext;
    let dir = context.filesDir;
    let filePath = dir + "/input.pdf";
    let res = fs.accessSync(filePath);
    if (res) {
      hilog.info(0x0000, TAG, "file exists 0");
    } else {
      hilog.info(0x0000, TAG, "file not exists");
      let content = context.resourceManager.getRawFileContentSync('rawfile/pdf_reference.pdf');
      let fdSand = fs.openSync(filePath, fs.OpenMode.WRITE_ONLY | fs.OpenMode.CREATE | fs.OpenMode.TRUNC);
      fs.writeSync(fdSand.fd, content.buffer);
      fs.closeSync(fdSand.fd);
    }
    let result = filePath;
    hilog.info(0x0000, TAG, "file exists");
    router.pushUrl({
      url: 'pages/PDFPreview',
      params: {
        src: result,
      }
    })
  } catch (e) {
    let err = e as BusinessError;
    hilog.error(0x0000, TAG, `Get exception: ${err}`);
  }
}

@Entry
@Component
struct Index {
  @State message: string = 'Hello World'

  aboutToAppear(): void {
    let context = getContext() as common.UIAbilityContext;
    const dir = context.filesDir + `/${getCurrentDateFormatted()}/`;
    fs.mkdir(dir);
  }

  // Write the PDF file in the file to the sandbox and go to the next page.
  private async copy_uri_and_jump(uri: string) {
    try {
      let dir = getContext().filesDir;
      let file = dir + "/temp.pdf";
      hilog.info(0x0000, TAG, "[rawfile_copy_to_sandbox] Path:" + file);
      let sourceFile = fs.openSync(uri, fs.OpenMode.READ_ONLY);
      fs.copyFile(sourceFile.fd, file).then(() => {
        hilog.info(0x0000, TAG, "copyFile succeed");
        router.pushUrl({
          url: 'pages/PDFPreview',
          params: {
            src: file,
          }
        })
      }).catch((err: BusinessError) => {
        hilog.error(0x0000, TAG, "copyFile failed with error:" + err);
      });
    } catch (error) {
      hilog.error(0x0000, TAG, "[rawfile_copy_to_sandbox] getRawFileDescriptor api Running failed " + error);
      hilog.info(0x0000, TAG, "[rawfile_copy_to_sandbox] Failed to copy the resource file to the application sandbox");
    }
  }

  build() {
    Row() {
      Column() {
        Button("Open the PDF file on the computer")
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            // Copy the file to the sandbox and push it to the next pdfViewer page to display the function.
            const documentSelectOptions = new picker.DocumentSelectOptions();
            documentSelectOptions.maxSelectNumber = 1; // (Optional) Select the maximum number of documents.
            // documentSelectOptions.defaultFilePathUri = "file://docs/storage/Users/currentUser/test";
            documentSelectOptions.fileSuffixFilters = ['.pdf'];
            let uris: Array<string> = [];
            const documentViewPicker = new picker.DocumentViewPicker();
            documentViewPicker.select(documentSelectOptions).then((documentSelectResult: Array<string>) => {
              uris = documentSelectResult;
              let uri = uris[0];
              this.copy_uri_and_jump(uri)
              hilog.info(0x0000, TAG, 'documentViewPicker.select to file succeed and uri are:' + uri);
            }).catch((err: BusinessError) => {
              hilog.info(0x0000, TAG, `Invoke documentViewPicker.select failed, code is ${err.code}, message is ${err.message}`);
            })
          })
        Button("Opening a resource PDF file")
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            copyResourceFileAndJump()
          }).margin({ top: 15 })
        Button("Opening the PdfView file for preview")
          .fontSize(20)
          .fontWeight(FontWeight.Bold)
          .margin({ top: 15 })
          .onClick(()=>{
            router.pushUrl({
              url: 'pages/PDFView'
            })
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}