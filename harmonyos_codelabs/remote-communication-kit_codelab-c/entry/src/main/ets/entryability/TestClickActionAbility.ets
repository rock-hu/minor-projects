/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2023. All rights reserved.
 */

import { Want } from '@kit.AbilityKit';
import { UIAbility } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';

export default class TestClickActionAbility extends UIAbility {
  onCreate(want: Want): void {
    // if (!CommonUtil.isNil(want.parameters)) {
    //   Logger.info('TestClickActionAbility onCreate, get click msg data: ', want.parameters);
    // } else {
    //   Logger.info('TestClickActionAbility onCreate.');
    // }
  }

  onNewWant(want: Want): void {
    // if (!CommonUtil.isNil(want.parameters)) {
    //   Logger.info('TestClickActionAbility onNewWant: ', want.parameters);
    // } else {
    //   Logger.info('TestClickActionAbility onNewWant.');
    // }
  }

  onDestroy(): void {
    // Logger.info('TestClickActionAbility onDestroy.');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    windowStage.loadContent('pages/ClickActionInnerPage');
  }
}
