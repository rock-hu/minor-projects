import { router } from '@kit.ArkUI'


@Entry
@Component
struct Index {

  build() {
    Row() {
      Column() {
        Button('ForEach')
          .width(200)
          .margin(10)
          .onClick(() => {
            router.pushUrl({ url: 'pages/ForEachListPage' })
          })
        Button('LazyForEach')
          .width(200)
          .margin(10)
          .onClick(() => {
            router.pushUrl({ url: 'pages/LazyForEachListPage' })
          })
      }
      .width('100%')
    }
    .height('100%')
  }
}