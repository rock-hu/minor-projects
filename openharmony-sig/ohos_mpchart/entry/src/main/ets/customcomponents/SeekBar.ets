/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

@ComponentV2
export default struct SeekBar {
  @Param model: SeekBarModel = new SeekBarModel()

  build() {
    Row() {
      Slider({
        value: this.model.value,
        min: this.model.min,
        max: this.model.max,
        style: SliderStyle.OutSet
      })
        .width('90%')
        .blockColor(0x009688)
        .trackColor(0xD8D8D8)
        .selectedColor(0x009688)
        .showSteps(true)
        .onChange((value: number, mode: SliderChangeMode) => {
          this.model.value = value
        })
      Text(this.model.value.toFixed(0)).fontSize(16)
    }
    .padding({ top: 10 })
    .width('100%')
  }
}

@ObservedV2
export class SeekBarModel {
  @Trace value: number = 45
  @Trace min: number = 0
  @Trace max: number = 200

  constructor(value ?: number) {
    this.value = value ? value : 0;
  }

  setValue(value: number): SeekBarModel {
    this.value = value ? value : 0;
    return this
  }

  getValue(): number {
    return this.value
  }

  setMin(min: number): SeekBarModel {
    this.min = min ? min : 0;
    return this
  }

  getMin(): number {
    return this.min
  }

  setMax(max: number): SeekBarModel {
    this.max = max ? max : 0;
    return this
  }

  getMax(): number {
    return this.max
  }
}
