/**
 * 最佳实践：应用安全编码实践
 * 场景十三：避免加载不可信的JavaScript脚本
 */

// [Start run_java_script]
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
          this.controller.runJavaScript('test()')
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
// [End run_java_script]