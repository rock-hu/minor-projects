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
  ChartColor,
  ChartGesture,
  Description,
  EntryOhos,
  Highlight,
  IBarDataSet,
  JArrayList,
  Legend,
  LegendHorizontalAlignment,
  LegendOrientation,
  LegendVerticalAlignment,
  MarkerView,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct BarChartMultiplePage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private data: BarData | null = null;
  private normalMarker: MarkerView | null = null;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS,Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'Multiple'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local seekBarX: SeekBarModel = new SeekBarModel()
  @Local seekBarY: SeekBarModel = new SeekBarModel()
  private isDrawIcons: boolean= false;

  @Monitor("seekBarX.value")
  seekBarXValueWatch(): void {
    const xValue = this.seekBarX.getValue();
    const yValue = this.seekBarY.getValue();
    this.setData(xValue == 0? 5: xValue, yValue == 0? 100: yValue);
  }

  @Monitor("seekBarY.value")
  seekBarYValueWatch(): void {
    this.seekBarXValueWatch();
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
        Utils.saveImage(this.title, this.model? this.model.context2d: null);
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
      LogUtil.log("ScrollBarChart onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("ScrollBarChart onNothingSelected");
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
    this.seekBarX.setValue(5)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(100)
      .setMax(200)
      .setMin(20)

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
      l.setEnabled(true);
      l.setVerticalAlignment(LegendVerticalAlignment.TOP);
      l.setHorizontalAlignment(LegendHorizontalAlignment.RIGHT);
      l.setOrientation(LegendOrientation.VERTICAL);
      l.setDrawInside(true);
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
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);
      // this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      this.leftAxis.setSpaceTop(30);
      this.leftAxis.enableGridDashedLine(10, 10, 0)
      // add limit lines
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setLabelCount(6, false);
      this.rightAxis.setSpaceTop(11);
    }

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setLabelCount(5, false);
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
    }

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);

    if (this.xAxis && this.leftAxis) {

      this.setData(5, 110);

    }

    // this.model.setVisibleXRangeMaximum(20);
  }

  private async setData(count: number, range: number) {

    let groupSpace: number = 0.08;
    let barSpace: number = 0.03; // x4 DataSet
    let barWidth: number = 0.2; // x4 DataSet
    // (0.2 + 0.03) * 4 + 0.08 = 1.00 -> interval per "group"

    let startYear: number = 1980;
    let endYear: number = startYear + count;

    let values1: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    let values2: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    let values3: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    let values4: JArrayList<BarEntry> = new JArrayList<BarEntry>();

    let randomMultiplier: number = range;
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    for (let i = startYear; i < endYear; i++) {
      values1.add(new BarEntry(i, (Math.random() * randomMultiplier),pixelMap))
      values2.add(new BarEntry(i, (Math.random() * randomMultiplier)))
      values3.add(new BarEntry(i, (Math.random() * randomMultiplier),pixelMap))
      values4.add(new BarEntry(i, (Math.random() * randomMultiplier)))
    }

    let set1: BarDataSet, set2: BarDataSet, set3: BarDataSet, set4: BarDataSet;


    if (this.model && this.model.getBarData() != null && this.model.getBarData()!
      .getDataSetCount() > 0) {
      set1 = this.model.getBarData()!.getDataSetByIndex(0) as BarDataSet;
      set2 = this.model.getBarData()!.getDataSetByIndex(1) as BarDataSet;
      set3 = this.model.getBarData()!.getDataSetByIndex(2) as BarDataSet;
      set4 = this.model.getBarData()!.getDataSetByIndex(3) as BarDataSet;
      set1.setValues(values1);
      set1.setDrawIcons(this.isDrawIcons);
      set2.setValues(values2);
      set3.setValues(values3);
      set3.setDrawIcons(this.isDrawIcons);
      set4.setValues(values4);
      this.model.getBarData()!.notifyDataChanged();
      this.model.notifyDataSetChanged();

    } else {
      set1 = new BarDataSet(values1, "Company A");
      set1.setColorsByArr([ChartColor.rgb(104, 241, 175)]);
      set1.setDrawIcons(this.isDrawIcons);
      set2 = new BarDataSet(values2, "Company B");
      set2.setColorsByArr([ChartColor.rgb(164, 228, 251)]);
      set3 = new BarDataSet(values3, "Company C");
      set3.setColorsByArr([ChartColor.rgb(242, 247, 158)]);
      set3.setDrawIcons(this.isDrawIcons);
      set4 = new BarDataSet(values4, "Company D");
      set4.setColorsByArr([ChartColor.rgb(255, 102, 0)]);

      let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
      dataSets.add(set1);
      dataSets.add(set2);
      dataSets.add(set3);
      dataSets.add(set4);

      let data: BarData = new BarData(dataSets);
      this.model?.setData(data);
    }

    if (this.model && this.xAxis) {
      this.model.getBarData()!.setBarWidth(barWidth);
      this.xAxis.setAxisMinimum(startYear);
      this.xAxis.setAxisMaximum(startYear + this.model.getBarData()!.getGroupWidth(groupSpace, barSpace) * count);
      this.model.groupBars(startYear, groupSpace, barSpace);
      this.model.invalidate();
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BarChart({ model: this.model })
          .width('100%')
          .height('70%')
        Divider()
          .width('1px')
          .padding({ top: 5 })

        Column() {
          SeekBar({ model: this.seekBarX })
          SeekBar({ model: this.seekBarY })
        }
      }
    }
  }
}