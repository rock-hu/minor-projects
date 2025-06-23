/**
 * 最佳实践: 静态场景低功耗规则
 */

// [Start StaticScenesRule1]
import { sensor } from '@kit.SensorServiceKit';
import { BusinessError } from '@kit.BasicServicesKit';

sensor.on(sensor.SensorId.ACCELEROMETER, (data: sensor.AccelerometerResponse) => {
  console.info("Succeeded in obtaining data. x: " + data.x + " y: " + data.y + " z: " + data.z);
}, { interval: 200000000 });
// [End StaticScenesRule1]

// [Start StaticScenesRule2]
sensor.on(sensor.SensorId.ACCELEROMETER, (data: sensor.AccelerometerResponse) => {
  console.info("Succeeded in obtaining data. x: " + data.x + " y: " + data.y + " z: " + data.z);
}, { interval: 200000000 });
// [End StaticScenesRule2]

// [Start StaticScenesRule3]
sensor.off(sensor.SensorId.ACCELEROMETER);
// [End StaticScenesRule3]

