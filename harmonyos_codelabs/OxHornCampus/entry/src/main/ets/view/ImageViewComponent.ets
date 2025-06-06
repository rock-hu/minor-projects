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

import { common } from '@kit.AbilityKit';
import { window } from '@kit.ArkUI';
import ZonesItem from '../viewmodel/ZonesItem';
import { CommonConstants as Const } from '../common/constants/CommonConstants';
import { DeviceScreen } from '../common/utils/DeviceScreen';
import { ImageDataSource } from '../viewmodel/ZonesViewModel';

@CustomDialog
export struct ImageView {
  private controller: CustomDialogController;
  private windowClass: window.Window | null = null;
  private deviceWidth: number = 0;
  private deviceHeight: number = 0;
  private imgWidth: number = 0;
  private imgHeight: number = 0;
  private displayHeight: number = 0
  @StorageLink('imageModalOpen') imageModalOpen: boolean = false;
  @Consume('introductionData') introductionData: ZonesItem;
  @StorageLink('curIndex') curIndex: number = 0;
  @Prop currentImageId: number;
  @State isGesture: boolean = false;
  @State imgScale: number = 1
  @State curScale: number = 1
  @State imgOffsetX: number = 0;
  @State imgOffsetY: number = 0;
  @State preOffsetX: number = 0;
  @State preOffsetY: number = 0;

  aboutToAppear() {
    let deviceSize = DeviceScreen.getDeviceSize(this.getUIContext().getHostContext()! as common.UIAbilityContext);
    deviceSize.then((data) => {
      this.windowClass = data;
      let properties = this.windowClass.getWindowProperties();
      this.deviceWidth = properties.windowRect.width;
      this.deviceHeight = properties.windowRect.height;
    });
    this.imageModalOpen = true;
    if (AppStorage.get<string>('isContinuation') === 'false') {
      this.curIndex = this.currentImageId;
    }
  }

  aboutToDisappear() {
    this.imageModalOpen = false;
  }

  /**
   * Detect boundary to keep the image in window.
   */
  detectBoundary(): void {
    let maxOffsetX = this.imgScale * this.deviceWidth / 2 - this.deviceWidth / 2;
    if (this.getUIContext().vp2px(this.imgOffsetX) > (maxOffsetX)) {
      this.imgOffsetX = this.getUIContext().px2vp(maxOffsetX);
    }
    if (this.getUIContext().vp2px(this.imgOffsetX) < -(maxOffsetX)) {
      this.imgOffsetX = -this.getUIContext().px2vp(maxOffsetX);
    }
    let maxOffsetY = this.imgScale * this.displayHeight / 2 - this.deviceHeight / 2;
    if (this.imgScale * this.displayHeight >= this.deviceHeight) {
      if (this.getUIContext().vp2px(this.imgOffsetY) > (maxOffsetY)) {
        this.imgOffsetY = this.getUIContext().px2vp(maxOffsetY);
      }
      if (this.getUIContext().vp2px(this.imgOffsetY) < -(maxOffsetY)) {
        this.imgOffsetY = -this.getUIContext().px2vp(maxOffsetY);
      }
    } else {
      this.imgOffsetY = 0;
    }
  }

  /**
   * Limit scale to keep the image clear.
   */
  limitScale(isReset: boolean): void {
    if (this.imgScale < 1) {
      this.imgScale = 1;
      this.curScale = 1;
      if (isReset) {
        this.imgOffsetX = 0;
        this.imgOffsetY = 0;
      }
      this.isGesture = false;
    } else if (this.imgScale > Const.MAX_SCALE) {
      this.imgScale = Const.MAX_SCALE;
      this.curScale = Const.MAX_SCALE;
    } else {
      this.curScale = this.imgScale;
    }
  }

  build() {
    Stack() {
      Swiper() {
        LazyForEach(new ImageDataSource(this.introductionData.imageList), (item: Resource) => {
          Column() {
            Blank()
              .onClick(() => {
                this.controller.close();
              })

            Image(item)
              .width(Const.FULL_PERCENT)
              .objectFit(ImageFit.Contain)
              .gesture(
                PinchGesture()
                  .onActionStart(() => {
                    this.isGesture = true;
                  })
                  .onActionUpdate((event: GestureEvent) => {
                    this.imgScale = this.curScale * event.scale;
                  })
                  .onActionEnd(() => {
                    this.limitScale(false);
                  })
              )

            Blank()
              .onClick(() => {
                this.controller.close();
              })
          }
          .width(Const.FULL_PERCENT)
          .height(Const.FULL_PERCENT)
          .justifyContent(FlexAlign.Center)
        })
      }
      .width(Const.FULL_PERCENT)
      .height(Const.FULL_PERCENT)
      .loop(false)
      .indicator(
        new DotIndicator().bottom($r('app.float.indicator_bottom'))
      )
      .index(this.curIndex)
      .onChange((index: number) => {
        this.curIndex = index;
      })
      .visibility(this.isGesture ? Visibility.Hidden : Visibility.Visible)

      Row() {
        Image(this.introductionData.imageList[this.curIndex])
          .objectFit(ImageFit.Contain)
          .scale({ x: this.imgScale, y: this.imgScale })
          .translate({ x: this.imgOffsetX, y: this.imgOffsetY })
          .onComplete((event) => {
            if (event) {
              this.imgWidth = event.width;
              this.imgHeight = event.height;
              this.displayHeight = this.deviceWidth * this.imgHeight / this.imgWidth;
            }
          })
      }
      .gesture(
        PinchGesture()
          .onActionUpdate((event: GestureEvent) => {
            this.imgScale = this.curScale * event.scale;
          })
          .onActionEnd(() => {
            this.detectBoundary();
            this.limitScale(true);
          })
      )
      .gesture(
        PanGesture()
          .onActionStart(() => {
            this.preOffsetX = this.imgOffsetX;
            this.preOffsetY = this.imgOffsetY;
          })
          .onActionUpdate((event: GestureEvent) => {
            this.imgOffsetX = this.preOffsetX + event.offsetX;
            this.imgOffsetY = this.preOffsetY + event.offsetY;
          })
          .onActionEnd(() => {
            this.detectBoundary();
          })
      )
      .visibility(this.isGesture ? Visibility.Visible : Visibility.Hidden)
    }
    .width(Const.FULL_PERCENT)
    .height(Const.FULL_PERCENT)
    .linearGradient({
      direction: GradientDirection.Top,
      colors: [[$r('app.color.color_gradient_1'), Const.COLOR_SCALE_1], [$r('app.color.color_gradient_2'), Const.COLOR_SCALE_2]]
    })
  }
}