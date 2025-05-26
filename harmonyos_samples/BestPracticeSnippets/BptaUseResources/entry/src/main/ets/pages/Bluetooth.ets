/**
 * 最佳实践：合理使用蓝牙资源
 */

// [Start blue_tooth]
import { UIAbility } from '@kit.AbilityKit';
import { ble } from '@kit.ConnectivityKit';

// [StartExclude blue_tooth]
const scanFilter: ble.ScanFilter = {}
const scanOptions: ble.ScanOptions = {}
const setting: ble.AdvertiseSetting = {}
const advData: ble.AdvertiseData = {
  serviceUuids: [],
  manufactureData: [],
  serviceData: []
}
const advResponse = undefined
// [EndExclude blue_tooth]
export default class EntryAbility extends UIAbility {
  // ...
  onForeground(): void {
    //Initiate Ble scan and broadcast as required by the service at the foreground
    ble.startBLEScan([scanFilter], scanOptions);
    ble.startAdvertising(setting, advData, advResponse);
  }

  onBackground(): void {
    // Return to the background to stop the Ble scanning and broadcast, which is the same as the application
    ble.stopBLEScan();
    ble.stopAdvertising();
  }
}
// [End blue_tooth]