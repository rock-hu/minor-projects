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

// [Start sampleCode]
import { abilityAccessCtrl, common, PermissionRequestResult } from '@kit.AbilityKit';
import { advertising, identifier } from '@kit.AdsKit';
import { AppStorageV2 } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { BannerAdPage } from './BannerAdPage';

const TAG: string = 'Ads Demo-Index';

@Entry
@ComponentV2
struct Index {
  private context: common.UIAbilityContext = this.getUIContext().getHostContext() as common.UIAbilityContext;
  private navPathStack: NavPathStack = AppStorageV2.connect(NavPathStack, () => new NavPathStack())!;
  private adRequestParams: advertising.AdRequestParams = {
    // 'testw6vs28auh3' is a dedicated test ad unit ID. Before releasing your app, replace the test ad unit ID with the formal one.
    adId: 'testw6vs28auh3',
    // Banner Ad.
    adType: 8,
    // Ad unit width, in vp.
    adWidth: 360,
    // Ad unit height, in vp.
    adHeight: 57
  };

  async aboutToAppear() {
    // OAID.
    this.adRequestParams.oaid = await requestOAID(this.context);
  }

  build() {
    Navigation(this.navPathStack) {
      Column() {
        Button($r('app.string.request_banner_ad_btn'))
          .fontSize(20)
          .fontWeight(FontWeight.Normal)
          .width('90%')
          .onClick(() => {
            this.navPathStack.pushPathByName('Banner', this.adRequestParams);
          })
      }
      .justifyContent(FlexAlign.Center)
      .height('100%')
      .width('100%')
    }
    .title($r('app.string.banner_ads_demo_title'))
    .titleMode(NavigationTitleMode.Mini)
    .mode(NavigationMode.Stack)
    .hideBackButton(true)
    .navDestination(this.pageMap)
  }

  @Builder
  pageMap(name: string) {
    if (name === 'Banner') {
      BannerAdPage()
    }
  }
}

async function requestOAID(context: Context): Promise<string | undefined> {
  let isPermissionGranted: boolean = false;
  try {
    const atManager: abilityAccessCtrl.AtManager = abilityAccessCtrl.createAtManager();
    const result: PermissionRequestResult =
      await atManager.requestPermissionsFromUser(context, ['ohos.permission.APP_TRACKING_CONSENT']);
    isPermissionGranted = result.authResults[0] === abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED;
  } catch (err) {
    hilog.error(0x0000, TAG, `Failed to request permission. Code is ${err.code}, message is ${err.message}`);
  }
  if (isPermissionGranted) {
    hilog.info(0x0000, TAG, 'Succeeded in requesting permission');
    try {
      const oaid = await identifier.getOAID();
      hilog.info(0x0000, TAG, 'Succeeded in getting OAID');
      return oaid;
    } catch (err) {
      hilog.error(0x0000, TAG, `Failed to get OAID. Code is ${err.code}, message is ${err.message}`);
    }
  } else {
    hilog.error(0x0000, TAG, 'Failed to request permission. User rejected');
  }
  return undefined;
}
// [End sampleCode]