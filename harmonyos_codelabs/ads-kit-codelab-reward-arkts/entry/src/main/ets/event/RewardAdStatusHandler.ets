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

import { commonEventManager, BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const KEY_REWARD_DATA = 'reward_ad_data';
const KEY_REWARD_STATUS = 'reward_ad_status';
const TAG = 'Ads Demo-RewardAdStatusHandler';

export class RewardAdStatusHandler {
  // Used to save the created subscriber object for subsequent subscription and unsubscription.
  private subscriber: commonEventManager.CommonEventSubscriber | null = null;

  public registerPPSReceiver(): void {
    if (this.subscriber) {
      this.unRegisterPPSReceiver();
    }
    // Subscriber information.
    const subscribeInfo: commonEventManager.CommonEventSubscribeInfo = {
      events: ['com.huawei.hms.pps.action.PPS_REWARD_STATUS_CHANGED'],
      // Setting publisherBundleName to com.huawei.hms.adsservice ensures that the subscriber only accepts and processes events from this specific package.
      // If publisherBundleName is not explicitly declared, the subscriber may receive forged events from other package names, resulting in security issues or misleading.
      publisherBundleName: 'com.huawei.hms.adsservice'
    };
    // Callback for subscriber creation.
    commonEventManager.createSubscriber(subscribeInfo,
      (err: BusinessError, commonEventSubscriber: commonEventManager.CommonEventSubscriber) => {
        if (err) {
          hilog.error(0x0000, TAG, '%{public}s', `createSubscriber error, code: ${err.code}, message: ${err.message}`);
          return;
        }
        hilog.info(0x0000, TAG, '%{public}s', 'succeeded in creating subscriber');
        this.subscriber = commonEventSubscriber;
        // Callback for common event subscription.
        if (!this.subscriber) {
          hilog.info(0x0000, TAG, '%{public}s', 'need create subscriber');
          return;
        }
        commonEventManager.subscribe(this.subscriber,
          (err: BusinessError, commonEventSubscriber: commonEventManager.CommonEventData) => {
            if (err) {
              hilog.error(0x0000, TAG, '%{public}s', `subscribe error, code: ${err.code}, message: ${err.message}`);
            } else {
              hilog.info(0x0000, TAG, '%{public}s', 'subscribe data');
              const status: string = commonEventSubscriber?.parameters?.[KEY_REWARD_STATUS];
              switch (status) {
                case AdStatus.AD_OPEN:
                  hilog.info(0x0000, TAG, '%{public}s', 'onAdOpen');
                  break;
                case AdStatus.AD_CLICKED:
                  hilog.info(0x0000, TAG, '%{public}s', 'onAdClick');
                  break;
                case AdStatus.AD_CLOSED:
                  hilog.info(0x0000, TAG, '%{public}s', 'onAdClose');
                  this.unRegisterPPSReceiver();
                  break;
                case AdStatus.AD_REWARDED:
                  const rewardData: Record<string, string | number> =
                    commonEventSubscriber?.parameters?.[KEY_REWARD_DATA];
                  const rewardType: string = rewardData?.rewardType as string;
                  const rewardAmount: number = rewardData?.rewardAmount as number;
                  hilog.info(0x0000, TAG, '%{public}s',
                    `onAdReward, rewardType: ${rewardType}, rewardAmount: ${rewardAmount}`);
                  break;
                case AdStatus.AD_VIDEO_START:
                  hilog.info(0x0000, TAG, '%{public}s', 'onAdVideoStart');
                  break;
                case AdStatus.AD_COMPLETED:
                  hilog.info(0x0000, TAG, '%{public}s', 'onAdCompleted');
                  break;
                default:
                  break;
              }
            }
          });
      });
  }

  public unRegisterPPSReceiver(): void {
    commonEventManager.unsubscribe(this.subscriber, (err: BusinessError) => {
      if (err) {
        hilog.error(0x0000, TAG, '%{public}s', `unsubscribe error, code: ${err.code}, message: ${err.message}`);
      } else {
        hilog.info(0x0000, TAG, '%{public}s', 'succeeded in unsubscribing');
        this.subscriber = null;
      }
    });
  }
}

enum AdStatus {
  AD_OPEN = 'onAdOpen',

  AD_CLICKED = 'onAdClick',

  AD_CLOSED = 'onAdClose',

  AD_REWARDED = 'onAdReward',

  AD_VIDEO_START = 'onVideoPlayBegin',

  AD_COMPLETED = 'onVideoPlayEnd'
}
