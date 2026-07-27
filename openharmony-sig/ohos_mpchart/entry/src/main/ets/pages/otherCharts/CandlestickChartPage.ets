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
  AxisDependency,
  CandleData,
  CandleDataSet,
  CandleEntry,
  CandleStickChart,
  CandleStickChartModel,
  Description,
  ICandleDataSet,
  JArrayList,
  Legend,
  Style,
  XAxis,
  XAxisPosition,
  YAxis
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct CandlestickChartPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON,'Animate X', 'Animate Y', 'Animate XY'];
  private title: string = 'CandleStickChartPage'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: CandleStickChartModel | undefined = undefined;
  titleSelectString: string = 'X'
  private isDrawIcons: boolean= false;
  @Local seekBarX: SeekBarModel = new SeekBarModel()
  @Local seekBarY: SeekBarModel = new SeekBarModel()

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
    if (index == undefined || index == -1) {
      return
    }

    if (this.model == null || this.model == undefined) {
      return
    }
    let data = this.model.getData();
    let sets: JArrayList<ICandleDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }

    switch (this.menuItemArr[index]) {
      case 'Animate X':
        this.titleSelectString = 'X'
        this.animate()
        break;
      case 'Animate Y':
        this.titleSelectString = 'Y'
        this.animate()
        break;
      case 'Animate XY':
        this.titleSelectString = 'XY'
        this.animate()
        break;
      case Constants.TOGGLE_DRAW_ICON:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as CandleDataSet;
            set.setDrawIcons(!this.isDrawIcons)
          }
          this.isDrawIcons=!this.isDrawIcons
          this.model.invalidate();
        }
        break;

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

  aboutToAppear() {
    this.seekBarX.setValue(40)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(100)
      .setMax(200)
      .setMin(20)
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new CandleStickChartModel();
    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    this.model.setMaxVisibleValueCount(60);
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(true);
    this.model.setDragDecelerationEnabled(true);
    this.model.setGridBackgroundColor(Color.White)
    this.setData(40,100);

    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(true);
    }

    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(7, false);
      leftAxis.setDrawGridLines(true);
      leftAxis.setDrawAxisLine(true);
    }

    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis.setLabelCount(7, false);
      rightAxis.setDrawGridLines(true);
      rightAxis.setDrawAxisLine(true);
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(true);
    }

  }

  private async setData(count: number, range: number): Promise<void>{

    let values: JArrayList<CandleEntry> = new JArrayList<CandleEntry>();
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    for (let i = 0; i < count; i++) {
      let val: number = (Math.random() * 40) + (range + 3);

      let high: number = Number(Math.random() * 9) + 8.0;
      let low: number = Number(Math.random() * 9) + 8.0;

      let open: number = Number(Math.random() * 6) + 1.0;
      let close: number = Number(Math.random() * 6) + 1.0;

      let even: boolean = i % 2 == 0;

      values.add(new CandleEntry(
        i,
        val + high,
        val - low,
        even ? val + open : val - open,
        even ? val - close : val + close,
        pixelMap));
    }

    let dataSet: CandleDataSet = new CandleDataSet(values, "Data Set");
    dataSet.setDrawIcons(this.isDrawIcons);
    dataSet.setAxisDependency(AxisDependency.LEFT);
    dataSet.setShadowColor(Color.Gray);
    dataSet.setShadowWidth(0.7);
    dataSet.setDecreasingPaintStyle(Style.FILL);
    dataSet.setDecreasingColor(Color.Red);
    dataSet.setIncreasingColor(Color.Green);
    dataSet.setIncreasingPaintStyle(Style.STROKE);
    dataSet.setNeutralColor(Color.Blue);

    let data: CandleData = new CandleData([dataSet]);
    data.setValueTextSize(7);
    if (this.model) {
      this.model.resetTracking();
      this.model.setData(data);
      this.model.invalidate();
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        CandleStickChart({ model: this.model })
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
}