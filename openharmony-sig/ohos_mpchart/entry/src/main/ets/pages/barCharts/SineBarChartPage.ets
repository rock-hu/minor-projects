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
  BarChart,
  BarChartModel,
  BarData,
  BarDataSet,
  BarEntry,
  ChartGesture,
  ColorTemplate,
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
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title';
import OtherSine from './OtherSine';
import Utils from '../../utils/Utils';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct SineBarChartPage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private data: BarData | null = null;
  private normalMarker: MarkerView | null = null;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS, Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'Sine'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()

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
    if (!barData) return
    let sets: JArrayList<IBarDataSet> | null = null;
    if (barData) {
      sets = barData.getDataSets();
    }

    switch (this.menuItemArr[index]) {
      case Constants.TOGGLE_BAR_BORDERS:
        for (let i = 0;i < barData.getDataSets().length(); i++) {
          let barDataSet = barData.getDataSets().get(i) as BarDataSet;
          barDataSet.setBarBorderWidth(barDataSet.getBarBorderWidth() == 1 ? 0 : 1)
        }
        this.model.invalidate()
        break;
      case Constants.TOGGLE_VALUES:
        if (!barData || !sets) {
          break;
        }
        for (let i = 0; i < sets.size(); i++) {
          let set = sets.get(i) as BarDataSet;
          set.setDrawValues(!set.isDrawValuesEnabled());
        }
        this.model.invalidate();
        break;
      case Constants.TOGGLE_HIGHLIGHT:
        if (barData != null) {
          barData.setHighlightEnabled(!barData.isHighlightEnabled());
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_PINCHZOOM:
        if (this.model.isPinchZoomEnabled()) {
          this.model.setPinchZoom(false);
        } else {
          this.model.setPinchZoom(true);
        }
        this.model.invalidate();
        break;
      case Constants.TOGGLE_AUTO_SCALE:
        this.model.setAutoScaleMinMaxEnabled(!this.model.isAutoScaleMinMaxEnabled());
        this.model.notifyDataSetChanged();
        break;
      case Constants.ANIMATE_X:
        this.titleSelectString = 'X'
        this.animate()
        break;
      case Constants.ANIMATE_Y:
        this.titleSelectString = 'Y'
        this.animate()
        break;
      case Constants.ANIMATE_XY:
        this.titleSelectString = 'XY'
        this.animate()
        break;
      case Constants.SAVE_IMAGE:
        Utils.saveImage(this.title, this.model ? this.model.context2d : null);
        break;
      default:

    }
    this.titleModel.setIndex(-1)
  }

  public animate() {
    if (this.model) {
      if (this.titleSelectString == 'X') {
        this.model.animateX(2000);
      } else if (this.titleSelectString == 'Y') {
        this.model.animateY(2000);
      } else if (this.titleSelectString == 'XY') {
        this.model.animateXY(2000, 2000);
      }
    }
  }

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("BarChartPage2 onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("BarChartPage2 onNothingSelected");
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
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BarChartModel();
    this.model.animateY(2000);
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

    // if more than 60 entries are displayed in the this.model, no values will be drawn
    this.model.setMaxVisibleValueCount(60);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);
    this.model.setDrawBarShadow(false);
    this.model.setDrawValueAboveBar(true);
    this.model.setHighlightFullBarEnabled(false);

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setEnabled(false);
    }

    // change the position of the y-labels
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(-2.5);
      this.leftAxis.setAxisMaximum(2.5);
      this.leftAxis.setLabelCount(6, false);
      this.leftAxis.setGranularityEnabled(true);
      this.leftAxis.setGranularity(0.1);

    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setLabelCount(6, false);
      this.rightAxis.setAxisMinimum(-2.5);
      this.rightAxis.setAxisMaximum(2.5);
      this.rightAxis.setGranularity(0.1);
    }

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    this.setData(50);
  }

  private setData(count: number) {
    let dataResult: number[] = new OtherSine().data.slice(0, count);

    let entries: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    for (let i = 0; i < count; i++) {
      entries.add(new BarEntry(i, dataResult[i]));
    }

    let set: BarDataSet | null = null;

    if (this.model) {
      let barData = this.model.getBarData();
      if (barData != null &&
        barData.getDataSetCount() > 0) {
        set = barData.getDataSetByIndex(0) as BarDataSet;
        set.setValues(entries);
        barData.notifyDataChanged();
        this.model.notifyDataSetChanged();
      } else {
        set = new BarDataSet(entries, "Sinus Function");
        set.setColorByColor(ChartColor.rgb(240, 120, 124));
        set.setDrawIcons(false);
        set.setDrawValues(false);

        let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
        dataSetList.add(set);
        let data: BarData = new BarData(dataSetList);
        data.setBarWidth(0.8);
        this.model.setData(data);
      }
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BarChart({ model: this.model })
          .width('100%')
          .height('80%')
        Divider()
          .width('1px')
          .padding({ top: 5 })
      }
    }
  }
}