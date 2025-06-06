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

import { Home } from '@ohos/home';
import { CommonConstants, VideoNavPathStack, WindowUtil } from '@ohos/commons';
import { VideoDetail } from '@ohos/videoDetail/src/main/ets/view/VideoDetail';

@Entry
@Component
struct Index {
  @Provide('pageInfo') pageInfo: VideoNavPathStack = new VideoNavPathStack();

  aboutToDisappear(): void {
    let windowUtil: WindowUtil | undefined = WindowUtil.getInstance();
    windowUtil!.offWindowSizeChange();
  }

  @Builder
  PageMap(name: string) {
    if (name === CommonConstants.PAGE_NAMES[1]) {
      VideoDetail()
    }
  }

  build() {
    Navigation(this.pageInfo) {
      Home()
    }
    .mode(NavigationMode.Stack)
    .navDestination(this.PageMap)
    .hideToolBar(true)
    .hideTitleBar(true)
    .hideBackButton(true)
    .width(CommonConstants.FULL_PERCENT)
    .height(CommonConstants.FULL_PERCENT)
  }
}