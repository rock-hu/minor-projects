import { Want } from '@kit.AbilityKit';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { BusinessError } from '@kit.BasicServicesKit';
import { image } from '@kit.ImageKit';
import Logger from '../utils/Logger';

let logger = Logger.getLogger('[ShareUIPage]');

@Entry
@Component
struct SystemShare {
  localStorage = this.getUIContext().getSharedLocalStorage();

  @State message: ResourceStr = $r('app.string.target_app');
  @State imageUri: string = '';
  @State pixelMap: image.PixelMap | null = null;
  private want: Want | undefined = this.localStorage?.get<Want>('ShareUIAbilityWant');

  aboutToAppear(): void {
    systemShare.getSharedData(this.want).then((data: systemShare.SharedData) => {
      let records: systemShare.SharedRecord[] = data.getRecords();
      records.forEach(async (record: systemShare.SharedRecord) => {
        switch (true) {
          case this.belongsToImage(record.utd):
            record.uri && (this.imageUri = record.uri);
            break;
          default:
            break;
        }
      });
    }).catch((error: BusinessError) => {
      logger.error(`getSharedData error. Code: ${error?.code}, message: ${error?.message}`);
    });
  }

  belongsToImage(recordUtd: string): boolean {
    let typeObj: utd.TypeDescriptor = utd.getTypeDescriptor(recordUtd);
    let result = typeObj.belongsTo(utd.UniformDataType.IMAGE);
    return result;
  }

  @Builder
  ShareContent() {
    Text($r('app.string.share_content'))
      .fontSize(28)
      .margin({ top: 20 })

    if (this.imageUri) {
      Image(this.imageUri)
        .width(200)
        .height(200)
    }
  }

  build() {
    Row() {
      Column() {
        Text(this.message)
          .fontSize(50)
          .fontWeight(FontWeight.Bold)

        this.ShareContent();
      }
      .width('100%')
    }
    .height('100%')
  }
}