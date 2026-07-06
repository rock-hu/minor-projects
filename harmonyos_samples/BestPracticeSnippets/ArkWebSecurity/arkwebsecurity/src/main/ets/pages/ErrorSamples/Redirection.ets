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

// [Start redirection_error]
import { webview } from '@kit.ArkWeb';
import { hilog } from '@kit.PerformanceAnalysisKit';

class TestObj {
  controller: webview.WebviewController

  constructor(webview_controller: webview.WebviewController) {
    this.controller = webview_controller
  }

  goto(uri: string) {
    try {
      this.controller.loadUrl(uri);
    } catch (error) {
      hilog.error(0x0000, 'ArkWebSecurity', `ErrorCode: ${error.code}, Message: ${error.message}`);
    }
  }
}

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();
  testObj = new TestObj(this.controller);

  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
        .javaScriptAccess(true)
        .javaScriptProxy({
          object: this.testObj,
          name: 'objName',
          methodList: ['goto'],
          controller: this.controller,
        })
    }
  }
}
// [End redirection_error]