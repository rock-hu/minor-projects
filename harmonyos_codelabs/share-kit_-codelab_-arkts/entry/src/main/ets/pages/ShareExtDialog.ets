import { UIExtensionContentSession, Want } from '@kit.AbilityKit';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { image } from '@kit.ImageKit';

let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct ShareExtDialog {
  @State message: string = 'Level-2 panel';
  @State imageUri: string = '';
  @State videoUri: string = '';
  @State pixelMap: image.PixelMap | null = null;
  private want: Want | undefined = storage.get<Want>('ShareExtAbilityWant');
  private session: UIExtensionContentSession | undefined = storage.get<UIExtensionContentSession>('ShareExtAbilitySession');

  aboutToAppear(): void {
    systemShare.getSharedData(this.want).then((data: systemShare.SharedData) => {
      let records: systemShare.SharedRecord[] = data.getRecords();
      records.forEach(async (record: systemShare.SharedRecord) => {
        console.info('getRecords', JSON.stringify(record));
        switch (record.utd) {
          case utd.UniformDataType.IMAGE:
            record.uri && (this.imageUri = record.uri);
            break;
          case utd.UniformDataType.VIDEO:
            record.uri && (this.videoUri = record.uri);
            if (record.thumbnail) {
              const iconArray: ArrayBuffer = record.thumbnail.buffer.slice(0);
              let source = image.createImageSource(iconArray);
              this.pixelMap = await source.createPixelMap();
              source.release();
            }
            break;
          default:
            break;
        }
      });
    })
  }

  createWant() {
    let data: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.TEXT,
      content: 'Hello HarmonyOS'
    });
    let options : systemShare.ShareControllerOptions = {
      previewMode: systemShare.SharePreviewMode.DETAIL,
      selectionMode: systemShare.SelectionMode.SINGLE
    };
    systemShare.getWant(data, options).then((want) => {
      console.info('getWant', JSON.stringify(want))
    });
  }

  @Builder
  ShareContent() {
    Text('Sharing data')
      .fontSize(28)
      .margin({ top: 20 })

    if (this.imageUri) {
      Image(this.imageUri)
        .width(200)
        .height(200)
    }

    if (this.videoUri) {
      Video({
        src: this.videoUri,
        previewUri: this.pixelMap
      })
        .width(200)
        .height(200)
    }
  }

  @Builder
  ShareAction(text: string, handelClick: () => void) {
    Button() {
      Text(text)
        .fontSize(25)
        .fontColor('#FFFFFF')
        .fontWeight(FontWeight.Bold)
    }
    .type(ButtonType.Capsule)
    .margin({ top: 20 })
    .backgroundColor('#0D9FFB')
    .width('40%')
    .height('5%')
    .onClick(() => handelClick())
  }

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)

        this.ShareContent();

        this.ShareAction('BACK', () => {
          this.createWant();
          this.session?.terminateSelfWithResult({ resultCode: systemShare.ShareAbilityResultCode.BACK });
        })

        this.ShareAction('CLOSE', () => {
          this.session?.terminateSelfWithResult({ resultCode: systemShare.ShareAbilityResultCode.CLOSE });
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}