import { pdfService, pdfViewManager, PdfView } from '@kit.PDFKit'
import { router } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { fileIo } from '@kit.CoreFileKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct PDFView {
  private controller: pdfViewManager.PdfController = new pdfViewManager.PdfController();

  aboutToAppear(): void {
    let context = getContext() as common.UIAbilityContext;
    let dir: string = context.filesDir
    // Ensure that the rawfile directory contains PDF files
    let filePath: string = dir + "/pdf_reference.pdf";
    fileIo.accessSync(filePath);
    let content: Uint8Array = context.resourceManager.getRawFileContentSync('rawfile/pdf_reference.pdf');
    let fdSand = fileIo.openSync(filePath, fileIo.OpenMode.WRITE_ONLY | fileIo.OpenMode.CREATE | fileIo.OpenMode.TRUNC);
    fileIo.writeSync(fdSand.fd, content.buffer);
    fileIo.closeSync(fdSand.fd);
    (async () => {
      this.controller.registerPageCountChangedListener((pageCount: number) => {
        hilog.info(0x0000, 'registerPageCountChanged-', pageCount.toString());
      });
      let loadResult1: pdfService.ParseResult = await this.controller.loadDocument(filePath);
      if (pdfService.ParseResult.PARSE_SUCCESS === loadResult1) {
        hilog.info(0x0000, 'aboutToAppear', 'PdfView Component has been successfully loaded!');
      }
    })()
  }

  @Builder
  Back() {
    Button("Back")
      .width(100)
      .fontSize(20)
      .padding({left: 30, top: 8})
      .margin({left: 10, top: 10})
      .onClick(() => {
        router.back({ url: 'pages/Index' });
      })
  }

  build() {
    Row() {
      PdfView({
        controller: this.controller,
        pageFit: pdfService.PageFit.FIT_WIDTH,
        showScroll: true
      })
        .id('pdfview_app_view')
        .layoutWeight(1)
        .overlay(this.Back(), {
          align: Alignment.TopStart
        });
    }
    .width('100%')
    .height('100%')
  }
}