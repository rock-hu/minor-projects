import { PromptAction, router, Router, window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG: string = 'UIContextSelf';

export class UIContextSelf {
  public static uiContext: UIContext;

  public static setUIContext(windowObj: window.Window): void {
    if (windowObj) {
      try {
        UIContextSelf.uiContext = windowObj.getUIContext();
        hilog.info(0x0001, TAG, 'Succeeded in getting getUIContext.');
      } catch (error) {
        hilog.error(0x0001, TAG, `Failed to get getUIContext by windowObj. Code: ${error.code}.`);
      }
    } else {
      hilog.error(0x0001, TAG, 'windowObj is invalid.');
    }
  }

  public static setUIContextByUIContent(uiContext: UIContext): void {
    if (!UIContextSelf.uiContext) {
      UIContextSelf.uiContext = uiContext;
      hilog.info(0x0001, TAG, 'Succeeded in getting getUIContext by UIContext.');
    }
  }

  public static getPromptAction(): PromptAction {
    if (!UIContextSelf.uiContext) {
      throw new Error('UIContext has not been set.');
    }
    return UIContextSelf.uiContext.getPromptAction();
  }

  public static showToast(message: Resource | string, time: number): void {
    try {
      UIContextSelf.getPromptAction().showToast({
        message: message,
        duration: time
      });
    } catch (error) {
      hilog.error(0x0001, TAG, `Failed to showToast by PromptAction. Code: ${error.code}.`);
    }
  }

  public static getHostContext(): Context | undefined {
    if (!UIContextSelf.uiContext) {
      throw new Error('UIContext has not been set.');
    }
    return UIContextSelf.uiContext.getHostContext();
  }

  public static getRouter(): Router {
    if (!UIContextSelf.uiContext) {
      throw new Error('UIContext has not been set.');
    }
    return UIContextSelf.uiContext.getRouter();
  }

  public static async replaceUrl(options: router.RouterOptions): Promise<void> {
    try {
      UIContextSelf.uiContext.getRouter().replaceUrl(options).catch((error: BusinessError) => {
        hilog.error(0x0001, TAG, `Failed to replaceUrl by getRouter. Code: ${error.code}.`);
      });
    } catch (error) {
      hilog.error(0x0001, TAG, `Failed to replaceUrl by getRouter. Code: ${error.code}.`);
    }
  }

  public static async pushUrl(options: router.RouterOptions): Promise<void> {
    try {
      UIContextSelf.uiContext.getRouter().pushUrl(options).catch((error: BusinessError) => {
        hilog.error(0x0001, TAG, `Failed to pushUrl by getRouter. Code: ${error.code}.`);
      });
    } catch (error) {
      hilog.error(0x0001, TAG, `Failed to pushUrl by getRouter. Code: ${error.code}.`);
    }
  }
}