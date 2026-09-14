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
  ArcListAttribute, // The properties of ArcList depend on ArcListAttribute and ArcListItemAttribute objects for import.
  ArcListItemAttribute
} from '@kit.ArkUI';
import { MediaService, SongItem } from 'mediacommon';
import { StyleConstants } from '../constants/StyleConstants';

@Builder
export function SongListBuilder() {
  SongList();
}

@Preview
@Component
struct SongList {
  readonly HOME_BTN_WIDTH: string = '90%';
  @Consume pageStack: NavPathStack;
  @StorageLink('songList') songList: SongItem[] = [];

  aboutToAppear(): void {
    MediaService.getInstance();
  }

  build() {
    NavDestination() {
      // [Start song_list]
      Column() {
        ArcList({ initialIndex: 0 }) {
          ForEach(this.songList, (item: SongItem, index: number) => {
            ArcListItem() {
              Row() {
                Image(item.label)
                  .width($r('app.float.home_icon_width'))
                  .height($r('app.float.home_icon_width'))
                  .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)

                Column() {
                  Text(item.title)
                    .fontWeight(FontWeight.Bold)
                    .fontColor($r('app.color.font_color'))
                  Text(item.singer)
                    .fontColor($r('app.color.text_color'))
                }
                .layoutWeight(1)
              }
              .width(this.HOME_BTN_WIDTH)
              .height($r('app.float.home_btn_height'))
              .padding({ left: $r('app.float.list_btn_padding'), right: $r('app.float.list_btn_padding') })
              .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)
              .focusable(true)
              .focusOnTouch(true)
              .backgroundColor($r('app.color.home_btn_background'))
            }
            .align(Alignment.Center)
            // [StartExclude song_list]
            .onClick(async () => {
              await MediaService.getInstance().loadAssent(index);
              this.pageStack.replacePathByName('songPage', null);
            })

            // [EndExclude song_list]
          }, (item: SongItem, index: number) => JSON.stringify(item) + index)
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

      // [End song_list]
    }
    .hideTitleBar(true)
  }
}