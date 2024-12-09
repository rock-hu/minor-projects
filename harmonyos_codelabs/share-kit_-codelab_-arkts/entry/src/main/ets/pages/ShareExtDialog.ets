import { common, UIExtensionContentSession, Want } from '@kit.AbilityKit';
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { image } from '@kit.ImageKit';
import { SymbolGlyphModifier } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

// 获取共享的storage实例
// Obtaining a Shared Storage Instance
let storage = LocalStorage.getShared();

@Entry(storage)
@Component
struct ShareExtDialog {
  @State message: ResourceStr = $r('app.string.level_2');
  @State imageUri: string = '';
  @State videoUri: string = '';
  @State pixelMap: image.PixelMap | null = null;
  @State text: string = '';
  private want: Want | undefined = storage.get<Want>('ShareExtAbilityWant');
  private session: UIExtensionContentSession | undefined = storage.get<UIExtensionContentSession>('ShareExtAbilitySession');

  aboutToAppear(): void {
    systemShare.getSharedData(this.want).then((data: systemShare.SharedData) => {
      let records: systemShare.SharedRecord[] = data.getRecords();
      records.forEach(async (record: systemShare.SharedRecord) => {
        switch (true) {
          case this.belongsToImage(record.utd):
            record.uri && (this.imageUri = record.uri);
            break;
          case this.belongsToVideo(record.utd):
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

  belongsToImage(recordUtd: string): boolean {
    /** 判断 接收的数据类型 是否属于自己可处理的数据类型
     * 背景：宿主应用推荐使用 精细化utd类型 传输数据. 系统应用如 图库/文管 等已做响应的整改.
     *      目标应用需使用推荐的方式判断utd类型 参见以下 图片类型的示例 其他类型文件同理.
     * 举例场景：目标应用可处理全部的图片类型 module.json5 中会配置 "utd": "general.image"
     * */
    /** Check whether the received data type is a data type that can be processed by the user.
     * Background: It is recommended that the refined UTD type be used for data transmission for host applications. System applications such as gallery and document management have been rectified.
     * The target application needs to use the recommended method to determine the UTD type. For details, see the following example of the image type. The same applies to other types of files.
     * Example scenario: The target application can process all image types, and "utd": "general.image" is configured in module.json5.
     * */

    /**
     * 【错误示例】 直接判断 utd 强等于 IMAGE 类型
     * 产生的问题：当宿主应用传递 'general.jpeg' 'general.png' 等数据类型时 会被错误的判断为不属于图片
     */
    /**
     * [Example] The utd type is strongly equal to the Image type.
     * Problem: When the host application transfers data types such as general.jpeg and general.png, the host application incorrectly considers that the data types are not images.
     */

    // if (recordUtd === utd.UniformDataType.IMAGE) {
    //   return true;
    // }
    // return false;

    // 【正确示例】 先获取标准化数据类型描述对象 再判断是否属于图片类型
    // [Example] Obtain the standard data type description object and then determine whether the object belongs to the image type.
    let typeObj : utd.TypeDescriptor = utd.getTypeDescriptor(recordUtd);
    let result = typeObj.belongsTo(utd.UniformDataType.IMAGE);
    return result;
  }

  belongsToVideo(recordUtd: string): boolean {
    let typeObj : utd.TypeDescriptor = utd.getTypeDescriptor(recordUtd);
    let result = typeObj.belongsTo(utd.UniformDataType.VIDEO);
    return result;
  }

  // 构造Want数据 开发者请勿私自修改Want数据
  // Construct Want data. Developers cannot modify Want data without permission.
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
    Text($r('app.string.share_content'))
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
  ShareAction(text: ResourceStr, handelClick: () => void) {
    Button() {
      Text(text)
        .fontSize(25)
        .fontColor('#FFFFFF')
        .fontWeight(FontWeight.Bold)
    }
    .type(ButtonType.Capsule)
    .margin({ top: 20 })
    .backgroundColor('#0D9FFB')
    .width(200)
    .height(40)
    .onClick(() => handelClick())
  }

  build() {
    Navigation() {
      Row() {
        Column() {
          Text(this.message)
            .fontSize(50)
            .fontWeight(FontWeight.Bold)

          this.ShareContent();

          this.ShareAction($r('app.string.action_start_ability'), () => {
            let paramsWant: Want = {
              bundleName: "com.example.sharekit_sample_demo",
              abilityName: "ShareUIAbility",
            };
            (getContext(this) as common.UIExtensionContext).startAbility(paramsWant).then(() => {
              console.info('startAbility show');
              this.session?.terminateSelfWithResult({ resultCode: systemShare.ShareAbilityResultCode.CLOSE });
            }).catch((error: BusinessError) => {
              console.error(`startAbility error. Code: ${error.code}, message: ${error.message}`);
            });
          })

          this.ShareAction($r('app.string.action_back'), () => {
            this.session?.terminateSelfWithResult({ resultCode: systemShare.ShareAbilityResultCode.BACK });
          })
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