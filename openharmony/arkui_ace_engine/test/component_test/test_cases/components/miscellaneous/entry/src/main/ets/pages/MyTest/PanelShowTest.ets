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
  assertFalse,
  assertTrue,
  Component,
  defineTest,
  Matcher,
  TestCaseLevel,
  Tester
} from '@ohos.arkui.componentTest'

export default async function panelShowTest() {
  /*
   * Panel组件 Show属性
   * 找到画面id为panelShow的Panel组件
   * 找到画面文本为PanelShow的Button组件
   * 获取panel的Show属性与期待值true比对
   * 点击button，变更panel的Show属性值为false
   * 获取panel的Show属性与期待值false比对
   */
  defineTest('panelShowTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log('panelShowTestDemo running')

    let driver: Tester = Tester.create()
    let panel: Component = await driver.findComponent(Matcher.create().id('panelShow').enabled(true))
    let button: Component = await driver.findComponent(Matcher.create().text('PanelShow').enabled(true))

    let show1: string = await panel.getInspectorInfo()
    assertTrue(JSON.parse(show1).$attrs.show)

    await button.tap()
    let show2: string = await panel.getInspectorInfo()
    assertFalse(JSON.parse(show2).$attrs.show)
  }, 0)
}