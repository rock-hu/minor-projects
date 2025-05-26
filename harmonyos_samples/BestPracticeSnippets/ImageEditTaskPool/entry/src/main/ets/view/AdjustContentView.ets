/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License,Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { taskpool } from '@kit.ArkTS';
import { image } from '@kit.ImageKit';
import { adjustIconList, IconStatus } from '../viewModel/IconListViewModel';
import { adjustImageValue, adjustSaturation } from '../utils/AdjustUtil';
import { AdjustId } from '../viewModel/OptionViewModel';
import { adjustOpacity } from '../utils/OpacityUtil';

@Component
export default struct AdjustContentView {
  @State currentAdjustIndex: number = 0;
  @Consume('currentAdjustData') currentAdjustData: Array<number>;
  adjustIconList: Array<IconStatus> = adjustIconList;

  @Builder
  TabBuilder(index: number, name: ResourceStr) {
    Column() {
      Row() {
        Image(this.currentAdjustIndex === index ? this.adjustIconList[index]?.chosen :
          this.adjustIconList[index]?.normal)
          .width(24)
          .height(24)
      }
      .alignItems(VerticalAlign.Center)
      .justifyContent(FlexAlign.Center)
      .backgroundColor('#333333')
      .borderRadius(20)
      .width(40)
      .height(40)

      Text(name)
        .fontColor(this.currentAdjustIndex === index ? '#006CDE' : Color.White)
        .fontSize(10)
        .padding({ top: 8 })
    }
    .width('100%')
  }

  build() {
    Tabs({ barPosition: BarPosition.End }) {
      TabContent() {
        Column() {
          SliderCustom({
            currentIndex: AdjustId.BRIGHTNESS.valueOf(),
            min: 1,
            max: 100,
            currentAdjustData: this.currentAdjustData
          })
        }
        .justifyContent(FlexAlign.End)
        .height('100%')
        .padding({
          bottom: 24
        })
      }
      .tabBar(this.TabBuilder(AdjustId.BRIGHTNESS, $r('app.string.brightness')))

      TabContent() {
        Column() {
          SliderCustom({
            currentIndex: AdjustId.TRANSPARENCY.valueOf(),
            min: 1,
            max: 100,
            currentAdjustData: this.currentAdjustData
          })
        }
        .justifyContent(FlexAlign.End)
        .height('100%')
        .padding({
          bottom: 24
        })
      }
      .tabBar(this.TabBuilder(AdjustId.TRANSPARENCY, $r('app.string.transparency')))

      TabContent() {
        Column() {
          SliderCustom({
            currentIndex: AdjustId.SATURATION.valueOf(),
            min: 1,
            max: 100,
            currentAdjustData: this.currentAdjustData
          })
        }
        .justifyContent(FlexAlign.End)
        .height('100%')
        .padding({
          bottom: 24
        })
      }
      .tabBar(this.TabBuilder(AdjustId.SATURATION, $r('app.string.saturation')))
    }
    .barHeight(60)
    .padding({ bottom: 24 })
    .onChange((index: number) => {
      this.currentAdjustIndex = index;
    })
  }
}

@Component
struct SliderCustom {
  @Prop currentIndex: number;
  @Link currentAdjustData: number[];
  @Prop min: number;
  @Prop max: number;
  @Consume('pixelMap') pixelMap?: image.PixelMap;
  @Consume('isPixelMapChange') isPixelMapChange: boolean;
  private postState: boolean = true;
  saturationLastSlider: number = 100;
  brightnessLastSlider: number = 100;
  deviceListDialogController: CustomDialogController = new CustomDialogController({
    builder: Dialog(),
    alignment: DialogAlignment.Center,
    autoCancel: false,
    customStyle: true
  });

  build() {
    Column() {
      Text(`${this.currentAdjustData[this.currentIndex]}`)
        .fontColor(Color.White)
        .margin({ top: -24 })
        .fontSize(16)
      Row() {
        Slider({
          value: this.currentAdjustData[this.currentIndex],
          step: 10,
          min: this.min,
          max: this.max
        })
          .height(20)
          .blockColor(Color.White)
          .selectedColor('#E6E6E6')
          .trackColor('#333333')
          .width('100%')
          .showSteps(true)
          .padding({
            right: 60,
            left: 60
          })
          .onChange((value: number, mode: SliderChangeMode) => {
            this.sliderChange(value > this.max ? this.max : value, mode);
          })
      }
      .width('100%')
      .justifyContent(FlexAlign.End)
    }
  }

