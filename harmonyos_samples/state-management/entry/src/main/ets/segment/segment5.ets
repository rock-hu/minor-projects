// [Start Case2_start]
@Entry
@Component
struct UnnecessaryState2 {
  @State message: string = '';


  // Define methods for changing state variables (intermediate variables are manipulated during method execution, state variables are modified only once)
  appendMsg(newMsg: string) {
    let message = this.message;
    message += newMsg;
    message += ';';
    message += '<br/>';
    this.message = message;
  }


  build() {
    Column() {
      Button('Click Print Log')
        .onClick(() => {
          this.appendMsg('Manipulating Temporary Variables'); // Calling encapsulated methods for changing state variables
        })
        .width('90%')
        .backgroundColor(Color.Blue)
        .fontColor(Color.White)
        .margin({ top: 10 })
    }
    .justifyContent(FlexAlign.Start)
    .alignItems(HorizontalAlign.Center)
    .margin({ top: 15 })
  }
}
// [End Case2_start]