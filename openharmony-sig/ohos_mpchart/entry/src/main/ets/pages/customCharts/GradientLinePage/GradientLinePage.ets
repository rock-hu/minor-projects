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
  JArrayList,EntryOhos,ILineDataSet,LineData,LineChart,LineChartModel,
  Mode,
  LineDataSet,
  AxisDependency,
  XAxisPosition,
} from '@ohos/mpchart';
import MyAxisLeftRender from './MyAxisRender';
import MyDataRender from './MyDataRender';

@Entry
@Component
struct Index {
  private model: LineChartModel = new LineChartModel();

  aboutToAppear() {
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
    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);
    // 创建 LineData 对象，使用 dataSetList数据，并将其传递给model
    let lineData: LineData = new LineData(dataSetList);
    this.model.getAxisLeft()?.setAxisLineWidth(2);
    this.model.getXAxis()?.setPosition(XAxisPosition.BOTTOM);
    this.model.getAxisRight()?.setEnabled(false);
    this.model.getDescription()?.setEnabled(false);
    this.model.setRenderer(new MyDataRender(this.model, this.model.getAnimator()!, this.model.getViewPortHandler()))
    this.model.setRendererLeftYAxis(new MyAxisLeftRender(this.model.getViewPortHandler(), this.model.getAxisLeft()!, this.model.getTransformer(AxisDependency.LEFT)!))
    this.model.setTouchEnabled(false);
    this.model?.setData(lineData);

  }

  build() {
    Column() {
      LineChart({ model: this.model })
        .width('100%')
        .height('50%')
        .backgroundColor(Color.White)
    }
  }
}