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

import { advertising, identifier } from '@kit.AdsKit';
import { Prompt } from '@kit.ArkUI';
import { abilityAccessCtrl, common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { InterstitialAdStatusHandler } from '../event/InterstitialAdStatusHandler';

const TAG = 'Ads Demo-LoadAd';

/**
 * Interstitial ad page.
 */
@Entry
@Component
export struct LoadAd {
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  private oaid: string = '';
  // Ad display parameters.
  private adDisplayOptions: advertising.AdDisplayOptions = {
    // Whether to mute the ad. By default, the ad is not muted.
    mute: false
  }
  // Ad configuration.
  private adOptions: advertising.AdOptions = {
    // Whether to allow ad asset download through mobile data. 0: no; 1: yes. If this parameter is not set, the advertiser's setting will be used.
    allowMobileTraffic: 0,
    // Whether you want your ad content to be treated as child-directed according to the regulations of COPPA. -1 (default value): uncertain; 0: no; 1: yes.
    tagForChildProtection: -1,
    // Whether you want the ad request to be processed in a way that is suitable for users in the European Economic Area (EEA) who are below the legal age of consent. -1 (default value): uncertain; 0: no; 1: yes.
    tagForUnderAgeOfPromise: -1,
    // Maximum ad content rating. W: aged 3 and up; PI: aged 7 and up, under parental guidance; J: teenagers aged 12 and up; A: adults aged 16 or 18 and up.
    adContentClassification: 'A'
  }
  // Interstitial video ad request parameters.
  private interstitialVideoAdReqParams: advertising.AdRequestParams = {
    adId: 'testb4znbuh3n2',
    adType: AdType.INTERSTITIAL_AD,
    adCount: 1
  }
  // Interstitial image ad request parameters.
  private interstitialImageAdReqParams: advertising.AdRequestParams = {
    adId: 'teste9ih9j0rc3',
    adType: AdType.INTERSTITIAL_AD,
    adCount: 1
  }

  aboutToAppear() {
    try {
      // Obtain the OAID through a promise callback.
      this.requestOAIDTrackingConsentPermissions(this.context);
    } catch (error) {
      hilog.error(0x0000, TAG, '%{public}s', `catch err, code:${error.code}, message: ${error.message}`);

    }
    hilog.info(0x0000, TAG, '%{public}s', 'aboutToAppear');
  }

  build() {
    Column() {
      this.actionBar($r('app.string.interstitial_ads_demo_title'))
      Column() {
        // Redirect to the interstitial video ad display page.
        this.customButton($r('app.string.request_interstitial_video_ad_btn'), () => {
          this.requestAd(this.interstitialVideoAdReqParams, this.adOptions);
        });

        // Redirect to the interstitial image ad display page.
        this.customButton($r('app.string.request_interstitial_image_ad_btn'), () => {
          this.requestAd(this.interstitialImageAdReqParams, this.adOptions);
        });

      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)
    }
    .width('100%')
    .height('100%')
  }

  private requestAd(adReqParams: advertising.AdRequestParams, adOptions: advertising.AdOptions): void {
    adReqParams.oaid = this.oaid;
    // Ad request callback listener.
    const adLoaderListener: advertising.AdLoadListener = {
      // Called when an ad request fails.
      onAdLoadFailure: (errorCode: number, errorMsg: string) => {
        hilog.error(0x0000, TAG, '%{public}s', `request ad errorCode is: ${errorCode}, errorMsg is: ${errorMsg}`);
        Prompt.showToast({
          message: 'request ad failed, code is: ' + errorCode + 'message is: ' + errorMsg,
          duration: 1000
        });
      },
      // Called upon ad request success.
      onAdLoadSuccess: (ads: Array<advertising.Advertisement>) => {
        hilog.info(0x0000, TAG, '%{public}s', 'succeeded in requesting ad!');

        // Save the requested ad content for display.
        hilog.info(0x0000, TAG, '%{public}s', `ads[0].adType is : ${ads[0].adType}`);
        // Register a status listener for interstitial ads to monitor their playback status.
        new InterstitialAdStatusHandler().registerPPSReceiver();
        // Call the API for displaying ads.
        advertising.showAd(ads[0], this.adDisplayOptions, this.context);
      }
    };
    // Create an AdLoader ad object.
    const load: advertising.AdLoader = new advertising.AdLoader(this.context);
    // Call the API for requesting ads.
    hilog.info(0x0000, TAG, '%{public}s', 'request ad!');
    load.loadAd(adReqParams, adOptions, adLoaderListener);
  }

  private requestOAIDTrackingConsentPermissions(context: common.Context): void {
    // Request Activity tracking for this app from users when they access a page.
    const atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    try {
      atManager.requestPermissionsFromUser(context, ['ohos.permission.APP_TRACKING_CONSENT']).then((data) => {
        if (data.authResults[0] === 0) {
          hilog.info(0x0000, TAG, '%{public}s', 'Succeeded in requesting permission');
          identifier.getOAID().then((data: string) => {
            this.oaid = data;
            hilog.info(0x0000, TAG, '%{public}s', `Succeeded in getting adsIdentifierInfo by promise`);
          }).catch((error: BusinessError) => {
            hilog.error(0x0000, TAG, '%{public}s', `Failed to get AdsIdentifierInfo , message: ${error.message}`);
          })
        } else {
          hilog.info(0x0000, TAG, '%{public}s', 'user rejected');
        }
      }).catch((err: BusinessError) => {
        hilog.error(0x0000, TAG, '%{public}s', `request permission failed, error: ${err.code} ${err.message}`);
      })
    } catch (err) {
      hilog.error(0x0000, TAG, '%{public}s', `catch err, code:${err.code}, message: ${err.message}`);
    }
  }

  @Builder
  actionBar(title: Resource | string) {
    Row() {
      if (title) {
        Text(title)
          .fontSize(24)
          .fontColor(Color.White)
          .fontWeight(FontWeight.Medium)
          .margin({ left: 16 })
      }
    }
    .width('100%')
    .height('10%')
    .padding({
      top: 16,
      bottom: 16,
      left: 24,
      right: 24
    })
    .backgroundColor(Color.Black)
  }

  @Builder
  customButton(mText: string | Resource, mOnClick: (event?: ClickEvent) => void) {
    Column() {
      Button(mText)
        .backgroundColor('#d3d4d6')
        .fontSize(20)
        .fontColor('#000')
        .fontWeight(FontWeight.Normal)
        .align(Alignment.Center)
        .type(ButtonType.Capsule)
        .width('90%')
        .height(40)
        .margin({ top: 10, bottom: 5 })
        .onClick(mOnClick);
    }
  }
}

enum AdType {
  // Interstitial ad.
  INTERSTITIAL_AD = 12
}
