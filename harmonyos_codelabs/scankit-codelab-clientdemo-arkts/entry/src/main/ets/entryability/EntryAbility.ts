import { router, window } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { Want, UIAbility, AbilityConstant } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

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
    windowStage.getMainWindow().then((windowObj: window.Window) => {
      windowObj.setWindowLayoutFullScreen(true).catch((error: BusinessError) => {
        if (error) {
          hilog.error(0x0000, TAG,
            `Failed to setWindowLayoutFullScreen. Code: ${error.code}, message: ${error.message}`);
          return;
        }
        hilog.info(0x0000, TAG, 'setWindowLayoutFullScreen Succeed!');
      })
    });

    windowStage.loadContent(this.page, (error, data) => {
      if (error.code) {
        hilog.error(0x0000, TAG, `Failed to load the content. Code: ${error.code}, message: ${error.message}`);
        return;
      }
      hilog.info(0x0000, TAG, 'Succeeded in loading the content.');
    });
  }

  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, TAG, 'Ability onNewWant');
    // Obtain the uri field from the want information.
    let uri = want?.uri;
    let status: router.RouterState = router.getState();
    if (status && status.name !== 'ScanAccess' && uri) {
      // Process the service based on the URI.
      router.pushUrl({
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
