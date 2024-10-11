import { hilog } from '@kit.PerformanceAnalysisKit';
import { notificationManager } from '@kit.NotificationKit';
import rcpSdk from 'libentry.so';

const kHttpServerAddress = "https://www.baidu.com";
@Entry
@Component
struct  requestContent{
  @State showToken: string = '';
  @State content: string = '';
  @State connectMs: number = 1000;
  @State transferMs: number = 1000;
  async aboutToAppear() {
    this.openSysSwitch();
  }

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Row() {
        Text('TimeOut')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#ff100f0f')
          .height(30)
          .width(220)
          .margin({ left: 4 })
      }
      .height(32)
      .width(124)
      .margin({ top: 32, left: 12 })

      Divider().color('gray').strokeWidth(1)
      Text(this.showToken)
        .fontColor(Color.Blue)
        .width('80%')
        .margin({ left: '10%'})
      Flex({ direction: FlexDirection.Column,
        justifyContent: FlexAlign.SpaceBetween }) {
        Text("connectMs:")
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#ff100f0f')
          .height(40)
          .width(220)
          .margin({ left: 8 })
        TextInput({
          text:String(this.connectMs)
        }).type(InputType.Number)
          .width('80%')
          .height(40)
          .margin({ left: '10%' })
          .backgroundColor('0x317aff')
          .fontColor('#000')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .borderRadius(10)
          .onChange((value:string) => {
            this.connectMs = parseInt(value);
          })

        Text("transferMs:")
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .fontColor('#ff100f0f')
          .height(40)
          .width(220)
          .margin({ left: 8 })
        TextInput({
          text:String(this.transferMs)
        }).type(InputType.Number)
          .width('80%')
          .height(40)
          .margin({ left: '10%'})
          .backgroundColor('0x317aff')
          .fontColor('#000')
          .fontSize(18)
          .fontWeight(FontWeight.Bold)
          .borderRadius(10)
          .onChange((value:string) => {
            this.transferMs = parseInt(value);
          })

        Button('timeout-test')
          .width('80%')
          .height(40)
          .margin({ left: '10%' })
          .backgroundColor('#fff38a07')
          .fontColor('#ffffffff')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .borderRadius(20)
          .onClick(this.getTimeoutSyn)

        Text("结果")
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .fontColor('#000')
          .height(30)
          .width(220)
          .margin({ left: 8 })
        TextArea({
          text: this.content
        })
          .width('80%')
          .height(260)
          .margin({ left: '10%',top: '10%' })
          .backgroundColor('0x317aff')
          .fontColor('#ffffffff')
          .fontSize(24)
          .fontWeight(FontWeight.Bold)
          .borderRadius(10)
      }
      .height(644)
      .margin({ top: 16 })
    }
    .linearGradient({
      direction: GradientDirection.Top,
      repeating: true,
      colors: [['#FFFFFF', 0], ['#FFFFFF', 1]]
    })
    .height('100%')
    .width('100%')
  }

  private getTimeoutSyn = async () => {
    this.content = 'waiting for response'
    let onCatch = false;
    try {
      const response = rcpSdk.GetWithTimeout(kHttpServerAddress, this.connectMs, this.transferMs)
      console.info('timeout--success', JSON.stringify(response))
      this.content=`timeout--success`
    } catch (err) {
      console.error('timeout--err',JSON.stringify(err))
      this.content=`timeout--err + ${JSON.stringify(err)}`
      onCatch = true;
    }
  }

  private openSysSwitch = async () => {
    await notificationManager.requestEnableNotification();
    hilog.info(0x0000, 'testTag', 'System notification switch is open');
  }
}