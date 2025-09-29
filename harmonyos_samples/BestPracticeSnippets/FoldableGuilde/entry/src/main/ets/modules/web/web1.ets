// xxx.ets
import { webview } from '@kit.ArkWeb';

class testClass {
  constructor() {
  }

  test(value: string): string {
    return 'ArkTS Hello World!' + value;
  }
}

@Component
export struct Web1 {
  webviewController: webview.WebviewController = new webview.WebviewController();
  // 声明需要注册的对象
  @State testObj: testClass = new testClass();

  build() {
    Column() {
      // web组件加载本地index.html页面
      Web({ src: $rawfile('index.html'), controller: this.webviewController })
        // 将对象注入到web端
        .javaScriptProxy({
          object: this.testObj,
          name: 'testObjName',
          methodList: ['test'],
          controller: this.webviewController
        })
    }
  }
}