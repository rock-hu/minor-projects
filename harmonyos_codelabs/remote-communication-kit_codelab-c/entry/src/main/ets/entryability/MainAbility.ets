/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 */

import { Want } from '@kit.AbilityKit';
import { UIAbility } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

export default class MainAbility extends UIAbility {
  async onCreate(want: Want): Promise<void> {
    // Logger.info('MainAbility onCreate, parameters: ', want.parameters);
  }

  onNewWant(want: Want): void {
    // Logger.info('MainAbility onNewWant, parameters: ', want.parameters);
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    windowStage.loadContent('pages/MainPage');
  }
}
