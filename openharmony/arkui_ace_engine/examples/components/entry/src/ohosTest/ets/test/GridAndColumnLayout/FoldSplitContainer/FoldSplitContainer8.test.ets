/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { describe, beforeAll, it } from '@ohos/hypium'
import {
  JsonType,
  KeyStruct,
  radioClick,
  Slide,
  sliderDrag,
  titleSearch,
  ToggleClick,
  searchToggle,
  getToggle,
  buttonClick
} from '../../../utils';
import { FoldSplitContainerStart } from './FoldSplitContainer';
import { Driver } from '@kit.TestKit';

const driver = Driver.create();

// cursorControl
const CURSOR_CONTROL_18 = 18;

export default function FoldSplitContainer8() {
  describe('FoldSplitContainer8', () => {
    beforeAll(async (done: Function) => {
      await FoldSplitContainerStart(done)
    })
    // obscured
    /*
     * 测试组件obscured支持设置null且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0761', 0, async (done: Function) => {
      //获取不到属性值无法进行断言
      await titleSearch('基础属性', 'obscured', '通用属性6')
      await radioClick({
        radioClickObj: 0
      })
      done();
    })
    /*
     * 测试组件obscured支持设置PLACEHOLDER且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0762', 0, async (done: Function) => {
      //获取不到属性值无法进行断言
      await titleSearch('基础属性', 'obscured', '通用属性6')
      await radioClick({
        radioClickObj: 1,
        toggleClick: {
          toggleClickType: ToggleClick.Close
        }
      })
      done();
    })
    // monopolizeEvents
    /*
     * 测试组件monopolizeEvents支持设置true且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0779', 0, async (done: Function) => {
      await titleSearch('基础属性', 'monopolizeEvents', '通用属性6')
      await radioClick({
        radioClickObj: 0,
        id: "FoldSplitContainer",
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
    it('ArkUI_FoldSplitContainer_Comp_0780', 0, async (done: Function) => {
      await titleSearch('基础属性', 'monopolizeEvents', '通用属性6')
      await radioClick({
        radioClickObj: 1,
        id: "FoldSplitContainer",
        arr: [
          new KeyStruct('$attrs', JsonType.obj),
          new KeyStruct('monopolizeEvents', JsonType.value)
        ],
        assertEqual: false
      })
      done();
    })
    //cursorControl
    /*
     * 测试组件cursorControl支持设置DEFAULT且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0781', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 0
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0782', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 1
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0783', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 2
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0784', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 3
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0785', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 4
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置WEST_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0786', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 5
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_SOUTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0787', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 6
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0788', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 7
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0789', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 8
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0790', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 9
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0791', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 10
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_EAST_SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0792', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 11
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置NORTH_WEST_SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0793', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 12
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CROSS且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0794', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 13
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_COPY且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0795', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 14
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_FORBID且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0796', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 15
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置COLOR_SUCKER且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0797', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 16
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_GRABBING且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0798', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 17
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_OPEN且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0799', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: CURSOR_CONTROL_18
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HAND_POINTING且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0800', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 19
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HELP且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0801', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 20
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MOVE且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0802', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 21
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置RESIZE_LEFT_RIGHT且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0803', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 22
      })
      done();
    })
    /*
    * 测试组件cursorControl支持设置RESIZE_UP_DOWN且获取属性正确
    * */
    it('ArkUI_FoldSplitContainer_Comp_0804', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'RESIZE_UP_DOWN'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SCREENSHOT_CHOOSE且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0805', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'SCREENSHOT_CHOOSE'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置SCREENSHOT_CURSOR且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0806', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'SCREENSHOT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置TEXT_CURSOR且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0807', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'TEXT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置ZOOM_IN且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0808', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'ZOOM_IN'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置ZOOM_OUT且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0809', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'ZOOM_OUT'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0810', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0811', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0812', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0813', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_SOUTH且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0814', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_SOUTH'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0815', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0816', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0817', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_SOUTH_WEST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0818', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_SOUTH_WEST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置MIDDLE_BTN_NORTH_SOUTH_WEST_EAST且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0819', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'MIDDLE_BTN_NORTH_SOUTH_WEST_EAST'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置HORIZONTAL_TEXT_CURSOR获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0820', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'HORIZONTAL_TEXT_CURSOR'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_CROSS且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0821', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'CURSOR_CROSS'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置CURSOR_CIRCLE且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0822', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'CURSOR_CIRCLE'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置LOADING且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0823', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'LOADING'
      })
      done();
    })
    /*
     * 测试组件cursorControl支持设置RUNNING且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0824', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await radioClick({
        radioClickObj: 'RUNNING'
      })
      done();
    })
    /*
     * 滑到cursorControl位置，关闭按钮，无需断言
     * */
    it('ArkUI_FoldSplitContainer_Comp_0825', 0, async (done: Function) => {
      await titleSearch('基础属性', 'cursorControl', '通用属性6')
      await getToggle(ToggleClick.Close)
      done();
    })
    //useEffect
    /*
     * 测试组件useEffect支持设置true且获取属性正确
     * */
    it('ArkUI_FoldSplitContainer_Comp_0826', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'useEffect', '通用属性6')
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
    it('ArkUI_FoldSplitContainer_Comp_0827', 0, async (done: Function) => {
      //无法获取属性值进行断言
      await titleSearch('基础属性', 'useEffect', '通用属性6')
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