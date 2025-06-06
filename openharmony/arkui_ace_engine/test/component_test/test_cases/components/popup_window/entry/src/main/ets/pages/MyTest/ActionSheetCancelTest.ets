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
import {
  assertEqual,
  assertFalse,
  assertTrue,
  Component,
  defineTest,
  Matcher,
  TestCaseLevel,
  Tester
} from '@ohos.arkui.componentTest'

export default async function actionSheetCancelTest() {
  /*
* ActionSheet组件 Cancel对象
* 找到画面文本为ActionSheetCancelTest1的Button组件
* 找到画面文本为actionText的Text组件
* 点击button弹出弹窗
* 点击遮蔽层，变更text本文的值为actionSheet_canceled1
* 获取text的text值与期待值actionSheet_canceled1比对
*/
  defineTest('actionSheetCancelTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log('actionSheetCancelTestDemo running')
    let driver: Tester = Tester.create()
    let button: Component = await driver.findComponent(Matcher.create().text('ActionSheetCancelTest1').enabled(true))
    let text: Component = await driver.findComponent(Matcher.create().text('actionText').enabled(true))

    await button.tap()
    await driver.tap(1, 1)
    let text1: string = await text.getText()
    assertEqual(text1, 'actionSheet_canceled1')
  }, 0)
}