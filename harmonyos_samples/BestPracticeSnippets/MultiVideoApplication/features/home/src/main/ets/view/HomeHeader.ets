/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { deviceInfo } from '@kit.BasicServicesKit';
import { BreakpointConstants, CommonConstants } from '@ohos/commons';
import { BreakpointType } from '@ohos/commons';
import { HomeConstants } from '../constants/HomeConstants';
import { getTabIndex } from './CommonView';

@Component
export struct HomeHeader {
  @State isShowingBorder: boolean = false;
  @Link isSearching: boolean;
  @StorageLink('scrollHeight') scrollHeight: number = 0;
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string =  BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentTopIndex') currentTopIndex: number = 0;
  // [Start home_header_build]
  build() {
    // [Start home_header]
    Column() {
      GridRow({
        columns: {
          sm: BreakpointConstants.GRID_ROW_COLUMNS[2],
          md: BreakpointConstants.GRID_ROW_COLUMNS[0],
          lg: BreakpointConstants.GRID_ROW_COLUMNS[0]
        }
      }) {
        GridCol({
          span: {
            sm: BreakpointConstants.GRID_COLUMN_SPANS[5],
            md: BreakpointConstants.GRID_COLUMN_SPANS[2],
            lg: BreakpointConstants.GRID_COLUMN_SPANS[2]
          }
        }) {
          this.TopTabBar()
        }
        .padding({
          top: deviceInfo.deviceType === CommonConstants.DEVICE_TYPE ? 0 : $r('app.float.search_top_padding_top')
        })
        .height(deviceInfo.deviceType === CommonConstants.DEVICE_TYPE ? $r('app.float.search_top_height') :
        $r('app.float.search_top_height_more'))

        GridCol({
          span: {
            sm: BreakpointConstants.GRID_COLUMN_SPANS[5],
            md: BreakpointConstants.GRID_COLUMN_SPANS[3],
            lg: BreakpointConstants.GRID_COLUMN_SPANS[3]
          }
        }) {
          this.searchBar()
        }
        .padding({ top: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM || deviceInfo.deviceType ===
          CommonConstants.DEVICE_TYPE ? 0 : $r('app.float.search_top_padding_top') })
        .height(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM || deviceInfo.deviceType ===
          CommonConstants.DEVICE_TYPE ? $r('app.float.search_top_height') : $r('app.float.search_top_height_more'))
      }
      // The background color of the top tab bar is changed during the slide-down process.
      .backgroundColor(this.scrollHeight >= new BreakpointType(HomeConstants.BACKGROUND_CHANGE_HEIGHT[0],
        HomeConstants.BACKGROUND_CHANGE_HEIGHT[1], HomeConstants.BACKGROUND_CHANGE_HEIGHT[2])
        .getValue(this.currentWidthBreakpoint) && this.currentTopIndex === 2 ? $r('app.color.home_content_background') :
        Color.Transparent)
    }
    .width(CommonConstants.FULL_PERCENT)
    // [End home_header]
  }
  // [End home_header_build]

