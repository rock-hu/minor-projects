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
  IBarDataSet,
  IPieDataSet,
  JArrayList,
  Legend,
  LegendOrientation,
  MPPointF,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  PieChart,
  PieChartModel,
  PieData,
  PieDataSet,
  PieEntry
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct HalfPieChartPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON,'Toggle Bar Borders', 'Animate X', 'Animate Y', 'Animate XY'];
  //标题栏标题
  private title: string = 'HalfPieChartPage'
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

    let data = this.model.getData();
    let sets: JArrayList<IPieDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case 'Animate X':
        this.model.animateX(2000)
        break;
      case 'Animate Y':
        this.model.animateY(2000)
        break;
      case 'Animate XY':
        this.model.animateXY(2000, 2000)
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

    }
    this.titleModel.setIndex(-1)
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
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    let l: Legend | null = this.model.getLegend();
    if(l){
      l.setEnabled(true);
      l.setOrientation(LegendOrientation.VERTICAL);
    }
    this.model.animateX(1000);
    this.model.setUsePercentValues(true);
    this.model.getDescription()?.setEnabled(false);
    this.model.setExtraOffsets(5, 10, 5, 5);

    this.model.setOnChartGestureListener(this.chartGestureListener);

    this.model.setDragDecelerationFrictionCoef(0.95);

    this.model.setCenterText("mpchart");
    this.model.setCenterTextSize(22);

    this.model.setDrawHoleEnabled(true);
    this.model.setHoleColor(Color.White);

    this.model.setTransparentCircleColor(Color.White);
    this.model.setTransparentCircleAlpha(110);

    this.model.setHoleRadius(58);
    this.model.setTransparentCircleRadius(61);

    this.model.setDrawCenterText(true);

    // enable rotation of the chart by touch
    this.model.setRotationEnabled(false);
    this.model.setHighlightPerTapEnabled(true);

    this.model.setMaxAngle(180);
    this.model.setRotationAngle(180);


    this.setData(4, 10);

  }

  // 初始化饼状图数据
  private async setData(count: number, range: number): Promise<void> {
    let entries: JArrayList<PieEntry> = new JArrayList<PieEntry>();
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
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
    }
  }
}