// [Start case_one]
import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { webview } from '@kit.ArkWeb';

export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam) {
    console.log('EntryAbility onCreate');
    webview.WebviewController.initializeWebEngine();
    // When pre-connecting, you need to replace' https://www.example.com' with the actual website address to visit
    // Specify that the second parameter is true, which means to pre-connect. If it is false, the interface will only pre-resolve the URL.
    webview.WebviewController.prepareForPageLoad('https://www.example.com/', true, 2);
    AppStorage.setOrCreate('abilityWant', want);
    console.log('EntryAbility onCreate done');
  }
}
// [End case_one]