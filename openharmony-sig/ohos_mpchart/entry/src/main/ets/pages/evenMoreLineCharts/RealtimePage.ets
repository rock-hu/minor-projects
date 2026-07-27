/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import { LineChart, LineChartModel } from '@ohos/mpchart'
import { XAxis, XAxisPosition } from '@ohos/mpchart';
import { YAxis, AxisDependency, YAxisLabelPosition } from '@ohos/mpchart'
import { LineData } from '@ohos/mpchart';
import { LineDataSet, Mode } from '@ohos/mpchart';
import { EntryOhos } from '@ohos/mpchart';
import { JArrayList } from '@ohos/mpchart';
import { ILineDataSet } from '@ohos/mpchart'
import { ColorTemplate, ChartColor } from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct RealtimePage {
  leftAxis: YAxis | null = null
  rightAxis: YAxis | null = null
  topAxis: XAxis = new XAxis()
  bottomAxis: XAxis = new XAxis(); //底部X轴
  @Local lineData: LineData = new LineData()
  colors = ColorTemplate.VORDIPLOM_COLORS
  private index: number = 0;
  @Local model: LineChartModel = new LineChartModel();
  isAddRunning: boolean = false;
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  private menuItemArr: Array<string> = [Constants.ADD_ENTRY, Constants.CLEAR_CHART, Constants.ADD_MULTIPLE];
  title: string = 'RealTimeLineChartPage'
  intervalID: number = -1;

  //标题栏菜单回调
  @Monitor("titleModel.index")
  menuCallback() {
    if (this.titleModel == null || this.titleModel == undefined) {
      return
    }
    let index: number = this.titleModel.getIndex()
    if (!this.model || index == undefined || index == -1) {
      return
    }
    let barData = this.model.getLineData();
    if (!barData) {
      return;
    }

    switch (this.menuItemArr[index]) {
      case Constants.ADD_MULTIPLE:
        if (!this.isAddRunning) {
          this.feedMultiple();
        }
        break;
      case Constants.ADD_ENTRY:
        if (!this.isAddRunning) {
          this.addEntry();
        }
        break;
      case Constants.CLEAR_CHART:
        clearInterval(this.intervalID);
        this.isAddRunning = false;
        this.clear();
        break;
      default:
        break;
    }
    this.titleModel.setIndex(-1)
  }

  aboutToDisappear() {
    if (this.intervalID != -1) {
      clearInterval(this.intervalID);
    }
  }

  aboutToAppear() {
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title
    let topAxis = this.model.getXAxis();
    if (topAxis) {
      topAxis.setLabelCount(5, false);
      topAxis.setPosition(XAxisPosition.TOP);
      topAxis.setAxisMinimum(0);
      topAxis.setAxisMaximum(50);
      topAxis.setDrawAxisLine(false)
      topAxis.setDrawLabels(true)
      topAxis.setDrawGridLines(false)
    }

    let leftAxis = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis = new YAxis(AxisDependency.LEFT);
      leftAxis.setLabelCount(6, false);
      leftAxis.setDrawGridLines(true);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      leftAxis.setAxisMinimum(0);
      leftAxis.setAxisMaximum(50);
    }

    let rightAxis = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis = new YAxis(AxisDependency.RIGHT);
      rightAxis.setDrawGridLines(true);
      rightAxis.setLabelCount(6, false);
      rightAxis.setAxisMinimum(0);
      rightAxis.setAxisMaximum(50);
      rightAxis.setDrawAxisLine(false);
      rightAxis.setDrawLabels(false);
      rightAxis.setDrawGridLines(false);
    }

    this.model.setData(this.lineData);

  }

  private values1: EntryOhos[] = [];
  private values = new JArrayList<EntryOhos>();

  private initCurveData(count: number, range: number): LineData {
    this.values1.push(new EntryOhos(count, range));
    this.values.clear();
    for (let i = 0; i < this.values1.length; i++) {
      this.values.add(new EntryOhos(this.values1[i].getX(), this.values1[i].getY()));
    }
    let dataSet = new JArrayList<ILineDataSet>();
    let set1 = new LineDataSet(this.values, "DataSet 1");
    set1.setDrawFilled(false);
    set1.setDrawValues(false);
    set1.setDrawCircles(true);
    set1.setDrawCircleHole(true)
    set1.setCircleRadius(4);
    set1.setCircleHoleRadius(2)
    set1.setCircleHoleColor(Color.White)
    set1.setCircleColor(Color.White)
    set1.setMode(Mode.LINEAR);
    set1.setColorByColor(ColorTemplate.colorRgb(51, 181, 229));
    set1.setLineWidth(1.5)
    dataSet.add(set1);
    return new LineData(dataSet)
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      LineChart({ model: this.model })
        .width('100%')
        .height('80%')

    }
    .backgroundColor(0xFFCCCCCC)
    .width('100%')
    .height('100%')
    .alignItems(HorizontalAlign.Start)
    .align(Alignment.Center)
    .borderWidth(2)
  }

  addEntry() {
    let data = this.model.getData();
    if (data != null) {
      let set = data.getDataSetByIndex(0);
      if (set == null) {
        set = this.createSet();
        data.addDataSet(set);
      }

      data.addEntry(new EntryOhos(set.getEntryCount(), (Math.random() * 40) + 30), 0);
      data.notifyDataChanged();

      this.model.notifyDataSetChanged();

      this.model.setVisibleXRangeMaximum(50);

      this.model.moveViewToX(data.getEntryCount());
    }
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
    this.model.clearValues();
  }

  createSet(): LineDataSet {
    let set = new LineDataSet(null, "Dynamic Data");
    set.setAxisDependency(AxisDependency.LEFT);
    set.setColorByColor(ColorTemplate.getHoloBlue());
    set.setCircleColor(Color.White);
    set.setLineWidth(2);
    set.setCircleRadius(4);
    set.setFillAlpha(65);
    set.setFillColor(ColorTemplate.getHoloBlue());
    set.setHighLightColor(ChartColor.rgb(244, 117, 117));
    set.setValueTextColor(Color.White);
    set.setValueTextSize(9);
    set.setDrawValues(false);
    return set;
  }

  feedMultiple(): void {
    this.isAddRunning = true;
    let counter = 0;
    this.intervalID = setInterval(() => {
      counter++;
      this.addEntry();
      LogUtil.log(`this.counter = ${counter}`);
      if (counter > 100) {
        clearInterval(this.intervalID);
        this.isAddRunning = false;
      }
    }, 300);
  }
}