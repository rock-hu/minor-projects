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
import { AvPlayerUtil, BreakpointConstants, CommonConstants, VideoNavPathStack, WindowUtil } from '@ohos/commons';
import { DetailConstants } from '../constants/DetailConstants';

@Component
export struct VideoPlayer {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentHeightBreakpoint') currentHeightBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentTime') currentTime: string = CommonConstants.INITIAL_TIME;
  @StorageLink('totalTime') totalTime: string = CommonConstants.INITIAL_TIME;
  @StorageLink('isHalfFolded') isHalfFolded: boolean = false;
  @StorageLink('creaseRegion') creaseRegion: number[] = [];
  @StorageLink('progress') progress: number = 0;
  @StorageLink('avplayerState') avplayerState: string = '';
  @StorageLink('isFullScreen') isFullScreen: boolean = false;
  @Link videoHeight: number;
  @Link isShowingSideBar: boolean;
  @Link foldStatus: display.FoldStatus;
  @Consume('pageInfo') pageInfo: VideoNavPathStack;
  private windowUtil?: WindowUtil = WindowUtil.getInstance();
  private avPlayerUtil?: AvPlayerUtil;
  private xComponentController: XComponentController = new XComponentController();
  private onFoldStatusChange: Callback<display.FoldStatus> = (data: display.FoldStatus) => {
    this.foldStatus = data;
    if (canIUse('SystemCapability.Window.SessionManager')) {
      if (data === display.FoldStatus.FOLD_STATUS_HALF_FOLDED) {
        let orientation: display.Orientation = display.getDefaultDisplaySync().orientation;
        if (orientation === display.Orientation.LANDSCAPE || orientation === display.Orientation.LANDSCAPE_INVERTED) {
          this.isHalfFolded = true;
          // Full-screen playback.
          if (!this.isFullScreen) {
            this.isFullScreen = true;
          }
        }
      } else {
        this.isHalfFolded = false;
      }
    }
  };

  aboutToAppear(): void {
    this.avPlayerUtil = AvPlayerUtil.getInstance();
    if (canIUse('SystemCapability.Window.SessionManager')) {
      display.on('foldStatusChange', this.onFoldStatusChange);
    }
  }

  aboutToDisappear(): void {
    if (canIUse('SystemCapability.Window.SessionManager')) {
      display.off('foldStatusChange');
    }
  }

  build() {
    // [Start video_player]
    Stack({ alignContent: this.isFullScreen ? Alignment.Center : Alignment.Bottom }) {
      Flex({
        // [StartExclude video_player]
        direction: FlexDirection.Column,
        justifyContent: this.isHalfFolded ? FlexAlign.Start : FlexAlign.Center,
        alignItems: ItemAlign.Start
        // [EndExclude video_player]
      }) {
        Column() {
          // [StartExclude video_player]
          XComponent({
            id: CommonConstants.PAGE_NAMES[1],
            type: XComponentType.SURFACE,
            controller: this.xComponentController
          })
            .onLoad(() => {
              this.avPlayerUtil?.createAvPlayer(this.xComponentController.getXComponentSurfaceId());
              AppStorage.setOrCreate('detailSurfaceId', this.xComponentController.getXComponentSurfaceId());
            })
            .width(this.isFullScreen ? -1 : this.videoHeight + DetailConstants.PERCENT_SIGN)
            .aspectRatio(CommonConstants.VIDEO_ASPECT_RATIO)
          // [EndExclude video_player]
        }
        .justifyContent(FlexAlign.Center)
        .height(this.isHalfFolded ? this.creaseRegion[0] : (this.isFullScreen ? CommonConstants.FULL_PERCENT : 'auto'))
        .width(CommonConstants.FULL_PERCENT)
      }
      .width(CommonConstants.FULL_PERCENT)
      .onClick(() => {
        if (this.isShowingSideBar) {
          this.isShowingSideBar = false;
        } else {
          this.avPlayerUtil?.playerStateControl();
        }
      })
      .priorityGesture(
        TapGesture({ count: 2 })
          .onAction((event: GestureEvent) => {
            if (event && deviceInfo.deviceType === CommonConstants.DEVICE_TYPE) {
              this.isFullScreen = true;
              this.windowUtil!.maximize();
            }
          })
      )
      // [StartExclude video_player]
      Column() {
        Row() {
          TimeText({ time: this.currentTime })
            .margin({
              left: $r('app.float.current_time_left'),
              right: $r('app.float.current_time_right')
            })

          Slider({
            min: 0,
            max: CommonConstants.PROGRESS_HUNDRED,
            step: 1,
            value: this.progress
          })
            .onChange((value: number, mode: SliderChangeMode) => {
              this.avPlayerUtil?.sliderChange(value, mode);
            })
            .layoutWeight(1)
            .selectedColor($r('app.color.selected_color'))

          TimeText({ time: this.totalTime })
            .margin({
              left: $r('app.float.total_time_left'),
              right: $r('app.float.total_time_right')
            })
        }
        .width(CommonConstants.FULL_PERCENT)
        .height($r('app.float.time_row_height'))
        .alignItems(VerticalAlign.Center)
        .visibility(this.isFullScreen ? Visibility.Visible : Visibility.None)

        Row() {
          Row() {
            Image(this.avplayerState === CommonConstants.AV_PLAYER_PLAYING_STATE ? $r('app.media.ic_public_pause')
              : $r('app.media.ic_public_play'))
              .height($r('app.float.icon_size'))
              .width($r('app.float.icon_size'))
              .margin({ left: $r('app.float.icon_margin') })
              .onClick(() => {
                this.avPlayerUtil?.playerStateControl();
              })
            ImgIcon({ img: $r('app.media.ic_public_play_next') })
            ImgIcon({ img: $r('app.media.ic_public_view_list_white') })
          }
          .margin({
            top: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? '0' : $r('app.float.icon_row_top'),
            bottom: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ?
            $r('app.float.icon_row_bottom_sm') : $r('app.float.icon_row_bottom')
          })

          Blank()

          Row() {
            TextButton({ content: DetailConstants.PLAYER_TEXT_LIST[0] })
              .onClick(() => {
                this.isShowingSideBar = !this.isShowingSideBar;
              })
            TextButton({ content: DetailConstants.PLAYER_TEXT_LIST[2] })
            TextButton({ content: DetailConstants.PLAYER_TEXT_LIST[3] })
          }
          .margin({
            top: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ? $r('app.float.button_row_top_sm') :
            $r('app.float.button_row_top'),
            bottom: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM ?
            $r('app.float.button_row_bottom_sm') : $r('app.float.button_row_bottom')
          })
        }
        .height($r('app.float.icon_button_row_height'))
        .width(CommonConstants.FULL_PERCENT)
        .visibility(this.isFullScreen ? Visibility.Visible : Visibility.None)
      }
      .height(this.isFullScreen ? CommonConstants.FULL_PERCENT : 'auto')
      .width(CommonConstants.FULL_PERCENT)
      .justifyContent(FlexAlign.End)
      .visibility(this.isFullScreen && !this.isShowingSideBar ? Visibility.Visible : Visibility.None)
      .priorityGesture(
        TapGesture({ count: 2 })
          .onAction((event: GestureEvent) => {
            if (event && deviceInfo.deviceType === CommonConstants.DEVICE_TYPE) {
              this.windowUtil!.maximize();
              this.windowUtil!.recover();
            }
          })
      )

      Row() {
        TimeText({ time: this.currentTime })
          .margin({
            left: $r('app.float.current_time_text_left'),
            right: $r('app.float.current_time_text_right')
          })

        Slider({
          min: 0,
          max: CommonConstants.PROGRESS_HUNDRED,
          step: 1,
          value: this.progress
        })
          .onChange((value: number, mode: SliderChangeMode) => {
            this.avPlayerUtil?.sliderChange(value, mode);
          })
          .layoutWeight(1)
          .selectedColor($r('app.color.episodes_font'))

        TimeText({ time: this.totalTime })
          .margin({
            left: $r('app.float.total_time_text_left'),
            right: $r('app.float.total_time_text_right')
          })

        Image($r('app.media.ic_public_enlarge'))
          .height($r('app.float.enlarge_size'))
          .width($r('app.float.enlarge_size'))
          .margin({ right: $r('app.float.enlarge_margin') })
          .fillColor(Color.White)
          .onClick(() => {
            this.isFullScreen = true;
          })

        Image($r('app.media.ic_public_fullscreen'))
          .height($r('app.float.enlarge_size'))
          .width($r('app.float.enlarge_size'))
          .margin({ right: $r('app.float.enlarge_margin') })
          .fillColor(Color.White)
          .visibility(deviceInfo.deviceType === CommonConstants.DEVICE_TYPE ? Visibility.Visible : Visibility.None)
          .onClick(() => {
            this.isFullScreen = true;
            this.windowUtil!.maximize();
          })
      }
      .width(CommonConstants.FULL_PERCENT)
      .height($r('app.float.time_row_height'))
      .alignItems(VerticalAlign.Center)
      .visibility(!this.isFullScreen ? Visibility.Visible : Visibility.None)
      // [EndExclude video_player]
      Image($r('app.media.ic_public_back'))
        .height($r('app.float.back_size'))
        .width($r('app.float.back_size'))
        .position({
          x: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG ? $r('app.float.back_position_x') :
          $r('app.float.back_position_x_lg'),
          y: $r('app.float.back_position_y')
        })
        .fillColor(Color.White)
        .onClick(() => {
          if (this.isHalfFolded) {
            this.isHalfFolded = false;
          }
          if (this.isFullScreen) {
            this.isFullScreen = false;
          } else {
            this.pageInfo.setPageName('home');
            this.pageInfo.pop();
          }
        })
    }
    // [End video_player]
    .height('auto')
    .layoutWeight(this.isFullScreen ? 1 : 0)
    .width(CommonConstants.FULL_PERCENT)
    .backgroundColor(Color.Black)
    .focusable(false)
  }
}

@Component
struct ImgIcon {
  public img?: Resource;

  build() {
    Image(this.img ? this.img : '')
      .height($r('app.float.icon_size'))
      .width($r('app.float.icon_size'))
      .margin({ left: $r('app.float.icon_margin') })
  }
}

@Component
struct TextButton {
  public content: ResourceStr = '';

  build() {
    Text(this.content)
      .fontSize($r('app.float.button_text_font'))
      .fontWeight(FontWeight.Normal)
      .fontColor(Color.White)
      .margin({ right: $r('app.float.button_text_margin') })
  }
}

@Component
struct TimeText {
  @Link time: string;

  build() {
    Text(this.time)
      .fontSize($r('app.float.time_font'))
      .fontColor(Color.White)
      .lineHeight($r('app.float.time_text_line'))
      .width($r('app.float.time_text_width'))
  }
}