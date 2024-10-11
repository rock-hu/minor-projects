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
  BarChart,
  BarChartModel,
  BarData,
  BarDataSet,
  BarEntry,
  ChartGesture,
  Description,
  EntryOhos,
  Highlight,
  IBarDataSet,
  JArrayList,
  Legend,
  MarkerView,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
  YAxisLabelPosition,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct BarChartNegativePage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private data: BarData | null = null;
  private normalMarker: MarkerView | null = null;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [];
  //标题栏标题
  private title: string = 'Negative'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local seekBarX: SeekBarModel = new SeekBarModel()
  @Local seekBarY: SeekBarModel = new SeekBarModel()
  @Local seekBarRadius: SeekBarModel = new SeekBarModel(0)

  @Monitor("seekBarX.value")
  seekBarXValueWatch(): void {
    this.setData(this.seekBarX.getValue(), this.seekBarY.getValue());
  }

  @Monitor("seekBarY.value")
  seekBarYValueWatch(): void {
    this.setData(this.seekBarX.getValue(), this.seekBarY.getValue());
  }

  @Monitor("seekBarRadius.value")
  seekBarTopRadius(): void {
    let barData = this.model?.getBarData()
    if (barData) {
      barData.setTopRadius(this.seekBarRadius.getValue())
      this.model?.setData(barData)
    }
  }

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
    let barData = this.model.getBarData();
    if (!barData) {
      return;
    }

    this.titleModel.setIndex(-1)
  }

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("BarChartNegativePage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("BarChartNegativePage onNothingSelected");
    }
  }
  private chartGestureListener: OnChartGestureListener = {
    onChartGestureStart: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
      LogUtil.log("-----------------chartGestureListener onChartGestureStart lastMode: " + lastPerformedGestureMode);
    },
    onChartGestureEnd: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
      LogUtil.log("-----------------chartGestureListener onChartGestureEnd lastMode: " + lastPerformedGestureMode);
    },
    onChartLongPressed: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartLongPressed");
    },
    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartDoubleTapped");
    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartSingleTapped");
    },
    onChartFling: (isTouchEvent: boolean, me1: TouchEvent | GestureEvent, me2: TouchEvent, velocityX: number, velocityY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartFling velocityX: " + velocityX + "  velocityY: " + velocityY);
    },
    onChartScale: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, scaleX: number, scaleY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartScale scaleX: " + scaleX + "  scaleY: " + scaleY);
    },
    onChartTranslate: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, dX: number, dY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate dx: " + dX + "  dy: " + dY);
    },
    onChartMove: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate moved ");
    }
  }

  aboutToAppear() {
    this.seekBarX.setValue(4)
      .setMax(50)
      .setMin(2)

    this.seekBarY.setValue(100)
      .setMax(200)
      .setMin(20)

    this.seekBarRadius.setValue(8)
      .setMax(15)
      .setMin(0)

    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BarChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    this.model.setOnChartGestureListener(this.chartGestureListener);
    this.model.setDragDecelerationEnabled(true);

    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(false);
    }

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(false);
    }

    // if more than 40 entries are displayed in the this.model, no values will be drawn
    this.model.setMaxVisibleValueCount(60);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(false);

    this.model.setDrawGridBackground(false);
    this.model.setGridBackgroundColor('#500000ff')
    this.model.setDrawBarShadow(false);

    this.model.setDrawValueAboveBar(true);
    this.model.setHighlightFullBarEnabled(false);


    // change the position of the y-labels
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      this.leftAxis.setDrawLabels(false);
      this.leftAxis.setSpaceTop(25);
      this.leftAxis.setSpaceBottom(25);
      this.leftAxis.setDrawAxisLine(false);
      this.leftAxis.setDrawGridLines(false);
      this.leftAxis.setDrawZeroLine(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);
      this.leftAxis.setZeroLineWidth(0.7);

      // add limit lines
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
    }

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
      this.xAxis.setDrawLabels(false);
      this.xAxis.setDrawGridLines(false);
      this.xAxis.setDrawAxisLine(false);
      this.xAxis.setCenterAxisLabels(true);
      this.xAxis.setLabelCount(5);
      this.xAxis.setGranularity(1);
    }

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);


    this.setData(4, 100);
    // this.model.setVisibleXRangeMaximum(20);
  }

  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let startAxis: number = 0;
    let endAxis: number = startAxis + count;
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    let valueColors: JArrayList<number> = new JArrayList<number>();
    for (let i = startAxis; i < endAxis; i++) {
      let val = Math.random() * range - range / 2;

      values.add(new BarEntry(i, val,pixelMap))
    }

    for (let i = 0; i < values.length(); i++) {
      if (values.at(i).getY() > 0) {
        valueColors.add(0xD34A58);
      } else {
        valueColors.add(0x6EBE66);
      }
    }


    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setValueTextColors(valueColors);
    dataSet.setColorsByVariable(valueColors.toArray());
    dataSet.setDrawValues(true);
    dataSet.setDrawIcons(true);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.8);
    barData.setTopRadius(this.seekBarRadius.getValue())

    if (this.model) {
      this.model.setData(barData);
      // this.model.setDrawBarShadow(true)
      this.model.setBorderWidth(5)
      this.model.setBorderColor(0xfff4ee)
      this.model.setDrawBorders(true)
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BarChart({ model: this.model })
          .width('100%')
          .height('65%')
        Divider()
          .width('1px')
          .padding({ top: 5 })
        Column() {
          SeekBar({ model: this.seekBarX })
          SeekBar({ model: this.seekBarY })
          SeekBar({ model: this.seekBarRadius })
          Row() {
            Checkbox()
              .onChange((value) => {
                this.model?.getAxisLeft()?.setInverted(value)
                this.setData(this.seekBarX.getValue(), this.seekBarY.getValue())
              })
            Text('Invert Left Axis')
              .fontSize(16)
              .layoutWeight(1)

            Checkbox()
              .onChange((value) => {
                this.model?.setDrawBarShadow(value)
                this.setData(this.seekBarX.getValue(), this.seekBarY.getValue())
              })
            Text('Enable Bar Shadow')
              .fontSize(16)
              .layoutWeight(1)
          }
        }
      }
    }
  }
}