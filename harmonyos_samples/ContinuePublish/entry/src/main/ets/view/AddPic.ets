/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { dataSharePredicates } from '@kit.ArkData';
import { fileIo as fs } from '@kit.CoreFileKit';
import { promptAction } from '@kit.ArkUI';
import { common } from '@kit.AbilityKit';
import { image } from '@kit.ImageKit';
import { CommonConstants } from '../constants/CommonConstants';
import { fileSelect } from '../utils/FileUtil';
import { ImageInfo } from '../model/ContentInfo';
import { hilog } from '@kit.PerformanceAnalysisKit';

/**
 * Image area.
 */
@Component
export struct AddPic {
  @StorageLink('imageUriArray') imageUriArray: Array<ImageInfo> = [];
  @StorageLink('attachments') attachments: Object = [];
  @State pixelMapImage: PixelMap | Resource | null = null;
  // Get the UIAbilityContext information of device A.
  private context = this.getUIContext().getHostContext() as common.UIAbilityContext;

  /**
   * Select picture method.
   */
  selectImage(): void {
    fileSelect().then((uri: Array<ResourceStr>) => {
      uri.forEach((item: ResourceStr) => {
        this.getThumbnail(item as string);
      });
    });
  }

  async getThumbnail(uri: string) {
    let phAccessHelper = photoAccessHelper.getPhotoAccessHelper(this.context);
    let predicates: dataSharePredicates.DataSharePredicates = new dataSharePredicates.DataSharePredicates();
    predicates.equalTo('uri', uri);
    let fetchOption: photoAccessHelper.FetchOptions = {
      fetchColumns: [],
      predicates: predicates
    };
    let fetchResult: photoAccessHelper.FetchResult<photoAccessHelper.PhotoAsset> =
      await phAccessHelper.getAssets(fetchOption);
    let asset: photoAccessHelper.PhotoAsset = await fetchResult.getFirstObject();
    asset.getThumbnail((err, pixelMap) => {
      if (err === undefined) {
        hilog.info(0x0000, '[AddPic]', 'getThumbnail successful ' + pixelMap);
        let imageName = asset.displayName.substring(0, (asset.displayName).indexOf('.'));
        this.PixelMapToBuffer(pixelMap, imageName);
        this.imageUriArray.push({ imagePixelMap: pixelMap, imageName: imageName });
      } else {
        hilog.error(0x0000, '[AddPic]', `getThumbnail fail with error: ${err.code}, ${err.message}`);
      }
    });
  }

  PixelMapToBuffer(pixelMap: image.PixelMap, displayName: string): void {
    const imagePackerApi: image.ImagePacker = image.createImagePacker();
    let packOpts: image.PackingOption = { format: 'image/jpeg', quality: 100 };
    imagePackerApi.packToData(pixelMap, packOpts).then((data: ArrayBuffer) => {
      this.writeDistributedFile(data, displayName)
    })
  }
  // [Start write_distributed_file]
  writeDistributedFile(buf: ArrayBuffer, displayName: string): void {
    // The asset is written to the distributed file directory.
    // Obtain the distributed file directory path.
    let distributedDir: string = this.context.distributedFilesDir;
    let fileName: string = '/' + displayName;
    let filePath: string = distributedDir + fileName;
    try {
      // Create a file in a distributed directory.
      let file = fs.openSync(filePath, fs.OpenMode.READ_WRITE | fs.OpenMode.CREATE);
      hilog.info(0x0000, '[AddPic]','Create file success.');
      // Write content to a file (if the asset is a picture, the picture can be converted to a buffer to write)
      fs.writeSync(file.fd, buf);
      // closed file.
      fs.closeSync(file.fd);
    } catch (error) {
      let err: BusinessError = error as BusinessError;
      hilog.info(0x0000, '[AddPic]',
        `Failed to openSync / writeSync / closeSync. Code: ${err.code}, message: ${err.message}`);
    }
  }
  // [End write_distributed_file]
  build() {
    Column() {
      this.addPic()
    }
  }

  /**
   *  Adding an image.
   */
  @Builder
  addPic() {
    Row() {
      List({ space: CommonConstants.LIST_COMM_SPACE }) {
        ForEach(this.imageUriArray, (item: ImageInfo) => {
          ListItem() {
            Image(item.imagePixelMap)
              .width($r('app.integer.add_image_area_size'))
              .height($r('app.integer.add_image_area_size'))
              .borderRadius($r('app.integer.border_radius3'))
              .visibility(item ? Visibility.Visible : Visibility.None)
          }
        }, (item: ImageInfo, index: number) => JSON.stringify(item) + index)
        this.addDefaultPic()
      }
      .listDirection(Axis.Horizontal)
      .scrollBar(BarState.Off)
    }
    .height($r('app.integer.add_image_area_size'))
    .width(CommonConstants.FULL_PERCENT)
    .alignItems(VerticalAlign.Center)
    .padding({
      right: $r('app.integer.comm_padding_margin'),
      left: $r('app.integer.comm_padding_margin')
    })
  }

  /**
   * Add default image.
   */
  @Builder
  addDefaultPic() {
    Row() {
      Column() {
        Image($r('app.media.ic_add_pic'))
          .objectFit(ImageFit.Cover)
          .width($r('app.integer.add_icon_size'))
          .height($r('app.integer.add_icon_size'))
        Text(this.imageUriArray.length === 0 ? $r('app.string.add_high_img') : $r('app.string.continue_add'))
          .fontSize($r('app.integer.text_size_body3'))
          .fontColor($r('app.color.add_image_background'))
          .margin({top:$r('app.integer.comm_padding_margin_mini')})
      }
      .justifyContent(FlexAlign.Center)
      .width(CommonConstants.FULL_PERCENT)
    }
    .borderStyle(BorderStyle.Dashed)
    .borderWidth($r('app.integer.default_border_width'))
    .borderColor($r('app.color.add_image_border_color'))
    .borderRadius($r('app.integer.border_radius3'))
    .width($r('app.integer.add_image_area_size'))
    .height($r('app.integer.add_image_area_size'))
    .margin({
      right: $r('app.integer.add_default_pic_right'),
      bottom: $r('app.integer.add_default_pic_bottom')
    })
    .onClick(() => {
      if (this.imageUriArray.length < CommonConstants.MAX_ADD_PIC) {
        this.selectImage();
      } else {
        this.getUIContext().getPromptAction().showToast({ message: $r('app.string.add_picture_prompt') });
      }
    })
  }
}