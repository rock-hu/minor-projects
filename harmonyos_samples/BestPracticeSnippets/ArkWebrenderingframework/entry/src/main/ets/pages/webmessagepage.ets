
import { webview } from '@kit.ArkWeb';

let nativeObj:object ;

@Component
struct webmessagepage {
  private webviewController: webview.WebviewController = new webview.WebviewController();
  // [Start web]
  build() {
    Column() {
      Web({
        src: './Index',
        controller: this.webviewController
      }).javaScriptProxy({
        object: nativeObj,
        name: "nativeObj",
        methodList: ["makePhoneCall"],
        controller: this.webviewController
      })
    }
  }
  // [End web]
}
