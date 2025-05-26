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

import { TopBar } from '../view/TopBar';
import { CenterPart } from '../view/CenterPart';
import { BottomBar } from '../view/BottomBar';
import { PreviewLists } from '../view/PreviewLists';
import { BaseConstants, BreakpointConstants } from '@ohos/commons';
import { deviceInfo } from '@kit.BasicServicesKit';
import { AlbumView } from '@ohos/album';
import { PictureEdit } from '@ohos/pictureEdit';

@Preview
@Component
export struct PictureViewIndex {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_MD;
  @Provide('pageInfos') pageInfos: NavPathStack = new NavPathStack()
  @Builder
  PageMap(name: string) {
    if (name === 'albumView') {
      AlbumView()
    }
    if (name === 'pictureEdit') {
      PictureEdit()
    }
  }
  build() {
    Navigation(this.pageInfos) {
        Flex({
          direction: FlexDirection.Column,
          alignItems: ItemAlign.Center
        }) {
          TopBar()
          CenterPart()
            .flexGrow(1)
          PreviewLists()
          if (this.currentBp !== BreakpointConstants.BREAKPOINT_LG) {
            BottomBar()
          }
        }    .padding({
          top: deviceInfo.deviceType === BaseConstants.DEVICE_2IN1 ? $r('app.float.zero') :
          $r('app.float.device_padding_top'),
          bottom: deviceInfo.deviceType !== BaseConstants.DEVICE_2IN1 ?
          $r('app.float.tab_content_pb') : $r('app.float.zero')
        })
    }
    .mode(NavigationMode.Stack)
    .height(BaseConstants.FULL_HEIGHT)
    .width(BaseConstants.FULL_WIDTH)
    .navDestination(this.PageMap)
    .hideTitleBar(true)
    .hideToolBar(true)
    .hideBackButton(true)
    .titleMode(NavigationTitleMode.Mini)

  }
}