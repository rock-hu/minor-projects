/**
 * 最佳实践：应用安全编码实践
 * 场景七：避免涉及口令输入的应用界面可以被截屏或录屏
 */
// [Start privacy_mode]
import { router, window } from '@kit.ArkUI';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

  onPageShow(): void {
    window.getLastWindow(this.getUIContext().getHostContext()).then((windowStage: window.Window) => {
      windowStage.setWindowPrivacyMode(true);
    });
  }
  onPageHide(): void {
    window.getLastWindow(this.getUIContext().getHostContext()).then((windowStage: window.Window) => {
      windowStage.setWindowPrivacyMode(false);
    });
  }
  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)
        Button('click to start Succ Page')
          .onClick(async () => {
            this.getUIContext().getRouter().pushUrl({ url: 'pages/loginSuccPage' });
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}
// [End privacy_mode]