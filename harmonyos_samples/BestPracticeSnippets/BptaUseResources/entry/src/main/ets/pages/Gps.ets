/**
 * 最佳实践：合理使用GPS资源
 */

// [Start gps_resources]
import { UIAbility } from '@kit.AbilityKit';
import { geoLocationManager } from '@kit.LocationKit';

// [StartExclude gps_resources]
let locationChange = (location: geoLocationManager.Location): void => {
  console.log('locationChanger:data:' + JSON.stringify(location));
};
// [EndExclude gps_resources]

export default class EntryAbility extends UIAbility {
  // ...
  onForeground(): void {
    // Create a location request based on service requirements at the foreground
    let requestInfo: geoLocationManager.LocationRequest = {
      'priority': geoLocationManager.LocationRequestPriority.ACCURACY,
      'timeInterval': 0,
      'distanceInterval': 0,
      'maxAccuracy': 0
    };
    let locationChange = (location: geoLocationManager.Location): void => {
      console.log('locationChanger:data:' + JSON.stringify(location));
    };
    //The change of the listening position
    geoLocationManager.on('locationChange', requestInfo, locationChange);
  }

  onBackground(): void {
    //The backstage cancels the listening
    geoLocationManager.off('locationChange', locationChange);
  }
}
// [End gps_resources]