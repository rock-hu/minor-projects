/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

import { BreakpointConstants } from '@ohos/commons';
import PictureViewConstants from '../constants/PictureViewConstants';

const IMAGE_ASPECT_RATIO: number = 0.5;

@Component
export struct PreviewLists {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_MD;

  build() {
    List({ initialIndex: 1 }) {
      ForEach(PictureViewConstants.PICTURES, (item: Resource) => {
        ListItem() {
          Image(item)
            .height($r('app.float.list_image_height'))
            .aspectRatio(IMAGE_ASPECT_RATIO)
            .autoResize(true)
            .margin({ left: $r('app.float.list_image_margin_left') })
        }
      }, (item: Resource, index: number) => index + JSON.stringify(item))
    }
    .height($r('app.float.list_image_height'))
    .padding({
      top: $r('app.float.list_margin_top'),
      bottom: $r('app.float.list_margin_bottom')
    })
    .listDirection(Axis.Horizontal)
    .scrollSnapAlign(ScrollSnapAlign.CENTER)
    .scrollBar(BarState.Off)
  }
}