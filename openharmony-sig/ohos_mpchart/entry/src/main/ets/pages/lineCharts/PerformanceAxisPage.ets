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
} from '@ohos/mpchart';

import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct PerformanceAxisPage {

  minOffset: number = 15;
  @Local private model: LineChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  //标题栏标题
  private title: string = 'PerformanceLineChart'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local seekBarX: SeekBarModel = new SeekBarModel()

  @Monitor("seekBarX.value")
  seekBarXValueWatch(): void {
    this.setData(this.seekBarX.getValue(), 500);
  }

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
    this.seekBarX.setValue(1000)
      .setMax(3000)
      .setMin(100)

    this.titleModel.title = this.title
    this.model = new LineChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);
    this.model.setDragDecelerationEnabled(true);

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setEnabled(true);//X轴是否显示
      this.xAxis.setPosition(XAxisPosition.TOP);
      this.xAxis.setDrawGridLines(true);//纵向线格显示
      this.xAxis.setGranularity(1);
      this.xAxis.setLabelCount(6);//设置X轴坐标个数
    }

    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setLabelCount(5, false);
      this.leftAxis.setDrawGridLines(false);
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      this.leftAxis.setAxisMinimum(-100);
      this.leftAxis.setAxisMaximum(600);
      this.leftAxis.setDrawAxisLine(true);

    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setLabelCount(5, false);
      this.rightAxis.setAxisMinimum(-100); // this replaces setStartAtZero(true)
      this.rightAxis.setAxisMaximum(600);
      this.rightAxis.setDrawAxisLine(false);
      this.rightAxis.setDrawLabels(false);
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(false);
    }

    this.setData(1000, 500);
    this.model.setMinOffset(this.minOffset);
  }

  /**
   * 初始化数据
   * @param count  曲线图点的个数
   * @param range  y轴范围
   */
  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values = new JArrayList<EntryOhos>();

    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range;
      values.add(new EntryOhos(i * 0.01, val,pixelMap));
    }

    let dataSet = new JArrayList<ILineDataSet>();

    let set1 = new LineDataSet(values, "DataSet 1");
    set1.setDrawFilled(false);
    set1.setDrawValues(true);
    set1.setDrawIcons(true)
    set1.setDrawCircles(false);
    set1.setMode(Mode.LINEAR);
    set1.setColorByColor(Color.Black);
    set1.setLineWidth(0.5)
    dataSet.add(set1);

    let lineData: LineData = new LineData(dataSet);
    lineData.setValueTextSize(10);

    if (this.model) {
      this.model.setData(lineData);
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.model , antiAliasing: false })
            .width('100%')
            .height('70%')
            .backgroundColor(Color.White);
        }

        Column() {
          SeekBar({ model: this.seekBarX })
        }
      }
    }
  }
}