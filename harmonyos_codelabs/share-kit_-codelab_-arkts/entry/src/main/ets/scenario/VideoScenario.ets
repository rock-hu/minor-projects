/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { fileUri } from '@kit.CoreFileKit';
import { image } from '@kit.ImageKit';
import { BusinessError } from '@kit.BasicServicesKit';

@Component
export default struct Posters {

  private async share() {
    const contextFaker: Context = getContext(this);
    let thumbnailPath = contextFaker.filesDir + '/exampleImage.jpg';
    const imageSourceApi: image.ImageSource = image.createImageSource(thumbnailPath);
    let opts: image.InitializationOptions = { size: { height: 6, width: 6 } }
    const pixelMap: image.PixelMap = await imageSourceApi.createPixelMap(opts);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    const buffer: ArrayBuffer = await imagePackerApi.packing(pixelMap, {
      format: 'image/jpeg',
      quality: 30,
    });

    let filePath = contextFaker.filesDir + '/exampleImage.jpg';
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.VIDEO,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Video title',
      description: 'Video Description',
      thumbnail: new Uint8Array(buffer),
    });

    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: systemShare.SelectionMode.SINGLE,
      previewMode: systemShare.SharePreviewMode.DETAIL,
    }).then(() => {
      console.info('ShareController show success.');
    }).catch((error: BusinessError) => {
      console.error(`ShareController show error. code: ${error.code}, message: ${error.message}`);
    });
  }

  build() {
    Row() {
      Text($r('app.string.video_type'))

      Button($r('app.string.share_btn'))
        .onClick(this.share)
    }
    .width('100%')
    .margin({ top: 12 })
    .padding({ left: 20, right: 20 })
    .justifyContent(FlexAlign.SpaceBetween)
  }
}
