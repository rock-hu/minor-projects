/**
 * 最佳实践：应用安全编码实践
 * 场景十三：避免加载不可信的JavaScript脚本
 */

// [Start run_java_script]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: $rawfile('index.html'), controller: this.controller })
        .javaScriptAccess(true)
        .onPageEnd(e => {
          this.controller.runJavaScript('test()')
            .then((result) => {
              console.log('result: ' + result);
            })
            .catch((error: BusinessError) => {
              console.error(`ErrorCode: ${error.code},  Message: ${error.message}`);
            })
          if (e) {
            console.info('url: ', e.url);
          }
        })
    }
  }
}
// [End run_java_script]