/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import { describe, beforeAll, it, expect, TestType, beforeEach } from '@ohos/hypium';
import { abilityDelegatorRegistry, Driver, ON } from '@kit.TestKit';
import { Want } from '@kit.AbilityKit';

const delegator = abilityDelegatorRegistry.getAbilityDelegator();
const bundleName = abilityDelegatorRegistry.getArguments().bundleName;
const WAIT_UI_READYMS = 1000;

export default function PinchEventTest() {
  describe('PinchEventTest', () => {
    beforeAll(async () => {
      let want: Want = {
        bundleName : bundleName,
        abilityName : "PinchAbility"
      }
      await delegator.startAbility(want);
    })

    /**
     * @tc.number:PinchEventTest_001
     * @tc.name: PinchEventTest_In
     * @tc.type: 0 || TestType.FUNCTION || Size.SMALLTEST || Level.LEVEL0
     * @tc.desc: Detect the uitest pinchIn function.
     */
    it('PinchEventTest_In', TestType.FUNCTION, async (done: Function) => {
      let driver = Driver.create();
      await driver.delayMs(WAIT_UI_READYMS);
      let image1 = await driver.findComponent(ON.id('pinch_ima'));
      let bounds1 = await image1.getBounds();
      await image1.pinchIn(0.5);
      await driver.delayMs(WAIT_UI_READYMS);
      let image2 = await driver.findComponent(ON.id('pinch_ima'));
      let bounds2 = await image2.getBounds();
      expect(bounds1).not().assertDeepEquals(bounds2);
      done();
    })

    /**
     * @tc.number:PinchEventTest_002
    * @tc.name: PinchEventTest_Out
     * @tc.type: 0 || TestType.FUNCTION || Size.SMALLTEST || Level.LEVEL0
     * @tc.desc: Detects the uitest pinchOut function.
     */
    it('PinchEventTest_Out', TestType.FUNCTION, async (done: Function) => {
      let driver = Driver.create();
      await driver.delayMs(WAIT_UI_READYMS);
      let image1 = await driver.findComponent(ON.id('pinch_ima'));
      let bounds1 = await image1.getBounds();
      await image1.pinchOut(1.2);
      await driver.delayMs(WAIT_UI_READYMS);
      let image2 = await driver.findComponent(ON.id('pinch_ima'));
      let bounds2 = await image2.getBounds();
      expect(bounds1).not().assertDeepEquals(bounds2);
      done();
    })
  })
}