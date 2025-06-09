import { window } from '@kit.ArkUI';
import { BusinessError } from '@kit.BasicServicesKit';
export class BreakpointHeight {
  updateHeightBreakpoint(): void {
    const uiContext: UIContext | undefined = AppStorage.get('uiContext');
    // [Start source_add3]
    let heightBreakpoint: string = AppStorage.get('heightBreakpoint') as string;
    if (heightBreakpoint === 'md') {
      let promise = window.getLastWindow(uiContext?.getHostContext());
      promise.then((mainWindow: window.Window) => {
        mainWindow.setPreferredOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
      }).catch((err: BusinessError) => {
        console.error(`Failed to obtain the top window. Cause code: ${err.code}, message: ${err.message}`);
      });
    }
    // [End source_add3]
  }
}