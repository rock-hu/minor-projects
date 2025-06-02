// [Start counter_example]
@Entry
@Component
struct Index {
  @State message: string = '';
  // Define methods for changing state variables (multiple modifications of state variables)
  appendMsg(newMsg: string) {
    this.message += newMsg;
    this.message += ';';
    this.message += '<br/>';
  }
  build() {
    Column() {
      Button('Click Print Log')
        .onClick(() => {
          this.appendMsg('Operational state variables'); // Calling encapsulated methods for changing state variables
        })
        .width('90%')
        .backgroundColor(Color.Blue)
        .fontColor(Color.White)
        .margin({ top: 10})
    }
    .justifyContent(FlexAlign.Start)
    .alignItems(HorizontalAlign.Center)
    .margin({  top: 15 })
  }
}
// [End counter_example]