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

// [Start setUrlTrustList1]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();
  urlTrustList: string =
    '{\'UrlPermissionList\':[{\'scheme\':\'https\', \'host\':\'trust.example.com\', \'port\':80, \'path\':\'test\'}]}'

  build() {
    Column() {
      Button('Setting the trustList')
        .onClick(() => {
          try {
            // Set up an allowlist to allow access only to trusted web pages.
            this.controller.setUrlTrustList(this.urlTrustList);
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
        })
      Button('Access the trust web')
        .onClick(() => {
          try {
            // Allowlist activated, access to trusted web pages is permitted.
            this.controller.loadUrl('https://trust.example.com/test');
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
        })
      Button('Access the untrusted web')
        .onClick(() => {
          try {
            // Allowlist activated, blocking access to untrusted web and displaying an alarm page.
            this.controller.loadUrl('http://untrust.example.com/test');
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
        })
    }
  }
}
// [End setUrlTrustList1]