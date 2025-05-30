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
  assertContain,
  assertEqual,
  assertFalse,
  assertTrue,
  Component,
  defineTest,
  Matcher,
  TestCaseLevel,
  Tester
} from '@ohos.arkui.componentTest';

export default async function circleStrokeColorTest() {
  /*
 * Circle组件 stroke属性
 * 找到画面id为circleStrokeColor1,circleStrokeColor2的Circle组件
 * 找到画面文本为CircleStrokeColor1,CircleStrokeColor2的Button组件
 * 获取circle1的stroke属性与期待值0x0000ff比对
 * 点击button1，变更circle1的stroke属性值为Color.Red
 * 获取circle1的stroke属性与期待值Color.Red比对
 * 获取circle2的stroke属性与期待值#FF0000FF比对
 * 点击button2，变更circle2的stroke属性值为$r('app.color.color_red')
 * 获取circle2的stroke属性与期待值'#FF0000'比对
 */
  defineTest('circleStrokeColorTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log("circleStrokeColorTestDemo running")

    let driver: Tester = Tester.create()
    let circle1: Component = await driver.findComponent(Matcher.create().id('circleStrokeColor1').enabled(true))
    let circle2: Component = await driver.findComponent(Matcher.create().id('circleStrokeColor2').enabled(true))
    let button1: Component = await driver.findComponent(Matcher.create().text('CircleStrokeColor1').enabled(true))
    let button2: Component = await driver.findComponent(Matcher.create().text('CircleStrokeColor2').enabled(true))

    let stroke1: string = await circle1.getInspectorInfo()
    assertEqual(JSON.parse(stroke1).$attrs.stroke, '#FF0000FF')
    await button1.tap()
    let stroke2: string = await circle1.getInspectorInfo()
    assertEqual(JSON.parse(stroke2).$attrs.stroke.toLowerCase(), Color.Red)
    let stroke3: string = await circle2.getInspectorInfo()
    assertEqual(JSON.parse(stroke3).$attrs.stroke, '#FF0000FF')
    await button2.tap()
    let stroke4: string = await circle2.getInspectorInfo()
    assertEqual(JSON.parse(stroke4).$attrs.stroke, $r('app.color.color_red'))
  }, 0)
}