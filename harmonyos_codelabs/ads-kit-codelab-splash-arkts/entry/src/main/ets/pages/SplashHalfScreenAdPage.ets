/*
 * Copyright 2023. Huawei Technologies Co., Ltd. All rights reserved.

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

import { router, Prompt } from '@kit.ArkUI';
import { advertising, AdComponent } from '@kit.AdsKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG = 'Ads Demo-SplashHalfScreenAdPage';

@Entry
@Component
export struct SplashHalfScreenAdPage {
  private ads: Array<advertising.Advertisement> = [];
  private displayOptions?: advertising.AdDisplayOptions;

  aboutToAppear() {
    hilog.info(0x0000, TAG, '%{public}s', 'SplashHalfScreenAdPage aboutToAppear');
    const params: Record<string, Object> = router.getParams() as Record<string, Object>;
    if (params && params.ads as Array<advertising.Advertisement> &&
      params.displayOptions as advertising.AdDisplayOptions) {
      this.ads = params.ads as Array<advertising.Advertisement>;
      this.displayOptions = params.displayOptions as advertising.AdDisplayOptions;
    } else {
      hilog.info(0x0000, TAG, '%{public}s', 'empty router params');
    }
  }

  build() {
    Column() {
      AdComponent({
        ads: this.ads, displayOptions: this.displayOptions,
        interactionListener: {
          onStatusChanged: (status: string, ad: advertising.Advertisement, data: string) => {
            switch (status) {
              case AdStatus.AD_OPEN:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdOpen');
                Prompt.showToast({
                  message: 'splash ad open',
                  duration: 1000
                });
                break;
              case AdStatus.AD_CLICKED:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdClick');

                Prompt.showToast({
                  message: 'splash ad click',
                  duration: 1000
                });
                break;
              case AdStatus.AD_CLOSED:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdClose');
                Prompt.showToast({
                  message: 'splash ad close',
                  duration: 1000
                });
                router.replaceUrl({
                  url: 'pages/AdsServicePage'
                })
                break;
            }
          }
        }
      })
        .width('100%')
        .height('87%')

      Column({}) {
        Row() {
          Image($r('app.media.video'))
            .width(24)
            .height(24)
            .margin({ right: 8 })
          Text($r('app.string.video'))
            .fontColor($r('app.color.half_screen_splash_text_color'))
            .fontSize(16)
            .width(50)
            .height(21)
        }
        .margin({ bottom: 8 })

        Column() {
          Text($r('app.string.copyright'))
            .fontColor($r('app.color.half_screen_splash_text_color'))
            .fontSize(9)
            .height(15)
        }
      }
      .margin({
        top: 29,
        left: 16,
        right: 16
      })
    }
    .width('100%')
    .height('100%')
  }
}

enum AdStatus {
  AD_OPEN = "onAdOpen",

  AD_CLICKED = "onAdClick",

  AD_CLOSED = "onAdClose"
}