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

import { image } from '@kit.ImageKit';
import CameraService from '../utils/CameraServiceOne';
import { CommonConstants } from '../common/CommonConstants';
import { promptAction } from '@kit.ArkUI';

@Builder
export function PageOneBuilder() {
  PageOne()
}

@Component
export struct PageOne {
  pathStack: NavPathStack = new NavPathStack();
  @State name: string = 'pageOne';
  @State isShowStridePixel: boolean = false;
  @StorageLink('stridePixel') @Watch('onStridePixel') stridePixel: image.PixelMap | undefined = undefined;
  @State imageWidth: number = 1080;
  @State imageHeight: number = 1080;
  @StorageLink('previewRotation') previewRotate: number = 0;

  onStridePixel(): void {
    this.isShowStridePixel = true;
  }

  aboutToAppear(): void {
    CameraService.initCamera(0, this.getUIContext());
  }

  aboutToDisappear(): void {
    CameraService.releaseCamera();
  }

  @Builder
  navDestinationTitleImg(res:Resource,fun:()=>void) {
    Image(res)
      .padding(8)
      .width(40)
      .height(40)
      .borderRadius('50%')
      .colorFilter('#171717')
      .backgroundColor('#e6e8e9')
      .onClick(() => {
        fun();
      })
  }

  @Builder
  navDestinationTitle() {
    Row() {
      this.navDestinationTitleImg($r('app.media.back'), () => {
        this.pathStack.pop();
      })

      Text($r('app.string.title_one'))
        .fontSize(20)
        .fontWeight(FontWeight.Bold)
        .textAlign(TextAlign.Start)
        .lineHeight(26)
        .padding({ left: 8 })
        .fontColor(Color.Black)
        .layoutWeight(1)

      this.navDestinationTitleImg($r('app.media.more'), () => {
        this.getUIContext().getPromptAction().showToast({ message: $r('app.string.only_show_for_ux') })
      })
    }
    .width('100%')
    .height(56)
    .margin({ top: 36 })
    .padding({
      left: 16,
      right: 16
    })
    .backgroundColor('#F1F3F5')
  }

  build() {
    NavDestination() {
      this.navDestinationTitle()
      Column() {
        if (this.isShowStridePixel) {
          Image(this.stridePixel)
            .width(this.getUIContext().px2vp(this.imageWidth))
            .height(this.getUIContext().px2vp(this.imageHeight))
            .margin({ top: 150 })
            .rotate({
              z: 0.5,
              angle: this.previewRotate
            })
        }
        Blank()
        PublishView()
      }
      .justifyContent(FlexAlign.Center)
      .height('90%')
      .width('100%')
    }
    .backgroundColor(Color.White)
    .hideTitleBar(true)
    .onBackPressed(() => {
      this.pathStack.pop();
      return true;
    })
    .onReady((context: NavDestinationContext) => {
      this.pathStack = context.pathStack;
    })
  }
}

// Share components at the bottom of the page
@Component
export struct PublishView {
  build() {
    Flex({ justifyContent: FlexAlign.SpaceBetween }) {
      Image($r('app.media.share'))
        .width(24)
        .height(24)
      Image($r('app.media.heart'))
        .width(24)
        .height(24)
      Image($r('app.media.square_and_pencil'))
        .width(24)
        .height(24)
      Image($r('app.media.trash'))
        .width(24)
        .height(24)
    }
    .backgroundColor('#F1F3F5')
    .width(CommonConstants.FULL_PERCENT)
    .height(86)
    .padding({
      bottom: 44,
      top: 16,
      left: 18,
      right: 18
    })
    .onClick(() => {
      this.getUIContext().getPromptAction().showToast({ message: $r('app.string.only_show_for_ux') });
    })
  }
}