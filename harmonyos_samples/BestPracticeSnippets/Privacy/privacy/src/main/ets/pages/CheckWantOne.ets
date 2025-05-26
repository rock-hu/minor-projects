/**
 * 最佳实践：应用安全编码实践
 * 场景二十五：建议对跨信任边界传入的Want进行合法性判断-确保访问的对象属性存在
 */


// [Start check_want_one]
import { hilog } from '@kit.PerformanceAnalysisKit';
import { UIAbility, Want, AbilityConstant } from '@kit.AbilityKit';


// Check function
function checkWant(want: Want) {
  if (want === null || want.parameters === null
    || want === undefined || want.parameters === undefined) {
    return false;
  }
  return true;
}


export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    hilog.info(0x0000, 'testTag', '%{public}s', 'Ability onCreate');
    if (checkWant(want)) {
      console.info("invalid want");
    } else {
      console.info("correct want");
    }
  }
}
// [End check_want_one]