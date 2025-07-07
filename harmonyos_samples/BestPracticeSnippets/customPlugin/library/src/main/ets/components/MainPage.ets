@Preview
@Component
export struct MainPage {
  @State message: string = 'Hello World';

  build() {
    Row() {
      Text(this.message)
        .fontSize(50)
        .fontWeight(FontWeight.Bold)
    }.height('100%')
  }
}