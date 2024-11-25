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
  YAxis,
  EntryOhos,
  LineDataSet,
  ILineDataSet,
  LineData,
  Mode,
  LineChart,
  LineChartModel,
  ColorTemplate,
  Legend,
  ChartColorStop,
  LegendForm,
  LegendVerticalAlignment,
  LegendHorizontalAlignment,
  LegendOrientation,
  ChartPixelMap
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title/index';
import SeekBar, { SeekBarModel } from '../../customcomponents/SeekBar';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct MultiplePage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_VALUES,Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_FILLED, Constants.TOGGLE_CIRCLES,
  Constants.TOGGLE_CUBIC, Constants.TOGGLE_STEPPED, Constants.TOGGLE_HORIZONTAL_CUBIC, Constants.TOGGLE_PINCHZOOM,
  Constants.TOGGLE_AUTO_SCALE, Constants.TOGGLE_HIGHLIGHT, Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  @Local private model: LineChartModel = new LineChartModel();
  //标题栏标题
  private title: string = 'MultiLineChart'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  topAxis: XAxis = new XAxis(); //顶部X轴
  bottomAxis: XAxis = new XAxis(); //底部X轴
  mWidth: number = 350; //表的宽度
  mHeight: number = 300; //表的高度
  minOffset: number = 15;
  leftAxis: YAxis | null = null;
  rightAxis: YAxis | null = null;
  lineData: LineData | null = null;
  private isDrawIcons: boolean= false;
  @Local titleSelectString: string = 'X'
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
    let data = this.model.getData();
    let sets: JArrayList<ILineDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case Constants.TOGGLE_VALUES:
        if (!data || !sets) {
          break;
        }
        for (let i = 0; i < sets.size(); i++) {
          let set = sets.get(i) as LineDataSet;
          set.setDrawValues(!set.isDrawValuesEnabled());
        }
        this.model.invalidate();
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
      case Constants.TOGGLE_HIGHLIGHT:
        if (data != null) {
          data.setHighlightEnabled(!data.isHighlightEnabled());
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_FILLED:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as LineDataSet;
            if (set.isDrawFilledEnabled()) {
              set.setDrawFilled(false);
            } else {
              set.setDrawFilled(true);
            }
          }
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_CIRCLES:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as LineDataSet;
            if (set.isDrawCirclesEnabled()) {
              set.setDrawCircles(false);
            } else {
              set.setDrawCircles(true);
            }
          }
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_CUBIC:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as LineDataSet;
            set.setMode(set.getMode() == Mode.CUBIC_BEZIER ? Mode.LINEAR : Mode.CUBIC_BEZIER);
          }
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_STEPPED:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as LineDataSet;
            set.setMode(set.getMode() == Mode.STEPPED ? Mode.LINEAR : Mode.STEPPED);
          }
          this.model.invalidate();
        }
        break;
      case Constants.TOGGLE_HORIZONTAL_CUBIC:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as LineDataSet;
            set.setMode(set.getMode() == Mode.HORIZONTAL_BEZIER ? Mode.LINEAR : Mode.HORIZONTAL_BEZIER);
          }
          this.model.invalidate();
        }
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
            let set = sets.get(i) as LineDataSet;
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
    if (this.titleSelectString == 'X') {
      this.model.animateX(2000);
    } else if (this.titleSelectString == 'Y') {
      this.model.animateY(2000);
    } else if (this.titleSelectString == 'XY') {
      this.model.animateXY(2000, 2000);
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

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(true);
      // 设置图例的位置
      legend.setVerticalAlignment(LegendVerticalAlignment.TOP);
      legend.setHorizontalAlignment(LegendHorizontalAlignment.CENTER);
      legend.setOrientation(LegendOrientation.HORIZONTAL);
      legend.setDrawInside(false);

      // 设置图例的样式和大小
      legend.setForm(LegendForm.DEFAULT);

      // 设置图例的颜色
      legend.setTextColor(Color.Black);
    }

    this.model.setMinOffset(this.minOffset);
    this.model.setDragDecelerationEnabled(true);
    this.model.setVisibleXRangeMaximum(5);
    if (this.lineData) {
      this.model.setData(this.lineData);
    }

  }

  /**
   * 初始化数据
   * @param count  曲线图点的个数
   * @param range  y轴范围
   */
  private async setData(count: number, range: number): Promise<void> {
    if(range == 0) {
      range = 100;
    }
    let values1 = new JArrayList<EntryOhos>();
    let values2 = new JArrayList<EntryOhos>();
    let values3 = new JArrayList<EntryOhos>();
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range + 3;
      values1.add(new EntryOhos(i, val,pixelMap));
    }

    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range + 3;
      values2.add(new EntryOhos(i, val));
    }

    for (let i = 0; i < count; i++) {
      let val: number = Math.random() * range + 3;
      values3.add(new EntryOhos(i, val));
    }

    let dataSet = new JArrayList<ILineDataSet>();

    let gradientFillColor = new JArrayList<ChartColorStop>();
    gradientFillColor.add(['#ffffff', 0.3]);
    gradientFillColor.add(['#00ffff', 1.0]);

    let set1 = new LineDataSet(values1, "DataSet 1");
    set1.setDrawFilled(false);
    set1.setDrawValues(true);
    set1.setDrawIcons(this.isDrawIcons);
    set1.setDrawCircles(true);
    set1.setDrawCircleHole(true);
    set1.setCircleColor(ColorTemplate.colorRgb(0, 255, 0));
    set1.setCircleRadius(4);
    set1.setCircleHoleRadius(2);
    set1.setMode(Mode.LINEAR);
    set1.setColorByColor(Color.Green);
    set1.setLineWidth(2.5)
    set1.enableDashedLine(10, 10, 0);
    dataSet.add(set1);

    let set2 = new LineDataSet(values2, "DataSet 2");
    set2.setDrawFilled(false);
    set2.setDrawValues(true);
    set1.setDrawIcons(this.isDrawIcons);
    set2.setDrawCircles(true);
    set2.setDrawCircleHole(true);
    set2.setCircleColor(ColorTemplate.colorRgb(255, 247, 140));
    set2.setCircleRadius(4);
    set2.setCircleHoleRadius(2);
    set2.setMode(Mode.LINEAR);
    set2.setColorByColor(Color.Orange);
    set2.setLineWidth(2.5);
    dataSet.add(set2);

    let set3 = new LineDataSet(values3, "DataSet 3");
    set3.setDrawFilled(false);
    set3.setDrawValues(true);
    set3.setDrawCircles(true);
    set3.setDrawCircleHole(true);
    set3.setCircleColor(ColorTemplate.colorRgb(255, 208, 140));
    set3.setCircleRadius(4);
    set3.setCircleHoleRadius(2);
    set1.setDrawIcons(this.isDrawIcons);
    set3.setMode(Mode.LINEAR);
    set3.setColorByColor(Color.Yellow);
    set3.setLineWidth(2.5);
    dataSet.add(set3);

    let lineData: LineData = new LineData(dataSet);
    if (this.model) {
      this.model.setData(lineData);
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {

        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.model })
            .height('75%')
        }

        Column() {
          SeekBar({ model: this.seekBarX })
          SeekBar({ model: this.seekBarY })
        }
      }
    }
  }
}
