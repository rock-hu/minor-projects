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

let Max: number = 4000
let i: number = 0
let end: number = 0
let segment: string = ''

/*
* Image组件 SynLoad属性
* 找到画面id为imageSynLoad的Image组件
* 找到画面文本为ImageSynLoad1的Button组件
* 获取image1的SynLoad属性与期待值True比对
* 点击button1，变更image1的SynLoad属性值为False
* 获取image1的SynLoad属性与期待值False比对
*/

export default async function imageSynLoadTest() {
  defineTest('imageSynLoadTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log("imageSynLoadTestDemo running")
    let driver: Tester = Tester.create()
    let image1: Component = await driver.findComponent(Matcher.create().id('imageSynLoad').enabled(true))
    let button: Component = await driver.findComponent(Matcher.create().text('ImageSynLoad1').enabled(true))
    let synLoad1: string = await image1.getInspectorInfo()
    assertTrue(JSON.parse(synLoad1).$attrs.syncLoad)
    await button.tap()
    let synLoad2: string = await image1.getInspectorInfo()
    assertFalse(JSON.parse(synLoad2).$attrs.syncLoad)
  }, 0)
}