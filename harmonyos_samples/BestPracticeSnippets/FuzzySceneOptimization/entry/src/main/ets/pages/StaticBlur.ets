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

// [Start static_start]
import { image } from '@kit.ImageKit';
import { effectKit } from '@kit.ArkGraphics2D';
import { window } from '@kit.ArkUI';

@Component
export struct StaticBlur {
  @Consume('navPathStack') navPathStack: NavPathStack;
  @State isShowStaticBlur: boolean = false;
  @State pixelMap: image.PixelMap | undefined = undefined;
  @State imgSource: image.ImageSource | undefined = undefined;
  @State bottomSafeHeight: number = 0; // bottom navigation bar height

  aboutToAppear() {
    window.getLastWindow(this.getUIContext().getHostContext()!, (err, windowBar) => {
      if (err.code) {
        return;
      }
      // get the height of the bottom navigation bar
      this.bottomSafeHeight =
        this.getUIContext().px2vp(windowBar.getWindowAvoidArea(window.AvoidAreaType.TYPE_NAVIGATION_INDICATOR).bottomRect.height);
      windowBar.setWindowLayoutFullScreen(true);
    });
  }

  async staticBlur() {
    let context = this.getUIContext().getHostContext()!;
    let resourceMgr = context.resourceManager; // retrieve the resourceManager object
    const fileData = await resourceMgr.getRawFileContent('test.png'); // retrieve images from the rawfile directory
    let buffer: ArrayBuffer = fileData.buffer.slice(0); // create an ArrayBuffer instance
    this.imgSource = image.createImageSource(buffer); // create an image source instance
    // create attributes for pixels
    let opts: image.InitializationOptions = {
      editable: true, // is it editable
      pixelFormat: 3, // pixel format. 3 represents RGBA_8888
      size: {
        // create image size
        height: 4,
        width: 6
      }
    };
    // create PixelMap
    await this.imgSource.createPixelMap(opts).then((pixelMap: image.PixelMap) => {
      const blurRadius = 3;
      let headFilter = effectKit.createEffect(pixelMap); // create Filter Instance
      if (headFilter !== null) {
        headFilter.blur(blurRadius); // set static blur. Add the blur effect to the effect list
        // retrieve the image of the source image with the added linked list effect PixelMap
        headFilter.getEffectPixelMap().then((pixelMap: image.PixelMap) => {
          this.pixelMap = pixelMap;
        });
      }
    })
  }

  @Builder
  staticBlurBuilder() {
    Stack({ alignContent: Alignment.Bottom }) {
      Image(this.pixelMap)
        .width('100%')
        .height('100%')
        .objectFit(ImageFit.Fill)
      Button('close')
        .width('90%')
        .height(40)
        .margin({ bottom: this.bottomSafeHeight + 16 })
        .onClick(() => {
          this.isShowStaticBlur = false;
        })
    }
    .width('100%')
    .height('100%')
  }

  build() {
    NavDestination() {
      Column() {
        Button('static blur')
          .width('90%')
          .height(40)
          .onClick(() => {
            this.isShowStaticBlur = true;
            // set static blur
            this.staticBlur();
          })
          .bindContentCover(this.isShowStaticBlur, this.staticBlurBuilder(), {
            modalTransition: ModalTransition.DEFAULT
          })
      }
      .padding({ bottom: this.bottomSafeHeight + 16 })
      .width('100%')
      .height('100%')
      .justifyContent(FlexAlign.End)
    }
    .hideTitleBar(true)
  }
}
// [End static_start]