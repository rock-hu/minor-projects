/**
 * 最佳实践：后台合理使用定位导航服务
 * 场景一：方式1
 */
// [Start bpta_gps_one]
import { geoLocationManager } from '@kit.LocationKit';

let requestInfo: geoLocationManager.LocationRequest = {
  'scenario': geoLocationManager.LocationRequestScenario.NO_POWER,
  'timeInterval': 0,
  'distanceInterval': 0,
  'maxAccuracy': 0
};
// [End bpta_gps_one]