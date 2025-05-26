/**
 * 最佳实践：应用安全编码实践
 * 场景十八：避免在SSL校验出错时继续加载页面
 */

// [Start on_ssl_error_event_receive]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController()
  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
        .onSslErrorEventReceive((event) => {
          console.info('ssl check failed,error is : ' + event.error.toString())
          event.handler.handleCancel();
        })
    }
  }
}
// [End on_ssl_error_event_receive]