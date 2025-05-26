/**
 * 最佳实践：应用安全编码实践
 * 场景二十四：避免注册返回含有全局认证凭据的JavaScriptProxy-修改方法二
 */

// [Start url_list_check]
// Index.ets
import { url } from '@kit.ArkTS';
import { webview } from '@kit.ArkWeb';

// Check Function
function checkUrl(surl: string): boolean {
  let tmpUrl = url.URL.parseURL(surl);
  console.info("res.scheme:" + tmpUrl.protocol) // scheme
  console.info("res.host:" + tmpUrl.host) // host
  console.info("res.port:" + tmpUrl.port) // port
  console.info("res.path:" + tmpUrl.pathname) // path
  console.info("res.ssp:" + tmpUrl.hostname) // ssp
  // After obtaining the values of the scheme, host, port, and path parameters, perform security verification based on the service requirements
  // The part is omitted here. The verification is performed based on the service requirements
  if ("校验成功") {
    return true;
  }
  return false
}

@Entry
@Component
struct Index {
  controller: webview.WebviewController = new webview.WebviewController()

  build() {
    Column() {
      Row() {
        Button('registerJavaScriptProxy').onClick(() => {
          this.controller.registerJavaScriptProxy(
            {
              getServiceToken: () => {
                return "DLFJSLDFJALGJLDFJSDDFISLDF323LSDJFLS212DLSFJOEV";
              }
            },
            "objName",
            ["getServiceToken"]
          );
          this.controller.refresh()
        })
      }

      Web({ src: $rawfile('H5CallETS.html'), controller: this.controller })
        .javaScriptAccess(true)
        .javaScriptProxy({
          object: {
            getServiceToken: () => {
              return "DLFJSLDFJALGJLDFJSDDFISLDF323LSDJFLS212DLSFJOEV";
            }
          },
          name: "objName",
          methodList: ["getServiceToken"],
          controller: this.controller
        })
        .onLoadIntercept((event) => {
          console.info('wzz:onLoadIntercept:' + event.data.toString())
          let tempUrl = event.data.toString()
          return checkUrl(tempUrl); // Returns true to block the load, otherwise allow the load
        })
    }
  }
}
// [End url_list_check]