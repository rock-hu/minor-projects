/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import Logger from './Logger';
import { Want } from '@kit.AbilityKit';

const TAG = '[Sample_StartMode]';
let contextCaller: AbilityContext, want:Want;

interface EventHub {
  emit(event: string, ...args: Object[]): void;
}

interface  AbilityContext {
  startAbility(want: Want) : void;
  eventHub: EventHub;
}

class  EventData {
  context: AbilityContext | null = null
  launchWant: Want | null = null
}

let that = this;

export function getContextData(): Want {
  Logger.info(TAG, 'UtilPage getContextData start');
  let context = getContext(that);
  let data: EventData = new EventData();
  context.eventHub.emit("getAbilityData", data);
  contextCaller = data.context as AbilityContext; // 拿到全局的context，即类似globalThis.mainAbilityContext
  want = data.launchWant as Want;
  Logger.info(TAG, 'UtilPage contextCaller ' + JSON.stringify(contextCaller));
  return want;
}

export function startMode(wantParameters: Record<string, string|number>, abilityName: string) {
  Logger.info(TAG, `${abilityName} start`);
  getContextData();
  let want: Want = {
    bundleName: 'ohos.samples.startmode',
    abilityName: abilityName,
    parameters: wantParameters
  }
  Logger.info(TAG, `${abilityName} contextCaller ${JSON.stringify(contextCaller)}`);
  try {
    contextCaller.startAbility(want);
  }catch {
    (err: Function) => {
      Logger.info(TAG, 'err is' + JSON.stringify(err));
    }
  }
  Logger.info(TAG, `${abilityName} end`);
}

export function totast() {
  AlertDialog.show(
    {
      message: $r('app.string.totast'),
      secondaryButton: {
        value: 'ok',
        action: () => {
          Logger.info(TAG, 'Callback when the second button is clicked')
        }
      }
    }
  )
}