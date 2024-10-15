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

import { advertising, AutoAdComponent, identifier } from '@kit.AdsKit';
import { common, abilityAccessCtrl } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG = 'Ads Demo-LoadAd';

/**
 * Banner ad page.
 */
@Entry
@Component
export struct LoadAd {
  private context: common.UIAbilityContext = getContext(this) as common.UIAbilityContext;
  // Ad display parameters.
  private adDisplayOptions: advertising.AdDisplayOptions = {
    // Interval for displaying an ad in carousel mode, in ms. The value ranges from 30000 to 120000.
    refreshTime: 30000
  };
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
  };
  // Banner ad request parameters.
  @State bannerAdReqParams: advertising.AdRequestParams = {
    adId: 'testw6vs28auh3',
    adType: AdType.BANNER_AD,
    adCount: 1,
    adWidth: 360,
    adHeight: 57
  };
  // Banner ad asset aspect ratio.
  private ratio: number = 1;
  @State visibilityState: Visibility = Visibility.None;

  aboutToAppear() {
    try {
      // Request user authorization.
      this.requestOAIDTrackingConsentPermissions(this.context);
      this.initData();
    } catch (err) {
      hilog.error(0x0000, TAG, '%{public}s', `catch err, code: ${err.code}, message: ${err.message}`);
    }
    hilog.info(0x0000, TAG, '%{public}s', 'aboutToAppear');
  }

  build() {
    Stack({ alignContent: Alignment.Top }) {
      this.actionBar($r('app.string.banner_ads_demo_title'))
      Column() {
        this.customButton($r('app.string.request_banner_ad_btn'), () => {
          this.visibilityState = Visibility.Visible;
        })
      }
      .width('100%')
      .height('80%')
      .justifyContent(FlexAlign.Center)

      Stack({ alignContent: Alignment.Bottom }) {
        this.buildImageView();
        this.buildBannerView();
      }
      .visibility(this.visibilityState)
    }
    .width('100%')
    .height('100%')
  }

  private requestOAIDTrackingConsentPermissions(context: common.Context): void {
    // Request Activity tracking for this app from users when they access a page.
    const atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    try {
      atManager.requestPermissionsFromUser(context, ['ohos.permission.APP_TRACKING_CONSENT']).then((data) => {
        if (data.authResults[0] === 0) {
          hilog.info(0x0000, TAG, '%{public}s', 'Succeeded in requesting permission');
          identifier.getOAID().then((data: string) => {
            this.bannerAdReqParams.oaid = data;
            hilog.info(0x0000, TAG, '%{public}s', `Succeeded in getting adsIdentifierInfo by promise`);
          }).catch((error: BusinessError) => {
            hilog.error(0x0000, TAG, '%{public}s', `getAdsIdentifierInfo failed, message: ${error.message}`);
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

  private initData() {
    const adWidth: number = this.bannerAdReqParams.adWidth as number;
    const adHeight: number = this.bannerAdReqParams.adHeight as number;
    if (adWidth > 0 && adHeight > 0) {
      this.ratio = adWidth / adHeight;
    }
  }

  @Builder
  buildImageView() {
    Image($r('app.media.banner_background'))
      .objectFit(ImageFit.Cover)
      .onError(() => {
        hilog.error(0x0000, TAG, '%{public}s', 'buildImageView image load error');
      })
      .onComplete(() => {
        hilog.info(0x0000, TAG, '%{public}s', 'buildImageView image load complete');
      })
  }

  @Builder
  buildBannerView() {
    Row() {
      AutoAdComponent({
        adParam: this.bannerAdReqParams,
        adOptions: this.adOptions,
        displayOptions: this.adDisplayOptions,
        interactionListener: {
          onStatusChanged: (status: string, ad: advertising.Advertisement, data: string) => {
            hilog.info(0x0000, TAG, '%{public}s', `status is ${status}`);
            switch (status) {
              case AdStatus.AD_OPEN:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdOpen');
                break;
              case AdStatus.AD_CLICKED:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdClick');
                break;
              case AdStatus.AD_CLOSED:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdClose');
                this.visibilityState = Visibility.None;
                break;
              case AdStatus.AD_LOAD:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdLoad');
                break;
              case AdStatus.AD_FAIL:
                hilog.info(0x0000, TAG, '%{public}s', 'onAdFail');
                this.visibilityState = Visibility.None;
                break;
            }
          }
        }
      })
    }
    .width('100%')
    .aspectRatio(this.ratio)
    .padding({ bottom: 5 })
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
        .onClick(mOnClick)
    }
  }
}

enum AdType {
  // Banner ad.
  BANNER_AD = 8
}

enum AdStatus {
  AD_LOAD = 'onAdLoad',

  AD_FAIL = 'onAdFail',

  AD_OPEN = 'onAdOpen',

  AD_CLICKED = 'onAdClick',

  AD_CLOSED = 'onAdClose'
}
