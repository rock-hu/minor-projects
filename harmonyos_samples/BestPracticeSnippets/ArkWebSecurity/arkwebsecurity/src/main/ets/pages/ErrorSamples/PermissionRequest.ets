/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

// [Start on_permission_request_error]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { abilityAccessCtrl, common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();

  aboutToAppear(): void {
    let atManager = abilityAccessCtrl.createAtManager();
    let context: Context = this.getUIContext().getHostContext() as common.UIAbilityContext;
    atManager.requestPermissionsFromUser(context, ['ohos.permission.CAMERA', 'ohos.permission.MICROPHONE'])
      .then((data) => {
        hilog.info(0x0000, 'ArkWebSecurity', 'data: ' + JSON.stringify(data));
        hilog.info(0x0000, 'ArkWebSecurity', 'data permission: ' + data.permissions);
        hilog.info(0x0000, 'ArkWebSecurity', 'data authResults: ' + data.authResults);
      }).catch((error: BusinessError) => {
      hilog.error(0x0000, 'ArkWebSecurity',
        `Failed to request permissions from user. Code is ${error.code}, message is ${error.message}`);
    })
  }

  build() {
    Column() {
      Web({ src: 'https://example.com/index.html', controller: this.controller })
        .onPermissionRequest((event) => {
          // Directly granting page permissions is a wrong approach.
          event.request.grant(event.request.getAccessibleResource());
        })
    }
  }
}
// [End on_permission_request_error]