// [Start my_ability_stage]
// MyAbilityStage.ets
import { AbilityStage, Want } from '@kit.AbilityKit';
import { httpRequest } from '../utils/NetRequest';
export default class MyAbilityStage extends AbilityStage {
  onCreate(): void {
    // Send a network request
    httpRequest();
  }

  onAcceptWant(want: Want): string {
    // Triggered in specified mode only.
    return 'MyAbilityStage';
  }
}
// [End my_ability_stage]