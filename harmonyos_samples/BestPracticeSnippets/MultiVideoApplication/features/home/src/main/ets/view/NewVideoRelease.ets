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

import { BreakpointConstants, CommonConstants } from '@ohos/commons';
import { BreakpointType } from '@ohos/commons';
import { HomeConstants } from '../constants/HomeConstants';
import { VideoImage, VideoImgViewModel } from '../viewmodel/VideoImgViewModel';
import { getTabIndex,SubtitleComponent,VideoContent,VideoImgComponent, VideoImgPlay, VideoImgRating, VideoTitle } from
  './CommonView';

@Component
export struct NewVideoRelease {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('windowWidth') windowWidth: number = 0;
  @StorageLink('currentTopIndex') currentTopIndex: number = 0;
  private newVideoImgList: VideoImage[] = new VideoImgViewModel().getNewVideoImgList();

  build() {
    Column() {
      SubtitleComponent({ title: HomeConstants.HOME_SUB_TITLES[0] })
        .margin({ top: $r('app.float.sub_title_row_margin') })

      // Video grid.
      Grid() {
        ForEach(this.newVideoImgList, (item: VideoImage, index: number) => {
          GridItem() {
            Column() {
              Stack({ alignContent: Alignment.Bottom }) {
                VideoImgComponent({ imgSrc:item.getImgSrc(), index: index })
                VideoImgPlay()
                VideoImgRating({ rating: item.getRating() })
              }
              .height(new BreakpointType($r('app.float.new_video_height_sm'), $r('app.float.new_video_height_md'),
                $r('app.float.new_video_height_lg')).getValue(this.currentWidthBreakpoint))
              .width(CommonConstants.FULL_PERCENT)
              .aspectRatio(HomeConstants.NEW_VIDEO_ASPECT_RATIO)

              VideoTitle({ title: item.getTitle() })
              VideoContent({ content: item.getContent() })
            }
            .alignItems(HorizontalAlign.Start)
          }
        }, (item: VideoImage, index: number) => index + JSON.stringify(item))
      }
      .tabIndex(getTabIndex(HomeConstants.DIRECTION_LIST[5]))
      .padding({
        left: new BreakpointType($r('app.float.home_content_padding_sm'), $r('app.float.home_content_padding_md'),
          $r('app.float.home_content_padding_lg')).getValue(this.currentWidthBreakpoint),
        right: new BreakpointType($r('app.float.home_content_padding_sm'), $r('app.float.home_content_padding_md'),
          $r('app.float.home_content_padding_lg')).getValue(this.currentWidthBreakpoint),
        top: $r('app.float.sub_title_row_margin'),
        bottom: $r('app.float.sub_title_row_margin')
      })
      .columnsTemplate(new BreakpointType(CommonConstants.VIDEO_GRID_COLUMNS[1], CommonConstants.VIDEO_GRID_COLUMNS[3],
        CommonConstants.VIDEO_GRID_COLUMNS[4]).getValue(this.currentWidthBreakpoint))
      .rowsTemplate(HomeConstants.NEW_VIDEO_ROWS_TEMPLATE)
      .width(CommonConstants.FULL_PERCENT)
      .columnsGap($r('app.float.new_video_columns_gap'))
      .height(this.getNewVideoHeight(this.currentWidthBreakpoint, this.windowWidth))
    }
    .padding({ left: this.currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG && this.currentTopIndex === 2 ?
      $r('app.float.side_bar_width') : 0 })
  }

  getNewVideoHeight(currentWidthBreakpoint: string, windowWidth: number): string {
    let result: number = this.getUIContext().px2vp(windowWidth) - new BreakpointType(HomeConstants.VIDEO_GRID_MARGIN[0],
      HomeConstants.VIDEO_GRID_MARGIN[1], HomeConstants.VIDEO_GRID_MARGIN[2]).getValue(this.currentWidthBreakpoint);
    if (currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_LG) {
      result = result - HomeConstants.LG_SIDEBAR_WIDTH;
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * HomeConstants.SIX) / HomeConstants.SEVEN;
    } else if (currentWidthBreakpoint === BreakpointConstants.BREAKPOINT_MD) {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * CommonConstants.FOUR) / HomeConstants.FIVE;
    } else {
      result = (result - HomeConstants.VIDEO_GRID_ITEM_SPACE * HomeConstants.TWO) / CommonConstants.THREE;
    }
    return (result / HomeConstants.NEW_VIDEO_ASPECT_RATIO + HomeConstants.NEW_VIDEO_DESCRIPTION_HEIGHT +
      HomeConstants.HEIGHT_UNIT);
  }
}