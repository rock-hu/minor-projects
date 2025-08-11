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

import { abilityAccessCtrl, bundleManager, Permissions } from '@kit.AbilityKit';
import Logger from '../utils/Logger';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@Component
struct Index {
  async requestCameraPermission() {
    const permissions: Array<Permissions> = ['ohos.permission.CAMERA'];
    const accessManager = abilityAccessCtrl.createAtManager();
    const bundleFlags = bundleManager.BundleFlag.GET_BUNDLE_INFO_WITH_APPLICATION;
    const bundleInfo = bundleManager.getBundleInfoForSelfSync(bundleFlags);
    const grantStatus = accessManager.checkAccessTokenSync(bundleInfo.appInfo.accessTokenId, permissions[0]);
    if (grantStatus === abilityAccessCtrl.GrantStatus.PERMISSION_DENIED) {
      // [Start permission_camera]
      accessManager.requestPermissionsFromUser(this.getUIContext().getHostContext(), ['ohos.permission.CAMERA'])
        .then((data) => {
          let grantStatus: Array<number> = data.authResults;
          if (grantStatus.length > 0 && grantStatus[0] === 0) {
            Logger.info('request permissions granted');
          } else {
            Logger.info('request permissions denied');
            // [StartExclude permission_camera]
            this.getUIContext().getPromptAction().showToast({
              message: 'This function requires the camera permission. Go to the settings page to authorize the permission'
            });
            // [EndExclude permission_camera]
          }
        })
          // [End permission_camera]
        .catch((error: BusinessError) => {
          Logger.error(`request permissions exception, Catch error:${JSON.stringify(error)}`);
        })
    } else {
      Logger.info('request permissions granted');
      this.getUIContext().getPromptAction().showToast({
        message: 'request permissions has granted'
      });
    }
  }

  build() {
    Column({ space: 12 }) {
      Button($r('app.string.get_approximately_location'))
        .height(40)
        .width('100%')
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/ApproximatelyLocationDemo' });
        })
      Button($r('app.string.use_picker'))
        .height(40)
        .width('100%')
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/PickerDemo' });
        })
      Button($r('app.string.apply_camera_permissions'))
        .height(40)
        .width('100%')
        .onClick(() => {
          this.requestCameraPermission();
        })
    }
    .height('100%')
    .width('100%')
    .padding(16)
    .justifyContent(FlexAlign.End)
  }
}