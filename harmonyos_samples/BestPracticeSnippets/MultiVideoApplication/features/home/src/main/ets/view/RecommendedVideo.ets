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

import { componentUtils } from '@kit.ArkUI';
import { deviceInfo } from '@kit.BasicServicesKit';
import { BreakpointConstants, CommonConstants } from '@ohos/commons';
import { BreakpointType, Logger, WindowUtil } from '@ohos/commons';
import { HomeConstants } from '../constants/HomeConstants';
import { VideoImage, VideoImgViewModel } from '../viewmodel/VideoImgViewModel';
import { getTabIndex, VideoContent, VideoImgRating, VideoTitle } from './CommonView';
import { RightClickMenu, VideoDialog } from './VideoDialog';

@Component
export struct RecommendedVideo {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('videoGridColumn') videoGridColumn: string = CommonConstants.VIDEO_GRID_COLUMNS[0];
  @StorageLink('windowWidth') windowWidth: number = 0;
  @StorageLink('currentTopIndex') currentTopIndex: number = 0;
  @State isShowingRecommendBorder: boolean = false;
  @State currentRecommendFocusIndex: number = 0;
  @State videoImgList: VideoImage[] = new VideoImgViewModel().getVideoImgList();
  private videoDialogController?: CustomDialogController;
  private windowUtil?: WindowUtil;
  // [Start recommended_video]
  build() {
    // Video grid layout.
    Grid() {
      ForEach(this.videoImgList, (item: VideoImage, index: number) => {
        GridItem() {
          Column() {
            // [Start recommended_video_stack]
            Stack({ alignContent: Alignment.Center }) {
              // [StartExclude recommended_video]
              Image(item.getImgSrc())
                .focusable(true)
                .groupDefaultFocus(index === 0 ? true : false)
                .objectFit(ImageFit.Fill)
                .width(CommonConstants.FULL_PERCENT)
                .height(CommonConstants.FULL_PERCENT)
                .draggable(false)
                .borderRadius($r('app.float.banner_focus_radius'))
                .id(JSON.stringify(item))
              Column() {
                VideoImgRating({ rating: item.getRating() })
              }
              .width(CommonConstants.FULL_PERCENT)
              .height(CommonConstants.FULL_PERCENT)
              .alignItems(HorizontalAlign.End)
              .justifyContent(FlexAlign.End)
              // [EndExclude recommended_video]
            }
            // [StartExclude recommended_video_stack]
            .width(CommonConstants.FULL_PERCENT)
            .aspectRatio(HomeConstants.VIDEO_DIALOG_ASPECT_RATIO)
            // [StartExclude recommended_video]
            .focusable(true)
            // The width and height vary with the container assembly and the aspect ratio remains unchanged.
            .hoverEffect(HoverEffect.Scale)
            // [EndExclude recommended_video]

            .gesture(
              LongPressGesture({ repeat: false })
                .onAction(() => {
                  if (index !== 0) {
                    Logger.info(`Please long press the first image`);
                    return;
                  }
                  // Obtains all attributes of a component.
                  let modePosition: componentUtils.ComponentInfo =
                    componentUtils.getRectangleById(JSON.stringify(item));
                  let windowOffset = modePosition.windowOffset;
                  let size = modePosition.size;
                  // Obtains the height of the component from the top.
                  let rectTop: number = px2vp(windowOffset.y);
                  let rectTop2: number = px2vp(windowOffset.y + Math.floor(size.height));
                  // Obtains the width of the component from the left.
                  let rectLeft: number = px2vp(windowOffset.x);
                  let topHeightNeeded: number = new BreakpointType(HomeConstants.VIDEO_DIALOG_HEIGHTS[0],
                    HomeConstants.VIDEO_DIALOG_HEIGHTS[1], HomeConstants.VIDEO_DIALOG_HEIGHTS[2])
                    .getValue(this.currentWidthBreakpoint) + rectTop - rectTop2;
                  if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_SM) {
                    topHeightNeeded += HomeConstants.HOME_HEADER_HEIGHT_SM;
                  }
                  let dialogYOffset: number;
                  // Adaptive pop-up window expansion direction.
                  if (topHeightNeeded < rectTop) {
                    dialogYOffset = rectTop2 - new BreakpointType(HomeConstants.VIDEO_DIALOG_HEIGHTS[0],
                      HomeConstants.VIDEO_DIALOG_HEIGHTS[1], HomeConstants.VIDEO_DIALOG_HEIGHTS[2])
                      .getValue(this.currentWidthBreakpoint);
                  } else {
                    dialogYOffset = rectTop;
                  }
                  this.windowUtil = WindowUtil.getInstance();
                  let isLayoutFullScreen: boolean = true;
                  let mainWindow = this.windowUtil!.getMainWindow();
                  isLayoutFullScreen = mainWindow!.getWindowProperties().isLayoutFullScreen;
                  // Subtract the width and height of the window in the PC/2in1 device.
                  if (deviceInfo.deviceType === CommonConstants.DEVICE_TYPE && !isLayoutFullScreen) {
                    dialogYOffset -= HomeConstants.WINDOW_UNDEFINED_TOP;
                    rectLeft -= HomeConstants.WINDOW_UNDEFINED_LEFT;
                  } else {
                    Logger.info(`No need to subtract extra height`);
                  }
                  this.videoDialogController = new CustomDialogController({
                    builder: VideoDialog(),
                    autoCancel: true,
                    customStyle: true,
                    alignment: DialogAlignment.TopStart,
                    offset: {
                      dx: rectLeft,
                      dy: dialogYOffset
                    }
                  });
                  // Display the customized pop-up window to play the video.
                  this.videoDialogController.open();
                }))
            // [EndExclude recommended_video_stack]
            .bindContextMenu(RightClickMenu(this.currentWidthBreakpoint), ResponseType.RightClick)
            // [End recommended_video_stack]

            VideoTitle({ title: item.getTitle() })
            VideoContent({ content: item.getContent() })
          }
          .alignItems(HorizontalAlign.Start)
        }
      }, (item: VideoImage, index: number) => index + JSON.stringify(item))
    }
    // [StartExclude recommended_video]
    .tabIndex(getTabIndex(HomeConstants.DIRECTION_LIST[4]))
    .editMode(false)
    // Sets the number of grid layout columns and evenly divides the width.
    .columnsTemplate(this.videoGridColumn)
    .rowsTemplate(CommonConstants.VIDEO_GRID_COLUMNS[0])
    .width(CommonConstants.FULL_PERCENT)
    .rowsGap($r('app.float.video_grid_gap'))
    .columnsGap($r('app.float.video_grid_gap'))
    // Dynamically calculates the total height based on the following numbers of breakpoints.
    .height(this.getGridHeight(this.videoGridColumn, this.currentWidthBreakpoint, this.windowWidth))
    .padding({
      left: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG && this.currentTopIndex === 2 ?
        new BreakpointType($r('app.float.home_recommended_padding_sm'), $r('app.float.home_recommended_padding_md'),
        $r('app.float.home_recommended_padding_lg')).getValue(this.currentWidthBreakpoint) :
        new BreakpointType($r('app.float.home_content_padding_sm'), $r('app.float.home_content_padding_md'),
        $r('app.float.home_content_padding_lg')).getValue(this.currentWidthBreakpoint),
      right: new BreakpointType($r('app.float.home_content_padding_sm'), $r('app.float.home_content_padding_md'),
        $r('app.float.home_content_padding_lg')).getValue(this.currentWidthBreakpoint),
      top: $r('app.float.recommended_video_padding_top'),
      bottom: $r('app.float.recommended_video_padding_bottom')
    })
    // Zooming and pinching functions of the grid.
    // [EndExclude recommended_video]
    .gesture(PinchGesture({ fingers: 2 }).onActionUpdate((event: GestureEvent) => {
      if (event.scale > 1 && this.currentWidthBreakpoint !== BreakpointConstants.BREAKPOINT_SM) {
        if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD) {
          animateTo({
            duration: HomeConstants.ANIMATION_DURATION
          }, () => {
            this.videoGridColumn = CommonConstants.VIDEO_GRID_COLUMNS[1];
          })
        } else {
          animateTo({
            duration: HomeConstants.ANIMATION_DURATION
          }, () => {
            this.videoGridColumn = CommonConstants.VIDEO_GRID_COLUMNS[2];
          })
        }
      } else if (event.scale < 1 && this.currentWidthBreakpoint !== BreakpointConstants.BREAKPOINT_SM) {
        if (this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD) {
          animateTo({
            duration: HomeConstants.ANIMATION_DURATION
          }, () => {
            this.videoGridColumn = CommonConstants.VIDEO_GRID_COLUMNS[2];
          })
        } else {
          animateTo({
            duration: HomeConstants.ANIMATION_DURATION
          }, () => {
            this.videoGridColumn = CommonConstants.VIDEO_GRID_COLUMNS[3];
          })
        }
      } else {
        Logger.info(`Two-finger operation is not supported`);
      }
    }))
  }

  getGridHeight(videoGridColumn: string, currentWidthBreakpoint: string, windowWidth: number): string {
    // Obtain the window width and subtract the blank parts on both sides.
    let result: number = px2vp(windowWidth) - new BreakpointType(HomeConstants.VIDEO_GRID_MARGIN[0],
      HomeConstants.VIDEO_GRID_MARGIN[1], HomeConstants.VIDEO_GRID_MARGIN[2]).getValue(this.currentWidthBreakpoint);
    if (currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) {
      result = result - HomeConstants.LG_SIDEBAR_WIDTH;
    }
    // Calculate the width of a single image based on the number of grid columns.
    if (videoGridColumn === CommonConstants.VIDEO_GRID_COLUMNS[0]) {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * 1) / HomeConstants.TWO;
    } else if (videoGridColumn === CommonConstants.VIDEO_GRID_COLUMNS[1]) {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * 2) / CommonConstants.THREE;
    } else if (videoGridColumn === CommonConstants.VIDEO_GRID_COLUMNS[2]) {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * 3) / CommonConstants.FOUR;
    } else {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * 4) / HomeConstants.FIVE;
    }
    // Calculate the height of a single image, title, and content, and calculate the total height of the grid layout.
    return result / HomeConstants.VIDEO_DIALOG_ASPECT_RATIO * HomeConstants.TWO +
      HomeConstants.VIDEO_GRID_DESCRIPTION_HEIGHT + HomeConstants.HEIGHT_UNIT;
  }
}
// [End recommended_video]