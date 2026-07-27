/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

import {
  JArrayList,
  EntryOhos,
  ILineDataSet,
  LineData,
  LineChart,
  LineChartModel,
  Mode,
  LineDataSet,
  AxisDependency,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';

@Extend(Text)
function textStyle() {
  .backgroundColor(Color.White)
  .width('100%')
  .textAlign(TextAlign.Center)
}

@Entry
@ComponentV2
struct HitTestBehaviorPage {
  private modelDefault: LineChartModel = new LineChartModel();
  private modelBlock: LineChartModel = new LineChartModel();
  private modelTransparent: LineChartModel = new LineChartModel();
  private modelNone: LineChartModel = new LineChartModel();
  //标题栏标题
  private title: string = 'HitTestBehaviorPage'
  lineData: LineData | null = null;

  @Builder
  placeHolder() {
    Text($r('app.string.placeholder_area'))
      .width('100%')
      .height(200)
      .backgroundColor(Color.Gray)
      .borderRadius(3)
      .margin(3)
      .textAlign(TextAlign.Center)
  }

  //标题栏菜单回调
  titleModel: ChartTitleModel = new ChartTitleModel()

  setModel(mode: HitTestMode) {
    this.modelDefault = new LineChartModel();
    this.modelDefault.getLegend()?.setEnabled(false);
    this.modelDefault.getDescription()?.setEnabled(false);
    this.modelDefault.setData(this.lineData);
    this.modelDefault.setHitTestMode(mode);
    this.modelDefault.invalidate();
  }

  setModelMode(model: LineChartModel, mode: HitTestMode) {
    model.getLegend()?.setEnabled(false);
    model.getDescription()?.setEnabled(false);
    model.setData(this.lineData);
    model.setHitTestMode(mode);
    model.setVisibleXRangeMaximum(10);
    model.setVisibleYRangeMaximum(50, AxisDependency.LEFT);
    model.moveViewTo(10, 50, AxisDependency.LEFT);
    model.invalidate();
  }

  aboutToAppear() {
    this.titleModel.title = this.title;

    // 创建一个 JArrayList 对象，用于存储 EntryOhos 类型的数据
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    // 循环生成 1 到 20 的随机数据，并添加到 values 中
    for (let i = 1; i <= 20; i++) {
      values.add(new EntryOhos(i, Math.random() * 100));
    }
    // 创建 LineDataSet 对象，使用 values 数据，并设置数据集的名称为 'DataSet'
    let dataSet = new LineDataSet(values, 'DataSet');
    dataSet.setMode(Mode.CUBIC_BEZIER);
    dataSet.setDrawCircles(false);
    dataSet.setColorByColor(Color.Blue);
    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);
    // 创建 LineData 对象，使用 dataSetList数据，并将其传递给model
    this.lineData = new LineData(dataSetList);
    this.setModelMode(this.modelDefault, HitTestMode.Default);
    this.setModelMode(this.modelBlock, HitTestMode.Block);
    this.setModelMode(this.modelTransparent, HitTestMode.Transparent);
    this.setModelMode(this.modelNone, HitTestMode.None);
  }

  @Builder
  lineChart(model: LineChartModel) {
    LineChart({ model: model })
      .width('100%')
      .height('20%')
      .backgroundColor(Color.White)
      .margin({ bottom: 20 })
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Scroll() {
        Column() {
          this.placeHolder()

          Text($r('app.string.hitTestModeDefault')).textStyle()
          this.lineChart(this.modelDefault);

          Text($r('app.string.hitTestModeBlock')).textStyle()
          this.lineChart(this.modelBlock);

          Text($r('app.string.hitTestModeTransparent')).textStyle()
          this.lineChart(this.modelTransparent);

          Text($r('app.string.hitTestModeNone')).textStyle()
          this.lineChart(this.modelNone);

          this.placeHolder()
        }.backgroundColor(Color.Gray)
      }
    }
  }
}