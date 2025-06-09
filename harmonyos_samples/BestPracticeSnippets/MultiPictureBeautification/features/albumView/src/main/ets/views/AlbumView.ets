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

import { SideColumn } from './SideColumn';
import { AlbumViewConstants, ImageData } from '../constants/AlbumViewConstants'
import { BaseConstants, BreakpointConstants } from '@ohos/commons/';
import { deviceInfo } from '@kit.BasicServicesKit';

// Grid unit.
const DEFAULT_GRID_TEMPLATE: string = '1fr';
// Animate Item.
const ANIMATE_TIME: number = 100;
// Max grid column.
const MAX_GRID_COLUMN = 16;
// Min grid column.
const MIN_GRID_COLUMN = 2;
// Finger number.
const FINGER_NUMBER = 2;
// Delay time
const DELAY = 70
// Final delay time
const FINAL_DELAY = 100

@Component
export struct AlbumView {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_SM;
  // Grid column num.
  @State gridColumn: number = 3;
  // Grid row num.
  @State gridRow: number = 2;
  @Consume('pageInfos') pageInfos: NavPathStack;
  // Save up setTimeOut
  timeOutUpDemo: number = 0
  // Save down setTimeOut
  timeOutDownDemo: number = 0

  // Change the number of Grid columns based on the width of the Grid component.
  // [Start get_grid_column]
  getGridColumn(value: Length): number {
    return Math.floor(2 * ((parseInt(JSON.stringify(value)) / 360) - 1) + 4);
  }
  // [End get_grid_column]

  // PinchGesture up
  up() {
    if (!this.timeOutUpDemo) {
      this.timeOutUpDemo = setTimeout(() => {
        this.getUIContext().animateTo({
          duration: ANIMATE_TIME
        }, () => {
          if (this.gridColumn > MIN_GRID_COLUMN) {
            this.gridColumn -= 1;
            clearTimeout(this.timeOutUpDemo)
            this.timeOutUpDemo = 0
          }
        })
      }, DELAY)
    }
    setTimeout(() => {
      this.timeOutUpDemo = 0
    }, FINAL_DELAY)
  }

  // PinchGesture down
  down() {
    if (!this.timeOutDownDemo) {
      this.timeOutDownDemo = setTimeout(() => {
        this.getUIContext().animateTo({
          duration: ANIMATE_TIME
        }, () => {
          if (this.gridColumn < MAX_GRID_COLUMN) {
            this.gridColumn += 1;
            clearTimeout(this.timeOutDownDemo)
            this.timeOutDownDemo = 0
          }
        })

      }, DELAY)
    }
    setTimeout(() => {
      this.timeOutDownDemo = 0
    }, FINAL_DELAY)

  }

  @Builder
  topRow() {
    Column() {
      Row() {
        Button({ type: ButtonType.Circle, stateEffect: false }) {
          Image($r('app.media.ic_public_back_black'))
            .width(BaseConstants.DEFAULT_ICON_SIZE)
            .height(BaseConstants.DEFAULT_ICON_SIZE)
            .onClick(() => {
              this.pageInfos.pop();
            })
        }
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .backgroundColor(Color.White)
        .margin({ right: $r('app.float.back_image_right') })

        Text(AlbumViewConstants.DES_TEXT)
          .fontSize(BaseConstants.FONT_SIZE_TWENTY)
          .height($r('app.float.text_image_height'))
        Blank()

        if (this.currentBp === BreakpointConstants.BREAKPOINT_LG) {
          Button({ type: ButtonType.Circle, stateEffect: false }) {
            Image($r('app.media.ic_gallery_material_select_checkbox'))
              .width(BaseConstants.DEFAULT_ICON_SIZE)
              .height(BaseConstants.DEFAULT_ICON_SIZE)
          }
          .width(BaseConstants.DEFAULT_ICON_SIZE)
          .height(BaseConstants.DEFAULT_ICON_SIZE)
          .backgroundColor(Color.White)
          .margin({ right: $r('app.float.select_button_margin_right') })
        }

        Button({ type: ButtonType.Circle, stateEffect: false }) {
          Image($r('app.media.ic_public_more'))
            .width(BaseConstants.DEFAULT_ICON_SIZE)
            .height(BaseConstants.DEFAULT_ICON_SIZE)
        }
        .width(BaseConstants.DEFAULT_ICON_SIZE)
        .height(BaseConstants.DEFAULT_ICON_SIZE)
        .backgroundColor(Color.White)

      }
      .width(BaseConstants.FULL_WIDTH)
      .height($r('app.float.tab_height'))
      .backgroundColor(Color.White)
      .padding({
        left: $r('app.float.tab_left'),
        right: $r('app.float.tab_right'),
        top: $r('app.float.tab_top'),
        bottom: $r('app.float.tab_bottom')
      })
      .visibility(Visibility.Visible)
    }
  }

  build() {
    NavDestination() {
      Flex() {
        if (this.currentBp === BreakpointConstants.BREAKPOINT_LG) {
          SideColumn()
            .width($r('app.float.side_width'))
        }
        // [Start album_view_change]
        Flex({ direction: FlexDirection.Column }) {
          // [StartExclude album_view_change]
          this.topRow()
          Flex() {
            Grid() {
              ForEach(AlbumViewConstants.IMAGE_LIST, (item: ImageData) => {
                GridItem() {
                  Image(item.src)
                    .objectFit(ImageFit.Cover)
                    .autoResize(true)
                    .borderColor(item.selected ? Color.Blue : Color.White)
                    .borderWidth(1)
                }.aspectRatio(1)
              }, (item: ImageData, index: number) => index + JSON.stringify(item))
            }
            .columnsTemplate(new Array(this.gridColumn).fill(DEFAULT_GRID_TEMPLATE).join(' '))

          }
          // [EndExclude album_view_change]
          .onAreaChange((oldValue: Area, newValue: Area) => {
            this.gridColumn = this.getGridColumn(newValue.width);
          })
          // [StartExclude album_view_change]
          .gesture(PinchGesture({ fingers: FINGER_NUMBER }).onActionUpdate((event: GestureEvent) => {
            if (event.scale > 1) {
              this.up()
            } else {
              this.down()
            }
          }))
          // [EndExclude album_view_change]
        }
        // [End album_view_change]
      }
      .padding({
        top: deviceInfo.deviceType === BaseConstants.DEVICE_2IN1 ? $r('app.float.zero') :
        $r('app.float.device_padding_top'),
        bottom: deviceInfo.deviceType !== BaseConstants.DEVICE_2IN1 ?
        $r('app.float.tab_content_pb') : $r('app.float.zero')
      })
    }
    .hideTitleBar(true)
  }
}