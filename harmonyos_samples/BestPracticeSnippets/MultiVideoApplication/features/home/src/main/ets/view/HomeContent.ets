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
import { BreakpointConstants, BreakpointType, CommonConstants } from '@ohos/commons';
import { WindowUtil } from '@ohos/commons';
import { BannerView } from './BannerView';
import { IconView } from './IconView';
import { RecommendedVideo} from './RecommendedVideo';
import { NewVideoRelease } from './NewVideoRelease';
import { DailyVideo } from './DailyVideo';
import { PreviousVideo } from './PreviousVideo';

@Component
export struct HomeContent {
  @StorageLink('currentWidthBreakpoint') currentWidthBreakpoint: string = BreakpointConstants.BREAKPOINT_LG;
  @StorageLink('currentTopIndex') currentTopIndex: number = 0;
  @State gridStatus: number = 1;
  private windowUtil?: WindowUtil;
  private isSearching: boolean = false;

  aboutToAppear(): void {
    this.windowUtil = WindowUtil.getInstance();
    if (deviceInfo.deviceType !== CommonConstants.DEVICE_TYPE) {
      this.windowUtil!.setFullScreen();
    }
  }

  build() {
    // [Start home_content_build]
    Column() {
      // [StartExclude home_content_build]
      BannerView()
      IconView()
      RecommendedVideo()
      NewVideoRelease()
      DailyVideo()
      PreviousVideo()
      // [EndExclude home_content_build]
    }
    // Set the background image to cover the side and top tabs.
    .backgroundImage(this.currentTopIndex === 2 && !this.isSearching ? new BreakpointType(
      $r('app.media.immersive_background_sm'), $r('app.media.immersive_background_md'),
      $r('app.media.immersive_background_lg')).getValue(this.currentWidthBreakpoint) : $r('app.media.white_background'))
    // Set the size of the background image.
    .backgroundImageSize({ width: CommonConstants.FULL_PERCENT, height: new BreakpointType(
      $r('app.float.immersive_background_height_sm'), $r('app.float.immersive_background_height_md'),
      $r('app.float.immersive_background_height_lg')).getValue(this.currentWidthBreakpoint) })
    // Set the uniform background color.
    .backgroundColor(this.currentTopIndex === 2 && !this.isSearching ? (this.currentWidthBreakpoint !== BreakpointConstants.BREAKPOINT_MD ?
      $r('app.color.home_content_background') : $r('app.color.home_content_background_md')) : Color.White)
    .width(CommonConstants.FULL_PERCENT)
    // [End home_content_build]
    .padding({ bottom: deviceInfo.deviceType !== CommonConstants.DEVICE_TYPE && this.currentWidthBreakpoint ===
      BreakpointConstants.BREAKPOINT_LG ? $r('app.float.bottom_navigation') : 0 })
  }
}