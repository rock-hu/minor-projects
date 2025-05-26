/**
 * 最佳实践：应用安全编码实践
 * 场景十六：避免加载不可信的JavaScript脚本—加载前进行白名单校验错误示例
 */

// [Start check_the_trust_list_error]
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
          let jsMethod: string = "alert(`xss`)" // 外部可控字段
          this.controller.runJavaScript(jsMethod)
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
// [End check_the_trust_list_error]