/**
 * 易错API的使用规范
 */
import { camera } from '@kit.CameraKit';


class cameraClass {
  cameraManager: camera.CameraManager | undefined = undefined;

  getCameraManager(context: Context) {
    this.cameraManager = camera.getCameraManager(context);
  }

  async addInput(cameraManager: camera.CameraManager, camera: camera.CameraDevice, session: camera.Session) {
    let cameraInput: camera.CameraInput | undefined = undefined;
    // [Start add_input]
    cameraInput = cameraManager.createCameraInput(camera);
    await cameraInput.open();
    session.beginConfig();
    session.addInput(cameraInput);
    // [End add_input]
  }

  async addOutput(cameraManager: camera.CameraManager, camera: camera.CameraDevice, session: camera.Session,
    cameraOutputCapability: camera.CameraOutputCapability) {
    let previewOutput: camera.CameraOutput | undefined = undefined;
    let cameraInput: camera.CameraInput | undefined = undefined;
    let profile: camera.Profile = cameraOutputCapability.previewProfiles[0];
    let surfaceId = 'xxx';
    // [Start add_output]
    previewOutput = cameraManager.createPreviewOutput(profile, surfaceId);
    cameraInput = cameraManager.createCameraInput(camera);
    await cameraInput.open();
    session.beginConfig();
    session.addInput(cameraInput);
    session.addOutput(previewOutput);
    // [End add_output]
  }

  async addOutput1(cameraManager: camera.CameraManager, camera: camera.CameraDevice, session: camera.Session,
    cameraOutputCapability: camera.CameraOutputCapability) {
    let previewOutput: camera.CameraOutput | undefined = undefined;
    let cameraInput: camera.CameraInput | undefined = undefined;
    let profile: camera.Profile = cameraOutputCapability.previewProfiles[0];
    let surfaceId = 'xxx';
    // [Start add_output1]
    previewOutput = cameraManager.createPreviewOutput(profile, surfaceId);
    cameraInput = cameraManager.createCameraInput(camera);
    await cameraInput.open();
    session.beginConfig();
    session.addInput(cameraInput);
    session.addOutput(previewOutput);
    session.commitConfig();
    // [End add_output1]
  }

}

let cameraInput:camera.CameraInput;
let cameraSession:camera.Session;
// [Start close_release]
async function onBackground() {
  await cameraInput.close();
  await cameraSession.release();
}
// [End close_release]