/**
 * 最佳实践：应用安全编码实践
 * 场景二十二：避免注册返回含有全局认证凭据的JavaScriptProxy
 */

// [Start java_script_proxy_error]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct Index {
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Row() {
        Button('registerJavaScriptProxy').onClick(() => {
          this.controller.registerJavaScriptProxy(
            {
              getServiceToken: () => {
                return 'DLFJSLDFJALGJLDFJSDDFISLDF323LSDJFLS212DLSFJOEV';
              }
            },
            'objName',
            ['getServiceToken'],
          );
          this.controller.refresh();
        })
      }

      Web({ src: $rawfile('H5CallETS.html'), controller: this.controller })
        .javaScriptAccess(true)
        .javaScriptProxy({
          object: {
            getServiceToken: () => {
              return 'DLFJSLDFJALGJLDFJSDDFISLDF323LSDJFLS212DLSFJOEV';
            }
          },
          name: 'objName',
          methodList: ['getServiceToken'],
          controller: this.controller
        })
    }
  }
}
// [End java_script_proxy_error]