/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { AbilityConstant, ConfigurationConstant, UIAbility, Want } from '@kit.AbilityKit';
import { hilog } from '@kit.PerformanceAnalysisKit';
import { window } from '@kit.ArkUI';
import { ModelData } from '../model/ModelData';

const DOMAIN = 0x0000;

let modelDataList: ModelData[] = [];

export default class EntryAbility extends UIAbility {
  // [Start LifeCycle_Ability_Create]
  onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
    this.context.getApplicationContext().setColorMode(ConfigurationConstant.ColorMode.COLOR_MODE_NOT_SET);

    // Get eventHub.
    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('Ability Create', new Date().toLocaleString()));
    // [StartExclude LifeCycle_Ability_Create]
    // [Start LifeCycle_Ability_Create_Emit]
    eventhub.emit('ability', modelDataList);
    // [End LifeCycle_Ability_Create_Emit]
    // [EndExclude LifeCycle_Ability_Create]

    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onCreate');
  }
  // [End LifeCycle_Ability_Create]

  // [Start LifeCycle_Ability_Destroy]
  // [Start LifeCycle_Ability_Destroy_Emit]
  // [Start LifeCycle_Ability_Destroy_Off]
  onDestroy(): void {
    // [StartExclude LifeCycle_Ability_Destroy_Emit]
    // [StartExclude LifeCycle_Ability_Destroy_Off]
    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('Ability Destroy', new Date().toLocaleString()));
    // [EndExclude LifeCycle_Ability_Destroy_Emit]
    // [StartExclude LifeCycle_Ability_Destroy]
    eventhub.emit('ability', modelDataList);
    // [EndExclude LifeCycle_Ability_Destroy_Off]

    // [StartExclude LifeCycle_Ability_Destroy_Emit]
    this.context.eventHub.off('ability');
    // [EndExclude LifeCycle_Ability_Destroy_Emit]
    // [EndExclude LifeCycle_Ability_Destroy]

    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onDestroy');
  }
  // [End LifeCycle_Ability_Destroy_Off]
  // [End LifeCycle_Ability_Destroy_Emit]
  // [End LifeCycle_Ability_Destroy]

  // [Start LifeCycle_Ability_WindowStage]
  // [Start LifeCycle_Ability_WindowStage_Emit]
  // [Start LifeCycle_Ability_WindowStage_AppStorage]
  // [Start LifeCycle_Ability_WindowStage_LoadContent]
  onWindowStageCreate(windowStage: window.WindowStage): void {
    // [StartExclude LifeCycle_Ability_WindowStage_Emit]
    // [StartExclude LifeCycle_Ability_WindowStage_AppStorage]
    // [StartExclude LifeCycle_Ability_WindowStage_LoadContent]
    // Main window is created, set main page for this ability.
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageCreate');

    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('WindowStage Create', new Date().toLocaleString()));
    // [EndExclude LifeCycle_Ability_WindowStage_Emit]
    // [StartExclude LifeCycle_Ability_WindowStage]
    eventhub.emit('ability', modelDataList);

    // [EndExclude LifeCycle_Ability_WindowStage_AppStorage]
    // [StartExclude LifeCycle_Ability_WindowStage_Emit]
    // Storage parameters.
    AppStorage.setOrCreate("modelDataList", modelDataList);

    // [EndExclude LifeCycle_Ability_WindowStage_LoadContent]
    // [StartExclude LifeCycle_Ability_WindowStage_AppStorage]
    // Set UI loading.
    windowStage.loadContent('pages/Index', (err) => {
      if (err.code) {
        hilog.error(DOMAIN, 'testTag', 'Failed to load the content. Cause: %{public}s', JSON.stringify(err));
        return;
      }
      hilog.info(DOMAIN, 'testTag', 'Succeeded in loading the content.');
    });
    // [EndExclude LifeCycle_Ability_WindowStage_AppStorage]
    // [EndExclude LifeCycle_Ability_WindowStage_Emit]
    // [EndExclude LifeCycle_Ability_WindowStage]
  }
  // [End LifeCycle_Ability_WindowStage_LoadContent]
  // [End LifeCycle_Ability_WindowStage_AppStorage]

  onWindowStageDestroy(): void {
    // [StartExclude LifeCycle_Ability_WindowStage_Emit]
    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('WindowStage Destroy', new Date().toLocaleString()));
    // [EndExclude LifeCycle_Ability_WindowStage_Emit]
    // [StartExclude LifeCycle_Ability_WindowStage]
    eventhub.emit('ability', modelDataList);
    // [EndExclude LifeCycle_Ability_WindowStage]

    // [StartExclude LifeCycle_Ability_WindowStage_Emit]
    // Main window is destroyed, release UI related resources.
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onWindowStageDestroy');
    // [EndExclude LifeCycle_Ability_WindowStage_Emit]
  }
  // [End LifeCycle_Ability_WindowStage_Emit]
  // [End LifeCycle_Ability_WindowStage]

  // [Start LifeCycle_Ability_Fore_Back]
  // [Start LifeCycle_Ability_Fore_Back_Emit]
  onForeground(): void {
    // [StartExclude LifeCycle_Ability_Fore_Back_Emit]
    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('Ability Foreground', new Date().toLocaleString()));
    // [EndExclude LifeCycle_Ability_Fore_Back_Emit]
    // [StartExclude LifeCycle_Ability_Fore_Back]
    eventhub.emit('ability', modelDataList);
    // [EndExclude LifeCycle_Ability_Fore_Back]

    // Ability has brought to foreground.
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onForeground');
  }

  onBackground(): void {
    // [StartExclude LifeCycle_Ability_Fore_Back_Emit]
    let eventhub = this.context.eventHub;
    modelDataList.push(new ModelData('Ability Background', new Date().toLocaleString()));
    // [EndExclude LifeCycle_Ability_Fore_Back_Emit]
    // [StartExclude LifeCycle_Ability_Fore_Back]
    eventhub.emit('ability', modelDataList);
    // [EndExclude LifeCycle_Ability_Fore_Back]

    // Ability has back to background.
    hilog.info(DOMAIN, 'testTag', '%{public}s', 'Ability onBackground');
  }
  // [End LifeCycle_Ability_Fore_Back_Emit]
  // [Start LifeCycle_Ability_Fore_Back]
}