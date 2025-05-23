import { display } from '@kit.ArkUI';
import Logger from '../../../common/Logger';
import { DisplayRotationAngle } from '../constants/CommonConstants';

const TAG: string = 'DeviceService';

export class DeviceService {
  public static width: number = 0; // Width of the device screen, in vp.
  public static height: number = 0; // Height of the device screen, in vp.
  public static widthPX: number = 0; // Width of the device screen, in px.
  public static heightPX: number = 0; // Height of the device screen, in px.
  public static densityPixels: number = 1; // Screen density of the device.
  public static rotation: number = DisplayRotationAngle.ROTATION_0; // Screen orientation of the app.
  public static portraitRatio: number = 9 / 16; // Portrait resolution of the camera preview stream.
  public static isFolding: boolean = false; // Whether the device is a foldable device.

  // Update the latest display information and match the proper portrait resolution of the camera preview stream.
  public static refreshDisplayInfoAndSelectSuitableRatio() {
    let displayClass: display.Display | null = DeviceService.getDisplayInfoSync();
    DeviceService.setDisplayInfo(displayClass);
    DeviceService.selectSuitableRatio();
  }

  public static getDisplayInfoSync(): display.Display | null {
    let displayClass: display.Display | null = null;
    try {
      displayClass = display.getDefaultDisplaySync();
      Logger.info(TAG, 'Succeed in getting default display.');
    } catch (error) {
      Logger.error(TAG, `Failed to getDefaultDisplaySync. Code: ${error.code}`);
    }
    return displayClass;
  }

  public static setDisplayInfo(displayClass: display.Display | null): void {
    if (displayClass !== null) {
      DeviceService.densityPixels = displayClass.densityPixels;
      DeviceService.widthPX = displayClass.width;
      DeviceService.heightPX = displayClass.height;
      DeviceService.width = px2vp(displayClass.width);
      DeviceService.height = px2vp(displayClass.height);
      DeviceService.rotation = displayClass.rotation;
      Logger.info(TAG,
        `setDisplayInfo, width: ${DeviceService.width}, height: ${DeviceService.height}, rotation: ${DeviceService.rotation}, densityPixels: ${DeviceService.densityPixels}`);
    }
  }

  // Match a proper portrait resolution based on the device width and height.
  public static selectSuitableRatio(): void {
    Logger.info(TAG, 'Start to select suitable portraitRatio.');
    let suitableRatio: number = 9 / 16;
    let portraitRatio: number;
    if (DeviceService.widthPX === 0 || DeviceService.heightPX === 0) {
      Logger.info(TAG,
        `The width or height of the display are abnormal. widthPX : ${DeviceService.widthPX}, heightPX : ${DeviceService.heightPX}`);
      return;
    }
    if (DeviceService.isLandscape()) {
      portraitRatio = DeviceService.heightPX / DeviceService.widthPX;
    } else {
      portraitRatio = DeviceService.widthPX / DeviceService.heightPX;
    }
    let cameraRatios: Array<number> = [9 / 16, 3 / 4, 1];
    let smallestRatioDelta: number = Infinity;
    for (let ratio of cameraRatios) {
      let ratioDelta = Math.abs(ratio - portraitRatio);
      if (ratioDelta < smallestRatioDelta) {
        smallestRatioDelta = ratioDelta;
        suitableRatio = ratio;
      }
    }
    DeviceService.portraitRatio = suitableRatio;
    Logger.info(TAG, `portraitRatio: ${DeviceService.portraitRatio}`);
  }

  // Check whether the device is in landscape mode.
  public static isLandscape(): boolean {
    return DeviceService.rotation === DisplayRotationAngle.ROTATION_90 ||
      DeviceService.rotation === DisplayRotationAngle.ROTATION_270;
  }

  public static getFoldStatusByDisplay(): display.FoldStatus {
    let foldStatus: display.FoldStatus = display.FoldStatus.FOLD_STATUS_UNKNOWN;
    try {
      foldStatus = display.getFoldStatus();
    } catch (error) {
      Logger.error(TAG, `Failed to getFoldStatusByDisplay. Code: ${error.code}`);
    }
    return foldStatus;
  }
}