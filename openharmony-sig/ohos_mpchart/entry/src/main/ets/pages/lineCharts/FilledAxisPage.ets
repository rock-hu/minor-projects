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

import {
  ChartColor,
  JArrayList,
  XAxis,
  XAxisPosition,
  YAxis,
  Description,
  Legend,
  OnChartValueSelectedListener,
  Highlight,
  EntryOhos,
  YAxisLabelPosition,
  LineDataSet,
  ILineDataSet,
  LineData,
  Mode,
  LineChart,
  LineChartModel,
  OnChartGestureListener,
  ChartGesture,
  ChartColorStop,

} from '@ohos/mpchart';

import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct FilledAxisPage {

  @Local private model: LineChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  minOffset: number = 15;
  //标题栏标题
  private title: string = 'FillLinePage'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("ScrollBarChart onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("ScrollBarChart onNothingSelected");
    }
  }
  private chartGestureListener: OnChartGestureListener = {

    onChartGestureStart: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
      LogUtil.log("-----------------chartGestureListener onChartGestureStart lastMode: " + lastPerformedGestureMode)
    },

    onChartGestureEnd: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
      LogUtil.log("-----------------chartGestureListener onChartGestureEnd lastMode: " + lastPerformedGestureMode)
    },


    onChartLongPressed: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartLongPressed")
    },


    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartDoubleTapped")

    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartSingleTapped")

    },

    onChartFling: (isTouchEvent: boolean, me1: TouchEvent | GestureEvent, me2: TouchEvent, velocityX: number, velocityY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartFling velocityX: " + velocityX + "  velocityY: " + velocityY)

    },

    onChartScale: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, scaleX: number, scaleY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartScale scaleX: " + scaleX + "  scaleY: " + scaleY)

    },

    onChartTranslate: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, dX: number, dY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate dx: " + dX + "  dy: " + dY)

    },
    onChartMove: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate moved ");
    }
  }

  aboutToAppear() {
    this.titleModel.title = this.title
    this.model = new LineChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setMaxVisibleValueCount(60);
    this.model.setDragDecelerationEnabled(true);
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setEnabled(false);//X轴是否显示
      this.xAxis.setPosition(XAxisPosition.TOP);
      this.xAxis.setDrawGridLines(true);//纵向线格显示
      this.xAxis.setGranularity(1);
      this.xAxis.setLabelCount(6);//设置X轴坐标个数
      this.xAxis.setAxisMinimum(0);
      this.xAxis.setAxisMaximum(40);//设置X轴最大值
    }

    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setLabelCount(6, false);
      this.leftAxis.setDrawGridLines(false);
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      this.leftAxis.setAxisMinimum(-250);
      this.leftAxis.setAxisMaximum(900);
      this.leftAxis.setDrawAxisLine(true);
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setLabelCount(6, false);
      this.rightAxis.setAxisMinimum(-250); // this replaces setStartAtZero(true)
      this.rightAxis.setAxisMaximum(900);
      this.rightAxis.setDrawAxisLine(true);
      this.rightAxis.setDrawLabels(false);
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(false);
    }

    this.setData(100, 60);

    this.model.setMinOffset(this.minOffset);
    this.model.setVisibleXRangeMaximum(50);
  }

  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values1 = new JArrayList<EntryOhos>();
    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range + 550;
      values1.add(new EntryOhos(i, val,pixelMap));
    }

    let values2 = new JArrayList<EntryOhos>();
    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range + 150;
      values2.add(new EntryOhos(i, val));
    }

    let gradientFillColor1 = new JArrayList<ChartColorStop>();
    gradientFillColor1.add(['#00ffff', 0.1])
    gradientFillColor1.add(['#ffffff', 1.0])

    let gradientFillColor2 = new JArrayList<ChartColorStop>();
    gradientFillColor2.add(['#ffffff', 0.1])
    gradientFillColor2.add(['#ffffff', 1.0])

    let dataSet = new JArrayList<ILineDataSet>();

    let set1 = new LineDataSet(values1, "DataSet 1");
    set1.setDrawFilled(true);
    set1.setDrawValues(true);
    set1.setDrawCircles(false);
    set1.setDrawIcons(true)
    set1.setMode(Mode.LINEAR);
    set1.setGradientFillColor(gradientFillColor1);
    set1.setColorByColor(ChartColor.rgb(255, 241, 46));
    set1.setLineWidth(2)
    dataSet.add(set1);

    let set2 = new LineDataSet(values2, "DataSet 2");
    set2.setDrawFilled(true);
    set2.setDrawValues(false);
    set2.setDrawCircles(false);
    set2.setMode(Mode.LINEAR);
    set2.setGradientFillColor(gradientFillColor2);
    set2.setColorByColor(ChartColor.rgb(255, 241, 46));
    set2.setLineWidth(2)
    dataSet.add(set2);

    let lineData: LineData = new LineData(dataSet);
    lineData.setValueTextSize(10);
    if (this.model) {
      this.model.setData(lineData);
    }
  }

  build() {
    Column(){
      title({ model: this.titleModel })
      Column() {
        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.model })
            .width('100%')
            .height('80%')
            .backgroundColor(Color.White);
        }
      }
    }
  }
}