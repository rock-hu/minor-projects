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
  ChartGesture,
  ColorTemplate,
  EntryOhos,
  Highlight,
  IPieDataSet,
  JArrayList,
  Legend,
  LegendHorizontalAlignment,
  LegendOrientation,
  LegendVerticalAlignment,
  MPPointF,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  PieChart,
  PieChartModel,
  PieData,
  PieDataSet,
  PieEntry,
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct PieChartPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_Y_VALUES, Constants.TOGGLE_DRAW_ICON,Constants.TOGGLE_X_VALUES,
  Constants.TOGGLE_PERCENT, Constants.TOGGLE_MINIMUM_ANGLES, Constants.TOGGLE_HOLE,Constants.TOGGLE_CURVED_SLICES,
  Constants.DRAW_CENTER_TEXT,Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'PieChart'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local seekBarX: SeekBarModel = new SeekBarModel()
  @Local seekBarY: SeekBarModel = new SeekBarModel()
  private isDrawIcons: boolean= false;

  @Monitor("seekBarX.value")
  seekBarXValueWatch(): void {
    this.setData(this.seekBarX.getValue(), this.seekBarY.getValue());
  }

  @Monitor("seekBarY.value")
  seekBarYValueWatch(): void {
    this.setData(this.seekBarX.getValue(), this.seekBarY.getValue());
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
    let pieData = this.model.getData();
    if (!pieData)  return
    let sets: JArrayList<IPieDataSet> | null = null;
    if (pieData) {
      sets = pieData.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case Constants.TOGGLE_Y_VALUES:
        if (!pieData || !sets) {
          break;
        }
        for (let i = 0; i < sets.size(); i++) {
          let set = sets.get(i) as PieDataSet;
          set.setDrawValues(!set.isDrawValuesEnabled());
        }
        this.model.invalidate();
        break;
      case Constants.TOGGLE_X_VALUES:
        this.model.setDrawEntryLabels(!this.model.isDrawEntryLabelsEnabled());
        this.model.invalidate();
        break;
      case Constants.TOGGLE_PERCENT:
        this.model.setUsePercentValues(!this.model.isUsePercentValuesEnabled());
        this.model.invalidate();
        break;
      case Constants.TOGGLE_MINIMUM_ANGLES:
        if (this.model.getMinAngleForSlices() == 0){
          this.model.setMinAngleForSlices(36);
        } else{
          this.model.setMinAngleForSlices(0);
        }
        this.model.notifyDataSetChanged();
        this.model.invalidate();
        break;
      case Constants.TOGGLE_HOLE:
        if (this.model.isDrawHoleEnabled()) {
          this.model.setDrawHoleEnabled(false);
        } else {
          this.model.setDrawHoleEnabled(true);
        }
        this.model.invalidate();
        break;
      case Constants.TOGGLE_CURVED_SLICES:
        let toSet: boolean = !this.model.isDrawRoundedSlicesEnabled() || !this.model.isDrawHoleEnabled();
        this.model.setDrawRoundedSlices(toSet);
        if (toSet && !this.model.isDrawHoleEnabled()) {
          this.model.setDrawHoleEnabled(true);
        }
        if (this.model && this.model.isDrawSlicesUnderHoleEnabled()) {
          this.model.setDrawSlicesUnderHole(false);
        }
        this.model.invalidate();
        break;
      case Constants.DRAW_CENTER_TEXT:
        if (this.model.isDrawCenterTextEnabled()){
          this.model.setDrawCenterText(false);
        } else{
          this.model.setDrawCenterText(true);
        }
        this.model.invalidate();
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
        break;
      case Constants.TOGGLE_DRAW_ICON:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as PieDataSet;
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

 @Local private model: PieChartModel = new PieChartModel();
  protected parties: string[] = [
    "Party A", "Party B", "Party C", "Party D", "Party E", "Party F", "Party G", "Party H",
    "Party I", "Party J", "Party K", "Party L", "Party M", "Party N", "Party O", "Party P",
    "Party Q", "Party R", "Party S", "Party T", "Party U", "Party V", "Party W", "Party X",
    "Party Y", "Party Z"
  ];
  pieData: PieData | null = null;
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("PieChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("PieChartPage onNothingSelected");
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
      .setMax(25)
      .setMin(0)

    this.seekBarY.setValue(10)
      .setMax(200)
      .setMin(0)

    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
      l.setOrientation(LegendOrientation.VERTICAL);
      l.setVerticalAlignment(LegendVerticalAlignment.TOP);
      l.setHorizontalAlignment(LegendHorizontalAlignment.RIGHT);
    }
    this.model.animateX(1000);
    this.model.setUsePercentValues(true);
    this.model.getDescription()?.setEnabled(false);
    this.model.setExtraOffsets(5, 10, 5, 5);

    this.model.setOnChartGestureListener(this.chartGestureListener);

    this.model.setDragDecelerationFrictionCoef(0.95);

    this.model.setCenterText("mpchart");
    this.model.setCenterTextSize(22)

    this.model.setDrawHoleEnabled(true);
    this.model.setHoleColor(Color.White);

    this.model.setTransparentCircleColor(Color.White);
    this.model.setTransparentCircleAlpha(110);
    this.model.setDragDecelerationEnabled(true);

    this.model.setHoleRadius(58);
    this.model.setTransparentCircleRadius(61);

    this.model.setDrawCenterText(true);

    this.model.setRotationAngle(0);
    // enable rotation of the chart by touch
    this.model.setRotationEnabled(true);
    this.model.setHighlightPerTapEnabled(true);

    this.setData(4, 10);

  }

  // 初始化饼状图数据
  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let entries: JArrayList<PieEntry> = new JArrayList<PieEntry>();

    // NOTE: The order of the entries when being added to the entries array determines their position around the center of
    // the chart.
    for (let i = 0; i < count; i++) {
      entries.add(new PieEntry(((Math.random() * range) + range / 5),
        this.parties[i % this.parties.length], pixelMap, undefined));
    }

    let dataSet: PieDataSet = new PieDataSet(entries, "Election Results");
    dataSet.setDrawIcons(this.isDrawIcons);
    dataSet.setSliceSpace(1);
    dataSet.setIconsOffset(new MPPointF(0, 40));
    dataSet.setSelectionShift(5);
    dataSet.setValueTextColor(Color.White);
    dataSet.setValueTextColor(25);

    // add a lot of colors
    let colors: JArrayList<number> = new JArrayList();
    for (let index = 0; index < ColorTemplate.COLORFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.COLORFUL_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.VORDIPLOM_COLORS.length; index++) {
      colors.add(ColorTemplate.VORDIPLOM_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.JOYFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.JOYFUL_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.LIBERTY_COLORS.length; index++) {
      colors.add(ColorTemplate.LIBERTY_COLORS[index]);
    }
    for (let index = 0; index < ColorTemplate.PASTEL_COLORS.length; index++) {
      colors.add(ColorTemplate.PASTEL_COLORS[index]);
    }
    colors.add(ColorTemplate.getHoloBlue());
    dataSet.setColorsByList(colors);
    let data: PieData = new PieData(dataSet);
    this.model.setData(data);
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      PieChart({ model: this.model })
        .width('100%')
        .height('70%')
        .backgroundColor(Color.White)
      Column() {
        SeekBar({ model: this.seekBarX })
        SeekBar({ model: this.seekBarY })
      }
    }
  }
}