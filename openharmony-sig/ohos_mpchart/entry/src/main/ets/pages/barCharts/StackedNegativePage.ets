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
  AxisBase,
  ChartColor,
  BarData,
  BarDataSet,
  BarEntry,
  ChartGesture,
  ColorTemplate,
  Description,
  EntryOhos,
  Highlight,
  HorizontalBarChart,
  HorizontalBarChartModel,
  IAxisValueFormatter,
  IBarDataSet,
  JArrayList,
  Legend,
  LimitLine,
  MarkerView,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
  IValueFormatter,
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import CustomMarkerView from '../../customcomponents/CustomMarkerView';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct StackedNegativePage {
  @Local private model: HorizontalBarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private limitLine1: LimitLine | null = null;
  private limitLine2: LimitLine | null = null;
  private data: BarData | null = null;
  private normalMarker: MarkerView | null = null;
  private stackMarker: CustomMarkerView | null = null;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS, Constants.TOGGLE_DRAW_ICON,Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'Stacked'

  private isDrawIcons: boolean= false;
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
    if (!barData)  return
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

      case Constants.TOGGLE_DRAW_ICON:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as BarDataSet;
            set.setDrawIcons(!this.isDrawIcons)
          }
          this.isDrawIcons=!this.isDrawIcons
          this.model.invalidate();
        }
        break;
      default :

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
        this.model.animateXY(2000,2000);
      }
    }
  }

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("StackedBarChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("StackedBarChartPage onNothingSelected");
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

  async aboutToAppear() {
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new HorizontalBarChartModel();
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
      this.leftAxis.setEnabled(false);
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setAxisMaximum(25);
      this.rightAxis.setAxisMinimum(-25);
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setDrawZeroLine(true);
      this.rightAxis.setLabelCount(7, false);
      this.rightAxis.setValueFormatter(new CustomFormatter());
    }

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTH_SIDED);
      this.xAxis.setDrawGridLines(false);
      this.xAxis.setDrawAxisLine(false);
      this.xAxis.setAxisMinimum(0);
      this.xAxis.setAxisMaximum(110);
      this.xAxis.setCenterAxisLabels(true);
      this.xAxis.setLabelCount(12);
      this.xAxis.setGranularity(10);
      this.xAxis.setValueFormatter(new CustomValueFormatter());
    }

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    this.stackMarker = new CustomMarkerView();

    this.data = await this.getStackData();
    this.data.setBarWidth(8.5)
    this.model.setData(this.data);
  }

  private getColors(): number[] {
    // have as many colors as stack-values per entry
    let colors: number[] = [];
    colors.push(ChartColor.rgb(67, 67, 67))
    colors.push(ChartColor.rgb(124, 181, 236))
    return colors;
  }

  private async getStackData(): Promise<BarData> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(5, [-10, 10],pixelMap));
    values.add(new BarEntry(15, [-12, 13],pixelMap));
    values.add(new BarEntry(25, [-15, 15],pixelMap));
    values.add(new BarEntry(35, [-17, 17],pixelMap));
    values.add(new BarEntry(45, [-19, 20],pixelMap));
    values.add(new BarEntry(45, [-19, 20],pixelMap));
    values.add(new BarEntry(55, [-19, 19]));
    values.add(new BarEntry(65, [-16, 16],pixelMap));
    values.add(new BarEntry(75, [-13, 14],pixelMap));
    values.add(new BarEntry(85, [-10, 11],pixelMap));
    values.add(new BarEntry(95, [-5, 6]));
    values.add(new BarEntry(105, [-1, 2],pixelMap));

    let set1: BarDataSet | null = null;


    set1 = new BarDataSet(values, "Statistics Vienna 2014");
    set1.setDrawIcons(false);
    set1.setColorsByVariable(this.getColors());
    set1.setStackLabels(["Births", "Divorces", "Marriages"]);

    set1.setDrawValues(true);
    let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSets.add(set1);

    let data: BarData = new BarData(dataSets);
    data.setValueTextColor(Color.White);
    return data;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        HorizontalBarChart({ model: this.model })
          .width('100%')
          .height('80%')
        Divider()
          .width('1px')
          .padding({ top: 5 })
      }
    }
  }
}

class CustomFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    return Math.abs(value).toFixed(0) + "m";
  }
}

class CustomValueFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    return Math.abs(value).toFixed(0) + "-" + (Number(Math.abs(value).toFixed(0)) + 10);
  }
}