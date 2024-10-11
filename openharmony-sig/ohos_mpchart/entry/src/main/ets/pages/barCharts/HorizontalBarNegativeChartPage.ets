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
  BarEntry,
  BarDataSet,
  BarData,
  Fill,
  IBarDataSet,
  HorizontalBarChart,
  HorizontalBarChartModel,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils'
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct HorizontalBarNegativeChartPage {
  @Local private model: HorizontalBarChartModel | undefined = undefined;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS,Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  private isDrawIcons: boolean= false;
  //标题栏标题
  private title: string = 'Horizontal'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local seekBarX: SeekBarModel = new SeekBarModel()
  @Local seekBarY: SeekBarModel = new SeekBarModel()

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
      LogUtil.log("HorizontalBarChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {

    }
  }

  aboutToAppear() {
    this.seekBarX.setValue(12)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(50)
      .setMax(200)
      .setMin(20)
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new HorizontalBarChartModel();
    this.model.animateX(2000)
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    this.model.setDrawBarShadow(false);
    this.model.setDrawValueAboveBar(true);
    this.model.setDragDecelerationEnabled(true);

    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    // if more than 60 entries are displayed in the chart, no values will be
    // drawn
    // this.model.setMaxVisibleValueCount(60);
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);

    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(false);
      xAxis.setGranularity(1);
      xAxis.setLabelCount(7);
      xAxis.setDrawAxisLine(true)
    }

    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(8, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      leftAxis.setSpaceTop(15);
      leftAxis.setDrawAxisLine(true)
    }


    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      rightAxis.setSpaceTop(15);
      rightAxis.setDrawAxisLine(true)
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(false);
    }
    this.setData(12, 50);

    this.model.setFitBars(true);
  }

  private async setData(count: number, range: number): Promise<BarData> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let spaceForBar: number = 10;

    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    for (let i = 0; i < count; i++) {
      let val = Math.random() * range - range / 2;
      values.add(new BarEntry(i * spaceForBar, val,pixelMap))
    }

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(this.isDrawIcons);

    dataSet.setColorByColor(0x8CEAFF);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setValueTextSize(10);
    barData.setBarWidth(9);
    if (this.model) {
      this.model.setData(barData);
    }
    return barData;

  }

    build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        HorizontalBarChart({ model: this.model })
          .width('100%')
          .height('70%')
        Column() {
          SeekBar({ model: this.seekBarX })
          SeekBar({ model: this.seekBarY })
        }
      }
    }
  }
}