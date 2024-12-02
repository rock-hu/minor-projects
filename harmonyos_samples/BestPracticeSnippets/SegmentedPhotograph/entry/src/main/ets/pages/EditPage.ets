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

import { image } from '@kit.ImageKit';
import { photoAccessHelper } from '@kit.MediaLibraryKit';
import { router } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from '../common/utils/Logger';
import { GlobalContext } from '../common/utils/GlobalContext';
import { Constants } from '../common/Constants';

const TAG: string = 'EditPage';

class RequestImageParams {
  context: Context | undefined = undefined;
  photoAsset: photoAccessHelper.PhotoAsset | undefined = undefined;
  callback: Function = () => {
  };
}

@Entry
@Component
struct EditPage {
  @State createPixelMapState: boolean = false;
  @State curPixelMap: image.PixelMap | undefined = undefined;
  @State photoUri: string = '';
  @StorageLink('photoMode') photoMode: string = '';
  private backIconLayoutWeight = 1;
  private textLayoutWeight = 8;
  // DocsCode 2
  photoBufferCallback: (arrayBuffer: ArrayBuffer) => void = (arrayBuffer: ArrayBuffer) => {
    Logger.info(TAG, 'photoBufferCallback is called');
    let imageSource = image.createImageSource(arrayBuffer);
    imageSource.createPixelMap((err: BusinessError, data: image.PixelMap) => {
      if (err) {
        Logger.info(TAG, `createPixelMap err:${err.code}`);
        return;
      }
      Logger.info(TAG, 'createPixelMap is called');
      this.curPixelMap = data;
    });
  };

  requestImage(requestImageParams: RequestImageParams): void {
    try {
      class MediaDataHandler implements photoAccessHelper.MediaAssetDataHandler<ArrayBuffer> {
        onDataPrepared(data: ArrayBuffer, map: Map<string, string>): void {
          Logger.info(TAG, 'onDataPrepared map' + JSON.stringify(map));
          requestImageParams.callback(data);
          Logger.info(TAG, 'onDataPrepared end');
        }
      };
      let requestOptions: photoAccessHelper.RequestOptions = {
        deliveryMode: photoAccessHelper.DeliveryMode.BALANCE_MODE,
      };
      const handler = new MediaDataHandler();
      photoAccessHelper.MediaAssetManager.requestImageData(requestImageParams.context, requestImageParams.photoAsset,
        requestOptions, handler);
    } catch (error) {
      Logger.error(TAG, `Failed in requestImage, error code: ${error.code}`);
    }
  }

  aboutToAppear() {
    Logger.info(TAG, 'aboutToAppear begin');
    if (this.photoMode === Constants.SUBSECTION_MODE) {
      let curPhotoAsset = GlobalContext.get().getT<photoAccessHelper.PhotoAsset>('photoAsset');
      this.photoUri = curPhotoAsset.uri;
      let requestImageParams: RequestImageParams = {
        context: getContext(),
        photoAsset: curPhotoAsset,
        callback: this.photoBufferCallback
      };
      this.requestImage(requestImageParams);
      Logger.info(TAG, `aboutToAppear photoUri: ${this.photoUri}`);
    } else if (this.photoMode === Constants.SINGLE_STAGE_MODE) {
      this.curPixelMap = GlobalContext.get().getT<image.PixelMap>('photoAsset');
    }
  }

  // DocsCode 2
  build() {
    Column() {
      Column() {
        // DocsCode 1
        Image($r('app.media.ic_public_back'))
          .objectFit(ImageFit.Cover)
          .onClick(() => {
            Logger.info(TAG, 'back onClick');
            router.back();
          })
          .width(40)
          .height(40)
        // DocsCode 1
      }
      .padding({ left: Constants.BACK_ICON_MARGIN })
      .width(Constants.FULL_PERCENT)
      .layoutWeight(this.backIconLayoutWeight)
      .alignItems(HorizontalAlign.Start)
      .justifyContent(FlexAlign.Center)
      .margin({ top: 44 })

      Column() {
        Image(this.curPixelMap)
          .objectFit(ImageFit.Cover)
          .width(Constants.FULL_PERCENT)
          .height(Constants.EIGHTY_PERCENT)
      }
      .width(Constants.FULL_PERCENT)
      .margin({ top: 68 })
      .layoutWeight(this.textLayoutWeight)
    }
    .width(Constants.FULL_PERCENT)
    .height(Constants.FULL_PERCENT)
    .backgroundColor($r('app.color.dialog_background_color'))
  }
}
