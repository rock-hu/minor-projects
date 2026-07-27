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
  BubbleEntry,
  IBubbleDataSet,
  BubbleDataSet,
  MPPointF,
  BubbleChart,
  BubbleChartModel,
  BubbleData
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils';
import Constants from '../../constants/Constants';


@Entry
@ComponentV2
struct BubbleChartPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON,'Animate X', 'Animate Y', 'Animate XY'];
  //标题栏标题
  private title: string = 'BubbleChartPage'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: BubbleChartModel | undefined = undefined;
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

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("BubbleChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {

    }
  }
  isDrawValuesEnable: boolean = true;
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
    let sets: JArrayList<IBubbleDataSet> | null = null;
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
            let set = sets.get(i) as BubbleDataSet;
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
    this.seekBarX.setValue(5)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(100)
      .setMax(200)
      .setMin(20)
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BubbleChartModel();

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);


    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setMaxVisibleValueCount(60);
    this.model.setPinchZoom(false);
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
    let values: JArrayList<BubbleEntry> = new JArrayList<BubbleEntry>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 41);

      if (Math.random() * 100 < 25) {
        values.add(new BubbleEntry(i, val));
      } else {
        values.add(new BubbleEntry(i, val));
      }
    }


    let dataSet: BubbleDataSet = new BubbleDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(false);


    let dataSetList: JArrayList<IBubbleDataSet> = new JArrayList<IBubbleDataSet>();
    dataSetList.add(dataSet);

    // let lineData: LineData = new LineData(dataSetList);
    // lineData.setValueTextSize(10);
    this.setData(5, 50);
    this.model.notifyDataSetChanged();
    this.model.setVisibleXRangeMaximum(7);
    this.model.setDragDecelerationEnabled(true);
  }

  /**
   * 初始化数据
   * @param count  曲线图点的个数
   * @param range  y轴范围
   */
  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values1 = new JArrayList<BubbleEntry>();
    let values2 = new JArrayList<BubbleEntry>();
    let values3 = new JArrayList<BubbleEntry>();

    //
    for (let i = 0; i < count; i++) {
      values1.add(new BubbleEntry(i, Math.random() * range, Math.random() * range, pixelMap));
      values2.add(new BubbleEntry(i, Math.random() * range, Math.random() * range, pixelMap));
      values3.add(new BubbleEntry(i, Math.random() * range, Math.random() * range, pixelMap));
    }

    let set1: BubbleDataSet = new BubbleDataSet(values1, "DS 1");
    set1.setDrawIcons(this.isDrawIcons);
    set1.setColorByColor(0x88c12552);
    set1.setIconsOffset(new MPPointF(0, 0));
    set1.setDrawValues(this.isDrawValuesEnable);


    let set2: BubbleDataSet = new BubbleDataSet(values2, "DS 2");
    set2.setDrawIcons(this.isDrawIcons);
    set2.setIconsOffset(new MPPointF(0, 0));
    set2.setColorByColor(0x88ff6600);
    set2.setDrawValues(this.isDrawValuesEnable);

    let set3: BubbleDataSet = new BubbleDataSet(values3, "DS 3");
    set3.setDrawIcons(this.isDrawIcons);
    set3.setIconsOffset(new MPPointF(0, 0));
    set3.setColorByColor(0x88f5c700);
    set3.setDrawValues(this.isDrawValuesEnable);


    let dataSets = new JArrayList<IBubbleDataSet>();
    dataSets.add(set1);
    dataSets.add(set2);
    dataSets.add(set3);
    let dataResult: BubbleData = new BubbleData(dataSets);
    dataResult.setDrawValues(this.isDrawValuesEnable);
    dataResult.setValueTextSize(8);
    dataResult.setValueTextColor(Color.White);
    dataResult.setHighlightCircleWidth(1.5);
    dataResult.setHighlightEnabled(true);
    dataResult.notifyDataChanged();

    if (this.model) {
      this.model.setData(dataResult);
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BubbleChart({ model: this.model })
          .width('100%')
          .height('70%')
          .backgroundColor(Color.White)
      }
      Column() {
        SeekBar({ model: this.seekBarX })
        SeekBar({ model: this.seekBarY })
      }
    }
  }
}