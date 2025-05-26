/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { common, wantAgent } from '@kit.AbilityKit';
import { avSession } from '@kit.AVSessionKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { VideoData } from '../model/VideoData';
import Logger from '../utils/Logger';
import { ImageUtil } from '../utils/ImageUtil';
import { BackgroundTaskManager } from '../utils/BackgroundTaskManager';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG = 'AvSessionController';

export class AvSessionController {
  private static instance: AvSessionController | null;
  private context: common.UIAbilityContext | undefined = undefined;
  private avSession: avSession.AVSession | undefined = undefined;
  private avSessionMetadata: avSession.AVMetadata | undefined = undefined;

  constructor() {
    this.initAvSession();
  }

  public static getInstance(): AvSessionController {
    if (!AvSessionController.instance) {
      AvSessionController.instance = new AvSessionController();
    }
    return AvSessionController.instance;
  }

  // [Start init_session]
  public initAvSession() {
    this.context = AppStorage.get('context');
    // [StartExclude init_session]
    if (!this.context) {
      Logger.info(TAG, `session create failed : context is undefined`);
      return;
    }
    // [EndExclude init_session]
    avSession.createAVSession(this.context, "SHORT_AUDIO_SESSION", 'video').then(async (avSession) => {
      this.avSession = avSession;
      hilog.info(0x0001, TAG, `session create successed : sessionId : ${this.avSession.sessionId}`);
      // Apply for background long-term tasks
      BackgroundTaskManager.startContinuousTask(this.context);
      this.setLaunchAbility();
      this.avSession.activate();
    });
  }
  // [End init_session]

  public getAvSession() {
    return this.avSession;
  }

  public getAvSessionMetadata() {
    return this.avSessionMetadata;
  }

  // [Start meta_data]
  public async setAVMetadata(curSource: VideoData, duration: number, uiContext: UIContext) {
    // [StartExclude meta_data]
    if (curSource === undefined) {
      Logger.error(TAG, 'SetAVMetadata Error, curSource is null');
      return;
    }
    // [EndExclude meta_data]
    const imagePixMap = await ImageUtil.getPixmapFromMedia(curSource.head, uiContext);
    let metadata: avSession.AVMetadata = {
      assetId: `${curSource.index}`, // Media ID
      title: this.context?.resourceManager.getStringSync(curSource.name), // title
      mediaImage: imagePixMap, // Pixel data or picture path address of a picture.
      duration: duration // Media duration, in ms
    };
    if (this.avSession) {
      this.avSession.setAVMetadata(metadata).then(() => { // Call the set session metadata interface
        this.avSessionMetadata = metadata;
        hilog.info(0x0001, TAG, "SetAVMetadata successfully");
      }).catch((err: BusinessError) => {
        hilog.error(0x0001, TAG, `SetAVMetadata BusinessError: code: ${err.code}, message: ${err.message}`);
      });
    }
  }
  // [End meta_data]

  // [Start launch]
  private setLaunchAbility() {
    // [StartExclude launch]
    if (!this.context) {
      return;
    }
    // [EndExclude launch]
    const wantAgentInfo: wantAgent.WantAgentInfo = {
      wants: [
        {
          bundleName: this.context.abilityInfo.bundleName, // Name of the application Bundle where the Ability to be started is located.
          abilityName: this.context.abilityInfo.name // Ability name to be started
        }
      ],
      operationType: wantAgent.OperationType.START_ABILITIES, // Action type, START_ABILITY indicates the Ability to open multiple pages.
      requestCode: 0, // A user-defined private value.
      wantAgentFlags: [wantAgent.WantAgentFlags.UPDATE_PRESENT_FLAG] // Action execution attribute, UPDATE_PRESENT_FLAG means replacing the extra data in the existing WantAgent with the extra data of the new WantAgent.
    };
    wantAgent.getWantAgent(wantAgentInfo).then((agent) => {
      if (this.avSession) {
        this.avSession.setLaunchAbility(agent);
      }
    });
  }
  // [End launch]

  // [Start update_is_play]
  public setAvSessionPlayState(playbackState: avSession.AVPlaybackState) {
    if (this.avSession) {
      this.avSession.setAVPlaybackState(playbackState, (err: BusinessError) => {
        if (err) {
          hilog.error(0x0001, TAG, `SetAVPlaybackState BusinessError: code: ${err.code}, message: ${err.message}`);
        } else {
          hilog.info(0x0001, TAG, "SetAVPlaybackState successfully");
        }
      });
    }
  }
  // [End update_is_play]

  // [Start init_session]
  async unregisterSessionListener() {
    // [StartExclude init_session]
    if (!this.avSession) {
      return;
    }
    this.avSession.off('play');
    this.avSession.off('pause');
    this.avSession.off('playNext');
    this.avSession.off('playPrevious');
    this.avSession.off('setLoopMode');
    this.avSession.off('seek');
    this.avSession.off('toggleFavorite');
    // [EndExclude init_session]
    // Destroy background long-term tasks
    BackgroundTaskManager.stopContinuousTask(this.context);
  }
  // [End init_session]
}