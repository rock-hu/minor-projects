import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import Logger from '../utils/Logger';

let logger = Logger.getLogger('[ShareUIAbility]');

interface Storage {
  ShareUIAbilityWant: Want
}

export default class ShareUIAbility extends UIAbility {
  private storageWant: Want = {}

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    logger.info('launchReason: ', String(launchParam.launchReason));
    this.storageWant = want;
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    logger.info('launchReason: ', String(launchParam.launchReason));
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    let para: Storage = { ShareUIAbilityWant: this.storageWant };
    let storage = new LocalStorage(para);
    windowStage.loadContent('pages/ShareUIPage', storage);
  }
}
