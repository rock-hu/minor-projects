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
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct TallBarChartPage {
  scroller: Scroller = new Scroller()
  @Local model: BarChartModel = new BarChartModel();
  minOffset: number = 15; //X轴线偏移量
  leftAxis: YAxis | null = null;
  rightAxis: YAxis | null = null;
  bottomAxis: XAxis = new XAxis();
  //标题栏标题
  private title: string = 'TallBarChart'
  titleModel: ChartTitleModel = new ChartTitleModel()

  public aboutToAppear() {
    let leftAxis = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(11, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(0);
      leftAxis.setAxisMaximum(100);
      leftAxis.enableGridDashedLine(10, 10, 0)
    }

    let rightAxis = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis = new YAxis(AxisDependency.RIGHT);
      rightAxis.setDrawGridLines(false);
      rightAxis.setLabelCount(7, false);
      rightAxis.setSpaceTop(11);
      rightAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      rightAxis.setAxisMaximum(100);
    }

    let xAxis = this.model.getXAxis();
    if (xAxis) {
      xAxis.setLabelCount(6, false);
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setAxisMinimum(0);
      xAxis.setAxisMaximum(12);
    }

    this.setData(12, 100)

    this.model.setDrawBarShadow(false);
    this.model.setDrawValueAboveBar(true);
    this.model.getDescription()?.setEnabled(false);
    this.model.setMaxVisibleValueCount(60);

  }

  build() {
    Column(){
      title({ model: this.titleModel })
      Scroll(this.scroller) {
        Column() {
          Text('START OF SCROLLVIEW').width('100%').textAlign(TextAlign.Center)
          Blank().height(350)
          BarChart({ model: this.model })
            .width('80%')
            .height('150%')
          Blank().height(700)

          Text('END OF SCROLLVIEW').width('100%').textAlign(TextAlign.Center)

        }.width('100%').alignItems(HorizontalAlign.Start)

      }.width('100%').height('100%')
    }

  }

  menuCallback(itemStr: string, index: number) {
    LogUtil.log('callback1:' + itemStr + ",index:" + index)
  }

  private setData(count: number, range: number) {

    let start: number = 0;

    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();

    for (let i = start; i < start + count; i++) {
      let multi = range;
      let val: number = Math.round(Math.random() * multi);
      values.add(new BarEntry(i, val))
    }

    let set1: BarDataSet;

    let barData = this.model.getBarData();

    if (barData != null &&
      barData.getDataSetCount() > 0) {
      set1 = barData.getDataSetByIndex(0) as BarDataSet;
      set1.setValues(values);
      barData.notifyDataChanged();
      this.model.notifyDataSetChanged();

    } else {
      set1 = new BarDataSet(values, "Data Set");

      set1.setColorsByVariable(ColorTemplate.VORDIPLOM_COLORS);
      set1.setDrawValues(false);

      let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
      dataSets.add(set1);

      let data: BarData = new BarData(dataSets);

      this.model.setData(data);
      this.model.setFitBars(true);
    }
  }
}