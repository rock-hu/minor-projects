import { router } from '@kit.ArkUI';
import AudioPlayHandler from '../entryability/intentHandlers/AudioPlayHandler';
import TextGetHandler from '../entryability/intentHandlers/TextGetHandler';
import { insightIntent } from '@kit.AbilityKit';
import { BusinessError } from '@ohos.base';

@Entry
@Component
struct SecondPage {
  @State content: ResourceStr = $r('app.string.load');
  private title = 'SecondPage';
  private audioPlayHandler = new AudioPlayHandler();
  private textGetHandler = new TextGetHandler();
  private audioHandlerInput: Record<string, string> = {
    'audioName': 'Dream It Possible.flac'
  };

  @State hiddenMessage: ResourceStr = $r('app.string.show');

  build() {
    Column() {
      Text(this.title)
        .fontSize(30)
        .fontWeight('bold')
        .fontColor('#333333')

      Row() {
        Button($r('app.string.back'))
          .width(60)
          .height(30)
          .onClick(() => this.getUIContext().getRouter().back())
      }
      .width('100%')
      .padding(16)

      // Function button area.
      Row() {
        Button($r('app.string.play'))
          .width('45%')
          .height(50)
          .backgroundColor('#409EFF')
          .fontColor(Color.White)
          .onClick(() => this.audioPlayHandler.execute(this.audioHandlerInput))

        Button($r('app.string.acquire'))
          .width('45%')
          .height(50)
          .backgroundColor('#67C23A')
          .fontColor(Color.White)
          .margin({ left: 10 })
          .onClick(() => {
            this.textGetHandler.execute().then((executeResult: insightIntent.ExecuteResult) => {
              this.hiddenMessage = String(executeResult.result?.audioName);
            }).catch((error: BusinessError) => {
              this.hiddenMessage = $r('app.string.hidden') + `error: ${error.message}`;
            })
          })
      }
      .width('100%')
      .justifyContent(FlexAlign.Center)
      .margin({ top: 20 })

      Text(this.hiddenMessage)
        .fontSize(20)
        .fontColor(Color.Black)
        .textAlign(TextAlign.Center)
        .margin(10)
        .padding(15)
        .backgroundColor('#F5F5F5')
        .borderRadius(8)
        .width('90%')

      // DeepLink
      Text($r('app.string.deep_link'))
        .fontSize(14)
        .fontColor(Color.Grey)
        .margin({ top: 30, bottom: 20 })
    }
    .width('100%')
    .height('100%')
    .backgroundColor('#F0F5FF')
  }
}