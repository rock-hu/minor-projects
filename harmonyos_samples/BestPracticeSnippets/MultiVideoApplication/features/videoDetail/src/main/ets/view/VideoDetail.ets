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

import { KeyCode } from '@kit.InputKit';
import { deviceInfo } from '@kit.BasicServicesKit';
import { display, window } from '@kit.ArkUI';
import {
  AvPlayerUtil,
  DeviceScreen,
  Logger,
  WindowUtil,
  BreakpointConstants,
  CommonConstants,
  DisplayUtil,
  VideoNavPathStack
} from '@ohos/commons';
import { SelfComment } from './SelfComment';
import { AllComments } from './AllComments';
import { VideoDetailView } from './VideoDetailView';
import { DetailConstants } from '../constants/DetailConstants';

@Component
export struct VideoDetail {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentHeightBreakpoint') currentHeightBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('windowWidth') windowWidth: number = 0;
  @StorageLink('isHalfFolded') @Watch('onHalfFoldedChange') isHalfFolded: boolean = false;
  @StorageLink('avplayerState') avplayerState: string = '';
  @StorageLink('isFullScreen') @Watch('onFullScreenChange') isFullScreen: boolean = false;
  @Consume('pageInfo') pageInfo: VideoNavPathStack;
  @State commentImgHeight: string = DetailConstants.INITIAL_COMMENT_IMAGE_HEIGHT;
  @State commentImgWidth: string = DetailConstants.INITIAL_COMMENT_IMAGE_WIDTH;
  @State relatedVideoHeight: number = DetailConstants.INITIAL_RELATED_VIDEO_HEIGHT;
  @State videoHeight: number = DetailConstants.INITIAL_VIDEO_HEIGHT;
  private avPlayerUtil?: AvPlayerUtil = AvPlayerUtil.getInstance();
  public screenHeight: number = 0;
  private windowUtil?: WindowUtil = WindowUtil.getInstance();
  private mainWindow?: window.Window;
  // [Start on_window_size_change]
  private onWindowSizeChange: (windowSize: window.Size) => void = (windowSize: window.Size) => {
    // [StartExclude on_window_size_change]
    if (this.pageInfo.getPageName() !== CommonConstants.PAGE_NAMES[1]) {
      return;
    }
    // [EndExclude on_window_size_change]
    if (((this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint !==
    BreakpointConstants.BREAKPOINT_SM) || this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) &&
      !this.isHalfFolded) {
      this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
    }
    // [StartExclude on_window_size_change]
    else if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint ===
    BreakpointConstants.BREAKPOINT_SM) {
      if (this.isFullScreen) {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE_RESTRICTED);
      } else {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.PORTRAIT);
      }
    } else if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM && this.currentHeightBreakpoint ===
    BreakpointConstants.BREAKPOINT_LG) {
      if (this.isFullScreen) {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE_RESTRICTED);
      } else {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.PORTRAIT);
      }
    }
    // [EndExclude on_window_size_change]
  };
  // [End on_window_size_change]
  private onHalfFoldedChange(): void {
    if (this.isHalfFolded) {
      this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE_RESTRICTED);
    } else {
      if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint ===
      BreakpointConstants.BREAKPOINT_MD) {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
      }
    }
  }
  // [Start on_full_screen_change]
  private onFullScreenChange(): void {
    if (((this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint !==
    BreakpointConstants.BREAKPOINT_SM) || this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) &&
      !this.isHalfFolded) {
      this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
    } else if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM && this.currentHeightBreakpoint ===
    BreakpointConstants.BREAKPOINT_LG) {
      if (this.isFullScreen) {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE_RESTRICTED);
      } else {
        this.windowUtil?.setMainWindowOrientation(window.Orientation.PORTRAIT);
      }
    } else if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && this.currentHeightBreakpoint ===
    BreakpointConstants.BREAKPOINT_SM && !this.isFullScreen) {
      this.windowUtil?.setMainWindowOrientation(window.Orientation.PORTRAIT);
    }
    if (deviceInfo.deviceType !== CommonConstants.DEVICE_TYPE) {
      if (this.isFullScreen) {
        this.windowUtil!.disableWindowSystemBar();
      } else {
        this.windowUtil!.enableWindowSystemBar();
      }
    }
  }
  // [End quick_start]
  // [Start on_full_screen_change]
  aboutToAppear() {
    // [StartExclude about_to_appear]
    DisplayUtil.getFoldCreaseRegion();
    this.screenHeight = DeviceScreen.getDeviceHeight();
    this.mainWindow = this.windowUtil!.getMainWindow();
    // [EndExclude about_to_appear]
    this.mainWindow?.on('windowSizeChange', this.onWindowSizeChange);
    if (this.currentWidthBreakpoint !== BreakpointConstants.BREAKPOINT_SM) {
      this.windowUtil!.setMainWindowOrientation(window.Orientation.AUTO_ROTATION_RESTRICTED);
    }
  }
  // [End about_to_appear]

  async aboutToDisappear() {
    this.isFullScreen = false;
    this.avPlayerUtil?.offTimeUpdate();
    await this.avPlayerUtil?.release();

    if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) {
      this.windowUtil!.setMainWindowOrientation(window.Orientation.LANDSCAPE);
    } else {
      this.windowUtil!.setMainWindowOrientation(window.Orientation.PORTRAIT);
    }
  }

  build() {
    NavDestination() {
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
            md: BreakpointConstants.GRID_COLUMN_SPANS[0],
            lg: BreakpointConstants.GRID_COLUMN_SPANS[0]
          }
        }) {
          // [Start video_detail]
          SideBarContainer() {
            Column() {
              // Sidebar area.
              // [StartExclude VideoDetail]
              Scroll() {
                AllComments({ commentImgHeight: $commentImgHeight, commentImgWidth: $commentImgWidth })
                  .visibility(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG ? Visibility.Visible :
                  Visibility.None)
              }
              .align(Alignment.Top)
              .scrollBar(BarState.Off)
              .layoutWeight(1)
              .width(CommonConstants.FULL_PERCENT)
              .padding({ bottom: $r('app.float.side_scroll_padding') })

              SelfComment()
                .visibility(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG ? Visibility.Visible :
                Visibility.None)
              // [EndExclude VideoDetail]
            }
            // [StartExclude VideoDetail]
            .justifyContent(FlexAlign.Start)
            .height(CommonConstants.FULL_PERCENT)
            .width(CommonConstants.FULL_PERCENT)
            .backgroundColor(Color.White)
            // [EndExclude VideoDetail]
            .onAreaChange((newValue: Area) => {
              if (newValue.width !== 0) {
                // Handling when the width of the sidebar changes.
                // [StartExclude VideoDetail]
                let height: number = DetailConstants.COMMENT_IMAGE_MIN_HEIGHT_NUMBER + (Number(newValue.width) -
                DetailConstants.SIDE_BAR_MIN_WIDTH_NUMBER) /
                  (this.getUIContext().px2vp(this.windowWidth) * DetailConstants.COMMENTS_AREA_PERCENT -
                  DetailConstants.SIDE_BAR_MIN_WIDTH_NUMBER) * (DetailConstants.COMMENT_IMAGE_MAX_HEIGHT_NUMBER -
                DetailConstants.COMMENT_IMAGE_MIN_HEIGHT_NUMBER);
                let width: number = DetailConstants.COMMENT_IMAGE_MIN_WIDTH_NUMBER + (Number(newValue.width) -
                DetailConstants.SIDE_BAR_MIN_WIDTH_NUMBER) /
                  (this.getUIContext().px2vp(this.windowWidth) * DetailConstants.COMMENTS_AREA_PERCENT -
                  DetailConstants.SIDE_BAR_MIN_WIDTH_NUMBER) * (DetailConstants.COMMENT_IMAGE_MAX_WIDTH_NUMBER -
                DetailConstants.COMMENT_IMAGE_MIN_WIDTH_NUMBER);
                this.commentImgHeight = JSON.stringify(height);
                this.commentImgWidth = JSON.stringify(width);
                // [EndExclude VideoDetail]
              }
            })

            Column() {
              // Content area.
              // [StartExclude VideoDetail]
              VideoDetailView({
                screenHeight: this.screenHeight,
                relatedVideoHeight: this.relatedVideoHeight,
                videoHeight: this.videoHeight
              })
                .layoutWeight(1)
              SelfComment()
                .visibility(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG || this.isFullScreen ?
                Visibility.None : Visibility.Visible)
              // [EndExclude VideoDetail]
            }
            // [StartExclude VideoDetail]
            .height(CommonConstants.FULL_PERCENT)
            .width(CommonConstants.FULL_PERCENT)
            // [EndExclude VideoDetail]
          }
          .showSideBar(this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG && !this.isFullScreen ? true :
            false)
          // [StartExclude VideoDetail]
          .showControlButton(false)
          .autoHide(false)
          .sideBarPosition(SideBarPosition.End)
          // [EndExclude VideoDetail]
          .sideBarWidth($r('app.float.side_bar_min_width'))
          .minSideBarWidth($r('app.float.side_bar_min_width'))
          .maxSideBarWidth(this.getUIContext().px2vp(this.windowWidth * DetailConstants.COMMENTS_AREA_PERCENT))
          // [End video_detail]
        }
        .height(CommonConstants.FULL_PERCENT)
      }
      .width(CommonConstants.FULL_PERCENT)
      .height(CommonConstants.FULL_PERCENT)
      .onBreakpointChange((breakPoints) => {
        if (breakPoints !== BreakpointConstants.BREAKPOINT_LG &&
          this.videoHeight < DetailConstants.INITIAL_VIDEO_HEIGHT) {
          this.relatedVideoHeight = 0;
        } else if (breakPoints === BreakpointConstants.BREAKPOINT_LG) {
          this.relatedVideoHeight = DetailConstants.INITIAL_RELATED_VIDEO_HEIGHT;
        } else {
          Logger.info(`No specific function`);
        }
      })
    }
    .hideTitleBar(true)
    .onShown(() => {
      if (this.avplayerState !== CommonConstants.AV_PLAYER_PLAYING_STATE) {
        this.avPlayerUtil!.playerStateControl();
      }
      if (canIUse('SystemCapability.Window.SessionManager')) {
        if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD && display.isFoldable()) {
          this.isHalfFolded = false;
        }
      }
    })
    .onKeyEvent((event?: KeyEvent) => {
      if (!event || event.type !== KeyType.Down) {
        return;
      }
      if (event.keyCode === KeyCode.KEYCODE_SPACE) {
        this.avPlayerUtil!.playerStateControl();
      }
      if (event.keyCode === KeyCode.KEYCODE_ESCAPE) {
        this.windowUtil!.recover();
      }
      if (event.keyCode === KeyCode.KEYCODE_DPAD_RIGHT) {
        this.avPlayerUtil!.fastForward();
      }
      if (event.keyCode === KeyCode.KEYCODE_DPAD_LEFT) {
        this.avPlayerUtil!.rewind();
      }
    })
  }
}