/**
 * 最佳实践：应用安全编码实践
 * 场景二十一：避免在用户同意前返回位置信息错误示例
 */

// [Start geolocation_access_error]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
        .geolocationAccess(true)
        .onGeolocationShow((event) => {
          if (event === undefined) {
            return;
          }
          event.geolocation.invoke(event.origin, true, true);
        })
    }
  }
}
// [End geolocation_access_error]