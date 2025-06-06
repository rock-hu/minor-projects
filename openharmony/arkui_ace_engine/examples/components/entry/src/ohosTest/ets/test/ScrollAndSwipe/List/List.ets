/*
 * Copyright (c) 2025 Hunan OpenValley Digital Industry Development Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
import { abilityDelegatorRegistry, Component, Driver, ON } from '@kit.TestKit';
import { Point } from '@ohos.UiTest';

const TAG = 'ListBootcampTest'

/*获取AbilityDelegator实例,管理Ability的生命周期和事件*/
const delegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()

/*初始化与UI元素交互的驱动对象*/
const driver = Driver.create();

export async function ListStart(done: Function) {

  console.info('beforeAll begin');

  try {

    await delegator.startAbility({
      bundleName: 'cn.ohos.components',
      abilityName: 'EntryAbility'
    });

    await driver.delayMs(5000);
    console.info(TAG + 'ListBootcamp001' + ' begin');

    let atomDisplays: Component = await driver.findComponent(ON.text('滚动与滑动'));
    await atomDisplays.click();
    await driver.delayMs(1000);

    let scrollSearch: Component = await driver.findComponent(ON.text('列表/List'))
    await scrollSearch.click()
    await driver.delayMs(1000);

    let Buttons: Array<Component> = await driver.findComponents(ON.type('Button'));
    await Buttons[1].click()
    await driver.delayMs(1000);

  } catch (err) {

    console.info(`beforeAll error: ${JSON.stringify(err)}`);

  }
  console.info(`beforeAll end`);
  done();

}
