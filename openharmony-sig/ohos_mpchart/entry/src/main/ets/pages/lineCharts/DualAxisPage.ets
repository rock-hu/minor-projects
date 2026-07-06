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
  LineDataSet,
  ILineDataSet,
  LineData,
  LineChart,
  LineChartModel,
  OnChartGestureListener,
  ChartGesture,
  ColorTemplate,
  Mode,
  ChartPixelMap,

} from '@ohos/mpchart';
import Constants from '../../constants/Constants';

import title, { ChartTitleModel } from '../../title';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import Utils from '../../utils/Utils';
import { LogUtil } from '../../utils/LogUtil';

@Entry
@ComponentV2
struct DualAxisPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_VALUES, Constants.TOGGLE_DRAW_ICON,Constants.TOGGLE_FILLED, Constants.TOGGLE_CIRCLES,
  Constants.TOGGLE_CUBIC, Constants.TOGGLE_STEPPED, Constants.TOGGLE_HORIZONTAL_CUBIC, Constants.TOGGLE_PINCHZOOM,
  Constants.TOGGLE_AUTO_SCALE, Constants.TOGGLE_HIGHLIGHT, Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];

  //标题栏标题
  private title: string = 'LineChartPage2'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  titleSelectString: string = 'X'

  @Local private model: LineChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private isDrawIcons: boolean= false;

  private dataSet : LineDataSet |null = null;
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
      LogUtil.log("-----------------chartGestureListener onChartGestureStart lastMode: " + lastPerformedGestureMode)
    },

    onChartGestureEnd: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, lastPerformedGestureMode: ChartGesture) => {
      LogUtil.log("-----------------chartGestureListener onChartGestureEnd lastMode: " + lastPerformedGestureMode)
    },


    onChartLongPressed: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartLongPressed")
    },


    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartDoubleTapped")

    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartSingleTapped")

    },

    onChartFling: (isTouchEvent: boolean, me1: TouchEvent | GestureEvent, me2: TouchEvent, velocityX: number, velocityY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartFling velocityX: " + velocityX + "  velocityY: " + velocityY)

    },

    onChartScale: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, scaleX: number, scaleY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartScale scaleX: " + scaleX + "  scaleY: " + scaleY)

    },

    onChartTranslate: (isTouchEvent: boolean, me: TouchEvent | GestureEvent, dX: number, dY: number) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate dx: " + dX + "  dy: " + dY)

    },
    onChartMove: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      LogUtil.log("-----------------chartGestureListener onChartTranslate moved ");
    }
  }

  toggleValues(data: LineData | null, sets: JArrayList<ILineDataSet> | null) {
    if (!data || !sets) {
      return;
    }
    for (let i = 0; i < sets.size(); i++) {
      let set = sets.get(i) as LineDataSet;
      set.setDrawValues(!set.isDrawValuesEnabled());
    }
    this.model?.invalidate();
  }

  togglePinchZoom() {
    this.model?.setPinchZoom(!this.model.isPinchZoomEnabled())
    this.model?.invalidate();
  }

  toggleAutoScale() {
    this.model?.setAutoScaleMinMaxEnabled(!this.model.isAutoScaleMinMaxEnabled());
    this.model?.notifyDataSetChanged();
  }

  toggleHighlight(data: LineData | null) {
    if (data !== null) {
      data.setHighlightEnabled(!data.isHighlightEnabled());
      this.model?.invalidate();
    }
  }

  toggleFilled(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawFilled(!set.isDrawFilledEnabled())
      }
      this.model?.invalidate();
    }
  }

  toggleCircles(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawCircles(!set.isDrawCirclesEnabled());
      }
      this.model?.invalidate();
    }
  }

  toggleCubic(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.CUBIC_BEZIER ? Mode.LINEAR : Mode.CUBIC_BEZIER);
      }
      this.model?.invalidate();
    }
  }

  toggleStepped(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.STEPPED ? Mode.LINEAR : Mode.STEPPED);
      }
      this.model?.invalidate();
    }
  }

  toggleHorizontalCubic(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.HORIZONTAL_BEZIER ? Mode.LINEAR : Mode.HORIZONTAL_BEZIER);
      }
      this.model?.invalidate();
    }
  }

  toggleDrawIcon(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawIcons(!this.isDrawIcons)
      }
      this.isDrawIcons = !this.isDrawIcons
      this.model?.invalidate();
    }
  }

  handleAnimate(s: string) {
    this.titleSelectString = s;
    this.animate();
  }

  saveImage() {
    Utils.saveImage(this.title, this.model ? this.model.context2d : null);
  }

  //标题栏菜单回调
  @Monitor("titleModel.index")
  menuCallback(){
    if (this.model === null || this.model === undefined) {
      return
    }
    let index: number = this.titleModel.getIndex()
    if(index === undefined || index === null || index === -1){
      return
    }
    let data = this.model.getData();
    let sets: JArrayList<ILineDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case Constants.TOGGLE_VALUES:
        this.toggleValues(data, sets)
        break;
      case Constants.TOGGLE_PINCHZOOM:
        this.togglePinchZoom()
        break;
      case Constants.TOGGLE_AUTO_SCALE:
        this.toggleAutoScale()
        break;
      case Constants.TOGGLE_HIGHLIGHT:
        this.toggleHighlight(data)
        break;
      case Constants.TOGGLE_FILLED:
        this.toggleFilled(sets)
        break;
      case Constants.TOGGLE_CIRCLES:
        this.toggleCircles(sets)
        break;
      case Constants.TOGGLE_CUBIC:
        this.toggleCubic(sets)
        break;
      case Constants.TOGGLE_STEPPED:
        this.toggleStepped(sets)
        break;
      case Constants.TOGGLE_HORIZONTAL_CUBIC:
        this.toggleHorizontalCubic(sets)
        break;
      case Constants.ANIMATE_X:
        this.handleAnimate('X')
        break;
      case Constants.ANIMATE_Y:
        this.handleAnimate('Y')
        break;
      case Constants.ANIMATE_XY:
        this.handleAnimate('XY')
        break;
      case Constants.SAVE_IMAGE:
        this.saveImage()
        break;
      case Constants.TOGGLE_DRAW_ICON:
        this.toggleDrawIcon(sets)
        break;
    }
    this.titleModel.setIndex(-1)
  }

  public animate() {
    if (this.model) {
      if (this.titleSelectString === 'X') {
        this.model.animateX(2000);
      } else if (this.titleSelectString === 'Y') {
        this.model.animateY(2000);
      } else if (this.titleSelectString === 'XY') {
        this.model.animateXY(2000,2000);
      }
    }

  }

  aboutToAppear() {
    this.seekBarX.setValue(18)
      .setMax(50)
      .setMin(5)

    this.seekBarY.setValue(30)
      .setMax(200)
      .setMin(20)
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new LineChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);
    this.model.setDragDecelerationEnabled(true);

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.TOP);
      this.xAxis.setDrawGridLines(true);//纵向线格显示
      this.xAxis.setGranularity(1);
      this.xAxis.setLabelCount(6);//设置X轴坐标个数
      this.xAxis.setTextColor(Color.White);
    }

    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setLabelCount(6, false);
      this.leftAxis.setTextColor(ColorTemplate.getHoloBlue())
      this.leftAxis.setDrawGridLines(true);
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      this.leftAxis.setDrawAxisLine(true);
      this.leftAxis.setGranularityEnabled(true);
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(true);//右Y轴是否显示
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setLabelCount(6, false);
      this.rightAxis.setTextColor(Color.Red);
      this.rightAxis.setDrawAxisLine(false);
      this.rightAxis.setDrawLabels(true);
      this.rightAxis.setDrawZeroLine(false);
      this.rightAxis.setGranularityEnabled(false);
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(false);
    }

    this.setData(18,30);//数据点的个数太多会导致数据不能显示

    this.model.setVisibleXRangeMaximum(50);//会影响数据点的显示，太大也不会显示
  }

  /**
   * 初始化数据
   * @param count  曲线图点的个数
   * @param range  y轴范围
   */
  private async setData(count: number, range: number): Promise<void> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)

    let values1 = new JArrayList<EntryOhos>();
    let values2 = new JArrayList<EntryOhos>();
    let values3 = new JArrayList<EntryOhos>();

    for (let i = 0; i < count; i++) {
      let yVal1: number = Math.random() * (range / 2) + 50;
      values1.add(new EntryOhos(i, yVal1, pixelMap));

      let yVal2: number = Math.random() * range + 100;
      values2.add(new EntryOhos(i, yVal2, pixelMap));

      let yVal3: number = Math.random() * range + 120;
      values3.add(new EntryOhos(i, yVal3, pixelMap));
    }

    let dataSet = new JArrayList<ILineDataSet>();

    let set1 = new LineDataSet(values1, "DataSet 1");
    set1.setDrawFilled(false);
    set1.setDrawValues(true);
    set1.setDrawIcons(this.isDrawIcons);
    set1.setDrawCircles(true);
    set1.setDrawCircleHole(false)
    set1.setCircleRadius(3);
    set1.setCircleColor(Color.White)
    set1.setColorByColor(ColorTemplate.colorRgb(140, 234, 255));
    set1.setLineWidth(2)
    dataSet.add(set1);

    let set2 = new LineDataSet(values2, "DataSet 2");
    set2.setDrawFilled(false);
    set2.setDrawValues(true);
    set2.setDrawIcons(this.isDrawIcons);
    set2.setDrawCircles(true);
    set2.setDrawCircleHole(false)
    set2.setCircleRadius(3);
    set2.setCircleColor(Color.White)
    set2.setColorByColor(Color.Red);
    set2.setLineWidth(2)
    dataSet.add(set2);

    let set3 = new LineDataSet(values3, "DataSet 3");
    set3.setDrawFilled(false);
    set3.setDrawValues(true);
    set3.setDrawCircles(true);
    set3.setDrawIcons(this.isDrawIcons)
    set3.setDrawCircleHole(false)
    set3.setCircleRadius(3);
    set3.setCircleColor(Color.White)
    set3.setColorByColor(Color.Yellow);
    set3.setLineWidth(2)
    dataSet.add(set3);
    let lineData:LineData = new LineData(dataSet);
    lineData.setValueTextSize(10);
    if (this.model) {
      this.model.setData(lineData);
    }
  }


  build() {
    Column() {
      title({ model: this.titleModel })

      Stack({ alignContent: Alignment.TopStart }) {
        LineChart({ model: this.model })
          .width('100%')
          .height('70%')
          .backgroundColor('#CCCCCC');
      }
      Column() {
        SeekBar({ model: this.seekBarX })
        SeekBar({ model: this.seekBarY })
      }
    }
  }
}