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

// [Start configCookieSync]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();
  unknown_url: string = ''; // unknown_url from external source
  urlTrustList: string =
    '{\'UrlPermissionList\':[{\'scheme\':\'http\', \'host\':\'trust.example.com\', \'port\':80, \'path\':\'test\'}]}'

  build() {
    Column() {
      Button('configCookieSync')
        .onClick(() => {
          try {
            // Set up an allowlist to allow access only to trusted web pages.
            this.controller.setUrlTrustList(this.urlTrustList);
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
          try {
            // Perform another validity check on unknown_url to implement more granular control.
            if (this.unknown_url === 'https://www.example.com') {
              webview.WebCookieManager.configCookieSync(this.unknown_url, 'a=b');
            }
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
        })
      Web({ src: this.unknown_url, controller: this.controller })
    }
  }
}
// [End configCookieSync]