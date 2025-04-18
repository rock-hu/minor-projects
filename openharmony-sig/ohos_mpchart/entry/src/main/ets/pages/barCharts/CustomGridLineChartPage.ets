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
  IBarDataSet,
  JArrayList,
  XAxis,
  XAxisPosition,
  YAxis,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';
import { DashPathEffect } from '@ohos/mpchart/src/main/ets/components/data/Paint';

interface XAxisColorSetter {
  colorText: string
  colorVal: string
  color: number
}

interface GridColorSetter {
  colorText: string
  colorVal: string
  color: number
}

@Entry
@ComponentV2
struct CustomGridLineChartPage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private data: BarData | null = null;

  // 事件显示文字
  @Local eventText: string = '';

  @Local colorSetter: XAxisColorSetter[] = [
    { colorText: '红色', color: Color.Red, colorVal: 'red' },
    { colorText: '蓝色', color: Color.Blue, colorVal: 'blue' },
  ]
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS,Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_VALUES,
    Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE,
    Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  private isDrawIcons: boolean= false;
  //标题栏标题
  private title: string = 'custom grid line'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()

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

  async aboutToAppear() {

    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BarChartModel();

    this.model.setDragEnabled(false); //禁用滑动
    this.model.setScaleEnabled(false); //禁用缩放


    // if more than 40 entries are displayed in the this.model, no values will be drawn
    this.model.setMaxVisibleValueCount(40);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(false);

    this.model.setDrawGridBackground(false);
    this.model.setGridBackgroundColor('#500000ff')
    this.model.setDrawBarShadow(false);

    this.model.setDrawValueAboveBar(false);
    this.model.setHighlightFullBarEnabled(false);

    // change the position of the y-labels
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
    }

    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setEnabled(false);
      this.rightAxis.setAxisMinimum(0);
    }

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(XAxisPosition.BOTTOM);
    }

    this.data =await this.getNormalData();
    this.model.setData(this.data);
    this.model.setVisibleXRangeMaximum(20);
  }

  private async getNormalData(): Promise<BarData> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 73.3,pixelMap));
    values.add(new BarEntry(2, 5.4));
    values.add(new BarEntry(3, 73.9));
    values.add(new BarEntry(4, 79.9,pixelMap));
    values.add(new BarEntry(5, 69.3,pixelMap));
    values.add(new BarEntry(6, 70.7));
    values.add(new BarEntry(7, 81.2,pixelMap));
    values.add(new BarEntry(8, 13.1));
    values.add(new BarEntry(9, 34.2,pixelMap));
    values.add(new BarEntry(10, 58.4,pixelMap));
    values.add(new BarEntry(11, 44.7));
    values.add(new BarEntry(12, 10.5,pixelMap));
    values.add(new BarEntry(13, 15.6,pixelMap));
    values.add(new BarEntry(14, 95.8,pixelMap));
    values.add(new BarEntry(15, 57.4,pixelMap));
    values.add(new BarEntry(16, 64.5,pixelMap));
    values.add(new BarEntry(17, 21.4));
    values.add(new BarEntry(18, 33.2));
    values.add(new BarEntry(19, 96.9,pixelMap));

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(this.isDrawIcons);
    dataSet.setColorByColor(Color.Pink);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.85);
    return barData;
  }

  // 网格线设置
  @Builder gridLineSettings() {
    Row() {
      Text($r("app.string.grid_line_color"))
      ForEach([
        {
          colorText: $r('app.string.red'),
          color: Color.Red,
          colorVal: 'red'
        },
        {
          colorText: $r('app.string.blue'),
          color: Color.Blue,
          colorVal: 'blue'
        }
      ], (item: GridColorSetter) => {
        Row() {
          Text(item.colorText)
          Radio({ value: item.colorVal, group: 'gridValue' })
            .checked(false)
            .onChange((isChecked: boolean) => {
              if (isChecked) {
                if (this.model) {
                  const yAxis = this.model.getAxisLeft()
                  if (yAxis) {
                    yAxis.setGridColor(item.color)
                    yAxis.setGridLineWidth(0.6)
                    yAxis.setGridDashedLine(new DashPathEffect([2, 2], 1))
                    this.model.invalidate();
                  }
                }
              }
            })
        }
      }, (item: GridColorSetter) => item.colorText)

    }.alignSelf(ItemAlign.Start)

    Row() {
      Text($r('app.string.custom_gridlines'));
      Checkbox({ name: 'customGridLine', group: 'gridLine' })
        .select(false)
        .onChange((isChecked: boolean) => {
          if (!this.leftAxis || !this.xAxis) return;
          if (this.model) {
            if (isChecked) {
              // 竖向网格线，x轴刻度为8的位置
              this.xAxis.addGridLine({
                position: 8,
                lineColor: Color.Blue,
                lineWidth: 0.6,
                dashPathEffect: { lineLength: 5, spaceLength: 10, phase: 2 }
              })

              this.xAxis.addGridLine({
                position: -0.4,
                lineColor: Color.Blue,
                lineWidth: 0.6,
                dashPathEffect: { lineLength: 5, spaceLength: 10, phase: 2 }
              })

              this.xAxis.addGridLine({
                position: 20,
                lineColor: Color.Blue,
                lineWidth: 0.6,
                dashPathEffect: { lineLength: 5, spaceLength: 10, phase: 2 }
              })

              // 横向网格线，y轴刻度为35的位置
              this.leftAxis.addGridLine({ position: 35, lineColor: Color.Red, lineWidth: 1, })

              this.leftAxis.addGridLine({ position: 45, lineColor: 0x999999, lineWidth: 1, })
              this.leftAxis.addGridLine({ position: 55, lineColor: Color.Blue, lineWidth: 1, })
            } else {
              const verticalLines = this.xAxis.getGridLines();
              const horizontalLines = this.leftAxis.getGridLines();
              verticalLines.clear();
              horizontalLines.clear();
            }
            this.model.invalidate();
          }
        })
    }.alignSelf(ItemAlign.Start)
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BarChart({ model: this.model })
          .width('100%')
          .height('30%')

        Divider()
          .width('1px')
          .padding({ top: 5 })

        Scroll() {
          Stack() {
            Column() {
              this.gridLineSettings()
            }
          }.align(Alignment.TopEnd);
        }
      }
    }
  }
}