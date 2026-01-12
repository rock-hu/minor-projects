/*
 * Copyright 2025. Huawei Technologies Co., Ltd. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 */

// [Start sampleCode]
import { advertising, AutoAdComponent } from '@kit.AdsKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { AdsViewModel } from '../viewmodel/AdsViewModel';

const TAG: string = 'Ads Demo-BannerAdPage';

@ComponentV2
export struct BannerAdPage {
  @Local private visibilityState: Visibility = Visibility.Visible;
  private adRequestParams: advertising.AdRequestParams = { adId: '' };
  private ratio: number = -1;
  private viewModel: AdsViewModel = new AdsViewModel();

  aboutToAppear() {
    this.adRequestParams = this.viewModel.getParamsFromNav();
    if (this.adRequestParams.adWidth && this.adRequestParams.adHeight) {
      this.ratio = this.adRequestParams.adWidth / this.adRequestParams.adHeight;
    }
  }

  build() {
    NavDestination() {
      Stack({ alignContent: Alignment.Bottom }) {
        Image($r('app.media.banner_background'))
        AutoAdComponent({
          adParam: this.adRequestParams,
          adOptions: this.viewModel.adOptions,
          displayOptions: this.viewModel.adDisplayOptions,
          interactionListener: {
            onStatusChanged: (status: string, ad: advertising.Advertisement, data: string) => {
              switch (status) {
                case 'onAdOpen':
                  hilog.info(0x0000, TAG, 'Status is onAdOpen');
                  break;
                case 'onAdClick':
                  hilog.info(0x0000, TAG, 'Status is onAdClick');
                  break;
                case 'onAdClose':
                  hilog.info(0x0000, TAG, `Status is onAdClose, Close Reason is ${data}`);
                  if (data === 'adCloseBtnClicked') {
                    // 关闭原因为点击关闭按钮，可根据实际场景添加处理逻辑
                  }
                  this.visibilityState = Visibility.None;
                  break;
                case 'onAdLoad':
                  hilog.info(0x0000, TAG, 'Status is onAdLoad');
                  break;
                case 'onAdFail':
                  hilog.error(0x0000, TAG, 'Status is onAdFail');
                  this.visibilityState = Visibility.None;
                  break;
              }
            }
          }
        })
          .width('100%')
          .aspectRatio(this.ratio)
          .visibility(this.visibilityState)
      }
      .width('100%')
      .height('100%')
    }
    .hideTitleBar(true)
  }
}
// [End sampleCode]