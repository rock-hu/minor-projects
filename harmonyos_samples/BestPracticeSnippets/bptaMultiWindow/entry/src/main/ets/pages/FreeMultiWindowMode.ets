/*
* 最佳实践: 一多窗口适配开发实践
*/
import { settings } from '@kit.BasicServicesKit';

// [Start FreeMultiWindowMode]
const context: UIContext | undefined = AppStorage.get('uiContext');
settings.getValue(context?.getHostContext(), 'window_pcmode_switch_status', settings.domainName.USER_PROPERTY).then((data) => {
  if (data) {
    console.info(`get settings value window_pcmode_switch_status:${JSON.stringify(data)}`);
    return;
  }
});
// [End FreeMultiWindowMode]