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

import { LineChartModel } from '@ohos/mpchart';
import { LineChart } from '@ohos/mpchart'
import { XAxis, XAxisPosition } from '@ohos/mpchart';
import { YAxis, AxisDependency, YAxisLabelPosition } from '@ohos/mpchart'
import { LineData } from '@ohos/mpchart';
import { LineDataSet, Mode } from '@ohos/mpchart';
import { EntryOhos } from '@ohos/mpchart';
import { JArrayList } from '@ohos/mpchart';
import { ILineDataSet } from '@ohos/mpchart'
import { ColorTemplate } from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct DynamicPage {
  leftAxis: YAxis | null = null
  rightAxis: YAxis | null = null
  topAxis: XAxis = new XAxis()
  bottomAxis: XAxis = new XAxis(); //底部X轴
  @Local lineData: LineData = new LineData()
  mWidth = 300
  mHeight = 300
  @Local flag: number = 0
  colors = ColorTemplate.VORDIPLOM_COLORS
  private index: number = 0;
  @Local lineChartModel: LineChartModel = new LineChartModel();
  //标题栏标题
  private title: string = 'Dynamic'
  titleModel: ChartTitleModel = new ChartTitleModel()

  aboutToAppear() {
    let topAxis = this.lineChartModel.getXAxis();
    if (topAxis) {
      topAxis.setLabelCount(5, false);
      topAxis.setAxisMinimum(0);
      topAxis.setAxisMaximum(50);
      topAxis.setDrawAxisLine(false)
      topAxis.setDrawLabels(true)
      topAxis.setDrawGridLines(true)
    }

    let leftAxis = this.lineChartModel.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(6, false);
      leftAxis.setDrawGridLines(true);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      leftAxis.setAxisMinimum(0);
      leftAxis.setAxisMaximum(50);
    }

    let rightAxis = this.lineChartModel.getAxisRight();
    if (rightAxis) {
      rightAxis.setDrawGridLines(true);
      rightAxis.setLabelCount(6, false);
      rightAxis.setAxisMinimum(0);
      rightAxis.setAxisMaximum(50);
    }
    this.lineChartModel.setData(this.lineData);
  }

  private values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();

  private initCurveData(count: number, range: number): LineData {
    this.values.add(new EntryOhos(count, range));
    let dataSet = new JArrayList<ILineDataSet>();
    let set1 = new LineDataSet(this.values, "DataSet 1");
    set1.setDrawFilled(false);
    set1.setDrawValues(true);
    set1.setDrawCircles(true);
    set1.setDrawCircleHole(true)
    set1.setCircleRadius(4);
    set1.setCircleHoleRadius(2)
    set1.setCircleHoleColor(Color.White)
    set1.setCircleColor(ColorTemplate.colorRgb(140, 234, 255))
    set1.setMode(Mode.LINEAR);
    set1.setColorByColor(ColorTemplate.colorRgb(140, 234, 255));
    set1.setLineWidth(1.5)
    dataSet.add(set1);
    return new LineData(dataSet)
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Button('Add Entry', { type: ButtonType.Capsule, stateEffect: true })
        .backgroundColor(0x317aff).width(120).margin({ left: 100, top: 25, bottom: 25 })
        .onClick(() => {
          this.addEntry();
        })

      if (this.flag == 0) {
        Text('No chart data available. Use the menu to add entries and data sets!')
          .width('100%')
          .height('100%')
          .textAlign(TextAlign.Center)
      } else {
        LineChart({ model: this.lineChartModel })
      }


    }.width('100%').height('80%').alignItems(HorizontalAlign.Start).align(Alignment.Center)
  }

  addEntry() {
    if (this.flag != 1) {
      this.flag = 1;
    }
    this.lineChartModel.setData(this.initCurveData(this.index++, Math.random() * 50));
  }

  removeLastEntry() {
    if (this.lineData != null) {
      let set: ILineDataSet | null = this.lineData.getDataSetByIndex(0)
      if (set != null) {
        let e: EntryOhos | null = set.getEntryForXValue(set.getEntryCount() - 1, Number.NaN)
        if (e) {
          this.lineData.removeEntry(e, 0)
        }
      }
    }
  }

  addDataSet() {

    if (this.lineData == null) {
      this.lineData = new LineData()
    } else {
      let count: number = this.lineData.getDataSetCount() + 1

      let dataSetByIndex = this.lineData.getDataSetByIndex(0);
      if (dataSetByIndex) {
        let amount: number = dataSetByIndex.getEntryCount()

        let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>()

        for (let i = 0;i < amount; i++) {
          values.add(new EntryOhos(i, (Math.random() * 50) + 50 * count))
        }

        let dataSet: LineDataSet = new LineDataSet(values, 'DataSet ' + count)

        dataSet.setLineWidth(2.5)
        dataSet.setCircleRadius(4.5)

        let color = this.colors[count%this.colors.length]

        dataSet.setColorByColor(color)
        dataSet.setCircleColor(color)
        dataSet.setHighLightColor(color)
        dataSet.setValueTextSize(10)
        dataSet.setValueTextColor(color)

        this.lineData.addDataSet(dataSet)
        this.lineData.notifyDataChanged()
      }
    }
    this.flag = 1
  }

  removeDataSet() {


    if (this.lineData != null) {
      let dataSet = this.lineData.getDataSetByIndex(this.lineData.getDataSetCount() - 1);
      if (dataSet) {
        this.lineData.removeDataSet(dataSet)
      }
    }

  }

  clear() {
    this.lineData.clearValues()
    this.flag = 0
  }

  menuCallback(itemStr: string, index: number) {
    LogUtil.log('lz index:' + index)
    switch (index) {
      case 0:
        break;
      case 1:
        this.addEntry()
      //prompt.showToast({ message: 'Entry added!', duration: 1000});
        break;
      case 2:
        this.removeLastEntry()
      //prompt.showToast({ message: 'Entry removed!', duration: 1000});
        break;
      case 3:
        this.addDataSet()
      //prompt.showToast({ message: 'DataSet added!', duration: 1000});
        break;
      case 4:
        this.removeDataSet()
      //prompt.showToast({ message: 'DataSet removed!', duration: 1000});
        break;
      case 5:
        this.clear()
      //prompt.showToast({ message: 'Chart cleared!', duration: 1000});
        break;
      case 6:
        break;
    }
  }

  createSet(): LineDataSet {
    let values = new JArrayList<EntryOhos>();

    for (let i = 0; i < 10; i++) {
      let yVal: number = (Math.random() * 50);
      values.add(new EntryOhos(i, yVal));
    }
    let set1: LineDataSet = new LineDataSet(values, "DataSet 1");

    set1.setDrawFilled(false);
    set1.setDrawValues(true);
    set1.setDrawCircles(true);
    set1.setDrawCircleHole(true)
    set1.setCircleRadius(4);
    set1.setCircleHoleRadius(2)
    set1.setCircleHoleColor(Color.White)
    set1.setCircleColor(ColorTemplate.colorRgb(140, 234, 255))
    set1.setMode(Mode.LINEAR);
    set1.setColorByColor(ColorTemplate.colorRgb(140, 234, 255));
    set1.setLineWidth(2.5)
    return set1;
  }
}