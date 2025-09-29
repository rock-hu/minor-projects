// utils/WindowsUtil.ets
import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';

@Observed
export class WindowUtil {
  private windowStage?: window.WindowStage;
  private mainWindowClass?: window.Window;

  // 获取实例
  static getInstance(): WindowUtil | undefined {
    if (!AppStorage.get<WindowUtil>('windowUtil')) {
      AppStorage.setOrCreate('windowUtil', new WindowUtil());
    }
    return AppStorage.get<WindowUtil>('windowUtil');
  }

  setMainWindowPortrait(): void {
    if (this.windowStage === undefined) {
      return;
    }
    this.windowStage.getMainWindow((err, windowClass: window.Window) => {
      if (err) {
        console.error(`Failed to get the main window. Cause code: ${err.code}, message: ${err.message}`);
      }
      this.mainWindowClass = windowClass;
    });
  }

  setPhoneAutoRotation(): void {
    if (this.mainWindowClass === undefined) {
      return;
    }
    // 设置传感器自动旋转模式
    this.mainWindowClass.setPreferredOrientation(window.Orientation.AUTO_ROTATION).catch((err: BusinessError) => {
      console.error(`Failed to set phone's screen auto rotation. Cause code: ${err.code}, message: ${err.message}`);
    });
  }

  setPhoneLandscape(): void {
    if (this.mainWindowClass === undefined) {
      return;
    }
    // 设置横屏显示模式
    this.mainWindowClass.setPreferredOrientation(window.Orientation.LANDSCAPE).catch((err: BusinessError) => {
      console.error(`Failed to set phone's screen landscape. Cause code: ${err.code}, message: ${err.message}`);
    });
  }

  setWindowStage(windowStage: window.WindowStage): void {
    this.windowStage = windowStage;
  }

  getMainWindow(): window.Window | undefined {
    return this.mainWindowClass;
  }
}
