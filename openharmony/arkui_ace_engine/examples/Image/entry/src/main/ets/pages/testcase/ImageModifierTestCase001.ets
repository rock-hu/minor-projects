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
import { $r } from '@ohos.arkui.component.Common';
import { ImageModifier } from '@kit.ArkUI';

/**
 * 验证Image的基础功能
 */
@Entry
@Component
struct ImageExample {
  @State imageSource: ResourceStr | undefined = undefined;
  @State imageFits: ImageFit[] = [];
  @State imageFitIndex: number = 0;
  @State imageRepeats: ImageRepeat[] = [];
  @State imageRepeatIndex: number = 0;
  @State imageRenderMode: ImageRenderMode[] = [];
  @State imageRenderModeIndex: number = 0;
  @State imageInterpolations: ImageInterpolation[] = [];
  @State imageInterpolationIndex: number = 0;
  @State fillColor: ResourceColor | undefined = undefined;
  fillColorIndex: number = 0;
  @State dynamicRangeModes: DynamicRangeMode[] = [];
  @State dynamicRangeModeIndex: number = 0;

  aboutToAppear(): void {
    this.pushDataToImageFits();
    this.pushDataToImageRepeat();
    this.pushDataToImageRenderMode();
    this.pushDataToImageInterpolation();
    this.pushDataToImageDynamicRangeMode();
  }

  private pushDataToImageDynamicRangeMode() {
    this.dynamicRangeModes.push(DynamicRangeMode.HIGH);
    this.dynamicRangeModes.push(DynamicRangeMode.CONSTRAINT);
    this.dynamicRangeModes.push(DynamicRangeMode.STANDARD);
    this.dynamicRangeModes.push(DynamicRangeMode.CONSTRAINT);
  }

  private pushDataToImageInterpolation() {
    this.imageInterpolations.push(ImageInterpolation.None);
    this.imageInterpolations.push(ImageInterpolation.Low);
    this.imageInterpolations.push(ImageInterpolation.Medium);
    this.imageInterpolations.push(ImageInterpolation.Low);
    this.imageInterpolations.push(ImageInterpolation.None);
    this.imageInterpolations.push(ImageInterpolation.High);
    this.imageInterpolations.push(ImageInterpolation.None);
  }

  private pushDataToImageRenderMode() {
    this.imageRenderMode.push(ImageRenderMode.Original);
    this.imageRenderMode.push(ImageRenderMode.Template);
    this.imageRenderMode.push(ImageRenderMode.Original);
  }

  private pushDataToImageRepeat() {
    this.imageRepeats.push(ImageRepeat.NoRepeat);
    this.imageRepeats.push(ImageRepeat.X);
    this.imageRepeats.push(ImageRepeat.XY);
    this.imageRepeats.push(ImageRepeat.XY);
    this.imageRepeats.push(ImageRepeat.XY);
    this.imageRepeats.push(ImageRepeat.Y);
    this.imageRepeats.push(ImageRepeat.Y);
    this.imageRepeats.push(ImageRepeat.Y);
  }

  private pushDataToImageFits() {
    this.imageFits.push(ImageFit.Contain);
    this.imageFits.push(ImageFit.Cover);
    this.imageFits.push(ImageFit.ScaleDown);
    this.imageFits.push(ImageFit.CENTER);
    this.imageFits.push(ImageFit.START);
    this.imageFits.push(ImageFit.TOP_START);
    this.imageFits.push(ImageFit.CENTER);
    this.imageFits.push(ImageFit.Auto);
    this.imageFits.push(ImageFit.BOTTOM_START);
    this.imageFits.push(ImageFit.BOTTOM_END);
    this.imageFits.push(ImageFit.END);
    this.imageFits.push(ImageFit.Auto);
    this.imageFits.push(ImageFit.Fill);
    this.imageFits.push(ImageFit.END);
    this.imageFits.push(ImageFit.None);
  }

  private changDataByImageRepeat() {
    this.imageRepeatIndex++;
    if (this.imageRepeatIndex >= this.imageRepeats.length) {
      this.imageRepeatIndex = 0;
    }
  }

  private changDataByImageFit() {
    this.imageFitIndex++;
    if (this.imageFitIndex >= this.imageFits.length) {
      this.imageFitIndex = 0;
    }
  }

  private changDataByImageRenderMode() {
    this.imageRenderModeIndex++;
    if (this.imageRenderModeIndex >= this.imageRenderMode.length) {
      this.imageRenderModeIndex = 0;
    }
  }

  private changDataByInterpolation() {
    this.imageInterpolationIndex++;
    if (this.imageInterpolationIndex >= this.imageInterpolations.length) {
      this.imageInterpolationIndex = 0;
    }
  }

