import { hilog } from '@kit.PerformanceAnalysisKit';
import { common } from '@kit.AbilityKit';
// [Start session_controller1]
import { avSession } from '@kit.AVSessionKit';
const TAG = 'AvSessionController';
export class AvSessionController {
  private avSession: avSession.AVSession | undefined = undefined;
  private context: common.UIAbilityContext | undefined = undefined;
  public initAvSession() {
    this.context = AppStorage.get('context');
    if (!this.context) {
      hilog.info(0x0001, TAG, "session create failed : context is undefined");
      return;
    }
    // Create an AVSession with the session type set to' video'.
    avSession.createAVSession(this.context, "SHORT_AUDIO_SESSION", 'video').then(async (avSession) => {
      this.avSession = avSession;
      hilog.info(0x0001, TAG, `session create successed : sessionId : ${this.avSession.sessionId}`);
      //Sets the UIAbility for being pulled up by the broadcast control center.
      this.setLaunchAbility();
      // Activate media session
      this.avSession.activate();
    });
  }
  // [StartExclude session_controller1]
  setLaunchAbility() {}
  // [EndExclude session_controller1]
}
// [End session_controller1]