import { AbilityConstant, Want } from '@kit.AbilityKit';
import { rpc } from '@kit.IPCKit';

interface WantScene {
  /**
   * 最佳实践：应用安全编码实践
   * 场景二十七：建议对跨信任边界传入的Want进行合法性判断-在不同场景下获取Want的方式
   */

  // [Start want_scene]
  // Application life cycle:
  // @ohos.application.AbilityStage.d.ts
  onAcceptWant(want: Want): string;
  // ability life cycle：
  // @ohos.application.Ability.d.ts
  onCreate(want: Want, param: AbilityConstant.LaunchParam): void;
  onContinue(wantParam: {}): AbilityConstant.OnContinueResult;
  onNewWant(want: Want, launchParams: AbilityConstant.LaunchParam): void;
  // ServiceExtensionAbility life cycle：
  // @ohos.application.ServiceExtensionAbility.d.ts
  onCreate(want: Want): void;
  onRequest(want: Want, startId: number): void;
  onConnect(want: Want): rpc.RemoteObject;
  onDisconnect(want: Want): void;
  onReconnect(want: Want): void;
  // [End want_scene]
}