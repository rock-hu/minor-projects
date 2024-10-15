import fs from '@ohos.file.fs';
import { systemShare } from '@kit.ShareKit';
import { common } from '@kit.AbilityKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { image } from '@kit.ImageKit';
import { fileUri } from '@kit.CoreFileKit';
import { promptAction } from '@kit.ArkUI';

interface IDemoItem {
  type: string;
  title: string;
  description: string;
  featureList: string[];
  handelClick: () => Promise<void>;
}

@Entry
@Component
struct Index {
  @State textSelectionMode: number = 0;
  @State imagePreviewMode: number = 0;
  @State videoPreviewMode: number = 0;

  private demoList: Array<IDemoItem> = [{
    type: 'text',
    title: 'Sharing Text',
    description: 'Text sharing allows users to share a piece of text to the target device or app.',
    featureList: [
      'When receiving the text, the target device will save the text in the file manager as a .txt file.',
      'When receiving the text, the target app can conveniently process the text. For example, when a piece of text is shared with Notes, a new note will be generated for the text.'
    ],
    handelClick: () => this.handelTextShare(),
  }, {
    type: 'image',
    title: 'Sharing Images',
    description: 'Image sharing allows users to share one or more images to the target device or app.',
    featureList: [
      'When receiving the images, the target device will save them to Gallery.',
      'When receiving the images, the target app can conveniently process the images. For example, a user can share an image with a MeeTime friend.'
    ],
    handelClick: () => this.handelImageShare(),
  }, {
    type: 'video',
    title: 'Sharing Videos',
    description: 'Video sharing allows users to share one or more videos to the target device or app.',
    featureList: [
      'When receiving the videos, the target device will save them to Gallery.',
      'When receiving the videos, the target app can conveniently process the videos. For example, a user can share a video with a MeeTime friend.'
    ],
    handelClick: () => this.handelVideoShare(),
  }];

