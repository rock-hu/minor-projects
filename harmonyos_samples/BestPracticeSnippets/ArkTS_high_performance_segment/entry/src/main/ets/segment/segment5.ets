// [Start Case11]
import lazy { A } from "./A"

@Entry
@Component
struct Index {
  build() {
    RelativeContainer() {
      Button('点击执行A')
        .onClick(() => {
          console.log('执行A' + A)
        })
    }
    .height('100%')
    .width('100%')
  }
}
// [End Case11]