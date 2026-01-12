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
  Description,
  EntryOhos,
  Highlight,
  JArrayList,
  Legend,
  LimitLabelPosition,
  LimitLine,
  MarkerView,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
  WaterfallDataSet,
  WaterfallEntry,
  WaterfallHighlight,
  WaterfallChart,
  WaterfallChartModel,
  IAxisValueFormatter,
  AxisBase,
  WaterfallData,
  IWaterfallDataSet,
  ChartPixelMap,
  BarDataSet,
  IBarDataSet,
  AxisDependency
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title';
import { LogUtil } from '../../utils/LogUtil';
import Utils from '../../utils/Utils'

@Entry
@ComponentV2
struct SimpleWaterfallChartPage {
  @Local private model: WaterfallChartModel | undefined = undefined;
  private dataSet: WaterfallDataSet | null = null;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private limitLine1: LimitLine | null = null;
  private limitLine2: LimitLine | null = null;
  private data: WaterfallData | null = null;
  private normalMarker: MarkerView | null = null;
  private  pixelMap:ChartPixelMap=new ChartPixelMap()
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON];
  private isDrawIcons: boolean= false;
  //标题栏标题
  private title: string = 'Waterfall'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("SimpleWaterfallChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("SimpleWaterfallChartPage onNothingSelected");
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
    let data = this.model!.getData();
    let sets: JArrayList<IWaterfallDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }
    switch (this.menuItemArr[index]) {
      case Constants.TOGGLE_DRAW_ICON:
        if (sets) {
          for (let i = 0; i < sets.size(); i++) {
            let set = sets.get(i) as WaterfallDataSet;
            set.setDrawIcons(!this.isDrawIcons)
          }
          this.isDrawIcons=!this.isDrawIcons
          this.model!.invalidate();
        }
        break;
    }
    this.titleModel.setIndex(-1)
  }


  async aboutToAppear() {
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title
    this.model = new WaterfallChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    this.model.setOnChartGestureListener(this.chartGestureListener);

    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(false);
    }

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(false);
    }

    // if more than 40 entries are displayed in the this.model, no values will be drawn
    this.model.setMaxVisibleValueCount(40);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(true);

    this.model.setDrawGridBackground(false);
    this.model.setGridBackgroundColor('#500000ff')
    this.model.setDrawBarShadow(false);
    this.model.setDragDecelerationEnabled(true);

    this.model.setDrawValueAboveBar(false);
    this.model.setHighlightFullBarEnabled(false);

    this.limitLine1 = new LimitLine(120, 'Upper Limit');
    this.limitLine1.setLineWidth(4);
    this.limitLine1.enableDashedLine(10, 10, 0);
    this.limitLine1.setLabelPosition(LimitLabelPosition.RIGHT_TOP);
    this.limitLine1.setTextSize(10);

    this.limitLine2 = new LimitLine(50, 'Lower Limit');
    this.limitLine2.setLineWidth(4);
    this.limitLine2.enableDashedLine(10, 10, 0);
    this.limitLine2.setLineColor(Color.Yellow);
    this.limitLine2.setLabelPosition(LimitLabelPosition.RIGHT_BOTTOM);
    this.limitLine2.setTextSize(10);

    // change the position of the y-labels
    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setAxisMinimum(0); // this replaces setStartAtZero(true)
      this.leftAxis.setDrawLimitLinesBehindData(false);
      this.leftAxis.setValueFormatter(new CustomFormatter());

      // add limit lines
      this.leftAxis.addLimitLine(this.limitLine1);
      this.leftAxis.addLimitLine(this.limitLine2);
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

    this.normalMarker = new MarkerView();
    this.model.setMarker(this.normalMarker);

    this.pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    this.data =await this.getNormalData();
    this.model.setData(this.data);
    this.model.setVisibleXRangeMaximum(6);
    this.model.invalidate();
  }

  private async getNormalData(): Promise<WaterfallData> {
    let pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();
    let h1 = new WaterfallHighlight(20, 30, Color.Gray);
    let h2 = new WaterfallHighlight(30, 40, Color.Gray);
    let h3 = new WaterfallHighlight(40, 60, Color.Green);
    let h4 = new WaterfallHighlight(60, 70, Color.Red);

    values.add(new WaterfallEntry(1, 10, 30, pixelMap, undefined, h1));
    values.add(new WaterfallEntry(2, 15, 50,pixelMap));
    values.add(new WaterfallEntry(3, 20, 90,pixelMap));
    values.add(new WaterfallEntry(4, 5, 95, pixelMap, undefined, h2, h3, h4));
    values.add(new WaterfallEntry(5, 5, 50,pixelMap));
    values.add(new WaterfallEntry(6, 45, 80, pixelMap, undefined, h2, h3));
    values.add(new WaterfallEntry(7, 20, 70,pixelMap));
    values.add(new WaterfallEntry(8, 40,50,pixelMap, undefined, h1, h3));
    values.add(new WaterfallEntry(9, 6, 78,pixelMap));
    values.add(new WaterfallEntry(10, 70, 80,pixelMap, undefined, h2, h4));
    values.add(new WaterfallEntry(11, 10, 45,pixelMap));
    values.add(new WaterfallEntry(12, 12, 30,pixelMap, undefined, h2, h3));
    values.add(new WaterfallEntry(13, 40, 64,pixelMap));
    values.add(new WaterfallEntry(14, 23, 56,pixelMap, undefined, h1, h3));
    values.add(new WaterfallEntry(15, 68, 78,pixelMap));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(true);
    this.dataSet.setDrawIcons(this.isDrawIcons);
    this.dataSet.setColorByColor(Color.Pink);
    this.dataSet.setValueTextSize(10)

    let dataSetList: JArrayList<IWaterfallDataSet> = new JArrayList<IWaterfallDataSet>();
    dataSetList.add(this.dataSet);

    let data: WaterfallData = new WaterfallData(dataSetList);
    data.setBarWidth(0.3);
    return data;
  }

  private async getNormalData2(): Promise<WaterfallData> {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();

    values.add(new WaterfallEntry(1, 10, 70,this.pixelMap));
    values.add(new WaterfallEntry(2, 15, 80,this.pixelMap));
    values.add(new WaterfallEntry(3, 20, 90,this.pixelMap));
    values.add(new WaterfallEntry(4, 5, 80,this.pixelMap));
    values.add(new WaterfallEntry(5, 5, 50,this.pixelMap));
    values.add(new WaterfallEntry(6, 45, 80,this.pixelMap));
    values.add(new WaterfallEntry(7, 60, 70,this.pixelMap));


    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(true);
    this.dataSet.setDrawIcons(this.isDrawIcons);
    this.dataSet.setColorByColor(Color.Pink);
    this.dataSet.setValueTextSize(10)

    // 根据Y刻度范围设置颜色
    let highlightList: WaterfallHighlight[] = [];
    highlightList.push(new WaterfallHighlight(0, 40, Color.Green));
    highlightList.push(new WaterfallHighlight(40, 60, Color.Orange));
    highlightList.push(new WaterfallHighlight(60, 100, Color.Pink));
    this.dataSet.setYAxisSegmentationColors(highlightList);

    let dataSetList: JArrayList<IWaterfallDataSet> = new JArrayList<IWaterfallDataSet>();
    dataSetList.add(this.dataSet);

    let data: WaterfallData = new WaterfallData(dataSetList);
    data.setBarWidth(0.3);
    return data;
  }

  private async getNormalData3(): Promise<WaterfallData> {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();

    values.add(new WaterfallEntry(1, 10, 90,this.pixelMap));
    values.add(new WaterfallEntry(2, 15, 80,this.pixelMap));
    values.add(new WaterfallEntry(3, 20, 90,this.pixelMap));
    values.add(new WaterfallEntry(4, 5, 80,this.pixelMap));
    values.add(new WaterfallEntry(5, 5, 50,this.pixelMap));
    values.add(new WaterfallEntry(6, 45, 80,this.pixelMap));
    values.add(new WaterfallEntry(7, 60, 70,this.pixelMap));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Gray);
    this.dataSet.setDrawIcons(true);
    this.dataSet.setDrawIcons(this.isDrawIcons);
    this.dataSet.setColorByColor(Color.Pink);
    this.dataSet.setValueTextSize(10)
    // 根据Y刻度范围设置颜色
    let highlightList: WaterfallHighlight[] = [];
    highlightList.push(new WaterfallHighlight(0, 40, '#cddc39'));
    highlightList.push(new WaterfallHighlight(40, 60, '#00bcd4'));
    highlightList.push(new WaterfallHighlight(60, 100, '#ff9800'));
    this.dataSet.setYAxisSegmentationColors(highlightList);
    // 最高点最低点高亮时颜色设置
    this.dataSet.setEnableMaxOrMinHighlightColor(true);
    this.dataSet.setMaxyHighlightColor(Color.Brown);
    this.dataSet.setMinyHighlightColor(Color.Yellow);
    let dataSetList: JArrayList<IWaterfallDataSet> = new JArrayList<IWaterfallDataSet>();
    dataSetList.add(this.dataSet);

    let data: WaterfallData = new WaterfallData(dataSetList);
    data.setBarWidth(0.3);
    return data;
  }

  private getNormalData4(): WaterfallData {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();
    let h9 = new WaterfallHighlight(-30, -15, Color.Blue)
    let h5 = new WaterfallHighlight(-15, 15, Color.Red)
    let h6 = new WaterfallHighlight(15, 20, Color.Pink)
    let h1 = new WaterfallHighlight(20, 30, Color.Red);
    let h7 = new WaterfallHighlight(30, 45, Color.Green);
    let h10 = new WaterfallHighlight(45, 60, Color.Orange);

    let h2 = new WaterfallHighlight(30, 40, Color.Pink);
    let h3 = new WaterfallHighlight(40, 60, Color.Green);
    let h4 = new WaterfallHighlight(60, 70, Color.Red);

    values.add(new WaterfallEntry(1, -30, 60, undefined, undefined, h9, h5, h6, h1, h7, h10));
    values.add(new WaterfallEntry(4, 5, 95, undefined, undefined, h2, h3, h4));
    values.add(new WaterfallEntry(6, 45, 80, undefined, undefined, h2, h3));

    this.dataSet = new WaterfallDataSet(values, 'DataSet');
    this.dataSet.setDrawIcons(false);
    this.dataSet.setColorByColor(Color.Brown);
    this.dataSet.setValueTextSize(10)

    let dataSetList: JArrayList<IWaterfallDataSet> = new JArrayList<IWaterfallDataSet>();
    dataSetList.add(this.dataSet);

    let data: WaterfallData = new WaterfallData(dataSetList);
    data.setBarWidth(0.3);
    return data;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        WaterfallChart({ model: this.model })
          .width('100%')
          .height('70%')
      }

      Column() {
        Row() {
          Text($r('app.string.normal'))
          Radio({ value: 'CUBIC_BEZIER', group: 'dataType' })
            .checked(true)
            .onChange(async (isChecked: boolean) => {
              if (isChecked) {
                this.data =await this.getNormalData()
                if (this.dataSet && this.model) {
                  this.model.setData(this.data);
                }
              }
            })
        }

        Row() {
          Text($r('app.string.segmented_by_y_axis'))
          Radio({ value: 'CUBIC_BEZIER', group: 'dataType' })
            .checked(false)
            .onChange(async (isChecked: boolean) => {
              if (isChecked) {
                this.data = await this.getNormalData2()
                if (this.dataSet && this.model) {
                  this.model.setData(this.data);
                }
              }
            })
        }

        Row() {
          Text($r('app.string.highlight_the_highest_and_lowest_points'))
          Radio({ value: 'CUBIC_BEZIER', group: 'dataType' })
            .checked(false)
            .onChange(async (isChecked: boolean) => {
              if (isChecked) {
                this.data = await this.getNormalData3()
                if (this.dataSet && this.model) {
                  this.model.setData(this.data);
                }
              }
            })
        }
      }.alignSelf(ItemAlign.Center)

      Row() {
        Text($r('app.string.test_data'))
        Radio({ value: 'test', group: 'dataType' })
          .checked(false)
          .onChange((isChecked: boolean) => {
            if (isChecked) {
              this.data = this.getNormalData4()
              if (this.dataSet && this.model) {
                this.model.getAxisLeft()?.setAxisMinimum(-30);
                this.model.setData(this.data);
              }
            }
          })
      }
    }
  }
}

class CustomFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    return Math.abs(value).toFixed(0) + "%";
  }
}