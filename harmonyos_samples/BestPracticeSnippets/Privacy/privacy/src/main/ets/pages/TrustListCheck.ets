/**
 * 最佳实践：应用安全编码实践
 * 场景二十三：避免注册返回含有全局认证凭据的JavaScriptProxy-修改方法一
 */

// [Start trust_list_check]
// Index.ets
import { uri } from '@kit.ArkTS';
import { webview } from '@kit.ArkWeb';
import { hilog } from '@kit.PerformanceAnalysisKit';

// Check function
function checkUrl(surl: string): boolean {
  let tmpUri: uri.URI = new uri.URI(surl);
  let res = tmpUri.normalize(); // Note that you need to normalize tmpUri first. Otherwise, security verification will be bypassed
  hilog.info(0x0000, 'privacy', 'res.scheme:' + res.scheme); // scheme
  hilog.info(0x0000, 'privacy', 'res.host:' + res.host); // host
  hilog.info(0x0000, 'privacy', 'res.port:' + res.port); // port
  hilog.info(0x0000, 'privacy', 'res.path:' + res.path); // path
  hilog.info(0x0000, 'privacy', 'res.ssp:' + res.ssp);
  // After obtaining the values of the scheme, host, port, and path parameters, perform security verification based on the service requirements
  // The part is omitted here. The verification is performed based on the service requirements
  if ('校验成功') {
    return true;
  }
  return false;
}

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
        .onLoadIntercept((event) => {
          hilog.info(0x0000, 'privacy', 'onLoadIntercept:' + event.data.toString());
          let tempUrl = event.data.toString();
          return checkUrl(tempUrl); // Returns true to block the load, otherwise allow the load
        })
    }
  }
}
// [End trust_list_check]