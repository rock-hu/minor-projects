/**
 * 最佳实践：使用DevEco Studio静态检测编码规范
 */

// [Start Foo2]
class foo {    //Here is an example of a naming style error, where foo is the incorrect class name used, and the correct class name should be Foo
  bar() {}
}
// [End Foo2]

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';

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
        })
    }
    .height('100%')
    .width('100%')
  }
}