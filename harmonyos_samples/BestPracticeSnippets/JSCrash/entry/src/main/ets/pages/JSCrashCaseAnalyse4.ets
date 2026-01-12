/**
 * 最佳实践：JSCrash问题案例分析
 */

import wifiManager from '@ohos.wifiManager';
import { Log as log } from './utils'
class BaseComponent {
  onStart(): void {

  }
}

class Index extends BaseComponent{
  isConnected: boolean = false;
  handleUpdateState() {}
  isWifiActive: boolean = false;

  // [Start operation_failed_case]
  onStart(): void {
    super.onStart();
    log.showInfo('onStart');

    wifiManager.on('wifiConnectionChange', (data) => {
      this.isConnected = data === 1 ? true : false;
      this.handleUpdateState();
    });

  }
  // [End operation_failed_case]
}
