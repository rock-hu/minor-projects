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
  ColorTemplate,
  Description,
  EntryOhos,
  Highlight,
  IBarDataSet,
  JArrayList,
  Legend,
  LimitLabelPosition,
  LimitLine,
  MarkerView,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import CustomMarkerView from '../../customcomponents/CustomMarkerView';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct StackedBarChartPage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private limitLine1: LimitLine | null = null;
  private limitLine2: LimitLine | null = null;
  private data: BarData | null = null;
  private normalMarker: MarkerView | null = null;
  private stackMarker: CustomMarkerView | null = null;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS,Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'Stacked'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  private isDrawIcons: boolean= false;

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
    this.model.setMaxVisibleValueCount(40);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(false);

    this.model.setDrawGridBackground(false);
    this.model.setGridBackgroundColor('#500000ff')
    this.model.setDrawBarShadow(false);

    this.model.setDrawValueAboveBar(false);
    this.model.setHighlightFullBarEnabled(false);

    this.limitLine1 = new LimitLine(120, 'Upper Limit');
    this.limitLine1.setLineWidth(4);
    this.limitLine1.enableDashedLine(10, 10, 0);
    this.limitLine1.setLabelPosition(LimitLabelPosition.RIGHT_TOP);
    this.limitLine1.setTextSize(10);

    this.limitLine2 = new LimitLine(50, 'Lower Limit');
    this.limitLine2.setLineWidth(4);
    this.limitLine2.enableDashedLine(10, 10, 0);
    this.limitLine2.setLineColor(Color.Yellow);
    this.limitLine2.setLabelPosition(LimitLabelPosition.RIGHT_BOTTOM);
    this.limitLine2.setTextSize(10);

    // change the position of the y-labels
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);

      // add limit lines
      this.leftAxis.addLimitLine(this.limitLine1);
      this.leftAxis.addLimitLine(this.limitLine2);
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
      this.rightAxis.setAxisMinimum(0);
    }

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.TOP);
    }

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);
    this.stackMarker = new CustomMarkerView();

    this.data =await this.getStackData();
    this.model.setData(this.data);
    this.model.setVisibleXRangeMaximum(20);
  }

  private getColors(): number[] {
    // have as many colors as stack-values per entry
    let colors: number[] = [];
    colors.push(ColorTemplate.MATERIAL_COLORS[0])
    colors.push(ColorTemplate.MATERIAL_COLORS[1])
    colors.push(ColorTemplate.MATERIAL_COLORS[2])
    return colors;
  }

  private async getStackData(): Promise<BarData> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, [38.0, 28.0, 39.8],pixelMap));
    values.add(new BarEntry(2, [18.2, 16.1, 16.1],pixelMap));
    values.add(new BarEntry(3, [45.8, 49.8, 26.7]));
    values.add(new BarEntry(4, [36.5, 43.8, 15.1]));
    values.add(new BarEntry(5, [51.5, 35.4, 24.1],pixelMap));
    values.add(new BarEntry(6, [44.9, 21.4, 15.9],pixelMap));
    values.add(new BarEntry(7, [32.9, 40.4, 32.7],pixelMap));
    values.add(new BarEntry(8, [24.3, 31.0, 47.1]));
    values.add(new BarEntry(9, [17.4, 48.4, 49.6],pixelMap));
    values.add(new BarEntry(10, [32.0, 29.8, 20.0],pixelMap));
    values.add(new BarEntry(11, [43.9, 52.7, 27.7]));
    values.add(new BarEntry(12, [28.4, 46.2, 51.8],pixelMap));
    values.add(new BarEntry(13, [42.6, 51.9, 44.0],pixelMap));
    values.add(new BarEntry(14, [14.8, 22.4, 31.2],pixelMap));
    values.add(new BarEntry(15, [50.3, 18.7, 18.3]));
    values.add(new BarEntry(16, [20.7, 22.9, 40.2],pixelMap));
    values.add(new BarEntry(17, [49.4, 38.3, 41.7]));
    values.add(new BarEntry(18, [45.9, 40.7, 29.5],pixelMap));
    values.add(new BarEntry(19, [37.1, 31.8, 42.9]));

    let set1: BarDataSet | null = null;


    set1 = new BarDataSet(values, "Statistics Vienna 2014");
    set1.setDrawIcons(this.isDrawIcons);
    set1.setColorsByVariable(this.getColors());
    set1.setStackLabels(["Births", "Divorces", "Marriages"]);

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