  async aboutToAppear(): Promise<void> {
    const context = getContext(this) as common.UIAbilityContext;
    const filesDir = context.filesDir;
    [
      [context.resourceManager.getMediaContentSync($r('app.media.exampleImage')), '/exampleImage.jpg'],
      [context.resourceManager.getMediaContentSync($r('app.media.exampleVideo')), '/exampleVideo.mp4']
    ].forEach(item => {
      let file = fs.openSync(filesDir + item[1], fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      let writeLen = fs.writeSync(file.fd, (item[0] as Uint8Array).buffer);
      console.info("write data to file succeed and size is:" + writeLen);
      fs.closeSync(file);
    })
  }

  private async handelTextShare(): Promise<void> {
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.TEXT,
      content: 'This is a text.',
      title: 'Text Title',
      description: 'Text Description',
      label: 'Text Content',
    });
    shareData.addRecord({
      utd: utd.UniformDataType.HYPERLINK,
      content: 'https://www.vmall.com/index.html?cid=128688',
      title: 'Huawei Vmall',
      description: 'Pura 70 Ultra',
      label: 'Huawei Vmall'
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: this.textSelectionMode,
      excludedAbilities: [systemShare.ShareAbilityType.COPY_TO_PASTEBOARD],
    });
  }

  private async handelImageShare(): Promise<void> {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleImage.jpg';
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.IMAGE,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Picture Title',
      description: 'Picture Description'
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      previewMode: this.imagePreviewMode == 0 ? systemShare.SharePreviewMode.DEFAULT : systemShare.SharePreviewMode.DETAIL,
      excludedAbilities: [systemShare.ShareAbilityType.SAVE_TO_MEDIA_ASSET],
    });
  }

  private async handelVideoShare(): Promise<void> {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleVideo.mp4';
    /** thumbnail buffer */
    let thumbnailPath = contextFaker.filesDir + '/exampleImage.jpg';
    const imageSourceApi: image.ImageSource = image.createImageSource(thumbnailPath);
    let opts: image.InitializationOptions = { size: { height: 6, width: 6 } }
    const pixelMap: image.PixelMap = await imageSourceApi.createPixelMap(opts);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    const buffer: ArrayBuffer = await imagePackerApi.packing(pixelMap, {
      format: 'image/jpeg',
      quality: 30
    });
    /** thumbnail buffer end */
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.VIDEO,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Video title',
      description: 'Video Description',
      thumbnail: new Uint8Array(buffer),
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    controller.on('dismiss', () => promptAction.showToast({ message: 'Share panel disappeared' }));
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      previewMode: this.videoPreviewMode === 0 ? systemShare.SharePreviewMode.DEFAULT : systemShare.SharePreviewMode.DETAIL
    });
  }

  @Builder
  PreviewModule(type: string) {
    Row() {
      if (type === 'text') {
        Image(this.textSelectionMode === 0 ? $r('app.media.effectTextSingle') : $r('app.media.effectTextBatch')).width(100);
      } else if (type === 'image') {
        Image(this.imagePreviewMode === 0 ? $r('app.media.effectImageDefault') : $r('app.media.effectImageDetail')).width(100);
      } else if (type === 'video') {
        Image(this.videoPreviewMode === 0 ? $r('app.media.effectVideoDefault') : $r('app.media.effectVideoDetail')).width(100);
      }
    }
    .width(150)
    .margin({ right: 12 })
  }

  @Builder
  RadioItem(text: string, value: string, group: string, checked: boolean, handelChange: (isChecked: boolean) => void) {
    Row() {
      Radio({ value, group })
        .checked(checked)
        .onChange(handelChange)
      Text(text)
    }
    .margin({ right: 12 })
  }

  @Builder
  DescriptionText(item: IDemoItem) {
    Text(item.description)
      .width('100%')
      .margin({ top: 12 })
      .fontSize(14)
    ForEach(item.featureList, (text: string, index: number) => {
      ListItem() {
        Text((index + 1) + '.' + text)
          .width('100%')
          .margin({ top: 4 })
          .fontSize(14)
      }
    })
  }

  @Builder
  DescriptionModule(item: IDemoItem) {
    Row() {
      if (item.type === 'text') {
        this.RadioItem('single', 'single', 'textGroup', this.textSelectionMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.textSelectionMode = 0;
          }
        })
        this.RadioItem('batch', 'batch', 'textGroup', this.textSelectionMode === 1, (isChecked: boolean) => {
          if (isChecked) {
            this.textSelectionMode = 1;
          }
        })
      } else if (item.type === 'image') {
        this.RadioItem('default', 'default', 'imageGroup', this.imagePreviewMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.imagePreviewMode = 0;
          }
        })
        this.RadioItem('detail', 'detail', 'imageGroup', this.imagePreviewMode === 1, (isChecked: boolean) => {
          if (isChecked) {
            this.imagePreviewMode = 1;
          }
        })
      } else if (item.type === 'video') {
        this.RadioItem('default', 'default', 'videoGroup', this.videoPreviewMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.videoPreviewMode = 0;
          }
        })
        this.RadioItem('detail', 'detail', 'videoGroup', this.videoPreviewMode === 1, (isChecked: boolean) => {
          if (isChecked) {
            this.videoPreviewMode = 1;
          }
        })
      }
    }
    .width('100%')
    .margin({ top: 12 })

    this.DescriptionText(item)
  }

  @Builder
  ScrollList() {
    List() {
      ForEach(this.demoList, (item: IDemoItem, index: number) => {
        ListItem() {
          Flex({ direction: FlexDirection.Row }) {
            this.PreviewModule(item.type);

            Column() {
              Flex({ direction: FlexDirection.Row }) {
                Text(item.title)
                  .flexGrow(1)
                  .fontSize(18)
                  .fontWeight(500)

                Button('Share')
                  .width(80)
                  .height(30)
                  .margin({ left: 12 })
                  .onClick(() => item.handelClick())
              }
              .width('100%')

              this.DescriptionModule(item);
            }
            .flexGrow(1)
          }
          .margin({ top: 12 })
        }
      })
    }
    .width('100%')
    .height('auto')
    .flexGrow(1)
    .padding(20)
  }

  build() {
    Flex() {
      Scroll() {
        Flex({ direction: FlexDirection.Column }) {
          Text('HarmonyOS ShareKit Demo')
            .fontSize(24)
            .textAlign(TextAlign.Center)
            .margin({ left: 20 })

          this.ScrollList();
        }
        .width('100%')
        .height('100%')
      }
      .width('100%')
      .height('100%')
      .padding({ top: 30 })
    }
    .width('100%')
    .height('100%')
  }
}
