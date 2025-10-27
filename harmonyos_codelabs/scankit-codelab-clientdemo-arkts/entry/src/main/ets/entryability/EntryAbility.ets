import { router, window } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { Want, UIAbility, AbilityConstant } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';
import { UIContextSelf } from '../utils/UIContextSelf';

const TAG: string = 'ScanKit EntryAbility';

export default class EntryAbility extends UIAbility {
  private page: string = 'pages/Index';

  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, TAG, 'Ability onCreate launchParam: %{public}s', launchParam);
    let uri = want?.uri;
    if (uri) {
      // Process the service based on the URI.
      this.page = 'pages/ScanAccess';
    }
  }

  onDestroy(): void {
    hilog.info(0x0000, TAG, 'Ability onDestroy');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    hilog.info(0x0000, TAG, 'Ability onWindowStageCreate');
    try {
      windowStage.getMainWindow().then((windowObj: window.Window) => {
        windowObj.setWindowLayoutFullScreen(true).catch((error: BusinessError) => {
          if (error) {
            hilog.error(0x0000, TAG,
              `Failed to setWindowLayoutFullScreen. Code: ${error.code}, message: ${error.message}`);
            return;
          }
          hilog.info(0x0000, TAG, 'setWindowLayoutFullScreen Succeed!');
        })

        try {
          windowStage.loadContent(this.page).then(() => {
            hilog.info(0x0000, TAG, 'Succeeded in loading the content.');
            UIContextSelf.setUIContext(windowObj);
          }).catch((error: BusinessError) => {
            hilog.error(0x0000, TAG, `Failed to load the content. Code: ${error.code}.`);
          })
        } catch (error) {
          hilog.error(0x0000, TAG, `Failed to load the content. Code: ${error.code}.`);
        }
      }).catch((error: BusinessError) => {
        hilog.error(0x0000, TAG, `Failed to getMainWindow. Code: ${error.code}.`);
      });
    } catch (error) {
      hilog.error(0x0000, TAG, `Failed to getMainWindow. Code: ${error.code}.`);
    }
  }

  onNewWant(want: Want, _: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, TAG, 'Ability onNewWant');
    // Obtain the uri field from the want information.
    let uri = want?.uri;
    let status: router.RouterState = UIContextSelf.getRouter().getState();
    if (status && status.name !== 'ScanAccess' && uri) {
      // Process the service based on the URI.
      UIContextSelf.pushUrl({
        url: 'pages/ScanAccess'
      });
    }
  }

  onWindowStageDestroy(): void {
    hilog.info(0x0000, TAG, 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    hilog.info(0x0000, TAG, 'Ability onForeground');
  }

  onBackground(): void {
    hilog.info(0x0000, TAG, 'Ability onBackground');
  }
}
