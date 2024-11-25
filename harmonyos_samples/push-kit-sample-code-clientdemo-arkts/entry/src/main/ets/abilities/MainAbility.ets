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

import { UIAbility, Want } from '@kit.AbilityKit';
import { notificationManager } from '@kit.NotificationKit';
import { promptAction, window } from '@kit.ArkUI';
import { pushService } from '@kit.PushKit';
import Logger from '../utils/Logger';
import { ExtraData } from './Model';
import { BusinessError } from '@kit.BasicServicesKit';

export default class MainAbility extends UIAbility {
  private localStorage = new LocalStorage();

  onCreate(want: Want): void {
    Logger.info('MainAbility onCreate');
    this.localStorage.setOrCreate('want', want);
    this.receiveMessage();
  }

  onNewWant(want: Want): void {
    Logger.info('MainAbility onNewWant');
    this.localStorage.setOrCreate('want', want);
  }

  async onWindowStageCreate(windowStage: window.WindowStage): Promise<void> {
    Logger.info('MainAbility onWindowStageCreate');
    await windowStage.loadContent('pages/MainPage', this.localStorage);
    await this.requestEnableNotification();
  }

  /**
   * Request permission to send notification
   */
  private async requestEnableNotification(): Promise<void> {
    try {
      await notificationManager.requestEnableNotification(this.context);
      Logger.info('Succeeded in requesting enable notification');
    } catch (e) {
      const err: BusinessError = e;
      Logger.error('Failed to request enable notification: %{public}d %{public}s', err.code, err.message);
    }
  }

  /**
   * Receives scenario-specific messages from Push Kit
   */
  private receiveMessage(): void {
    try {
      pushService.receiveMessage('IM', this, (payload) => {
        try { // recommend to add try-catch block to capture exceptions
          Logger.info(`receiveMessage, type: ${payload.type}`);
          // get extraData passed by REST API
          const extraData: ExtraData = JSON.parse(JSON.parse(payload.data).data);

          // TODO: modify it based on your service scenario
          promptAction.showToast({
            message: $r('app.string.express_message', extraData.time, extraData.amount),
            duration: 5000,
          });
        } catch (e) {
          const err: BusinessError = e;
          Logger.error('Failed to process message: %{public}d %{public}s', err.code, err.message);
        }
      });
    } catch (e) {
      const err: BusinessError = e;
      Logger.error('Failed to receive message: %{public}d %{public}s', err.code, err.message);
    }
  }
}
