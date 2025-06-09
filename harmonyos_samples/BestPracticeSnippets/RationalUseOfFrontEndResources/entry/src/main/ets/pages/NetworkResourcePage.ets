/**
 * 最佳实践: 网络资源低功耗建议
 */
import { webview } from '@kit.ArkWeb';

@Component
struct NetworkResourcePage {

  webviewController: webview.WebviewController = new webview.WebviewController();


  build() {
    Column() {
      Button('prefetchPopularPage')
        .onClick(() => {
          // [Start NetworkResource]
          // Load web resources in advance
          this.webviewController.prefetchPage('url');
          // [End NetworkResource]
        })
    }
  }
}