import { router } from '@kit.ArkUI'

@Entry
@Component
struct Index {
  build() {
    Column() {
      Row() {
        Button('分层图标处理')
          .onClick(() => {
            router.pushUrl({ url: 'pages/GetHdsLayeredIcon' })
          })
      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .margin({ top: 30, bottom: 30 })

      Row() {
        Button('单层图标处理')
          .onClick(() => {
            router.pushUrl({ url: 'pages/GetHdsIcon' })
          })
      }
      .justifyContent(FlexAlign.Center)
      .width('100%')
      .margin({ top: 30, bottom: 30 })
    }
  }
}