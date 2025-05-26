/**
 * 最佳实践：应用安全编码实践
 * 场景二十：避免在用户同意前返回位置信息
 */

// [Start geolocation_access]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController()

  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
        .geolocationAccess(true)
        .onGeolocationShow((event) => {
          if (event === undefined) {
            return
          }
          AlertDialog.show({
            title: 'title',
            message: 'text',
            confirm: {
              value: 'onConfirm',
              action: () => {
                event.geolocation.invoke(event.origin, true, true);
              }
            },
            cancel: () => {
              event.geolocation.invoke(event.origin, false, true);
            }
          })
        })
    }
  }
}
// [End geolocation_access]