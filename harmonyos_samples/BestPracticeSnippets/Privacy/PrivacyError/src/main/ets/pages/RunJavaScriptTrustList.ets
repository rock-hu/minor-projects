/**
 * 最佳实践：应用安全编码实践
 * 场景十六：避免加载不可信的JavaScript脚本—加载前进行白名单校验错误示例
 */

// [Start check_the_trust_list_error]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: $rawfile('index.html'), controller: this.controller })
        .javaScriptAccess(true)
        .onPageEnd(e => {
          let jsMethod: string = 'alert(`xss`)' // Externally controllable fields
          this.controller.runJavaScript(jsMethod)
            .then((result) => {
              hilog.info(0x0000, 'privacy', 'result: ' + result);
            })
            .catch((error: BusinessError) => {
              hilog.error(0x0000, 'privacy', `ErrorCode: ${error.code},  Message: ${error.message}`);
            })
          if (e) {
            hilog.info(0x0000, 'privacy', 'url: ', e.url);
          }
        })
    }
  }
}
// [End check_the_trust_list_error]