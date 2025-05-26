// [Start reduce_import]
// 优化减少import的模块
/*import { ConfigurationConstant, contextConstant, wantConstant } from '@kit.AbilityKit';
import { GesturePath, GesturePoint } from '@kit.AccessibilityKit';
import { distributedAccount, osAccount } from '@kit.BasicServicesKit';
import { Configuration } from '@kit.ArkUI';
import { atomicService } from '@kit.ScenarioFusionKit';
import { sim } from '@kit.TelephonyKit';*/


import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';


export default class EntryAbility extends UIAbility {
  // ...
}
// [End reduce_import]