  sliderChange(value: number, mode: SliderChangeMode): void {
    if ((mode === SliderChangeMode.End) && (value !== this.currentAdjustData[this.currentIndex])) {
      this.currentAdjustData[this.currentIndex] = Math.round(value);
      switch (this.currentIndex) {
        case AdjustId.BRIGHTNESS:
          this.postProcess(AdjustId.BRIGHTNESS, value);
          break;
        case AdjustId.TRANSPARENCY:
          if (this.pixelMap) {
            adjustOpacity(this.pixelMap, Math.round(value))
              .then((pixelMap?: image.PixelMap) => {
                if (pixelMap) {
                  this.pixelMap = pixelMap;
                  this.isPixelMapChange = !this.isPixelMapChange;
                }
              });
          }
          break;
        case AdjustId.SATURATION:
          this.postProcess(AdjustId.SATURATION, value);
          break;
        default:
          break;
      }
    }
  }

  // [Start postProcess_start]
  async postProcess(type: AdjustId, value: number) {
    // [StartExclude postProcess_start]
    if (!this.pixelMap) {
      return;
    }
    let sliderValue = type === AdjustId.BRIGHTNESS ? this.brightnessLastSlider : this.saturationLastSlider;
    const bufferArray = new ArrayBuffer(this.pixelMap.getPixelBytesNumber());
    // [EndExclude postProcess_start]
    this.pixelMap.readPixelsToBuffer(bufferArray)
      .then(() => {
        const buffers: ArrayBuffer[] = splitArrayBuffer(bufferArray, 240);
        const group = splitTask(buffers, type, sliderValue, value);
        // [StartExclude postProcess_start]
        // [Start execute_start]
        taskpool.execute(group, taskpool.Priority.HIGH).then((ret) => {
          // Combine the results of each task execution
          const entireArrayBuffer = mergeArrayBuffers(ret);
          // Update the UI based on the calculation results
          this.updatePixelMap(entireArrayBuffer);
        });
        // [End execute_start]
        if (this.postState) {
          this.deviceListDialogController.open();
        }
        this.postState = false;
        if (type === AdjustId.BRIGHTNESS) {
          this.brightnessLastSlider = Math.round(value);
        } else {
          this.saturationLastSlider = Math.round(value);
        }
        // [EndExclude postProcess_start]
      })
  }
  // [StartExclude postProcess_start]
  updatePixelMap(ret: ArrayBuffer) {
    const newPixel = this.pixelMap as image.PixelMap;
    newPixel.writeBufferToPixels(ret);
    this.pixelMap = newPixel;
    this.isPixelMapChange = !this.isPixelMapChange;
    this.deviceListDialogController.close();
    this.postState = true;
  }
}

@CustomDialog
export struct Dialog {
  controller?: CustomDialogController;

  build() {
    Column() {
      LoadingProgress()
        .color(Color.White)
        .width('30%')
        .height('30%')
    }
  }
}

// [EndExclude postProcess_start]
/**
 * Each task processes a portion of the pixel data and adds the task to the task group.
 *
 */
function splitTask(buffers: ArrayBuffer[], type: AdjustId, sliderValue: number, value: number): taskpool.TaskGroup {
  // Creating a Task Group
  let group: taskpool.TaskGroup = new taskpool.TaskGroup();
  for (const buffer of buffers) {
    group.addTask(imageProcessing, {
      // Add a task to a task group
      type,
      bufferArray: buffer,
      sliderValue,
      value
    });
  }
  return group;
}
// [End postProcess_start]

@Concurrent
async function imageProcessing(args: ImageProcessing) {
  const type: AdjustId = args.type;
  const bufferArray: ArrayBuffer = args.bufferArray;
  const value: number = args.value;
  const sliderValue: number = args.sliderValue;
  if (type === AdjustId.BRIGHTNESS) {
    return adjustImageValue(bufferArray, sliderValue, value);
  } else if (type === AdjustId.SATURATION) {
    return adjustSaturation(bufferArray, sliderValue, value);
  } else {
    return undefined;
  }
}

interface ImageProcessing {
  type: AdjustId;
  bufferArray: ArrayBuffer;
  sliderValue: number;
  value: number;
}

function splitArrayBuffer(buffer: ArrayBuffer, n: number): ArrayBuffer[] {
  let num = Math.floor(buffer.byteLength / n);
  while (num % 4 !== 0) {
    num += 1;
  }
  let result: ArrayBuffer[] = [];
  for (let index = 0; index < n; index++) {
    if (index === n - 1) {
      result[index] = buffer.slice(index * num);
    } else {
      result[index] = buffer.slice(index * num, (index + 1) * num);
    }
  }
  return result;
}

function mergeArrayBuffers(buffers: Object[]) {
  let thisBuffers = buffers as ArrayBuffer[];
  // Calculate the combined total length
  let totalLength = thisBuffers.reduce((length, buffer) => {
    length += buffer.byteLength;
    return length;
  }, 0);
  // Create a new ArrayBuffer
  let mergedBuffer = new ArrayBuffer(totalLength);
  // Create a Uint8Array to operate the new Uint8Array
  let mergedArray = new Uint8Array(mergedBuffer);
  let offset = 0;
  for (let buffer of thisBuffers) {
    let array = new Uint8Array(buffer);
    mergedArray.set(array, offset);
    offset += array.length;
  }
  return mergedBuffer;
}