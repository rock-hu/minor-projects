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

import { LengthMetrics } from '@kit.ArkUI';
import {
  ArcList,
  ArcListItem,
  ArcListAttribute // The properties of ArcList depend on the ArcListAttribute object for import.
} from '@kit.ArkUI';
import { StyleConstants } from '../constants/StyleConstants';

class Menu {
  icon: Resource;
  text: Resource;
  pathName: string;

  constructor(icon: Resource, text: Resource, pathName: string) {
    this.icon = icon;
    this.text = text;
    this.pathName = pathName;
  }
}

@Preview
@Component
export struct Home {
  @Consume pageStack: NavPathStack;
  readonly HOME_BTN_WIDTH: string = '90%';
  private menuList: Menu[] = [
    new Menu(
      $r('app.media.hottest_playlists'),
      $r('app.string.home_hottest_playlists'),
      'playList'
    ),
    new Menu(
      $r('app.media.my_favorite'),
      $r('app.string.home_my_favorite'),
      'songList'
    ),
    new Menu(
      $r('app.media.is_playing'),
      $r('app.string.home_is_playing'),
      'songPage'
    ),
    new Menu(
      $r('app.media.setting'),
      $r('app.string.home_setting'),
      'setting'
    )
  ];

  build() {
    // [Start home_list]
    Column() {
      ArcList({ initialIndex: 0 }) {
        ForEach(this.menuList, (item: Menu) => {
          ArcListItem() {
            Row() {
              Image(item.icon)
                .width($r('app.float.home_icon_width'))
                .height($r('app.float.home_icon_width'))
                .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)
                .backgroundColor($r('app.color.home_icon_background'))
                .padding($r('app.float.home_icon_padding'))

              Text(item.text)
                .fontColor($r('app.color.font_color'))
                .fontSize($r('app.float.home_font_size'))

              Image($r('app.media.chevron_right'))
                .width($r('app.float.home_icon_jump_width'))
            }
            .width(this.HOME_BTN_WIDTH)
            .height($r('app.float.home_btn_height'))
            .padding({ left: $r('app.float.list_btn_padding'), right: $r('app.float.list_btn_padding') })
            .justifyContent(FlexAlign.SpaceBetween)
            .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)
            .backgroundColor($r('app.color.home_btn_background'))
            // [StartExclude home_list]
            .onClick(() => {
              if (item.pathName === 'setting') {
                return;
              }
              this.pageStack.replacePathByName(item.pathName, null);
            })

            // [EndExclude home_list]
          }
        }, (item: Menu, index: number) => JSON.stringify(item) + index)
      }
      .scrollBar(BarState.Off)
      .space(LengthMetrics.vp(5))
      .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)
      .focusable(true)
      .focusOnTouch(true)
      .defaultFocus(true)
    }
    .align(Alignment.Center)
    .width(StyleConstants.FULL_WIDTH)
    .height(StyleConstants.FULL_HEIGHT)
    .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)

    // [End home_list]
  }
}