  @Builder
  // [Start home_header_tap_bar]
  TopTabBar() {
    Row() {
      Column() {
        List() {
          ForEach(HomeConstants.TOP_TOPIC_LIST, (item: string, index: number) => {
            // [Start home_header_tap_bar]
            ListItem() {
              Text(item)
                .fontSize(this.currentTopIndex === index ? $r('app.float.search_tab_font_selected') :
                  $r('app.float.search_tab_font'))
                .fontWeight(this.currentTopIndex === index ? CommonConstants.FONT_WEIGHT_700 :
                  CommonConstants.FONT_WEIGHT_500)
                .width(this.currentTopIndex === index ? $r('app.float.top_text_width_selected') :
                  $r('app.float.top_text_width'))
                .textAlign(TextAlign.Center)
                .fontColor(this.currentTopIndex === 2 ? Color.White : $r('app.color.normal_font_color'))
                .lineHeight(index === this.currentTopIndex ? $r('app.float.top_tab_list_height_selected') :
                  $r('app.float.top_tab_list_height'))
                .focusable(true)
                .groupDefaultFocus(index === 0 ? true : false)
                .borderRadius($r('app.float.text_focus_radius'))
            }
            .align(Alignment.Center)
            .margin({
              top: this.currentTopIndex === index ? $r('app.float.top_tab_list_top_selected') :
                $r('app.float.top_tab_list_top'),
              bottom: this.currentTopIndex === index ? $r('app.float.top_tab_list_bottom_selected') :
                $r('app.float.top_tab_list_bottom')
            })
            .padding({
              left: new BreakpointType(8, 12, 18).getValue(this.currentWidthBreakpoint),
              right: new BreakpointType(8, 12, 18).getValue(this.currentWidthBreakpoint)
            })
            .onClick(() => {
              this.currentTopIndex = index;
              this.scrollHeight = 0;
            })
            // [End home_header_tap_bar]
          }, (item: number, index: number) => index + JSON.stringify(item))
        }
        .tabIndex(getTabIndex(HomeConstants.DIRECTION_LIST[1]))
        .scrollBar(BarState.Off)
        .listDirection(Axis.Horizontal)
        .padding({ left: $r('app.float.search_top_padding') })
      }
      .alignItems(HorizontalAlign.Center)
      .layoutWeight(1)
      // [Start home_header_tap_bar]
      Image($r('app.media.ic_public_more'))
        .width($r('app.float.top_tab_img_size'))
        .height($r('app.float.top_tab_img_size'))
        .margin({
          left: $r('app.float.top_tab_img_margin'),
          right: $r('app.float.top_tab_img_margin')
        })
        .fillColor(this.currentTopIndex === 2 ? Color.White : Color.Black)
        .visibility(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? Visibility.Visible : Visibility.None)
      // [Start home_header_tap_bar]
    }
    .height($r('app.float.top_bar_height'))
    .width(CommonConstants.FULL_PERCENT)
  }
  // [End home_header_tap_bar]
  // [Start home_header_search_bar]
  @Builder
  searchBar() {
    Row() {
      Stack({ alignContent: Alignment.Start }) {
        // [[StartExclude home_header_search_bar]
        TextInput({ placeholder: HomeConstants.SEARCH_PLACEHOLDER })
          .placeholderFont({
            size: $r('app.float.search_input_font'),
            weight: FontWeight.Normal
          })
          .placeholderColor(this.currentTopIndex === 2 ? Color.White : $r('app.color.normal_font_color'))
          .height($r('app.float.search_input_height'))
          .fontSize($r('app.float.search_input_font'))
          .padding({
            left: $r('app.float.search_input_left'),
            right: $r('app.float.search_input_right')
          })
          .width(CommonConstants.FULL_PERCENT)
          .onClick(() => {
            this.isSearching = true;
          })
          .onSubmit((enterKey: EnterKeyType) => {
            if (enterKey === EnterKeyType.Go) {
              this.isSearching = true;
            }
          })
          .enterKeyType(EnterKeyType.Go)
          .enableKeyboardOnFocus(false)
          .backgroundColor(this.currentTopIndex === 2 ? $r('app.color.search_bar_background') : '')
          .tabIndex(getTabIndex(HomeConstants.DIRECTION_LIST[1]))

        // Search image.
        Image(this.currentTopIndex === 2 ? $r("app.media.ic_public_search_white") :
          $r('app.media.ic_public_search'))
          .width($r('app.float.search_img_size'))
          .height($r('app.float.search_img_size'))
          .margin({ left: $r('app.float.search_img_left') })
        // [EndExclude home_header_search_bar]
      }
      // [StartExclude home_header_search_bar]
      .alignSelf(ItemAlign.Center)
      .layoutWeight(1)
      // [EndExclude home_header_search_bar]
      // [StartExclude home_header_search_bar]
      // History image.
      Image(this.currentTopIndex === 2 ? $r('app.media.ic_public_history_white') : $r('app.media.ic_public_history'))
        .width($r('app.float.history_img_size'))
        .height($r('app.float.history_img_size'))
        .margin({ left: $r('app.float.history_img_left') })

      Image(this.currentTopIndex === 2 ? $r('app.media.ic_public_more_white') : $r('app.media.ic_public_more'))
        .width($r('app.float.history_img_size'))
        .height($r('app.float.history_img_size'))
        .margin({ left: $r('app.float.history_img_left') })
        .visibility(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? Visibility.None : Visibility.Visible)
      // [EndExclude home_header_search_bar]
    }
    .padding({
      left: $r('app.float.search_top_padding'),
      right: $r('app.float.search_top_padding')
    })
    .justifyContent(FlexAlign.Center)
    .width(CommonConstants.FULL_PERCENT)
    .height(CommonConstants.FULL_PERCENT)
  }
  // [End home_header_search_bar]
}