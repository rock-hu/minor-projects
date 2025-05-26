// [Start reduce_ability_stage_time]
import { AbilityStage, Want } from '@kit.AbilityKit';

const LARGE_NUMBER = 100000000;
const DELAYED_TIME = 1000;

export default class MyAbilityStage extends AbilityStage {
  onCreate(): void {
    // Time-consuming operation
    // this.computeTask();
    this.computeTaskAsync(); // Asynchronous mission
  }

  onAcceptWant(want: Want): string {
    // Trigger in specified mode only
    return 'MyAbilityStage';
  }

  private computeTask(): void {
    let count = 0;
    while (count < LARGE_NUMBER) {
      count++;
    }
  }

  private computeTaskAsync(): void {
    setTimeout(() => { // SetTimeout is used here to achieve asynchronous delayed operation.
      this.computeTask();
    }, DELAYED_TIME);
  }
}
// [End reduce_ability_stage_time]