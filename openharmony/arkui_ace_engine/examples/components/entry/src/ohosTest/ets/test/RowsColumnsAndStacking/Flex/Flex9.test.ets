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
import { describe, beforeAll, beforeEach, afterEach, afterAll, it, expect } from '@ohos/hypium'
import { abilityDelegatorRegistry, Component, Driver, ON } from '@kit.TestKit';

import {
  buttonClick,
  JsonType,
  KeyStruct,
  radioClick,
  Slide,
  sliderDrag,
  ToggleClick,
  scroll,
  Search,
  searchParentBlock,
  searchRadio,
  titleSearch,
  getToggle,
  searchToggle
} from '../../../utils/index';
import { On, Point } from '@ohos.UiTest';
import { FlexStart } from '../../../utils/RowsColumnsAndStacking/Flex';


/*获取AbilityDelegator实例,管理Ability的生命周期和事件*/
const delegator: abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()

/*初始化与UI元素交互的驱动对象*/
const driver = Driver.create();


export default function Flex9Test() {
  describe('Flex9Test', () => {
    // Defines a test suite. Two parameters are supported: test suite name and test suite function.
    beforeAll(async (done: Function) => {
      await FlexStart(done)
      // Presets an action, which is performed only once before all test cases of the test suite start.
      // This API supports only one parameter: preset action function.
    })
    beforeEach(() => {
      // Presets an action, which is performed before each unit test case starts.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: preset action function.
    })
    afterEach(() => {
      // Presets a clear action, which is performed after each unit test case ends.
      // The number of execution times is the same as the number of test cases defined by **it**.
      // This API supports only one parameter: clear action function.
    })
    afterAll(() => {
      // Presets a clear action, which is performed after all test cases of the test suite end.
      // This API supports only one parameter: clear action function.
    })
    /*
     * 测试组件renderFit支持设置BOTTOM_LEFT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0770', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 7,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.BOTTOM_LEFT'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置BOTTOM_RIGHT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0771', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 8,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.BOTTOM_RIGHT'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_FILL且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0772', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 9,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_FILL'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_CONTAIN且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0773', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 10,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_CONTAIN'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_CONTAIN_TOP_LEFT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0774', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 11,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_CONTAIN_TOP_LEFT'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_CONTAIN_BOTTOM_RIGHT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0775', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 12,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_CONTAIN_BOTTOM_RIGHT'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_COVER且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0776', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 13,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_COVER'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_COVER_TOP_LEFT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0777', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 14,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_COVER_TOP_LEFT'
      })
      done();
    })
    /*
     * 测试组件renderFit支持设置RESIZE_COVER_BOTTOM_RIGHT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0778', 0, async (done: Function) => {
      await titleSearch('基础属性', 'renderFit', '通用属性7')
      await radioClick({
        radioClickObj: 15,
        toggleClick: {
          toggleClickType: ToggleClick.Close
        },
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('renderFit', JsonType.value)
        ],
        assertEqual: 'RenderFit.RESIZE_COVER_BOTTOM_RIGHT'
      })
      done();
    })
    // monopolizeEvents
    /*
     * 测试组件monopolizeEvents支持设置true且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0779', 0, async (done: Function) => {
      await titleSearch('基础属性', 'monopolizeEvents', '通用属性7')
      await radioClick({
        radioClickObj: 0,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('monopolizeEvents', JsonType.value)
        ],
        assertEqual: true
      })
      done();
    })
    /*
     * 测试组件monopolizeEvents支持设置false且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0780', 0, async (done: Function) => {
      await titleSearch('基础属性', 'monopolizeEvents', '通用属性7')
      await radioClick({
        radioClickObj: 1,
        id: "Flex",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('monopolizeEvents', JsonType.value)
        ],
        assertEqual: false
      })
      done();
    })
    // cursorControl
    /*
     * 测试组件cursorControl支持设置DEFAULT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0781', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 0
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0782', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 1
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0783', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 2
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0784', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 3
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0785', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 4
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置WEST_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0786', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 5
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_SOUTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0787', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 6
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0788', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 7
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0789', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 8
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0790', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 9
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0791', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 10
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_EAST_SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0792', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 11
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_WEST_SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0793', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 12
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CROSS且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0794', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 13
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_COPY且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0795', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 14
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_FORBID且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0796', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 15
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置COLOR_SUCKER且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0797', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 16
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_GRABBING且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0798', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 17
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_OPEN且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0799', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 18
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_POINTING且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0800', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 19
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HELP且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0801', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 20
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MOVE且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0802', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 21
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置RESIZE_LEFT_RIGHT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0803', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 22
      })
      done();
    })
    /*
    * 测试组件cursorControl支持设置RESIZE_UP_DOWN且获取属性正确
    * */
    it('ArkUI_Flex_Comp_0804', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'RESIZE_UP_DOWN'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SCREENSHOT_CHOOSE且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0805', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'SCREENSHOT_CHOOSE'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SCREENSHOT_CURSOR且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0806', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'SCREENSHOT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置TEXT_CURSOR且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0807', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'TEXT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置ZOOM_IN且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0808', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'ZOOM_IN'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置ZOOM_OUT且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0809', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'ZOOM_OUT'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0810', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0811', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0812', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0813', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_SOUTH且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0814', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_SOUTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0815', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0816', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0817', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0818', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_SOUTH_WEST_EAST且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0819', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_SOUTH_WEST_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HORIZONTAL_TEXT_CURSOR获取属性正确
     * */
    it('ArkUI_Flex_Comp_0820', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'HORIZONTAL_TEXT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_CROSS且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0821', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'CURSOR_CROSS'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_CIRCLE且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0822', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'CURSOR_CIRCLE'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置LOADING且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0823', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'LOADING'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置RUNNING且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0824', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await radioClick({
        radioClickObj: 'RUNNING'
      })
      done();
    })
    /*
     * 滑到cursorControl位置，关闭按钮，无需断言
     * */
    it('ArkUI_Flex_Comp_0825', 0, async (done: Function) => {
      await titleSearch('基础属性', 'cursorControl', '通用属性7')
      await getToggle(ToggleClick.Close)
      done();
    })
    //useEffect
    /*
     * 测试组件useEffect支持设置true且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0826', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'useEffect', '通用属性7')
      await radioClick({
        radioClickObj: 0,
        toggleClick: {
          toggleClickObj: 'useEffect'
        }
      })
      done();
    })
    /*
     * 测试组件useEffect支持设置true且获取属性正确
     * */
    it('ArkUI_Flex_Comp_0827', 0, async (done: Function) => {
      // 无法获取属性值进行断言
      await titleSearch('基础属性', 'useEffect', '通用属性7')
      await radioClick({
        radioClickObj: 1,
        toggleClick: {
          toggleClickType: ToggleClick.Close,
          toggleClickObj: 'useEffect'
        }
      })
      done();
    })
  })
}