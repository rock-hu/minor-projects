import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import testNapi from 'libentry.so';
import webview from '@ohos.web.webview';

export default class EntryAbility extends UIAbility {
  // [Start set_scheme_handler]
  // EntryAbility.ets
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    // 注册三方协议的配置。
    testNapi.registerCustomSchemes();
    // 初始化Web组件内核，该操作会初始化Browser进程以及创建BrowserContext。
    webview.WebviewController.initializeWebEngine();
    // 设置SchemeHandler。
    testNapi.setSchemeHandler();
  }
  // [End set_scheme_handler]

  onDestroy(): void {

  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        return;
      }
    });
  }

  onWindowStageDestroy(): void {

  }

  onForeground(): void {

  }

  onBackground(): void {

  }
};