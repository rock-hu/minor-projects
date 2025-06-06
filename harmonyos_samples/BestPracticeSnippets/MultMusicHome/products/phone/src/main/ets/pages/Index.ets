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

import { BreakpointConstants, RouterUrlConstants, StyleConstants } from '@ohos/constantsCommon';
import { LivePage } from '@ohos/live';
import { MusicListPage } from '@ohos/musicList';
import { MusicCommentPage } from '@ohos/musicComment';
import IndexItem from '../viewmodel/IndexItem';
import IndexViewModel from '../viewmodel/IndexViewModel';
import { HomeConstants } from '../common/constants/HomeConstants';
// [Start title]
@Entry
@Component
struct Index {
  // [StartExclude title]
  @State indexItemList: IndexItem[] = IndexViewModel.getIndexItemList();
  @StorageLink('pageIndexInfos') pageIndexInfos: NavPathStack = new NavPathStack();

  @Builder
  PagesMap(name: string, param?: object) {
    if (name === RouterUrlConstants.LIVE) {
      NavDestination() {
        LivePage()
      }
      .hideTitleBar(true)
    } else if (name === RouterUrlConstants.MUSIC_LIST) {
      NavDestination() {
        MusicListPage()
      }
      .hideTitleBar(true)
    } else if (name === RouterUrlConstants.MUSIC_COMMENT) {
      NavDestination() {
        MusicCommentPage()
      }
      .hideTitleBar(true)
    }
  }
  // [EndExclude title]
  build() {
    Navigation(this.pageIndexInfos) {
      GridRow({
        breakpoints: {
          value: BreakpointConstants.BREAKPOINT_VALUE,
          reference: BreakpointsReference.WindowSize
        },
        columns: {
          sm: BreakpointConstants.COLUMN_SM,
          md: BreakpointConstants.COLUMN_MD,
          lg: BreakpointConstants.COLUMN_LG
        },
        gutter: { x: BreakpointConstants.GUTTER_X },
        direction: GridRowDirection.Row
      }) {
        GridCol({
          span: {
            sm: BreakpointConstants.SPAN_SM,
            md: BreakpointConstants.SPAN_MD,
            lg: BreakpointConstants.SPAN_LG
          },
          offset: {
            md: BreakpointConstants.OFFSET_MD,
            lg: BreakpointConstants.OFFSET_LG
          }
        }) {
          Column({ space: HomeConstants.COLUMN_SPACE }) {
            ForEach(this.indexItemList, (item: IndexItem) => {
              Column() {
                Text(item.title)
                  // [StartExclude title]
                  .fontSize($r('app.float.title_font_size'))
                  .fontColor(Color.White)
                // [EndExclude title]
                Text(item.description)
                  // [StartExclude title]
                  .fontSize($r('app.float.description_font_size'))
                  .opacity(HomeConstants.TEXT_OPACITY)
                  .fontColor(Color.White)
                  .margin({
                    top: $r('app.float.description_margin_top')
                  })
                // [EndExclude title]
                Blank()
                Column() {
                  Button() {
                    Text(item.button)
                      // [StartExclude title]
                      .fontSize($r('app.float.button_font_size'))
                      .fontColor(Color.White)
                    // [EndExclude title]
                  }
                  // [StartExclude title]
                  .backgroundColor($r('app.color.button_background_color'))
                  .borderRadius($r('app.float.button_border_radius'))
                  .width($r('app.float.button_width'))
                  .height($r('app.float.button_height'))
                  .onClick(() => {
                    this.pageIndexInfos.pushPathByName(item.url, null);
                  })
                  // [EndExclude title]
                }
                // [StartExclude title]
                .alignItems(HorizontalAlign.End)
                .width(StyleConstants.FULL_WIDTH)
                // [EndExclude title]
              }
              // [StartExclude title]
              .width(StyleConstants.FULL_WIDTH)
              .height($r('app.float.item_height'))
              .backgroundImage(item.icon)
              .backgroundImageSize({
                width: StyleConstants.FULL_WIDTH,
                height: $r('app.float.item_height')
              })
              .borderRadius($r('app.float.item_border_radius'))
              .padding($r('app.float.item_padding'))
              .alignItems(HorizontalAlign.Start)
              .justifyContent(FlexAlign.SpaceBetween)
              // [EndExclude title]
            }, (item: IndexItem, index?: number) => index + JSON.stringify(item))
          }
        }
      }
      // [StartExclude title]
      .padding({
        top: $r('app.float.column_padding_top'),
        left: $r('app.float.column_padding_left'),
        right: $r('app.float.column_padding_right')
      })
      // [EndExclude title]
    }
    // [StartExclude title]
    .mode(NavigationMode.Stack)
    .navDestination(this.PagesMap)
    .height(StyleConstants.FULL_HEIGHT)
    .hideTitleBar(true)
    .hideToolBar(true)
    // [EndExclude title]
  }
}
// [End title]