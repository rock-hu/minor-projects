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

import { window } from '@kit.ArkUI';
import { Constants } from '../common/Constants';
import { webview } from '@kit.ArkWeb';

// DocsCode 1735042061937
// src/main/ets/pages/Index.ets

@Entry
@Component
struct Index {
  private isFullScreen: boolean = false;
  private handler: FullScreenExitHandler | null = null;
  // DocsDot
  @StorageProp(Constants.SYSTEM_LANGUAGE_KEY) language: string = '';
  private webController: webview.WebviewController = new webview.WebviewController();
  private windowClass: window.Window | null = null;

  changeOrientation(orientation: window.Orientation) {
    this.windowClass?.setPreferredOrientation(orientation);
  }
  // DocsDot

  build() {
    Column() {
      Web({
        src: $rawfile(/zh/.test(this.language) ? 'index_cn.html' : 'index_en.html'),
        controller: this.webController
      })
        // ...
        .onFullScreenEnter((event) => {
          this.handler = event.handler;
          this.changeOrientation(window.Orientation.LANDSCAPE);
          this.isFullScreen = true;
        })
    }
  }
}
// DocsCode 1735042061937