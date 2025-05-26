/**
 * 最佳实践：应用安全编码实践
 * 场景十五：避免加载不可信的JavaScript脚本—加载前进行白名单校验
 */

// [Start check_the_trust_list]
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
          let whiteMethods = ["test()", "test1()"]
          let jsMethod: string = "alert(`xss`)" // Externally controllable fields
          // Check the trustlist
          if (whiteMethods.indexOf(jsMethod) === -1) {
            console.error("input method not in whiteList")
            return
          }
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
// [End check_the_trust_list]