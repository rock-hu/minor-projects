import { common, Want } from '@kit.AbilityKit';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';

let storage = LocalStorage.getShared();

@Entry
@Component
struct SystemShare {
  @State message: string = 'Target Application';
  @State textContent: string = '';
  @State htmlLink: string = '';
  private want: Want | undefined = storage.get<Want>('ShareUIAbilityWant');

  aboutToAppear(): void {
    systemShare.getSharedData(this.want).then((data: systemShare.SharedData) => {
      let records: systemShare.SharedRecord[] = data.getRecords();
      records.forEach((record: systemShare.SharedRecord) => {
        switch (record.utd) {
          case utd.UniformDataType.TEXT:
            record.content && (this.textContent = record.content);
            break;
          case utd.UniformDataType.HYPERLINK:
            record.content && (this.htmlLink = record.content);
            break;
          default:
            break;
        }
      });
    })
  }

  @Builder
  ShareContent() {
    Text('Sharing data')
      .fontSize(28)
      .margin({ top: 20 })

    if (this.textContent) {
      Text(this.textContent)
        .fontSize(20)
        .margin({ top: 20 })
    }

    if (this.htmlLink) {
      Text(this.htmlLink)
        .fontSize(20)
        .margin({ top: 20 })
    }
  }

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)

        this.ShareContent();

        Button() {
          Text('BACK')
            .fontSize(25)
            .fontColor('#FFFFFF')
            .fontWeight(FontWeight.Bold)
        }
        .type(ButtonType.Capsule)
        .margin({ top: 20 })
        .backgroundColor('#0D9FFB')
        .width('40%')
        .height('5%')
        .onClick(() => {
          const context = getContext(this) as common.UIAbilityContext;
          context.terminateSelf();
        })
      }
      .width('100%')
    }
    .height('100%')
  }
}