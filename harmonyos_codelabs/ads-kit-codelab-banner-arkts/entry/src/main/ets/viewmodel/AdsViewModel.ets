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
import { advertising } from '@kit.AdsKit';
import { AppStorageV2 } from '@kit.ArkUI';

export class AdsViewModel {
  // Ad configuration.
  adOptions: advertising.AdOptions = {
    // Whether you want your ad content to be treated as child-directed according to the regulations of COPPA.
    tagForChildProtection: -1,
    // Maximum ad content rating.
    adContentClassification: 'A',
    // Whether to request non-personalized ads.
    nonPersonalizedAd: 0,
    // Whether to allow ad asset download through mobile data.
    allowMobileTraffic: 0,
    // Whether you want the ad request to be processed in a way that is suitable for users in the European Economic Area (EEA) who are below the legal age of consent.
    tagForUnderAgeOfPromise: -1
  };

  // Ad display parameters.
  adDisplayOptions: advertising.AdDisplayOptions = {
    // Interval for displaying an ad in carousel mode, in ms.
    refreshTime: 30000
  };

  private navPathStack: NavPathStack;

  constructor() {
    this.navPathStack = AppStorageV2.connect(NavPathStack)!;
  }

  getParamsFromNav(): advertising.AdRequestParams {
    return this.navPathStack.getParamByName('Banner')[0] as advertising.AdRequestParams;
  }
}
// [End sampleCode]