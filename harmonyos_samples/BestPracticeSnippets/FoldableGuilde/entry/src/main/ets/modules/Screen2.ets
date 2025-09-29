// FoldableScreen.ets
import { display } from '@kit.ArkUI'
import { WindowUtil } from '../utils/WindowUtil';

@Component
export struct FoldableScreen {
  @State isFoldable: Boolean = false;
  @State foldStatus: display.FoldStatus = 1;
  private windowUtil?: WindowUtil;

  aboutToAppear() {
    this.windowUtil = WindowUtil.getInstance();
    this.windowUtil?.setPhoneAutoRotation();

    // 实时监听设备折叠状态
    try {
      display.on('foldStatusChange', (data: display.FoldStatus) => {
        console.info('Monitor the folding status of the device in real-time: ' + JSON.stringify(data));
        if (data === display.FoldStatus.FOLD_STATUS_HALF_FOLDED) {
          this.windowUtil?.setPhoneLandscape();
        } else if (data === display.FoldStatus.FOLD_STATUS_EXPANDED) {
          this.windowUtil?.setPhoneAutoRotation();
        }
      });
    } catch (err) {
      console.error(`Failed to monitor the folding status of the device. Cause code: ${err.code}, message: ${err.message}`);
    }
  }

  build() {
    Column() {
      Text('demo')
    }
    .margin({ top: 100 })
  }
}
