/**
 * 最佳实践：应用安全编码实践
 * 场景五：建议隐式启动应用组件时避免携带个人数据错误示例
 */

// [Start want_info_error]
import { Want } from "@kit.AbilityKit";
// [StartExclude want_info_error]
import { common } from '@kit.AbilityKit';
const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let context = uiContext!.getHostContext()!;
class NoInfo {
  private context = context as common.UIAbilityContext

  wantNoInfo(): void {
    // [EndExclude want_info_error]
    let wantInfo:Want = {
      deviceId: '',
      action: "ability.want.test",
      parameters: {
        "password": "xxxxxxxx"
      }
    }
    try{
      let data = this.context.startAbility(wantInfo)
      console.info("startAbility success " + JSON.stringify(data));
    } catch (err) {
      console.error("startAbility  with error message: " + err.message + ", error code: " + err.code);
    }
    // [End want_info_error]
  }
}
