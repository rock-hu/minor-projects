/**
 * 最佳实践: 崩溃监测实践
 */
import router from '@ohos.router';
import { c } from './XXXIsNotCallableCase'
import { c1 } from './XXXIsNotCallableFix'

@Entry
@Component
struct Index {
  @State goto: string[] =
    ['CanNotGetPrototypeOnNonEcmaObjectCase', 'CannotReadPropertyXXXOfUndefinedCase',
      'CannotReadPropertyXXXOfUndefinedFix', 'OperationFailedCase',
      'OperationFailedFix', 'OutOfMemoryErrorCase', 'TaskpoolFailedToSerializeResultCase',
      'TaskpoolFailedToSerializeResultFix', 'ThrowNewError', 'XXXIsNotInitializedFix', 'XXXIsNotInitializedCase']
  scroller: Scroller = new Scroller()

  build() {
    Row() {
      Scroll(this.scroller) {
        Column() {
          Text('JSCrash案例')
            .fontSize(30)
            .fontWeight(FontWeight.Bold)
          ForEach(this.goto, (item: string) => {
            Button() {
              Text(item)
                .fontSize(20)
                .fontWeight(FontWeight.Bold)
                .fontColor("#ffffff")
            }
            .type(ButtonType.Capsule)
            .margin({
              top: 5,
              bottom: 30
            })

            .backgroundColor("#ff224999")
            .width('90%')
            .height('8%')
            .onClick(() => {
              router.pushUrl({ url: `pages/${item}` })
            })
          })
          Button('XXXIsNotCallableCase')
            .fontWeight(FontWeight.Bold)
            .width('90%')
            .height('8%')
            .backgroundColor("#ff224999")
            .type(ButtonType.Capsule)
            .margin({
              top: 5,
              bottom: 30
            })
            .fontSize(20)
            .onClick(() => {
              c();
            })
          Button('XXXIsNotCallableFix')
            .fontWeight(FontWeight.Bold)
            .width('90%')
            .height('8%')
            .backgroundColor("#ff224999")
            .type(ButtonType.Capsule)
            .margin({
              top: 5,
              bottom: 30
            })
            .fontSize(20)
            .onClick(() => {
              c1();
            })
        }
      }
      .scrollable(ScrollDirection.Vertical) // 滚动方向纵向
      .scrollBar(BarState.On) // 滚动条常驻显示
      .scrollBarColor(Color.Gray) // 滚动条颜色
      .scrollBarWidth(10) // 滚动条宽度
      .friction(0.6)
      .edgeEffect(EdgeEffect.None)
      .width('100%')
    }
    .height('100%')
  }
}
