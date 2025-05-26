// [Start icon_item]
@Component
export  struct IconItem {
  @StorageLink('renderGroupFlag') renderGroupFlag: boolean = false;
  image: string | Resource = '';
  text: string | Resource = '';

  build() {
    Flex({
      direction: FlexDirection.Column,
      justifyContent: FlexAlign.Center,
      alignContent: FlexAlign.Center
    }) {
      Image(this.image)
        .height(20)
        .width(20)
        .objectFit(ImageFit.Contain)
        .margin({ left: 15 })

      Text(this.text)
        .fontSize(10)
        .fontColor('#182431')
        .margin({ top: 5 })
        .width(50)
        .opacity(0.8)
        .textAlign(TextAlign.Center)
    }
    .backgroundColor('#e3e3e3')
    .width(50)
    .height(50)
    .borderRadius(25)
    // Call renderGroup within IconItem, false to disable, true to enable.
    .renderGroup(this.renderGroupFlag)
  }
}
// [End icon_item]