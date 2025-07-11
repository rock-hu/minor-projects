/**
 * 最佳实践：应用安全编码实践
 * 场景十二：避免加载不安全的URL或页面
 */
// [Start web_view_safe_url]
import { uri } from '@kit.ArkTS';
import { webview } from '@kit.ArkWeb';
import { hilog } from '@kit.PerformanceAnalysisKit';

// Check function
function checkUrl(str: string): boolean {
  let tmpUri = new uri.URI(str);
  let res =
    tmpUri.normalize(); // Note that you need to normalize tmpUri first. Otherwise, security verification will be bypassed
  hilog.info(0x0000, 'privacy', 'res.scheme:' + res.scheme); // Protocol
  hilog.info(0x0000, 'privacy', 'res.host:' + res.host); // domain name
  hilog.info(0x0000, 'privacy', 'res.port:' + res.port); // Ports
  hilog.info(0x0000, 'privacy', 'res.path:' + res.path); // Path
  hilog.info(0x0000, 'privacy', 'res.ssp:' + res.ssp);
  // After obtaining the values of scheme, host, port, and path, perform security verification based on the actual service
  // Omit the part here. Perform the verification based on service requirements
  if ('校验成功') {
    return true;
  }
  return false;
}

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Button('loadUrl')
        .onClick(() => {
          this.controller.loadUrl('www.huawei.com');
        })
        .margin({ top: 50 })
      Web({ src: 'www.huawei.com', controller: this.controller })
        .onLoadIntercept((event) => {
          hilog.info(0x0000, 'privacy', 'onLoadIntercept:' + event.data.toString());
          let tempUrl = event.data.toString();
          return checkUrl(tempUrl); // Return true to block the load, otherwise allow the load
        })
    }
  }
}
// [End web_view_safe_url]