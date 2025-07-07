/**
 * 最佳实践：应用安全编码实践
 * 场景六：建议隐式启动应用组件时避免携带个人数据
 */

// [Start want_info]
import { Want } from '@kit.AbilityKit';
// [StartExclude want_info]
import { common } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getHostContext()!;
class NoInfo {
  private context = context as common.UIAbilityContext;

  wantNoInfo(): void {
    // [EndExclude want_info]
    let wantInfo:Want = {
      deviceId: '',
      action: 'ability.want.test',
      bundleName:'com.example.myapplication10',
      abilityName:'MainAbility1',
      parameters: {
        'password': 'xxxxxxxx'
      }
    }
    try{
      let data = this.context.startAbility(wantInfo);
      hilog.info(0x0000, 'privacy', 'startAbility success ' + JSON.stringify(data));
    } catch (err) {
      hilog.error(0x0000, 'privacy', 'startAbility  with error message: ' + err.message + ', error code: ' + err.code);
    }
    // [End want_info]
  }
}
