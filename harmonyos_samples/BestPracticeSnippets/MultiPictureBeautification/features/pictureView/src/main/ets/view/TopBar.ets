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

import { BaseConstants, BreakpointConstants } from '@ohos/commons';
import PictureViewConstants, { ActionInterface } from '../constants/PictureViewConstants';

const TITLE: string = '2020年12月24日';
const SUBTITLE: string = '下午04:00 呼和浩特市';

@Component
export struct TopBar {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_MD;
  @Consume('pageInfos') pageInfos: NavPathStack

  build() {
    Flex({
      direction: FlexDirection.Row,
      alignItems: ItemAlign.Center,
    }) {
      Column() {
        Flex({
          justifyContent: FlexAlign.SpaceBetween,
          direction: FlexDirection.Row,
          alignItems: ItemAlign.Stretch
        }) {
          Row() {
            Image($r('app.media.ic_public_back_black'))
              .height(BaseConstants.DEFAULT_ICON_SIZE)
              .width(BaseConstants.DEFAULT_ICON_SIZE)
              .margin({ right: $r('app.float.back_image_right') })
            Column() {
              Text(TITLE)
                .fontFamily(BaseConstants.FONT_FAMILY_MEDIUM)
                .fontSize(BaseConstants.FONT_SIZE_TWENTY)
                .fontWeight(BaseConstants.FONT_WEIGHT_FIVE)
              Text(SUBTITLE)
                .fontFamily(BaseConstants.FONT_FAMILY_NORMAL)
                .fontSize(BaseConstants.FONT_SIZE_FOURTEEN)
                .fontWeight(BaseConstants.FONT_WEIGHT_FOUR)
            }
            .alignItems(HorizontalAlign.Start)
            .padding($r('app.float.back_area_padding'))
          }

          Row() {
            Image($r('app.media.ic_public_picture'))
              .height(BaseConstants.DEFAULT_ICON_SIZE)
              .width(BaseConstants.DEFAULT_ICON_SIZE)
              .autoResize(true)
              .onClick(() => {
                this.pageInfos.pushPath(new NavPathInfo('albumView', []));
              })
            Image($r('app.media.ic_public_detail'))
              .height(BaseConstants.DEFAULT_ICON_SIZE)
              .width(BaseConstants.DEFAULT_ICON_SIZE)
              .margin({ left: $r('app.float.detail_image_left') })
            if (this.currentBp === BreakpointConstants.BREAKPOINT_LG) {
              ForEach(PictureViewConstants.ACTIONS, (item: ActionInterface) => {
                Image(item.icon)
                  .height(BaseConstants.DEFAULT_ICON_SIZE)
                  .width(BaseConstants.DEFAULT_ICON_SIZE)
                  .margin({ left: $r('app.float.detail_image_left') })
                  .onClick(() => {
                    if (item.icon_name === PictureViewConstants.EDIT_ICON_NAME) {
                      this.pageInfos.pushPath(new NavPathInfo('pictureEdit', []));
                    }
                  })
              }, (item: ActionInterface, index: number) => index + JSON.stringify(item))
            }
          }
        }
      }
    }
    .height($r('app.float.top_bar_height'))
    .margin({
      top: $r('app.float.top_bar_top'),
      bottom: $r('app.float.top_bar_bottom'),
      left: $r('app.float.top_bar_left'),
      right: $r('app.float.top_bar_right')
    })
  }
}