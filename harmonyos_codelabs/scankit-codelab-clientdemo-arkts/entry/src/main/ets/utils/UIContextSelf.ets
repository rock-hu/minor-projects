import { PromptAction, Router, window } from '@kit.ArkUI';
import { hilog } from '@kit.PerformanceAnalysisKit';

const TAG: string = 'UIContextSelf';

export class UIContextSelf {
  public static uiContext: UIContext;

  public static setUIContext(windowObj: window.Window): void {
    if (windowObj) {
      try {
        UIContextSelf.uiContext = windowObj.getUIContext();
        hilog.info(0x0001, TAG, 'Succeed in getting getUIContext.');
      } catch (error) {
        hilog.error(0x0001, TAG, `Failed to get getUIContext by windowObj. Code: ${error.code}.`);
      }
    } else {
      hilog.error(0x0001, TAG, 'windowObj is invalid.');
    }
  }

  public static getPromptAction(): PromptAction {
    if (!UIContextSelf.uiContext) {
      throw new Error('UIContext has not been set.');
    }
    return UIContextSelf.uiContext.getPromptAction();
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
}