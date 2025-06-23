// HelloWorldPage.ets
import router from '@ohos.router';

@Entry
@Component
struct HelloWorldPage {
  @State message: string = 'First Page';

  // Jump to the second page
  private navigateToSecond() {
    this.getUIContext().getRouter().pushUrl({ url: 'pages/SecondPage' });
  }

  build() {
    Column() {
      Text(this.message)
        .fontSize(30)
        .fontWeight('bold')
        .fontColor('#333333')

      Text($r('app.string.title'))
        .margin({ top: 30 })

      Column() {
        Text($r('app.string.text1'))
          .margin({ top: 10 })
        Text($r('app.string.text2'))
          .margin({ top: 10 })
        Text($r('app.string.text3'))
          .margin({ top: 10 })
      }

      // Add a bottom jump button (more prominent)
      Button($r('app.string.button'), { type: ButtonType.Capsule })
        .width('80%')
        .height(50)
        .margin({ top: 30, bottom: 20 })
        .backgroundColor('#409EFF')
        .fontColor(Color.White)
        .fontSize(18)
        .onClick(() => this.navigateToSecond())
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#FFFFFF')
  }
}