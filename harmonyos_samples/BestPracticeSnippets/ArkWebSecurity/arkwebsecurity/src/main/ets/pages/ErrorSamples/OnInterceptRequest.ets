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

// [Start onInterceptRequest_error]
import { webview } from '@kit.ArkWeb';
import { fileIo } from '@kit.CoreFileKit';
import { util } from '@kit.ArkTS';
import { hilog } from '@kit.PerformanceAnalysisKit';

@Entry
@Component
struct WebComponent {
  controller: webview.WebviewController = new webview.WebviewController();
  responseWeb: WebResourceResponse = new WebResourceResponse();
  heads: Header[] = [];

  build() {
    Column() {
      Web({ src: 'www.example.com', controller: this.controller })
      // Intercept URL resource access through onInterceptRequest and customize the response message.
        .onInterceptRequest((event) => {
          try {
            // The URL passed by event.request.getRequestUrl() should be restricted to a specific path,
            // and the presence of '../' should be checked.
            let file = fileIo.openSync(event.request.getRequestUrl(), fileIo.OpenMode.READ_ONLY | fileIo.OpenMode.CREATE);
            let arrayBuffer: ArrayBuffer = new ArrayBuffer(6);
            fileIo.readSync(file.fd, arrayBuffer, { offset: 0, length: arrayBuffer.byteLength });
            fileIo.closeSync(file);
            let decoder = util.TextDecoder.create('utf-8');
            let stringData = decoder.decodeToString(new Uint8Array(arrayBuffer));
            let head1: Header = {
              headerKey: 'Connection',
              headerValue: 'keep-alive'
            }
            let head2: Header = {
              headerKey: 'Cache-Control',
              headerValue: 'no-cache'
            }
            this.heads.push(head1);
            this.heads.push(head2);

            const promise: Promise<String> = new Promise((resolve: Function) => {
              this.responseWeb.setResponseHeader(this.heads);
              // After the file is read, it will be encapsulated and the response will be called back to the web page.
              this.responseWeb.setResponseData(stringData);
              this.responseWeb.setResponseEncoding('utf-8');
              this.responseWeb.setResponseMimeType('text/html');
              this.responseWeb.setResponseCode(200);
              this.responseWeb.setReasonMessage('OK');
              resolve('success');
            })
            promise.then(() => {
              hilog.info(0x0000, 'ArkWebSecurity', 'prepare response ready');
              this.responseWeb.setResponseIsReady(true);
            })
            this.responseWeb.setResponseIsReady(false);
          } catch (error) {
            hilog.error(0x0000, 'ArkWebSecurity', `ErrorCode: ${error.code}, Message: ${error.message}`);
          }
          return this.responseWeb;
        })
    }
  }
}
// [End onInterceptRequest_error]