  private changDataByDynamicRangeMode() {
    this.dynamicRangeModeIndex++;
    if (this.dynamicRangeModeIndex >= this.dynamicRangeModes.length) {
      this.dynamicRangeModeIndex = 0;
    }
  }

  private changDataByFillColor() {
    this.fillColorIndex++;
    switch (this.fillColorIndex) {
      case 0:
        this.fillColor = Color.Red;
        break;
      case 1:
        this.fillColor = '#FF000000';
        break;
      case 1:
        this.fillColor = Color.White;
        break;
      case 1:
        this.fillColor = Color.Yellow;
        break;
      case 1:
        this.fillColor = Color.Transparent;
        break;
      case 2:
        this.fillColor = 111;
        break;
      case 3:
        this.fillColor = $r('app.string.color');
        break;
      case 4:
        this.fillColor = undefined;
        break;
    }
  }

  build() {
    Column() {
      Flex({ direction: FlexDirection.Column, alignItems: ItemAlign.Start }) {
        Row() {
          // 加载png格式图片
          Image($r('app.media.sky'))
            .width(120)
            .height(140)
            .margin(17)
            .overlay('png', { align: Alignment.Bottom, offset: { x: 0, y: 20 } })
            .border({ width: 2, color: Color.Pink })
            .objectFit(this.imageFits[this.imageFitIndex])
            .objectRepeat(this.imageRepeats[this.imageRepeatIndex])
            .renderMode(this.imageRenderMode[this.imageRenderModeIndex])
            .interpolation(this.imageInterpolations[this.imageInterpolationIndex])
            .fillColor(this.fillColor)
            .dynamicRangeMode(this.dynamicRangeModes[this.dynamicRangeModeIndex])
            .attributeModifier(new MyImageModifier())
          Button('change imageFitIndex').onClick(() => {
            this.changDataByImageFit();
            console.log(`imageFitIndex: ${this.imageFitIndex}`);
          })
          Button('change imageRepeatIndex').onClick(() => {
            this.changDataByImageRepeat();
            console.log(`imageRepeatIndex: ${this.imageRepeatIndex}`);
          })
          Button('change imageRenderModeIndex').onClick(() => {
            this.changDataByImageRenderMode();
            console.log(`imageRenderModeIndex: ${this.imageRenderModeIndex}`);
          })
          Button('change imageInterpolationIndex').onClick(() => {
            this.changDataByInterpolation();
            console.log(`imageInterpolationIndex: ${this.imageInterpolationIndex}`);
          })
          Button('change fillColor').onClick(() => {
            this.changDataByFillColor();
            console.log(`fillColor: ${this.fillColorIndex}`);
          })
          Button('change dynamicRangeModes').onClick(() => {
            this.changDataByDynamicRangeMode();
            console.log(`dynamicRangeModes: ${this.dynamicRangeModeIndex}`);
          })

          // 加载base64格式图片
          Image($r('app.media.sky'))
            .width(110)
            .height(110)
            .margin(15)
            .overlay('gif', { align: Alignment.Bottom, offset: { x: 0, y: 20 } })
            .border({ width: 2, color: Color.Pink })
            .objectFit(ImageFit.BOTTOM_START)
        }
      }
    }.height(320).width(360).padding({ right: 10, top: 10 })
  }
}

class MyImageModifier extends ImageModifier {
  private colorFilterMatrix: number[] = [1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1, 0, 1, 0]

  applyNormalAttribute?(instance: ImageAttribute): void {
    instance.alt('hello1');
    instance.objectFit(ImageFit.Contain);
    instance.objectRepeat(ImageRepeat.X);
    instance.interpolation(ImageInterpolation.Low);
    instance.renderMode(ImageRenderMode.Original);
    instance.sourceSize({ width: 100, height: 200 });
    instance.matchTextDirection(false);
    instance.fitOriginalSize(false);
    instance.fillColor('#ffffffff');
    instance.autoResize(true);
    instance.syncLoad(true);
    instance.copyOption(CopyOptions.None);
    instance.colorFilter(this.colorFilterMatrix);
    instance.draggable(false);
    instance.enableAnalyzer(true);
    instance.resizable({
      slice: {
        left: '12',
        right: '44',
        bottom: '55',
        top: '44'
      }
    })
    instance.width(100);
    instance.height(200);
    instance.privacySensitive(true);
    instance.dynamicRangeMode(DynamicRangeMode.HIGH);

    instance.onError((err) => {
      console.log(`onError: ${JSON.stringify(err)}`);
    });
    instance.onComplete((info) => {
      console.log(`onComplete: ${JSON.stringify(info)}`);
    });
    instance.onFinish(() => {
      console.log(`onFinish`);
    })
  }
}