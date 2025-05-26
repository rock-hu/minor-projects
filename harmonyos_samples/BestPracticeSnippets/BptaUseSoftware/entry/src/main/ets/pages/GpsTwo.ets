/**
 * 最佳实践：后台合理使用定位导航服务
 * 场景二：方式2
 */
// [Start bpta_gps_two]
import { geoLocationManager } from '@kit.LocationKit';

let requestInfo: geoLocationManager.LocationRequest = {
  'priority': geoLocationManager.LocationRequestPriority.LOW_POWER,
  'timeInterval': 0,
  'distanceInterval': 0,
  'maxAccuracy': 0
};
// [End bpta_gps_two]