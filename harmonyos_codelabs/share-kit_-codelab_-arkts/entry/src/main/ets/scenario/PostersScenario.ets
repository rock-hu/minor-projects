/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { fileUri } from '@kit.CoreFileKit';
import { image } from '@kit.ImageKit';

@Component
export default struct Posters {

  private async share() {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleImage.jpg';

    let thumbnailPath = contextFaker.filesDir + '/exampleImage.jpg';
    const imageSourceApi: image.ImageSource = image.createImageSource(thumbnailPath);
    let opts: image.InitializationOptions = { size: { height: 6, width: 6 } }
    const pixelMap: image.PixelMap = await imageSourceApi.createPixelMap(opts);
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    const buffer: ArrayBuffer = await imagePackerApi.packing(pixelMap, {
      format: 'image/jpeg',
      quality: 30,
    });

    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.IMAGE,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Picture Title',
      description: 'Picture Description',
      label: 'Poster'
    });
    shareData.addRecord({
      utd: utd.UniformDataType.HYPERLINK,
      content: 'https://www.vmall.com/index.html?cid=128688',
      title: 'Huawei Vmall',
      description: 'Pura 70 Ultra',
      label: 'Huawei Vmall',
      thumbnail: new Uint8Array(buffer),
    });

    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: systemShare.SelectionMode.SINGLE,
      previewMode: systemShare.SharePreviewMode.DETAIL,
    });
  }

  build() {
    Row() {
      Text($r('app.string.poster_type'))

      Button($r('app.string.share_btn'))
        .onClick(this.share)
    }
    .width('100%')
    .margin({ top: 12 })
    .padding({ left: 20, right: 20 })
    .justifyContent(FlexAlign.SpaceBetween)
  }
}
