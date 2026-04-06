/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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
import { http } from '@kit.NetworkKit';
import httpGet from '../common/utils/HttpUtil';
import StyleConstant from '../common/constant/StyleConstant';
import CommonConstant from '../common/constant/CommonConstants';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { BusinessError } from '@kit.BasicServicesKit';

const DOMAIN = 0x0000;
const TAG = 'WebPage';
const FORMAT = '%{public}s';

@Entry
@Component
struct WebPage {
  @StorageLink('statusBarHeight') statusBarHeight: number = 0;
  @StorageLink('bottomHeight') bottomHeight: number = 0;
  @State buttonName: Resource = $r('app.string.request_button_name');
  @State webVisibility: Visibility = Visibility.Hidden;
  @State webSrc: ResourceStr = CommonConstant.SERVER;
  controller: webview.WebviewController = new webview.WebviewController();

  build() {
    Column() {
      Row() {
        Image($r('app.media.ic_network_global'))
          .height($r('app.float.image_height'))
          .width($r('app.float.image_width'))
        TextInput({ placeholder: $r('app.string.input_address'), text: this.webSrc })
          .height($r('app.float.text_input_height'))
          .layoutWeight(1)
          .backgroundColor(Color.White)
          .onChange((value: string) => {
            this.webSrc = value;
          })
      }
      .margin({
        top: $r('app.float.default_margin'),
        left: $r('app.float.default_margin'),
        right: $r('app.float.default_margin')
      })
      .height($r('app.float.default_row_height'))
      .backgroundColor(Color.White)
      .borderRadius($r('app.float.border_radius'))
      .padding({
        left: $r('app.float.default_padding'),
        right: $r('app.float.default_padding')
      })

      Row() {
        Web({ src: this.webSrc, controller: this.controller })
          .zoomAccess(true)
          .height(StyleConstant.FULL_HEIGHT)
          .width(StyleConstant.FULL_WIDTH)
      }
      .visibility(this.webVisibility)
      .height(StyleConstant.WEB_HEIGHT)
      .width(StyleConstant.FULL_WIDTH)
      .align(Alignment.Top)

      Row() {
        Button(this.buttonName)
          .fontSize($r('app.float.button_font_size'))
          .width(StyleConstant.BUTTON_WIDTH)
          .height($r('app.float.button_height'))
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            this.onRequest();
          })
      }
      .height($r('app.float.default_row_height'))
    }
    .padding({
      top: this.statusBarHeight,
      bottom: this.bottomHeight
    })
    .width(StyleConstant.FULL_WIDTH)
    .height(StyleConstant.FULL_HEIGHT)
    .backgroundImage($r('app.media.ic_background_image', ImageRepeat.NoRepeat))
    .backgroundImageSize(ImageSize.Cover)
  }

  async onRequest() {
    if (this.webVisibility === Visibility.Hidden) {
      this.webVisibility = Visibility.Visible;
      try {
        let result = await httpGet(this.getResourceString(this.webSrc as Resource));
        if (result && result.responseCode === http.ResponseCode.OK) {
          this.controller.clearHistory();
          this.controller.loadUrl(this.webSrc);
        }
      } catch (error) {
        try {
          this.getUIContext().getPromptAction().showToast({ message: $r('app.string.http_response_error') })
        } catch (error) {
          let err = error as BusinessError;
          hilog.error(0x0000, 'WebPage', `showToast failed, error code=${err.code}, message=${err.message}`);
        }
      }
    } else {
      this.webVisibility = Visibility.Hidden;
    }
  }

  getResourceString(resource: Resource): string {
    let resourceString: string = '';
    try {
      resourceString = this.getUIContext().getHostContext()!.resourceManager.getStringSync(resource.id);
    } catch (err) {
      hilog.error(DOMAIN, TAG, FORMAT, `Failed to getResourceString. Cause code: ${err.code},message: ${err.message}`);
    }
    return resourceString;
  }
}