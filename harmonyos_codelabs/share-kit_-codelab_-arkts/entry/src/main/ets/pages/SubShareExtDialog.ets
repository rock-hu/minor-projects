import { UIExtensionContentSession, Want } from '@kit.AbilityKit';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { SymbolGlyphModifier } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from '../utils/Logger';

let logger = Logger.getLogger('[ShareExtDialog]');

@Entry
@Component
struct ShareExtDialog {
  localStorage = this.getUIContext().getSharedLocalStorage();
  @State message: ResourceStr = $r('app.string.level_2');
  @State imageUri: string = '';
  private want: Want | undefined = this.localStorage?.get<Want>('ShareExtAbilityWant');
  private session: UIExtensionContentSession | undefined =
    this.localStorage?.get<UIExtensionContentSession>('ShareExtAbilitySession');

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
    Navigation() {
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
    .title($r('app.string.level_2'))
    .hideBackButton(true)
    .titleMode(NavigationTitleMode.Mini)
    .margin({ top: 8 })
    .menus([
      {
        value: 'Close',
        symbolIcon: new SymbolGlyphModifier($r('sys.symbol.xmark')),
        action: () => {
          this.session?.terminateSelfWithResult({ resultCode: systemShare.ShareAbilityResultCode.CLOSE });
        }
      }
    ])
  }
}