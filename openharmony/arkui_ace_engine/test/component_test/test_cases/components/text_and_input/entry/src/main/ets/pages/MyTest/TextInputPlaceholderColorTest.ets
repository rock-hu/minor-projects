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
import { assertEqual, Component, defineTest, Matcher, TestCaseLevel, Tester } from '@ohos.arkui.componentTest'

export default async function textInputPlaceholderColorTest() {
  /*
   * TextInput组件 PlaceholderColor属性
   * 找到画面id为textInputPlaceholderColor1,textInputPlaceholderColor2的TextInput组件
   * 找到画面文本为TextInputPlaceholderColor1,TextInputPlaceholderColor2的Button组件
   * 获取textInput1的PlaceholderColor属性与期待值Color.Green比对
   * 获取textInput2的PlaceholderColor属性与期待值'#ffffffff'比对
   * 点击button1，变更textInput2的PlaceholderColor属性值为'#99182431'
   * 获取textInput2的PlaceholderColor属性与期待值'#99182431'比对
   * 点击button2，变更textInput2的PlaceholderColor属性值为$r('sys.color.ohos_id_color_text_hyperlink')
   * 获取textInput2的PlaceholderColor属性与期待值$r('sys.color.ohos_id_color_text_hyperlink')比对
   */
  defineTest('textInputPlaceholderColorTestDemo', {
    level: TestCaseLevel.LEVEL0
  }, async () => {
    console.log("textInputPlaceholderColorTestDemo running")
    let driver: Tester = Tester.create()
    let textInput1: Component =
      await driver.findComponent(Matcher.create().id('textInputPlaceholderColor1').enabled(true))
    let textInput2: Component =
      await driver.findComponent(Matcher.create().id('textInputPlaceholderColor2').enabled(true))
    let button1: Component =
      await driver.findComponent(Matcher.create().text('TextInputPlaceholderColor1').enabled(true))
    let button2: Component =
      await driver.findComponent(Matcher.create().text('TextInputPlaceholderColor2').enabled(true))

    let placeholderColor1: string = await textInput1.getInspectorInfo()
    assertEqual(JSON.parse(placeholderColor1).$attrs.placeholderColor.toLowerCase(), Color.Green)

    let placeholderColor2: string = await textInput2.getInspectorInfo()
    assertEqual(JSON.parse(placeholderColor2).$attrs.placeholderColor.toLowerCase(), '#ffffffff')

    await button1.tap()
    let placeholderColor3: string = await textInput2.getInspectorInfo()
    assertEqual(JSON.parse(placeholderColor3).$attrs.placeholderColor, '#99182431')

    await button2.tap()
    let placeholderColor4: string = await textInput2.getInspectorInfo()
    assertEqual(JSON.parse(placeholderColor4).$attrs.placeholderColor, $r('sys.color.ohos_id_color_text_hyperlink'))
  }, 0)
}