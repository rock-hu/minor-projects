/**
 * 最佳实践: 视频场景图层低功耗建议
 */
import { webview } from '@kit.ArkWeb';

@Component
struct VideoSceneLayers {

  webviewController: webview.WebviewController = new webview.WebviewController();


  build() {
    Column() {
      Button('prefetchPopularPage')
        .onClick(() => {
          // [Start Video_scene_layers]
          // Load web resources in advance
          this.webviewController.prefetchPage('url');
          // [End Video_scene_layers]。
        })
    }
  }
}