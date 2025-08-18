// [Start case_three]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  webviewController: webview.WebviewController = new webview.WebviewController();
  build() {
    Column() {
      Web({ src: 'https://www.example.com/', controller: this.webviewController})
        .onPageEnd(() => {
          // Pre-connected https://www.example1.com/
          webview.WebviewController.prepareForPageLoad('https://www.example.com/', true, 2);
        })
    }
  }
}
// [End case_three]