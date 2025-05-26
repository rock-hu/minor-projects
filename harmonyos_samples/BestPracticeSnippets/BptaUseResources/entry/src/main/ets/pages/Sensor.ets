/**
 * 最佳实践：合理使用传感器资源
 */

// [Start sensor_resources]
import { UIAbility } from '@kit.AbilityKit';
import { sensor } from '@kit.SensorServiceKit';

export default class EntryAbility extends UIAbility {
  // ...
  onForeground(): void {
    //In the foreground, listen to the required type of sensor based on the service requirements
    sensor.on(sensor.SensorId.ACCELEROMETER, (data: sensor.AccelerometerResponse) => {
      console.info("Succeededinobtainingdata.x:" + data.x + "y:" + data.y + "z:" + data.z);
    }, {
      interval: 100000000
    });
  }

  onBackground(): void {
    //The backstage cancels the listening
    sensor.off(sensor.SensorId.ACCELEROMETER);
  }
}
// [End sensor_resources]