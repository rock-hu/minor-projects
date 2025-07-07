/**
 * 最佳实践：应用安全编码实践
 * 场景十九：避免在SSL校验出错时继续加载页面错误示例
 */

// [Start on_ssl_error_event_receive_error]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
        .onSslErrorEventReceive((event) => {
          event.handler.handleConfirm();
        })
    }
  }
}
// [End on_ssl_error_event_receive_error]