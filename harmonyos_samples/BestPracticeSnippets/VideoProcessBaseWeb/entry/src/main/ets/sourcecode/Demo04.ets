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

import { webview } from '@kit.ArkWeb';
import { window } from '@kit.ArkUI';
import { Constants } from '../common/Constants';

@Entry
@Component
struct Index {
  @StorageProp(Constants.SYSTEM_LANGUAGE_KEY) language: string = '';
  private isFullScreen: boolean = false;
  private windowClass: window.Window | null = null;
  private handler: FullScreenExitHandler | null = null;
  private webController: webview.WebviewController = new webview.WebviewController();

  changeOrientation(orientation: window.Orientation) {
    this.windowClass?.setPreferredOrientation(orientation);
  }

  // [Start on_back_press]
  // src/main/ets/pages/Index.ets
  onBackPress(): boolean | void {
    if (this.isFullScreen && this.handler) {
      this.handler.exitFullScreen();
      return true;
    }
    return false;
  }
  // [End on_back_press]

  build() {
    Column() {
      // [Start rawfile]
      // src/main/ets/pages/Index.ets
      Web({
        src: $rawfile(/zh/.test(this.language) ? 'index_cn.html' : 'index_en.html'),
        controller: this.webController
      })
        .onFullScreenExit(() => {
          this.changeOrientation(window.Orientation.PORTRAIT);
          this.isFullScreen = false;
        })
      // [End rawfile]
    }
  }
}