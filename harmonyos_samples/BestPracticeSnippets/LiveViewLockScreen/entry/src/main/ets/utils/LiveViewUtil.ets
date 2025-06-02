/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { application, Want, wantAgent } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
// [Start createLiveView]
import { liveViewManager } from '@kit.LiveViewKit';
// [StartExclude createLiveView]
import { RouteData } from '../model/RouteDataModel';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG: string = '[LiveViewUtil]';

export class LiveViewUtil {
  private context = application.getApplicationContext();
  private defaultLiveView?: liveViewManager.LiveView;
  private sequence: number = 0;

  public async startLiveView(params: RouteData) {
    // Verify that the live view switch is turned on
    if (!await LiveViewUtil.isLiveViewEnabled()) {
      hilog.error(0x0000, TAG, '%{public}s', 'Live view is disabled.');
      return;
    }
    this.defaultLiveView = await this.createPrimaryLiveVie();
    this.updateLiveViewInfo(params);

    return await liveViewManager.startLiveView(this.defaultLiveView);
  }

  public async updateLiveView(params: RouteData): Promise<boolean> {
    try {
      // live view is disabled or default view is not init
      if (!await LiveViewUtil.isLiveViewEnabled() || !this.defaultLiveView) {
        hilog.error(0x0000, TAG, '%{public}s', 'updateLiveView, live view is disabled.');
        return false;
      }
      this.updateLiveViewInfo(params);
      const result = await liveViewManager.updateLiveView(this.defaultLiveView);
      if (result) {
        hilog.info(0x0000, TAG, '%{public}s', 'updateLiveView.');
      }
      return true;
    } catch (error) {
      hilog.error(0x0000, TAG,
        `Failed to update live view. Cause code: ${(error as BusinessError).code}, message: ${(error as BusinessError).message}`);
      return false;
    }
  }

  // [Start closeLiveView]
  // Close live view.
  public async closeLiveView() {
    // Ensure that the sequence is greater than the current live window page.
    this.sequence++;
    this.defaultLiveView = await this.createPrimaryLiveVie();
    await liveViewManager.stopLiveView(this.defaultLiveView).then(() => {
      this.sequence = 0;
      this.defaultLiveView = undefined;
      hilog.info(0x0000, TAG, '%{public}s', 'Succeeded in stopping liveView, result: %{public}');
    }).catch((error: BusinessError) => {
      hilog.error(0x0000, TAG, '%{public}s',
        `Failed to stop liveView. Cause code: ${error.code}, message: ${error.message}`);
    });
    return;
  }
  // [End closeLiveView]

  private static async isLiveViewEnabled(): Promise<boolean> {
    return await liveViewManager.isLiveViewEnabled();
  }

  private updateLiveViewInfo(params: RouteData) {
    let liveView: liveViewManager.LiveView = this.defaultLiveView!;
    if (params) {
      liveView.sequence = this.sequence
      this.sequence++;
      liveView.liveViewData.capsule = {
        type: liveViewManager.CapsuleType.CAPSULE_TYPE_TEXT,
        status: 1,
        icon: 'turn_right_light_square.png',
        backgroundColor: this.context.resourceManager.getStringSync($r('app.string.live_view_background').id),
        title: `${params.distance}${this.context.resourceManager.getStringSync($r('app.string.meter').id)}`,
      }
      liveView.liveViewData.primary.title =
        `${this.context.resourceManager.getStringSync($r('app.string.live_view_primary_title_first')
          .id)}${params.distance}${this.context.resourceManager.getStringSync($r('app.string.live_view_primary_title_second')
          .id)}`;
      liveView.liveViewData.primary.content = [{ text: this.context.resourceManager.getStringSync(params.roadName) }];
      liveView.liveViewData.primary.liveViewLockScreenAbilityName = 'LiveViewExtAbility';
      liveView.liveViewData.primary.liveViewLockScreenAbilityParameters = { '': '' };
      if (params.roadInfo.laneNum === 1) {
        liveView.liveViewData.primary.layoutData = {
          layoutType: liveViewManager.LayoutType.LAYOUT_TYPE_NAVIGATION,
          currentNavigationIcon: 'turn_straight_light_rectangle.png',
          navigationIcons: ['turn_straight_light_rectangle.png', 'turn_right_dark_rectangle.png']
        };
      } else {
        liveView.liveViewData.primary.layoutData = {
          layoutType: liveViewManager.LayoutType.LAYOUT_TYPE_NAVIGATION,
          currentNavigationIcon: 'turn_right_light_rectangle.png',
          navigationIcons: ['turn_straight_dark_rectangle.png', 'turn_right_light_rectangle.png']
        }
      }
    }
    this.defaultLiveView = liveView;
  }

  private async createPrimaryLiveVie(): Promise<liveViewManager.LiveView> {
    // [EndExclude createLiveView]
    // Construct live window request body.
    let liveView: liveViewManager.LiveView = {
      id: 0,
      sequence: this.sequence,
      // Application scenarios of the live window. NAVIGATION: Navigation.
      event: 'NAVIGATION',
      liveViewData: {
        // Live view capsule related parameters
        capsule: {
          type: liveViewManager.CapsuleType.CAPSULE_TYPE_TEXT,
          status: 1,
          icon: 'turn_right_light_square.png',
          backgroundColor: this.context.resourceManager.getStringSync($r('app.string.live_view_background').id),
          title: this.context.resourceManager.getStringSync($r('app.string.live_view_title').id),
        },
        // Live view card related parameters
        primary: {
          title: this.context.resourceManager.getStringSync($r('app.string.live_view_title').id),
          content: [{ text: this.context.resourceManager.getStringSync($r('app.string.live_view_content').id) }],
          // Add LiveViewLockScreenExtensionAbility name to build lock screen live view
          liveViewLockScreenAbilityName: 'LiveViewExtAbility',
          liveViewLockScreenAbilityParameters: { liveViewParameters: '' },
          keepTime: 0,
          clickAction: await LiveViewUtil.buildWantAgent()
        }
      }
    };
    // [End createLiveView]
    return liveView;
  }

  private static async buildWantAgent(): Promise<Want> {
    const wantAgentInfo: wantAgent.WantAgentInfo = {
      wants: [{
        bundleName: 'com.example.mapliveviewsample',
        abilityName: 'EntryAbility'
      } as Want],
      actionType: wantAgent.OperationType.START_ABILITIES,
      requestCode: 0,
      actionFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG]
    };
    const agent: object = await wantAgent.getWantAgent(wantAgentInfo);
    return agent;
  }
}