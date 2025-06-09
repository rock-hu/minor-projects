// utils/WindowsUtil.ets
import { window } from '@kit.ArkUI';

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
      this.mainWindowClass = windowClass;
    });
  }

  setPhoneAutoRotation(): void {
    if (this.mainWindowClass === undefined) {
      return;
    }
    // 设置传感器自动旋转模式
    this.mainWindowClass.setPreferredOrientation(window.Orientation.AUTO_ROTATION);
  }

  setPhoneLandscape(): void {
    if (this.mainWindowClass === undefined) {
      return;
    }
    // 设置横屏显示模式
    this.mainWindowClass.setPreferredOrientation(window.Orientation.LANDSCAPE);
  }

  setWindowStage(windowStage: window.WindowStage): void {
    this.windowStage = windowStage;
  }

  getMainWindow(): window.Window | undefined {
    return this.mainWindowClass;
  }
}
