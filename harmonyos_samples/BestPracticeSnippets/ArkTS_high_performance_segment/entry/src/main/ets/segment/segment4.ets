// [Start Case10]
// A为任意可以被引入的ets文件
import { A } from "./A"


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
    // ...
  }
}
// [End Case10]
