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

import { ColorTemplate } from '@ohos/mpchart';
import { XAxis, XAxisPosition } from '@ohos/mpchart';
import { YAxis, AxisDependency, YAxisLabelPosition } from '@ohos/mpchart';
import { BarEntry } from '@ohos/mpchart';
import { JArrayList } from '@ohos/mpchart';
import { BarDataSet } from '@ohos/mpchart';
import { BarData } from '@ohos/mpchart';
import { BarChart, BarChartModel } from '@ohos/mpchart';
import { IBarDataSet } from '@ohos/mpchart';
import { LegendEntry } from '@ohos/mpchart';

interface legendArrItem {
  index: number,
  legendItem: LegendEntry
}

@Entry
@ComponentV2
export default struct fragSimpleBarIndex {
  @Param barModel: BarChartModel = new BarChartModel();
  minOffset: number = 15;

  aboutToAppear() {
    this.setData(12, 100);

    let leftAxis = this.barModel.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(7, false);
      leftAxis?.setSpaceTop(15);
      leftAxis?.setAxisMinimum(0);
      leftAxis?.setAxisMaximum(100);
      leftAxis?.enableGridDashedLine(10, 10, 0);
    }

    let rightAxis = this.barModel.getAxisRight();
    if (rightAxis) {
      rightAxis.setDrawGridLines(false);
      rightAxis.setLabelCount(7, false);
      rightAxis.setSpaceTop(11);
      rightAxis.setAxisMinimum(0);
      rightAxis.setAxisMaximum(10);
      rightAxis.setDrawAxisLine(false);
      rightAxis.setDrawLabels(false);
    }

    let xAxis = this.barModel.getXAxis();
    if (xAxis) {
      xAxis.setLabelCount(5, false);
      xAxis.setAxisMinimum(0);
      xAxis.setAxisMaximum(12);
    }


    this.barModel.setDrawBarShadow(false);
    this.barModel.setDrawValueAboveBar(true);
    this.barModel.getDescription()?.setEnabled(false);
    this.barModel.setMaxVisibleValueCount(60);
  }

  build() {
    Column() {
      Stack({ alignContent: Alignment.TopStart }) {
        BarChart({ model: this.barModel })
          .width('100%')
          .height('70%')
      }
    }.alignItems(HorizontalAlign.Start)
  }

  private setData(count: number, range: number) {
    let start: number = 1;

    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();

    for (let i = start; i < start + count; i++) {
      let multi = range;
      let val: number = (Math.random() * (multi - 6));
      values.add(new BarEntry(i, val))
    }
    let set1: BarDataSet;

    let barData = this.barModel.getBarData();
    if (barData != null &&
      barData.getDataSetCount() > 0) {
      set1 = barData.getDataSetByIndex(0) as BarDataSet;
      set1.setValues(values);
      barData.notifyDataChanged();
      this.barModel.notifyDataSetChanged();

    } else {
      set1 = new BarDataSet(values, "Company A");
      set1.setColorsByVariable(ColorTemplate.VORDIPLOM_COLORS);
      set1.setDrawValues(true);

      let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
      dataSets.add(set1);

      let data: BarData = new BarData(dataSets);

      data.setValueTextSize(10)
      this.barModel.setData(data);
      this.barModel.setFitBars(true);
    }
  }
}