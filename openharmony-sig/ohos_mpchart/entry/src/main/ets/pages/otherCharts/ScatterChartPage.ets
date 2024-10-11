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
  ScatterChart,
  ScatterChartModel,
  ScatterData,
  ScatterDataSet,
  IScatterDataSet,
  ColorTemplate,
  ChartShape,
} from '@ohos/mpchart';
import CustomScatterShapeRenderer from '../../otherChart/CustomScatterShapeRenderer';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import { LogUtil } from '../../utils/LogUtil';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';


@Entry
@ComponentV2
struct Index {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON,'Animate X', 'Animate Y', 'Animate XY'];
  //标题栏标题
  private title: string = 'ScatterChartPage'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: ScatterChartModel | undefined = undefined;
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

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("ScatterChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("ScatterChartPage onNothingSelected");
    }
  }
  private isDrawValuesEnable: boolean = true;
  titleSelcetString: string = 'X'

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
    let sets: JArrayList<IScatterDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case 'Animate X':
        this.titleSelcetString = 'X'
        this.animate()
        break;
      case 'Animate Y':
        this.titleSelcetString = 'Y'
        this.animate()
        break;
      case 'Animate XY':
        this.titleSelcetString = 'XY'
        this.animate()
        break;
      case Constants.TOGGLE_DRAW_ICON:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as ScatterDataSet;
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
      if (this.titleSelcetString == 'X') {
        this.model.animateX(2000);
      } else if (this.titleSelcetString == 'Y') {
        this.model.animateY(2000);
      } else if (this.titleSelcetString == 'XY') {
        this.model.animateXY(2000, 2000);
      }
    }
  }

  aboutToAppear() {
    this.seekBarX.setValue(20)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(100)
      .setMax(200)
      .setMin(20)
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    LogUtil.log('aboutToAppear-----' + 'into')
    this.model = new ScatterChartModel();

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);


    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setMaxVisibleValueCount(160);
    this.model.setPinchZoom(false);
    this.model.setDragDecelerationEnabled(true);
    this.model.setDrawGridBackground(false);

    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(false);
      xAxis.setGranularity(1);
      xAxis.setLabelCount(7);
    }

    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(8, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(0);
    }

    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      rightAxis.setSpaceTop(15);
      rightAxis.setAxisMinimum(0);
    }

    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 41);

      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val));
      } else {
        values.add(new EntryOhos(i, val));
      }
    }

    let dataSet: ScatterDataSet = new ScatterDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(false);

    let dataSetList: JArrayList<IScatterDataSet> = new JArrayList<IScatterDataSet>();
    dataSetList.add(dataSet);

    this.setData(20, 100);

    this.model.notifyDataSetChanged();
  }

  /**
   * 初始化数据
   * @param xRange  x轴范围
   * @param yRange  y轴范围
   */
  private async setData(xRange: number, yRange: number): Promise<void>{
    let values1 =await this.generateRandomData(xRange, yRange);
    let values2 =await this.generateRandomData(xRange, yRange);
    let values3 =await this.generateRandomData(xRange, yRange);

    let set1 = new ScatterDataSet(values1, "DS 1");
    set1.setScatterShape(ChartShape.SQUARE);
    set1.setDrawIcons(this.isDrawIcons)
    set1.setColorByColor(ColorTemplate.COLORFUL_COLORS[0]);

    let set2 = new ScatterDataSet(values2, "DS 2");
    set2.setScatterShape(ChartShape.CIRCLE);
    set2.setDrawIcons(this.isDrawIcons)
    set2.setScatterShapeHoleColor(ColorTemplate.COLORFUL_COLORS[3]);
    set2.setScatterShapeHoleRadius(3);
    set2.setColorByColor(ColorTemplate.COLORFUL_COLORS[1]);

    let set3 = new ScatterDataSet(values3, "DS 3");
    set3.setShapeRenderer(new CustomScatterShapeRenderer());
    set3.setDrawIcons(this.isDrawIcons)
    set3.setColorByColor(ColorTemplate.COLORFUL_COLORS[2]);

    set1.setScatterShapeSize(8);
    set2.setScatterShapeSize(8);
    set3.setScatterShapeSize(8);

    let dataSets: JArrayList<IScatterDataSet> = new JArrayList<IScatterDataSet>();
    dataSets.add(set1); // add the data sets
    dataSets.add(set2);
    dataSets.add(set3);

    let dataResult: ScatterData = new ScatterData(dataSets);
    dataResult.setDrawValues(this.isDrawValuesEnable);
    dataResult.setValueTextSize(8);
    dataResult.setHighlightEnabled(true);

    dataResult.setValueTextSize(10);
    if (this.model) {
      this.model.setData(dataResult);
    }
    dataResult.notifyDataChanged();
  }

  private async generateRandomData(xRange: number, yRange: number): Promise<JArrayList<EntryOhos>> {
    let values = new JArrayList<EntryOhos>();
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    for (let i = 0; i < xRange; i++) {
      let x = i; // Random x value within specified count.
      let y = Math.random() * yRange; // Random y value within specified range.
      values.add(new EntryOhos(x, y,pixelMap));
    }
    return values;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        ScatterChart({ model: this.model })
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