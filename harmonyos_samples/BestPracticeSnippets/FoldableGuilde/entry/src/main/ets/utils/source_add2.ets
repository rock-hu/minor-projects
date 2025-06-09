import { window, display } from '@kit.ArkUI'
import { BusinessError } from '@kit.BasicServicesKit';
export class Breakpoint {
  // [Start source_add2]
  // Update horizontal breakpoints according to window width
  updateWidthBreakpoint(): void {
    const uiContext: UIContext | undefined = AppStorage.get('uiContext');
    let promise = window.getLastWindow(uiContext?.getHostContext());
    promise.then((mainWindow: window.Window) => {
      let windowRect: window.Rect = mainWindow.getWindowProperties().windowRect;
      let windowWidthVp: number = windowRect.width / display.getDefaultDisplaySync().densityPixels;
      let widthBp: string = '';
      if (windowWidthVp < 320) {
        widthBp = 'xs';
      } else if (windowWidthVp >= 320 && windowWidthVp < 600) {
        widthBp = 'sm';
      } else if (windowWidthVp >= 600 && windowWidthVp < 840) {
        widthBp = 'md';
      } else if (windowWidthVp >= 840 && windowWidthVp < 1440) {
        widthBp = 'lg';
      } else {
        widthBp = 'xl';
      }
      AppStorage.setOrCreate('widthBreakpoint', widthBp);
    }).catch((err: BusinessError) => {
      console.error(`Failed to obtain the top window. Cause code: ${err.code}, message: ${err.message}`);
    });
  }
  // Update the vertical breakpoint according to the window width-height ratio
  updateHeightBreakpoint(): void {
    const uiContext: UIContext | undefined = AppStorage.get('uiContext');
    let promise = window.getLastWindow(uiContext?.getHostContext());
    promise.then((mainWindow: window.Window) => {
      let windowRect: window.Rect = mainWindow.getWindowProperties().windowRect;
      let windowWidthVp: number = windowRect.width / display.getDefaultDisplaySync().densityPixels;
      let windowHeightVp: number = windowRect.height / display.getDefaultDisplaySync().densityPixels;
      let heightBp: string = '';
      let aspectRatio: number = windowHeightVp / windowWidthVp;
      if (aspectRatio < 0.8) {
        heightBp = 'sm';
      } else if (aspectRatio >= 0.8 && aspectRatio < 1.2) {
        heightBp = 'md';
      } else {
        heightBp = 'lg';
      }
      AppStorage.setOrCreate('heightBreakpoint', heightBp);
    }).catch((err: BusinessError) => {
      console.error(`Failed to obtain the top window. Cause code: ${err.code}, message: ${err.message}`);
    });
  }
  // [End source_add2]
}