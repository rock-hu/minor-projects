/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2024-2024. All rights reserved.
 */
import { systemShare } from '@kit.ShareKit';
import { uniformTypeDescriptor as utd } from '@kit.ArkData';
import { common } from '@kit.AbilityKit';
import { fileUri } from '@kit.CoreFileKit';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from '../utils/Logger';

let logger = Logger.getLogger('[ImageScenario]');

@Component
export default struct ZipScenario {

  private async share() {
    const contextFaker: Context = getContext(this);
    let filePath = contextFaker.filesDir + '/exampleZip.zip';

    let utdTypeId = utd.getUniformDataTypeByFilenameExtension('.zip', utd.UniformDataType.ARCHIVE);

    let shareData: systemShare.SharedData = new systemShare.SharedData({
      utd: utdTypeId,
      uri: fileUri.getUriFromPath(filePath),
      title: 'Zip Title',
      description: 'Zip Description',
    });

    let controller: systemShare.ShareController = new systemShare.ShareController(shareData);
    let context = getContext(this) as common.UIAbilityContext;
    controller.show(context, {
      selectionMode: systemShare.SelectionMode.SINGLE,
      previewMode: systemShare.SharePreviewMode.DETAIL,
    }).then(() => {
      logger.info('ShareController show success.');
    }).catch((error: BusinessError) => {
      logger.error(`ShareController show error. code: ${error.code}, message: ${error.message}`);
    });
  }

  build() {
    Row() {
      Text($r('app.string.compressed_package_type'))

      Button($r('app.string.share_btn'))
        .onClick(this.share)
    }
    .width('100%')
    .margin({ top: 12 })
    .padding({ left: 20, right: 20 })
    .justifyContent(FlexAlign.SpaceBetween)
  }
}
