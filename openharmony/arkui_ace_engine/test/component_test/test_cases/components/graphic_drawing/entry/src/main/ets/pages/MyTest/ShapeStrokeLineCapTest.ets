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

import { assertEqual, assertContain, assertFalse, assertTrue } from '@ohos.arkui.componentTest';
import { TestCaseLevel, defineTest } from '@ohos.arkui.componentTest'
import { Tester, Component, Matcher } from '@ohos.arkui.componentTest'

export default async function shapeStrokeLineCapTest() {
  /*
   * Shape组件 strokeLineCap属性
   * 找到画面id为shapeStrokeLineCap1,shapeStrokeLineCap2的Shape组件
   * 找到画面文本为ShapeStrokeLineCap的Button组件
   * 获取shape1的strokeLineCap属性与期待值LineCapStyle.Round比对
   * 点击button,变更shape1的strokeLineCap属性值为LineCapStyle.Square
   * 获取shape1的strokeLineCap属性与期待值LineCapStyle.Square比对
   * 获取shape2的strokeLineCap属性与期待值LineCapStyle.Butt比对
   */
  defineTest('shapeStrokeLineCapTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log("shapeStrokeLineCapTestDemo running")
    let driver: Tester = Tester.create()
    let shape1: Component = await driver.findComponent(Matcher.create().id('shapeStrokeLineCap1').enabled(true))
    let shape2: Component = await driver.findComponent(Matcher.create().id('shapeStrokeLineCap2').enabled(true))
    let button: Component = await driver.findComponent(Matcher.create().text('ShapeStrokeLineCap').enabled(true))

    let strokeLineCap1: string = await shape1.getInspectorInfo()
    assertEqual(JSON.parse(strokeLineCap1).$attrs.strokeLineCap, 'LineCapStyle.Round')

    await button.tap()
    let strokeLineCap2: string = await shape1.getInspectorInfo()
    assertEqual(JSON.parse(strokeLineCap2).$attrs.strokeLineCap, 'LineCapStyle.Square')

    let strokeLineCap3: string = await shape2.getInspectorInfo()
    assertEqual(JSON.parse(strokeLineCap3).$attrs.strokeLineCap, 'LineCapStyle.Butt')
  }, 0)
}