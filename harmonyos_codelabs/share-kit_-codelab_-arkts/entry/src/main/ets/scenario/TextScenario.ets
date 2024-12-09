/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

@Component
export default struct TextScenario {

  private async share() {
    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utd.UniformDataType.TEXT,
      content: 'This is a text.',
      title: 'Text Content',
      description: 'Text Description',
      thumbnail: new Uint8Array()
    });

    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: systemShare.SelectionMode.SINGLE,
      previewMode: systemShare.SharePreviewMode.DEFAULT,
    }).then(() => {
      console.info('ShareController show success.');
    }).catch((error: BusinessError) => {
      console.error(`ShareController show error. code: ${error.code}, message: ${error.message}`);
    });
  }

  build() {
    Row() {
      Text($r('app.string.text_type'))

      Button($r('app.string.share_btn'))
        .onClick(this.share)
    }
    .width('100%')
    .margin({ top: 12 })
    .padding({ left: 20, right: 20 })
    .justifyContent(FlexAlign.SpaceBetween)
  }
}
