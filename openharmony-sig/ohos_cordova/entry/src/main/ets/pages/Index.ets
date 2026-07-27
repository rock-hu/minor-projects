import { router } from '@kit.ArkUI';

@Entry
@Component
struct Index {
  @State
  message: string = 'Hello World';

  private onTestCordovaBtnClick() {
    router.pushUrl({
      url: 'pages/Test'
    })
  }

  build() {
    RelativeContainer() {
      Text(this.message)
        .id('HelloWorld')
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
        .alignRules({
          center: { anchor: '__container__', align: VerticalAlign.Center },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })

      Button('测试Cordova')
        .id('TestCorodva')
        .fontColor(24)
        .fontWeight(FontWeight.Bold)
        .alignRules({
          top: { anchor: 'HelloWorld', align: VerticalAlign.Bottom },
          middle: { anchor: '__container__', align: HorizontalAlign.Center }
        })
        .padding({
          top: 4,
          bottom: 4,
          left: 8,
          right: 8
        })
        .margin({
          top: 24
        })
        .onClick(() => this.onTestCordovaBtnClick())
    }
    .height('100%')
    .width('100%')
  }
}