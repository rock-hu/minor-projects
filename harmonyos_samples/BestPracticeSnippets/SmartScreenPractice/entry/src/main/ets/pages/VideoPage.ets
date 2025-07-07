/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

@Entry
@Component
struct VideoPage {
  @Consume('pageInfos') pageInfo: NavPathStack;
  @State focusIndex: number = 1;
  @State selectIndex: number = 2;
  @State isShowMenu: boolean = false;
  private speedArr: string[] = ['0.75x', '1.0x', '1.25x', '1.5x'];

  // [Start Video]
  build() {
    Stack() {
      Image($r('app.media.ic_animals'))
        .width('100%')
        .height('100%')
        .focusable(true)
        .defaultFocus(true)
        .zIndex(1)
        // Bind the menu event. When the menu key is true, change the value of isShowMenu to true to display the menu settings.
        .onKeyEvent((event) => {
          if (canIUse('SystemCapability.MultimodalInput.Input.Core')) {
            if (event.keyCode === KeyCode.KEYCODE_MENU) {
              this.isShowMenu = true;
            }
          }
        })
      // [StartExclude Video]
      Text($r('app.string.animal_world'))
        .fontWeight(500)
        .fontSize(20)
        .lineHeight(27)
        .fontColor('#E6FFFFFF')
        .position({
          x: 48,
          y: 37
        })
        .zIndex(2)
      Column() {
        Progress({ value: 30, total: 100, type: ProgressType.Linear })
          .width('100%')
          .height(4)
          .color($r('sys.color.container_modal_unfocus_background'))
      }
      .visibility(this.isShowMenu ? Visibility.Hidden : Visibility.Visible)
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.End)
      .backgroundColor('#00000000')
      .padding({
        left: 48,
        right: 48,
        bottom: 50
      })
      .zIndex(2)
      // [EndExclude Video]
      // Menu settings.
      Column({ space: 20 }) {
        // [StartExclude Video]
        Text($r('app.string.image'))
          .fontSize(20)
          .lineHeight(27)
          .fontWeight(500)
          .fontColor('#E6FFFFFF')
        Column({ space: 18 }) {
          Text($r('app.string.double_speed'))
            .fontSize(24)
            .lineHeight(32)
            .fontWeight(500)
            .fontColor('#E6FFFFFF')
          // [EndExclude Video]
          Row({ space: 24 }) {
            ForEach(this.speedArr, (item: string, index: number) => {
              Button(item)
                .width(156)
                .height(40)
                .fontSize(18)
                .fontWeight(500)
                .borderRadius(43)
                .fontColor(this.focusIndex === index ? '#E6000000' : '#E6FFFFFF')
                .backgroundColor(this.focusIndex === index ? '#FFF1F3F5' : this.selectIndex === index ?
                  '#33F1F3F5' : '#00000000')
                .onFocus(() => {
                  this.focusIndex = index;
                })
                .onClick(() => {
                  this.selectIndex = index;
                })
            }, (item: string, index: number) => index + item)
          }
        }
        .alignItems(HorizontalAlign.Start)
        // [StartExclude Video]
        Text($r('app.string.image_scale'))
          .fontSize(20)
          .lineHeight(27)
          .fontWeight(500)
          .fontColor('#E6FFFFFF')
        // [EndExclude Video]
      }
      .visibility(this.isShowMenu ? Visibility.Visible: Visibility.Hidden)
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.End)
      .alignItems(HorizontalAlign.Start)
      .zIndex(2)
      .padding({
        left: 48,
        bottom: 37
      })
    }
  }
  // [End Video]
}

@Builder
export function VideoPageBuilder() {
  NavDestination() {
    VideoPage()
  }
  .hideTitleBar(true)
}