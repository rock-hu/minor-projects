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

// [Start run_javascript_callback]
import { webview } from '@kit.ArkWeb';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();
  dataFromH5: string = '' // Messages passed from web pages
  @State webResult: string = '';

  build() {
    Column() {
      Text(this.webResult).fontSize(20)
      Web({ src: $rawfile('index.html'), controller: this.controller })
        .javaScriptAccess(true)
        .onPageEnd(event => {
          try {
            // Use regular expressions to match all single and double quotes.
            const regex = /['"\(\)]/g;
            // Use the replace method to replace all matched characters with an empty string.
            let sanitizedStr = this.dataFromH5.replace(regex, '');
            this.controller.runJavaScript(
              // Perform security filtering on the data_from_H5 passed through the web page before calling back.
              'javascript:alert(' + sanitizedStr + ')',
              (error, result) => {
                if (error) {
                  hilog.error(0x0000, 'ArkWebSecurity',
                    `run JavaScript error, ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
                  return;
                }
                if (result) {
                  this.webResult = result;
                  hilog.info(0x0000, 'ArkWebSecurity', `The test() return value is: ${result}`);
                }
              });
            if (event) {
              hilog.info(0x0000, 'ArkWebSecurity', 'url: ', event.url);
            }
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity',
              `ErrorCode: ${(error as BusinessError).code}, Message: ${(error as BusinessError).message}`);
          }
        })
    }
  }
}
// [End run_javascript_callback]