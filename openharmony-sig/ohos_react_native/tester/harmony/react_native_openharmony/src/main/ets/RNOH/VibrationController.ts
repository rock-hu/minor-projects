import vibrator from '@ohos.vibrator'
import { RNOHLogger } from "./RNOHLogger"

export class VibrationController {
  private logger: RNOHLogger

  constructor(logger: RNOHLogger) {
    this.logger = logger.clone("Vibration")
  }

  public vibrate(pattern: number) {
    vibrator.startVibration({
      type: 'time',
      duration: pattern,
    }, {
      id: 0,
      usage: 'simulateReality'
    }, (error) => {
      const logger = this.logger.clone('vibrate');
      if (error) {
        if (error.code === 201) {
          logger?.error('Permission denied. Check if \'ohos.permission.VIBRATE\' has been granted');
        }
        logger.error(error);
      }
    });
  }

  public cancel() {
    vibrator.stopVibration();
  }
}