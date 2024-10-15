import { pdfService } from '@kit.PDFKit';
import { fileIo as fs, fileUri as fileuri } from '@kit.CoreFileKit';
import { image } from '@kit.ImageKit';
import { font, router } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG = '[PDFKit_LOG_TAG]';

@Entry
@Component
struct PDFPreview {
  @State filePath: string = (router.getParams() as Record<string, string>)['src'];
  @State pageCount: number = 0;
  @State pageIndex: number = 0;
  @State _pixelMap: image.PixelMap | undefined = undefined;
  private document: pdfService.PdfDocument = new pdfService.PdfDocument;

  // Open the selected PDF file.
  openFile() {
    let t0 = Date.now();
    this.document.loadDocument(this.filePath, '');
    let t1 = Date.now();
    hilog.info(0x0000, 'openFile-', "loadDocument time: " + (t1 - t0));
  }

  // Prompt box.
  showDialog(title: string, msg: string) {
    AlertDialog.show(
      {
        title: title,
        message: msg,
        autoCancel: true,
        alignment: DialogAlignment.Bottom,
        offset: { dx: 0, dy: -20 },
        gridCount: 3,
        confirm: {
          value: 'Close',
          action: () => {
            hilog.info(0x0000, 'showDialog', 'Button-clicking callback');
          }
        },
      })
  }

  aboutToAppear() {
    this.openFile(); // Open a file.
    let page: pdfService.PdfPage = this.document.getPage(0); // The first page is opened by default.
    this._pixelMap = page.getPagePixelMap(); // Set the parameters of the Image component.
  }

  aboutToDisappear() {
    this.document.releaseDocument();
  }

  build() {
    Column() {
      Row() {
        Button ("Back")
          .fontSize(12)
          .onClick(() => {
            router.back({ url: 'pages/Index' });
          })
        Button ("Previous")
          .fontSize(12)
          .onClick(async () => {
            this.pageIndex--;
            if (this.pageIndex < 0) {
              this.showDialog ('Prompt','This is the first page.');
              this.pageIndex++;
              return;
            }
            let page: pdfService.PdfPage = this.document.getPage(this.pageIndex); // Set the current page to the previous page.
            this._pixelMap = page.getPagePixelMap(); // Set the parameters of the Image component.
          }).margin({ left: 15 })
        Button ("Next")
          .fontSize(12)
          .onClick(async () => {
            this.pageIndex++;
            if (this.pageIndex >= this.document.getPageCount()) {
              this.showDialog ('Prompt','This is the last page.');
              this.pageIndex--;
              return;
            }
            let page: pdfService.PdfPage = this.document.getPage(this.pageIndex); // Set the current page to the next page.
            this._pixelMap = page.getPagePixelMap(); // Set the parameters of the Image component.
          }).margin({ left: 15 })
      }
      Row() {
        // Display content.
        Image(this._pixelMap)
          .width('80%')
          .height('80%')
          .objectFit(ImageFit.Contain)
          .border({ width: 1, color: Color.Blue })
      }
      Row() {
        Button ("Convert PDF to image.")
          .fontSize(12)
          .onClick(async () => {
            let uri = 'file://docs/storage/Users/currentUser' + '/outputImg'; // Image storage path.
            let fileUri: fileuri.FileUri = new fileuri.FileUri(uri);
            let path = fileUri.path;
            fs.mkdir(path);
            const result: boolean = this.document.convertToImage(path, 0); // Convert the PDF file to an image.
            if (result) {
              this.showDialog('The PDF file is successfully converted to an image.','Storage path: user directory/outputImg');
            } else {
              this.showDialog('Failed to convert the PDF to an image.', 'Check user permissions.');
            }
          }).margin({ left: 15 })

        Button("Add sample text.")
          .fontSize(12)
          .onClick(async () => {
            let page: pdfService.PdfPage = this.document.getPage(this.pageIndex);
            let textStyle: pdfService.TextStyle = new pdfService.TextStyle;
            let fontList: Array<string> = font.getSystemFontList();
            let fontInfo = new pdfService.FontInfo();
            if (fontList.length > 0) {
              fontInfo.fontPath = font.getFontByName(fontList[0]).path;
            } else {
              // Ensure that the font path exists.
              fontInfo.fontPath = '/system/fonts/HarmonyOS_Sans.ttf';
            }
            textStyle.fontInfo = fontInfo;
            textStyle.textSize = 20;
            page.addTextObject("HelloWold123", 0, 0, textStyle); // Add sample text.
            this._pixelMap = page.getPagePixelMap();
          }).margin({ left: 15 })
        Button("Save the file.")
          .fontSize(12)
          .onClick(async () => {
            let context = getContext() as common.UIAbilityContext;
            let dir: string = context.filesDir;
            let savePath: string = dir + "/output.pdf";
            const result: boolean = this.document.saveDocument(savePath); //Save the file.
            if (result) {
              this.showDialog('The PDF file is successfully saved.','Storage path: /data/storage/el2/base/haps/entry/files/output.pdf');
            } else {
              this.showDialog('Failed to save the PDF file.', 'Check user permissions.');
            }
          }).margin({ left: 15 })
      }
    }
    .width('100%')
  }
}