// [Start entry_ability_demo]
import { AbilityConstant, UIAbility, Want } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI'

const LARGE_NUMBER = 100000000;
const DELAYED_TIME = 1000;


export default class EntryAbility extends UIAbility {
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    // Time-consuming operation
    // this.computeTask();
    this.computeTaskAsync(); // Asynchronous tasks
  }

  onWindowStageCreate(windowStage: window.WindowStage): void {
    windowStage.loadContent('pages/Index', (err, data) => {
      if (err.code) {
        console.error('Failed to load the content. Cause: ' + JSON.stringify(err) ?? '');
        return;
      }
      console.info('Succeeded in loading the content. Data: ' + JSON.stringify(data) ?? '');
    });

    // Time-consuming operation
    // this.computeTask();
    // this.computeTaskAsync(); // Asynchronous mission
  }

  onForeground(): void {
    // Time-consuming operation
    // this.computeTask();
    // this.computeTaskAsync(); // Asynchronous mission
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
// [End entry_ability_demo]