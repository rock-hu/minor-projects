import { display, window } from '@kit.ArkUI'
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit';

@Entry
@Component
export struct Movie {
  @State isFoldable: Boolean = false
  @State foldStatus: display.FoldStatus = 1
  private mainWindow: window.Window | undefined;
  private context: common.UIAbilityContext = this.getUIContext().getHostContext() as common.UIAbilityContext;
  private onDetailFoldStatusChange: Callback<display.FoldStatus> = (data: display.FoldStatus) => {
    if (data === display.FoldStatus.FOLD_STATUS_FOLDED) {
      // 当前折叠状态为折叠态，设置窗口横屏显示
      this.mainWindow?.setPreferredOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE);
    } else if (data === display.FoldStatus.FOLD_STATUS_EXPANDED) {
      // 如果折叠状态为展开态，设置窗口跟随传感器自动旋转，且受控制中心的旋转开关控制
      this.mainWindow?.setPreferredOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
    }
  };

  // 如果使用Navigation路由跳转，需要在NavDestination的onShown()和onHidden()事件中注册监听
  onPageShow(): void {
    // 获取应用主窗口
    let promise = window.getLastWindow(this.context);
    promise.then((data: window.Window) => {
      this.mainWindow = data;
      console.info('Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
      let foldStatus: display.FoldStatus = display.getFoldStatus();
      if (foldStatus === display.FoldStatus.FOLD_STATUS_FOLDED) {
        // 当前折叠状态为折叠态，设置窗口横屏显示
        this.mainWindow?.setPreferredOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE);
      } else if (foldStatus === display.FoldStatus.FOLD_STATUS_EXPANDED) {
        // 如果折叠状态为展开态，设置窗口跟随传感器自动旋转，且受控制中心的旋转开关控制
        this.mainWindow?.setPreferredOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
      }
      // 监听折叠屏折叠状态
      display.on('foldStatusChange', this.onDetailFoldStatusChange);
    }).catch((err: BusinessError) => {
      console.error(`Failed to obtain the top window. Cause code: ${err.code}, message: ${err.message}`);
    });
  }

  onPageHide(): void {
    display.off('foldStatusChange');
  }

  build() {
    Column() {
      Text('demo')
    }
    .margin({ top: 100 })
  }
}