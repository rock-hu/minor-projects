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

// [Start LoadURLWithSensitiveData_error]
import { webview } from '@kit.ArkWeb';

@Entry
@Component
struct WebComponent {
  @State message: string = 'Hello World';
  webviewController: webview.WebviewController = new webview.WebviewController();
  accessToken: string = 'xxxx'; // After the user completes the login operation, the token returned by the server.
  untrustedURL: string = '' // Any URL to be loaded

  build() {
    Column() {
      Web({ src: this.untrustedURL + '?token=' + this.accessToken, controller: this.webviewController })
    }
  }
}
// [End LoadURLWithSensitiveData_error]