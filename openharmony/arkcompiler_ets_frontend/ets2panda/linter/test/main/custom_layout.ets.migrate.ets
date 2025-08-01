/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
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

import { CustomLayout } from '@kit.ArkUI';

import {
  Entry,
  Component,
  Column,
  Builder,
  ForEach,
  Text,
  BuilderParam,
  State,
  SizeResult,
  GeometryInfo,
  Layoutable,
  ConstraintSizeOptions,
  Measurable,
  MeasureResult,
} from '@kit.ArkUI';

@Entry
@Component
struct Index {
  build() {
    Column() {
      CustomLayout1({ builder: ColumnChildren })
      CustomLayout2({ builder: ColumnChildren })
      CustomLayout3({ builder: ColumnChildren })
    }
  }
}

@Builder
function ColumnChildren() {
  ForEach([1.0, 2.0, 3.0], (index: number) => { //暂不支持lazyForEach的写法
    Text('S' + index)
      .fontSize(30.0)
      .width(100.0)
      .height(100.0)
      .borderWidth(2.0)
      .offset({ x: 10.0, y: 20.0 })
  })
}

@Component
@CustomLayout
struct CustomLayout1 {
  @Builder
  doNothingBuilder() {
  };

  @BuilderParam builder: () => void = this.doNothingBuilder;
  @State startSize: number = 100.0;
  result: SizeResult = {
    width: 0.0,
    height: 0.0
  };

  onPlaceChildren(selfLayoutInfo: GeometryInfo, children: Array<Layoutable>, constraint: ConstraintSizeOptions) {
    let startPos: number = 300.0;
    children.forEach((child) => {
      let pos: number = startPos - child.measureResult.height;
      child.layout({ x: pos, y: pos })
    })
  }

  build() {
    this.builder()
  }
}

@Component
@CustomLayout
struct CustomLayout2 {
  @Builder
  doNothingBuilder() {
  };

  @BuilderParam builder: () => void = this.doNothingBuilder;
  @State startSize: number = 100.0;
  result: SizeResult = {
    width: 0.0,
    height: 0.0
  };

  onMeasureSize(selfLayoutInfo: GeometryInfo, children: Array<Measurable>, constraint: ConstraintSizeOptions) {
    let size: number = 100.0;
    children.forEach((child) => {
      let result: MeasureResult = child.measure({ minHeight: size, minWidth: size, maxWidth: size, maxHeight: size })
      size += result.width / 2.0
      ;
    })
    this.result.width = 100.0;
    this.result.height = 400.0;
    return this.result;
  }

  build() {
    this.builder()
  }
}

@Component
@CustomLayout
struct CustomLayout3 {
  @Builder
  doNothingBuilder() {
  };

  @BuilderParam builder: () => void = this.doNothingBuilder;
  @State startSize: number = 100.0;
  result: SizeResult = {
    width: 0.0,
    height: 0.0
  };

  onMeasureSize(selfLayoutInfo: GeometryInfo, children: Array<Measurable>, constraint: ConstraintSizeOptions) {
    let size: number = 100.0;
    children.forEach((child) => {
      let result: MeasureResult = child.measure({ minHeight: size, minWidth: size, maxWidth: size, maxHeight: size })
      size += result.width / 2.0;
    })
    this.result.width = 100.0;
    this.result.height = 400.0;
    return this.result;
  }

  onPlaceChildren(selfLayoutInfo: GeometryInfo, children: Array<Layoutable>, constraint: ConstraintSizeOptions) {
    let startPos: number = 300.0;
    children.forEach((child) => {
      let pos: number = startPos - child.measureResult.height;
      child.layout({ x: pos, y: pos })
    })
  }

  build() {
    this.builder()
  }
}

@Component
struct CustomLayout4 {
  build() {

  }
}