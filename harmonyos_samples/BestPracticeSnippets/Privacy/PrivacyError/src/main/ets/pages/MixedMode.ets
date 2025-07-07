/**
 * 最佳实践：应用安全编码实践
 * 场景十七：避免将mixedMode属性配置成All
 */

// [Start mixed_mode]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Web({ src:'www.huawei.com', controller: this.controller })
        .mixedMode(MixedMode.All)
    }
  }
}
// [End mixed_mode]