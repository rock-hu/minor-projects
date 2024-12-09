/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import { common } from '@kit.AbilityKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { fileUri } from '@kit.CoreFileKit';
import { promptAction } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { image } from '@kit.ImageKit';
import { resourceManager } from '@kit.LocalizationKit';

interface IDemoItem {
  type: string;
  title: ResourceStr;
  description: ResourceStr;
  featureList: ResourceStr[];
  handelClick: () => Promise<void>;
}

@Component
export default struct BaseDemo {
  @State textSelectionMode: number = 0;
  @State imagePreviewMode: number = 0;
  @State videoPreviewMode: number = 0;

  private thumbnailBuffer: Uint8Array | undefined = undefined;

  private demoList: Array<IDemoItem> = [{
    type: 'text',
    title: $r('app.string.text_sharing'),
    description: $r('app.string.text_sharing_description'),
    featureList: [$r('app.string.share_selection_mode'), $r('app.string.share_ui_ability'), $r('app.string.share_excluded_abilities_copy')],
    handelClick: () => this.handelTextShare(),
  }, {
    type: 'image',
    title: $r('app.string.image_sharing'),
    description: $r('app.string.image_sharing_description'),
    featureList: [$r('app.string.share_preview_mode'), $r('app.string.share_extension_ability'), $r('app.string.share_excluded_abilities_print')],
    handelClick: () => this.handelImageShare(),
  }, {
    type: 'video',
    title: $r('app.string.video_sharing'),
    description:  $r('app.string.video_sharing_description'),
    featureList: [$r('app.string.share_preview_mode'), $r('app.string.share_extension_ability'), $r('app.string.share_close_listening')],
    handelClick: () => this.handelVideoShare(),
  }];

