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
  XAxisPosition,
  IAxisValueFormatter,
  AxisBase,
} from '@ohos/mpchart';

class RightAxisFormatter implements IAxisValueFormatter {
  maxNumber: number = 0;
  constructor(maxNumber: number) {
    this.maxNumber = maxNumber;
  }
  getFormattedValue(value: number, axis: AxisBase): string {
    return ((value / this.maxNumber) * 100).toFixed(0) + "%";
  }
}

@Entry
@ComponentV2
struct LeftRightAxisPage {
  private model: LineChartModel = new LineChartModel();
  minNumber: number = 0;
  maxNumber: number = 500;
  @Local dataReady: boolean = false;

  setData() {
    // 创建一个 JArrayList 对象，用于存储 EntryOhos 类型的数据
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    // 循环生成 1 到 20 的随机数据，并添加到 values 中
    for (let i = 1; i <= 20; i++) {
      values.add(new EntryOhos(i, Math.random() * 500));
    }
    // 创建 LineDataSet 对象，使用 values 数据，并设置数据集的名称为 'DataSet'
    let dataSet = new LineDataSet(values, 'DataSet');
    dataSet.setMode(Mode.CUBIC_BEZIER);
    dataSet.setDrawCircles(false);
    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);
    // 创建 LineData 对象，使用 dataSetList数据，并将其传递给model
    let lineData: LineData = new LineData(dataSetList);
    let leftAxis = this.model.getAxisLeft();
    let rightAxis = this.model.getAxisRight();

    leftAxis?.setAxisMinimum(this.minNumber);
    leftAxis?.setAxisMaximum(this.maxNumber);
    //设置有6个标签
    leftAxis?.setLabelCount(6, true);

    rightAxis?.setAxisMinimum(this.minNumber);
    rightAxis?.setAxisMaximum(this.maxNumber);
    //设置有6个标签
    rightAxis?.setLabelCount(6, true);

    //设置标签文本转换器
    rightAxis?.setValueFormatter(new RightAxisFormatter(this.maxNumber));
    this.model.getXAxis()?.setPosition(XAxisPosition.BOTTOM);
    this.model.getDescription()?.setEnabled(false);
    this.model.setTouchEnabled(false);
    this.model?.setData(lineData);
    this.dataReady = true;
  }

  aboutToAppear() {
    // 模拟网络请求，延时1秒获取数据
    setTimeout(() => {
      this.setData()
    }, 200);
  }

  build() {
    RelativeContainer() {
      if (this.dataReady) {
        LineChart({ model: this.model })
          .width('100%')
          .height('50%')
          .backgroundColor(Color.White)
          .alignRules({
            left: { anchor: '__container__', align: HorizontalAlign.Start },
            top: { anchor: '__container__', align: VerticalAlign.Top }
          })
      } else {
        LoadingProgress()
          .color(Color.Blue)
          .width(30)
          .height(30)
          .alignRules({
            left: { anchor: '__container__', align: HorizontalAlign.Center },
            top: { anchor: '__container__', align: VerticalAlign.Center },
          })
          .offset({
            x:-15,
            y:-15
          })
      }
    }
    .width("100%")
    .height("100%")
  }
}