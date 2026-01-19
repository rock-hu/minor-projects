/**
 * 最佳实践：JSCrash问题案例分析
 */

import wifiManager from '@ohos.wifiManager';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  isConnected: boolean = false;

  handleUpdateState() {
  }

  isWifiActive: boolean = false;

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize($r('app.float.page_text_font_size'))
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .onClick(() => {
          this.message = 'Welcome';
          // [Start JSCrashCaseAnalyse4]
          wifiManager.on('wifiConnectionChange', (data) => {
            this.isConnected = data === 1 ? true : false;
            this.handleUpdateState();
          });
          // [End JSCrashCaseAnalyse4]
        })
    }
    .height('100%')
    .width('100%')
  }
}