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

import { display } from '@kit.ArkUI';
import { deviceInfo } from '@kit.BasicServicesKit';
import { BreakpointConstants, CommonConstants } from '@ohos/commons';
import { DetailConstants } from '../constants/DetailConstants';

@Component
export struct FooterEpisodes {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentHeightBreakpoint') currentHeightBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('isFullScreen') isFullScreen: boolean = false;
  @Link isShowingSideBar: boolean;
  @Link foldStatus: display.FoldStatus;

  build() {
    Column() {
      // Selection bottom bar.
      Row() {
        Text(DetailConstants.PLAYER_TEXT_LIST[0])
          .fontSize($r('app.float.title_selected_font'))
          .fontColor(Color.White)
          .fontWeight(CommonConstants.FONT_WEIGHT_500)
          .lineHeight($r('app.float.title_selected_line'))
          .width($r('app.float.title_selected_width'))
          .margin({ right: $r('app.float.title_selected_margin') })
        Text(DetailConstants.PLAYER_TEXT_LIST[1])
          .fontSize($r('app.float.title_font'))
          .fontColor(Color.White)
          .fontWeight(FontWeight.Normal)
          .lineHeight($r('app.float.title_line'))
          .width($r('app.float.title_width'))
          .opacity(CommonConstants.TEXT_OPACITY[2])
      }
      .margin({
        top: $r('app.float.title_row_top'),
        bottom: $r('app.float.title_row_bottom')
      })
      .width(CommonConstants.FULL_PERCENT)
      .height($r('app.float.title_row_height'))

      List({ space: CommonConstants.LIST_SPACE }) {
        ForEach(DetailConstants.PLAYER_EPISODE, (item: string, index: number) => {
          ListItem() {
            Row() {
              Text(item)
                .fontSize($r('app.float.title_font'))
                .fontColor(index === 1 ? $r('app.color.font_selected') : Color.White)
                .fontWeight(FontWeight.Normal)

              Image($r('app.media.video_playing'))
                .height($r('app.float.playing_size'))
                .width($r('app.float.playing_size'))
                .position({
                  x: $r('app.float.playing_position_x'),
                  y: $r('app.float.playing_position_y')
                })
                .visibility(index === 1 ? Visibility.Visible : Visibility.None)
            }
            .justifyContent(FlexAlign.Center)
            .borderRadius($r('app.float.episode_row_radius'))
            .backgroundColor($r('app.color.episode_row_background'))
            .width(CommonConstants.FULL_PERCENT)
            .height($r('app.float.episode_row_height'))
          }
        }, (item: string, index: number) => index + JSON.stringify(item))
      }
      .edgeEffect(EdgeEffect.None)
      .scrollBar(BarState.Off)
      .width(CommonConstants.FULL_PERCENT)
      .layoutWeight(1)
      .padding({ bottom: $r('app.float.episode_list_bottom') })
      .lanes(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD ? DetailConstants.EPISODE_LIST_LANES[0] :
        DetailConstants.EPISODE_LIST_LANES[1], $r('app.float.episode_list_lanes_space'))
    }
    .layoutWeight(1)
    .width(CommonConstants.FULL_PERCENT)
    .visibility(this.isShowingFooter())
    .backgroundColor(Color.Black)
    .padding({
      left: $r('app.float.episode_col_padding'),
      right: $r('app.float.episode_col_padding')
    })
  }

  isShowingFooter(): Visibility {
    if (!this.isShowingSideBar || !this.isFullScreen) {
      return Visibility.None;
    }
    if (deviceInfo.deviceType === CommonConstants.DEVICE_TYPE) {
      return this.currentWidthBreakpoint !== BreakpointConstants.BREAKPOINT_LG ? Visibility.Visible : Visibility.None;
    }
    if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint !==
      BreakpointConstants.BREAKPOINT_SM) {
      return Visibility.Visible;
    }
    return Visibility.None;
  }
}