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
import { AdComponent, advertising } from '@kit.AdsKit';
import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { AdsViewModel } from '../viewmodel/AdsViewModel';

const TAG: string = 'Ads Demo-SplashAdPage';

@Entry
@ComponentV2
struct SplashAdPage {
  private viewModel: AdsViewModel = new AdsViewModel(this.getUIContext());

  aboutToAppear() {
    window.getLastWindow(this.getUIContext().getHostContext(), (err: BusinessError, win: window.Window) => {
      // Enable the full-screen immersive mode.
      win.setWindowLayoutFullScreen(true);
      // Set the screen to portrait.
      win.setPreferredOrientation(window.Orientation.PORTRAIT);
    });
    const adRequestParams: advertising.AdRequestParams = this.viewModel.getParamsFromRouter();
    this.viewModel.loadAd(adRequestParams);
  }

  aboutToDisappear() {
    window.getLastWindow(this.getUIContext().getHostContext(), (err: BusinessError, win: window.Window) => {
      // Disable the full-screen mode. Set the mode based on your needs.
      win.setWindowLayoutFullScreen(false);
      // Set screen orientation to the default value. You can change the value based on the actual situation.
      win.setPreferredOrientation(window.Orientation.UNSPECIFIED);
    });
  }

  build() {
    RelativeContainer() {
      // Slogan image.
      Image($r('app.media.slogan'))
        .width('100%')
        .height('100%')
        .zIndex(0)
      // Display the custom icon, app name, and copyright information.
      Column() {
        Row() {
          Image($r('app.media.video'))
            .width(24)
            .height(24)
            .margin(8)
          Text($r('app.string.video'))
            .fontColor($r('app.color.splash_text_font_color'))
            .fontSize(16)
        }
        .margin({ bottom: 8 })

        Column() {
          Text($r('app.string.copyright'))
            .fontColor($r('app.color.splash_text_font_color'))
            .fontSize(9)
        }
      }
      .zIndex(1)
      .alignRules({ bottom: { anchor: '__container__', align: VerticalAlign.Bottom } })
      .width('100%')
      .height('13%')

      if (this.viewModel.ads.length !== 0) {
        if (this.viewModel.ads[0]?.isFullScreen) {
          this.splashFullScreen()
        } else {
          this.splashHalfScreen()
        }
      }
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  private splashHalfScreen() {
    AdComponent({
      ads: [this.viewModel.ads[0]],
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
              hilog.info(0x0000, TAG, 'Status is onAdClose');
              this.viewModel.routeToHome();
              break;
          }
        }
      }
    })
      .zIndex(1)
      .width('100%')
      .height('87%')
      // Custom component animation.
      .transition(TransitionEffect.OPACITY.animation({ duration: 1000, curve: Curve.Friction }))
      .alignRules({ top: { anchor: '__container__', align: VerticalAlign.Top } })
  }

  @Builder
  private splashFullScreen() {
    AdComponent({
      ads: [this.viewModel.ads[0]],
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
              hilog.info(0x0000, TAG, 'Status is onAdClose');
              this.viewModel.routeToHome();
              break;
          }
        }
      }
    })
      .zIndex(1)
      .width('100%')
      .height('100%')
  }
}
// [End sampleCode]