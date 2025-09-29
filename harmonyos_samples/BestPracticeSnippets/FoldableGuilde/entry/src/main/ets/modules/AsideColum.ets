@Component
export struct AsideColumn {
  @State l_width: string = '100%';

  build() {
    Row() {
      Column() {
        Text('1')
      }
      .backgroundColor(Color.Pink)
      .onClick(() => {
        this.l_width = this.l_width === '100%' ? '50%' : '100%';
      })
      .height('100%')
      .width(this.l_width)

      Column() {
        Text('1')
      }
      .backgroundColor(Color.Gray)
      .width('50%')
    }
    .justifyContent(FlexAlign.SpaceBetween)
  }
}