  private async handelTextShare(): Promise<void> {
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.TEXT,
      content: 'This is a text.',
      title: 'Text Title',
      description: 'Text Description',
      label: 'Text Title',
    });
    shareData.addRecord({
      utd: utd.UniformDataType.HYPERLINK,
      content: 'https://www.vmall.com/index.html?cid=128688',
      title: 'Huawei Vmall',
      description: 'Pura 70 Ultra',
      label: 'Huawei Vmall',
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: this.textSelectionMode,
      previewMode: systemShare.SharePreviewMode.DETAIL,
      excludedAbilities: [systemShare.ShareAbilityType.COPY_TO_PASTEBOARD],
    }).then(() => {
      console.info('HuaweiShare_ show');
    }).catch((error: BusinessError) => {
      console.error(`HuaweiShare_ show error. Code: ${error.code}, message: ${error.message}`);
    });
  }

  private async handelImageShare(): Promise<void> {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleImage.jpg';
    let utdTypeId = utd.getUniformDataTypeByFilenameExtension('.jpg', utd.UniformDataType.IMAGE);
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utdTypeId,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Picture Title',
      description: 'Picture Description',
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      previewMode: this.imagePreviewMode == 0 ? systemShare.SharePreviewMode.DEFAULT : systemShare.SharePreviewMode.DETAIL,
      selectionMode: systemShare.SelectionMode.SINGLE,
      excludedAbilities: [systemShare.ShareAbilityType.PRINT],
    }).then(() => {
      console.info('HuaweiShare_ show');
    }).catch((error: BusinessError) => {
      console.error(`HuaweiShare_ show error. Code: ${error.code}, message: ${error.message}`);
    });
  }

  private async handelVideoShare(): Promise<void> {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleVideo.mp4';
    /** Solution 1 */
    // const resourceMgr: resourceManager.ResourceManager = contextFaker.resourceManager;
    // const fileData: Uint8Array = await resourceMgr.getMediaContent($r('app.media.icon'));
    // this.thumbnailBuffer = fileData;
    /** Solution 2 */
    let thumbnailPath = contextFaker.filesDir + '/exampleImage.jpg';
    const imageSourceApi: image.ImageSource = image.createImageSource(thumbnailPath);
    let opts: image.InitializationOptions = { size: { height: 6, width: 6 } };
    const pixelMap: image.PixelMap = await imageSourceApi.createPixelMap(opts);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    const buffer: ArrayBuffer = await imagePackerApi.packing(pixelMap, {
      format: 'image/jpeg',
      quality: 30,
    });
    this.thumbnailBuffer = new Uint8Array(buffer);
    let utdTypeId = utd.getUniformDataTypeByFilenameExtension('.mp4', utd.UniformDataType.VIDEO);
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utdTypeId,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Video title',
      description: 'Video Description',
      thumbnail: this.thumbnailBuffer,
    });
    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    const callback = () => {
      console.info('HuaweiShare_ dismiss run');
      promptAction.showToast({ message: 'Share panel disappeared' });
    };
    controller.on('dismiss', callback);
    // controller.off('dismiss', callback);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      previewMode: this.videoPreviewMode === 0 ? systemShare.SharePreviewMode.DEFAULT : systemShare.SharePreviewMode.DETAIL
    }).then(() => {
      console.info('HuaweiShare_ show');
    }).catch((error: BusinessError) => {
      console.error(`HuaweiShare_ show error. Code: ${error.code}, message: ${error.message}`);
    });
  }

  @Builder
  PreviewModule(type: string) {
    Row() {
      if (type === 'text') {
        Image(this.textSelectionMode === 0 ? $r('app.media.share_preview_text1') : $r('app.media.share_preview_text2')).width(100);
      } else if (type === 'image') {
        Image(this.imagePreviewMode === 0 ? $r('app.media.share_preview_image1') : $r('app.media.share_preview_image2')).width(100);
      } else if (type === 'video') {
        Image(this.videoPreviewMode === 0 ? $r('app.media.share_preview_video1') : $r('app.media.share_preview_video2')).width(100);
      }
    }
    .width(150)
    .margin({ right: 12 })
  }

  @Builder
  RadioItem(text: ResourceStr, value: string, group: string, checked: boolean, handelChange: (isChecked: boolean) => void) {
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
    ForEach(item.featureList, (text: ResourceStr, index: number) => {
      ListItem() {
        Row() {
          Text(index + 1 + '.').fontSize(14)
          Text(text).fontSize(14)
        }
        .width('100%')
        .margin({ top: 4 })
        .alignItems(VerticalAlign.Top)
      }
    })
  }

  @Builder
  DescriptionModule(item: IDemoItem) {
    Row() {
      if (item.type === 'text') {
        this.RadioItem($r('app.string.single_mode'), 'single', 'textGroup', this.textSelectionMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.textSelectionMode = 0;
          }
        })
        this.RadioItem($r('app.string.batch_mode'), 'batch', 'textGroup', this.textSelectionMode === 1, (isChecked: boolean) => {
          if (isChecked) {
            this.textSelectionMode = 1;
          }
        })
      } else if (item.type === 'image') {
        this.RadioItem($r('app.string.default_mode'), 'default', 'imageGroup', this.imagePreviewMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.imagePreviewMode = 0;
          }
        })
        this.RadioItem($r('app.string.detail_mode'), 'detail', 'imageGroup', this.imagePreviewMode === 1, (isChecked: boolean) => {
          if (isChecked) {
            this.imagePreviewMode = 1;
          }
        })
      } else if (item.type === 'video') {
        this.RadioItem($r('app.string.default_mode'), 'default', 'videoGroup', this.videoPreviewMode === 0, (isChecked: boolean) => {
          if (isChecked) {
            this.videoPreviewMode = 0;
          }
        })
        this.RadioItem($r('app.string.detail_mode'), 'detail', 'videoGroup', this.videoPreviewMode === 1, (isChecked: boolean) => {
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

                Button($r('app.string.share_btn'))
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
          .margin({ bottom: 12 })
        }
      })
    }
    .width('100%')
    .height('auto')
    .flexGrow(1)
    .padding({ top: 12, left: 20, right: 20 })
  }

  build() {
    Scroll() {
      Flex({ direction: FlexDirection.Column }) {
        this.ScrollList();
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height('100%')
  }
}