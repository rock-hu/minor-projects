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
import { Adaptive } from '../viewmodel/AdaptiveViewModel'

const FINGER_NUM: number = 2

@Component
export struct CenterPart {
  @StorageLink('currentBreakpoint') currentBp: string = BreakpointConstants.BREAKPOINT_MD;
  @State scaleValue: number = 1;
  @State pinchValue: number = 1;

  build() {
    Flex({ direction: FlexDirection.Column }) {
      Blank()
      // [Start center_part]
      Row() {
        Column() {
          Image($r('app.media.photo'))
            .autoResize(true)
        }
      }
      // [StartExclude center_part]
      .height(Adaptive.PICTURE_HEIGHT(this.currentBp))
      .width(Adaptive.PICTURE_WIDTH(this.currentBp))
      .scale({ x: this.scaleValue, y: this.scaleValue, z: 1 })
      // [EndExclude center_part]
      .gesture(PinchGesture({ fingers: FINGER_NUM })
        .onActionUpdate((event: GestureEvent | undefined) => {
          if (event) {
            this.scaleValue = this.pinchValue * event.scale;
          }
        })
        .onActionEnd(() => {
          this.pinchValue = this.scaleValue;
        }))
      // [End center_part]
      Blank()
        .height(this.currentBp === BreakpointConstants.BREAKPOINT_SM ? $r('app.float.center_blank_height_lg') : 0)
    }
    .margin({
      top: $r('app.float.center_margin_top'),
      bottom: $r('app.float.center_margin_bottom')
    })
  }
}