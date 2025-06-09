// FoldableScreen.ets
import { display } from '@kit.ArkUI'
import { WindowUtil } from '../utils/WindowUtil';

@Entry
@Component
export struct FoldableScreen {
  @State isFoldable: Boolean = false
  @State foldStatus: display.FoldStatus = 1
  private windowUtil?: WindowUtil;

  aboutToAppear() {
    this.windowUtil = WindowUtil.getInstance();
    this.windowUtil?.setPhoneAutoRotation();

    // 实时监听设备折叠状态
    display.on('foldStatusChange', (data: display.FoldStatus) => {
      console.info('实时监听设备折叠状态: ' + JSON.stringify(data));
      if (data === display.FoldStatus.FOLD_STATUS_HALF_FOLDED) {
        this.windowUtil?.setPhoneLandscape()
      } else if (data === display.FoldStatus.FOLD_STATUS_EXPANDED) {
        this.windowUtil?.setPhoneAutoRotation();
      }
    });
  }

  build() {
    Column() {
      Text('demo')
    }
    .margin({ top: 100 })
  }
}
