/**
 * 最佳实践：应用安全编码实践
 * 场景十二：避免加载不安全的URL或页面
 */
// [Start web_view_safe_url]
import { uri } from '@kit.ArkTS';
import { webview } from '@kit.ArkWeb';

// Check function
function checkUrl(str: string): boolean {
  let tmpUri = new uri.URI(str);
  let res =
    tmpUri.normalize(); // Note that you need to normalize tmpUri first. Otherwise, security verification will be bypassed
  console.info("res.scheme:" + res.scheme) // Protocol
  console.info("res.host:" + res.host) // domain name
  console.info("res.port:" + res.port) // Ports
  console.info("res.path:" + res.path) // Path
  console.info("res.ssp:" + res.ssp)
  // After obtaining the values of scheme, host, port, and path, perform security verification based on the actual service
  // Omit the part here. Perform the verification based on service requirements
  if ("校验成功") {
    return true;
  }
  return false
}

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Button("loadUrl")
        .onClick(() => {
          this.controller.loadUrl('www.huawei.com')
        })
        .margin({ top: 50 })
      Web({ src: 'www.huawei.com', controller: this.controller })
        .onLoadIntercept((event) => {
          console.info('onLoadIntercept:' + event.data.toString())
          let tempUrl = event.data.toString()
          return checkUrl(tempUrl) // Return true to block the load, otherwise allow the load
        })
    }
  }
}

// [End web_view_safe_url]