import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { url } from '@kit.ArkTS';
import { BusinessError } from '@kit.BasicServicesKit';

const DOMAIN = 0x0000;
let uiContext: UIContext | null = null;

export default class EntryAbility extends UIAbility {

  onCreate(): void {
    AppStorage.setOrCreate('context', this.context);
    hilog.info(DOMAIN, 'testTag', '[EntryAbility] onCreate completed');
  }

  onDestroy(): void {
    hilog.info(DOMAIN, 'testTag', '[EntryAbility] onDestroy completed');
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    // Main window is created, set main page for this ability
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    windowStage.loadContent('pages/MainPage', (err) => {
      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      windowStage.getMainWindow((err: BusinessError, data) => {
        let windowClass = data;
        uiContext = windowClass.getUIContext();
        AppStorage.set('uiContext', uiContext);
      })

      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
  }

  onWindowStageDestroy(): void {
    // Main window is destroyed, release UI related resources
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
  }

  onForeground(): void {
    // Ability has brought to foreground
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // Ability has back to background
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }

  // [Start onNewWant]
  onNewWant(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(DOMAIN, 'testTag', 'Received URI:', want.uri);
    const uri = want.uri;
    if (uri) {
      const urlObj = url.URL.parseURL(uri);
      let pathname = urlObj.pathname;
      uiContext?.getRouter().pushUrl({ url: 'pages' + pathname });
    }
  }
  // [End onNewWant]
}