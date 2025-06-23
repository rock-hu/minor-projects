
@Entry
@Component
struct MainPage {
  build() {
    Flex({ direction: FlexDirection.Column }) {
      Text('NewHTTP')
        .fontSize(36)
        .fontWeight(FontWeight.Bold)
        .fontColor('#000000')
        .height(48)
        .width('80%')
        .margin({top: '7.2%', left: '5%'})
      Button({type: ButtonType.Normal}){
        Row() {
          Column() {
            Text('TimeOut')
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.White)
            Rect()
              .radius(1)
              .fill('#0A59F7')
              .height(2)
              .width(50)
          }
          .width(220)
          .height('100%')
          .alignItems(HorizontalAlign.Start)
          .padding({ left: 20, top: 30 })
        }
        .width('100%')
        .height(100)
      }
      .width('90%')
      .height(120)
      .margin({ top: '11%', left: '5%' })
      .backgroundColor('#0A59F7')
      .borderRadius(24)
      .onClick(this.getTimeout)

      Button({type: ButtonType.Normal}){
        Row() {
          Column() {
            Text('TransferRange')
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.White)
            Rect()
              .radius(1)
              .fill('#0A59F7')
              .height(2)
              .width(50)
          }
          .width(220)
          .height('100%')
          .alignItems(HorizontalAlign.Start)
          .padding({ left: 20, top: 30 })
        }
        .width('100%')
        .height(100)
      }
      .width('90%')
      .height(120)
      .margin({ top: '11%', left: '5%' })
      .backgroundColor('#0A59F7')
      .borderRadius(24)
      .onClick(this.getTransferRange)

      Button({type: ButtonType.Normal}){
        Row() {
          Column() {
            Text('BaseAddress')
              .fontSize(24)
              .fontWeight(FontWeight.Bold)
              .fontColor(Color.White)
            Rect()
              .radius(1)
              .fill('#0A59F7')
              .height(2)
              .width(50)
          }
          .width(220)
          .height('100%')
          .alignItems(HorizontalAlign.Start)
          .padding({ left: 20, top: 30 })
        }
        .width('100%')
        .height(100)
      }
      .width('90%')
      .height(120)
      .margin({ top: '11%', left: '5%' })
      .backgroundColor('#0A59F7')
      .borderRadius(24)
      .onClick(this.getBaseAddress)

    }


    .linearGradient({
      direction: GradientDirection.Top,
      repeating: true,
      colors: [['#FFFFFF', 0], ['#FFFFFF', 1]]
    })
    .height('100%')
    .width('100%')
  }

  private getTimeout = () => {
    this.getUIContext().getRouter().pushUrl({ url: 'pages/TimeOut' });
  }
  private getTransferRange = () => {
    this.getUIContext().getRouter().pushUrl({ url: 'pages/TransferRange' });
  }
  private getBaseAddress = () => {
    this.getUIContext().getRouter().pushUrl({ url: 'pages/BaseAddress' });
  }
}