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
  ChartGesture,
  ChartPixelMap,
  Description,
  EntryOhos,
  Fill,
  Highlight,
  IBarDataSet,
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
  YAxisLabelPosition
} from '@ohos/mpchart';
import CustomMarkerView from '../../customcomponents/CustomMarkerView';
import promptAction from '@ohos.promptAction';
import title, { ChartTitleModel } from '../../title';
import LongPressMarkerView from '../../customcomponents/LongPressMarkerView';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';
import { LogUtil } from '../../utils/LogUtil';
import MpSlider from '../../customView/MpSlider';
import MpSelect, { ValueObject } from '../../customView/MpSelect';

@Entry
@ComponentV2
struct ScrollBarChartPage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private limitLine1: LimitLine | null = null;
  private limitLine2: LimitLine | null = null;
  private data: BarData | null = null;
  private mBarWidth: string = '85';
  private mBackgroundColor: string = '#500000ff';
  private mLastMaxVisibleXCount: string = '20';
  @Local maxLabelSize: number = 20;
  private normalMarker: MarkerView | null = null;
  private stackMarker: CustomMarkerView | null = null;
  private isChangedMaxVisibleXCount: boolean = false;
  private isDrawIcons: boolean= false;
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_BAR_BORDERS, Constants.TOGGLE_DRAW_ICON,Constants.TOGGLE_VALUES,
  Constants.TOGGLE_HIGHLIGHT, Constants.TOGGLE_PINCHZOOM, Constants.TOGGLE_AUTO_SCALE, Constants.TOGGLE_ROUNDED,
  Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  titleSelectString: string = 'X'
  //标题栏标题
  private title: string = 'BarChart'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  private  pixelMap:ChartPixelMap=new ChartPixelMap()
  @Local bgColors: Array<Color | string> = ['#C7EDCC', '#FDE6E0', '#FAF9DE', '#DCE2F1', '#E9EBFE', Color.White]
  @Local limitLinePosition: number = 3;
  @Local limitLinePositionList: number[] = [
    0, 1, 2, 3
  ]
  @Local limitLinePositionValueList: Array<ValueObject> = [
    { value: 'none' },
    { value: 'above' },
    { value: 'below' },
    { value: 'both' }
  ]
  @Local widthPercentage: number = 85;

  @Monitor("maxLabelSize")
  setMaxLabelSize() {
    if (this.model) {
      this.model.setVisibleXRangeMaximum(this.maxLabelSize);
      this.model.setVisibleXRangeMinimum(this.maxLabelSize);
      this.model.notifyDataSetChanged();
      this.model.invalidate();
    }
  }

  @Monitor("widthPercentage")
  setWidthPercentage() {
    if (this.model && this.data) {
      this.data.setBarWidth(this.widthPercentage / 100);
      this.model.invalidate();
    }
  }

  @Monitor("limitLinePosition")
  changeLimitLine() {
    this.leftAxis?.removeAllLimitLines();
    switch (this.limitLinePosition) {
      case 0:
        break;
      case 1:
        this.limitLine1 && this.leftAxis?.addLimitLine(this.limitLine1);
        break;
      case 2:
        this.limitLine2 && this.leftAxis?.addLimitLine(this.limitLine2);
        break;
      case 3:
        this.limitLine1 && this.leftAxis?.addLimitLine(this.limitLine1);
        this.limitLine2 && this.leftAxis?.addLimitLine(this.limitLine2);
        break;
    }
    this.model?.invalidate();
  }

  @Builder
  colorButton(color: string | number) {
    Button().backgroundColor(color).borderWidth(1).aspectRatio(1).onClick(() => {
      if (this.model) {
        this.model.setGridBackgroundColor(color);
        this.model.setDrawGridBackground(true);
        this.model.invalidate();
      }
    })
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
      case Constants.TOGGLE_ROUNDED:
        if (!barData) {
          break;
        }
        if (barData.getTopRadius() == 0) {
          barData.setTopRadius(5);
        }else{
          barData.setTopRadius(0);
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

  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("ScrollBarChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("ScrollBarChartPage onNothingSelected");
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

  async aboutToAppear() {

    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BarChartModel();
    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    this.model.setOnChartGestureListener(this.chartGestureListener);

    this.model.setDragEnabled(false); //禁用滑动
    this.model.setScaleEnabled(false); //禁用缩放
    let description: Description | null = this.model.getDescription()
    if (description) {
      description.setEnabled(true);
    }

    let l: Legend | null = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
    }

    // if more than 40 entries are displayed in the this.model, no values will be drawn
    this.model.setMaxVisibleValueCount(40);

    // scaling can now only be done on x- and y-axis separately
    this.model.setPinchZoom(false);

    this.model.setDrawGridBackground(false);
    this.model.setGridBackgroundColor('#500000ff')
    this.model.setDrawBarShadow(false);

    this.model.setDrawValueAboveBar(false);
    this.model.setHighlightFullBarEnabled(false);
    //设置长按事件自定义view
    this.model.setLongPressMarker(new LongPressMarkerView());
    // //设置是否支持拖拽
    // this.model.setDragEnabled(false);
    // //设置是否支持长按高亮
    // this.model.setHighlightPerLongPressEnabled(false);
    // //设置是否支持双击放大
    // this.model.setDoubleTapToZoomEnabled(false);
    // //设置是否支持点击高亮
    // this.model.setHighlightPerTapEnabled(false);

    this.limitLine1 = new LimitLine(80, 'Upper Limit');
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
    this.stackMarker = new CustomMarkerView();

    this.pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    this.data =await this.getNormalData();
    this.model.setData(this.data);
    this.model.setVisibleXRangeMaximum(20);
  }

  private async getNormalData(): Promise<BarData> {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 73.3,this.pixelMap));
    values.add(new BarEntry(2, 5.4,this.pixelMap));
    values.add(new BarEntry(3, 73.9));
    values.add(new BarEntry(4, 79.9));
    values.add(new BarEntry(5, 69.3));
    values.add(new BarEntry(6, 70.7,this.pixelMap));
    values.add(new BarEntry(7, 81.2,this.pixelMap));
    values.add(new BarEntry(8, 13.1,this.pixelMap));
    values.add(new BarEntry(9, 34.2));
    values.add(new BarEntry(10, 58.4));
    values.add(new BarEntry(11, 44.7,this.pixelMap));
    values.add(new BarEntry(12, 10.5,this.pixelMap));
    values.add(new BarEntry(13, 15.6));
    values.add(new BarEntry(14, 95.8));
    values.add(new BarEntry(15, 57.4,this.pixelMap));
    values.add(new BarEntry(16, 64.5));
    values.add(new BarEntry(17, 21.4,this.pixelMap));
    values.add(new BarEntry(18, 33.2));
    values.add(new BarEntry(19, 96.9,this.pixelMap));

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

  private async getGradientData(): Promise<BarData> {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 32.9));
    values.add(new BarEntry(2, 44.7));
    values.add(new BarEntry(3, 2.1,this.pixelMap));
    values.add(new BarEntry(4, 46.9,this.pixelMap));
    values.add(new BarEntry(5, 8.8));
    values.add(new BarEntry(6, 82.1));
    values.add(new BarEntry(7, 86.9,this.pixelMap));
    values.add(new BarEntry(8, 15.9,this.pixelMap));
    values.add(new BarEntry(9, 29.4));
    values.add(new BarEntry(10, 71.6,this.pixelMap));
    values.add(new BarEntry(11, 55.7,this.pixelMap));
    values.add(new BarEntry(12, 35.8));
    values.add(new BarEntry(13, 67.7,this.pixelMap));
    values.add(new BarEntry(14, 34.9));
    values.add(new BarEntry(15, 50.9,this.pixelMap));
    values.add(new BarEntry(16, 40.5));
    values.add(new BarEntry(17, 27.1,this.pixelMap));
    values.add(new BarEntry(18, 45.7,this.pixelMap));
    values.add(new BarEntry(19, 26.8));

    let dataSet: BarDataSet = new BarDataSet(values, 'DataSet');
    dataSet.setHighLightColor(Color.Black);
    dataSet.setDrawIcons(this.isDrawIcons);
    let startColor1: string = '#ffffbb33';
    let startColor2: string = '#ff33b5e5';
    let startColor3: string = '#ffffbb33';
    let startColor4: string = '#ff99cc00';
    let startColor5: string = '#ffff4444';
    let endColor1: string = '#ff0099cc';
    let endColor2: string = '#ffaa66cc';
    let endColor3: string = '#ff669900';
    let endColor4: string = '#ffcc0000';
    let endColor5: string = '#ffff8800';
    let gradientFills: JArrayList<Fill> = new JArrayList<Fill>();
    gradientFills.add(new Fill(startColor1, endColor1));
    gradientFills.add(new Fill(startColor2, endColor2));
    gradientFills.add(new Fill(startColor3, endColor3));
    gradientFills.add(new Fill(startColor4, endColor4));
    gradientFills.add(new Fill(startColor5, endColor5));

    dataSet.setFills(gradientFills);

    let dataSetList: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSetList.add(dataSet);

    let barData: BarData = new BarData(dataSetList);
    barData.setBarWidth(0.85);
    return barData;
  }

  private async getStackData(): Promise<BarData> {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, [38.0, 28.0, 39.8]));
    values.add(new BarEntry(2, [18.2, 16.1, 16.1]));
    values.add(new BarEntry(3, [45.8, 49.8, 26.7],this.pixelMap));
    values.add(new BarEntry(4, [36.5, 43.8, 15.1]));
    values.add(new BarEntry(5, [51.5, 35.4, 24.1],this.pixelMap));
    values.add(new BarEntry(6, [44.9, 21.4, 15.9],this.pixelMap));
    values.add(new BarEntry(7, [32.9, 40.4, 32.7]));
    values.add(new BarEntry(8, [24.3, 31.0, 47.1]));
    values.add(new BarEntry(9, [17.4, 48.4, 49.6],this.pixelMap));
    values.add(new BarEntry(10, [32.0, 29.8, 20.0],this.pixelMap));
    values.add(new BarEntry(11, [43.9, 52.7, 27.7],this.pixelMap));
    values.add(new BarEntry(12, [28.4, 46.2, 51.8]));
    values.add(new BarEntry(13, [42.6, 51.9, 44.0],this.pixelMap));
    values.add(new BarEntry(14, [14.8, 22.4, 31.2],this.pixelMap));
    values.add(new BarEntry(15, [50.3, 18.7, 18.3]));
    values.add(new BarEntry(16, [20.7, 22.9, 40.2],this.pixelMap));
    values.add(new BarEntry(17, [49.4, 38.3, 41.7],this.pixelMap));
    values.add(new BarEntry(18, [45.9, 40.7, 29.5]));
    values.add(new BarEntry(19, [37.1, 31.8, 42.9],this.pixelMap));

    let set1: BarDataSet | null = null;


    set1 = new BarDataSet(values, "Statistics Vienna 2014");
    set1.setDrawIcons(this.isDrawIcons);
    set1.setColorsByArr([Color.Red, Color.Green, Color.Pink]);
    set1.setStackLabels(["Births", "Divorces", "Marriages"]);

    let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSets.add(set1);

    let data: BarData = new BarData(dataSets);
    data.setValueTextColor(Color.White);
    return data;
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
              Button($r('app.string.refresh'))
                .onClick(() => {
                  if (this.model) {
                    this.model.notifyDataSetChanged();
                    this.model.invalidate();
                  }
                })
              Row() {
                Text($r('app.string.data_type'))
                Row() {
                  Text($r('app.string.normal'))
                  Radio({ value: 'normal', group: 'dataType' })
                    .checked(true)
                    .onChange(async (isChecked: boolean) => {
                      if (isChecked) {
                        this.data = await this.getNormalData();
                        if (this.model) {
                          if (this.normalMarker) {
                            this.model.setMarker(this.normalMarker);
                          }
                          this.model.setData(this.data);

                          if (this.isChangedMaxVisibleXCount) {
                            this.model.setVisibleXRangeMaximum(Number(this.mLastMaxVisibleXCount));
                            this.model.setVisibleXRangeMinimum(Number(this.mLastMaxVisibleXCount));
                            this.data.notifyDataChanged();
                            this.model.notifyDataSetChanged();
                            this.model.invalidate();
                          }
                        }
                      }
                    })
                }

                Row() {
                  Text($r('app.string.gradient'))
                  Radio({ value: 'gradient', group: 'dataType' })
                    .checked(false)
                    .onChange(async (isChecked: boolean) => {
                      if (isChecked) {
                        this.data =await this.getGradientData();
                        if (this.model) {
                          if (this.normalMarker) {
                            this.model.setMarker(this.normalMarker);
                          }
                          this.model.setData(this.data);

                          if (this.isChangedMaxVisibleXCount) {
                            this.model.setVisibleXRangeMaximum(Number(this.mLastMaxVisibleXCount));
                            this.model.setVisibleXRangeMinimum(Number(this.mLastMaxVisibleXCount));
                            this.data.notifyDataChanged();
                            this.model.notifyDataSetChanged();
                            this.model.invalidate();
                          }
                        }
                      }
                    })
                }

                Row() {
                  Text($r('app.string.stack'))
                  Radio({ value: 'stack', group: 'dataType' })
                    .checked(false)
                    .onChange(async(isChecked: boolean) => {
                      if (isChecked) {
                        this.data =await this.getStackData();
                        if (this.model) {
                          if (this.stackMarker) {
                            this.model.setMarker(this.stackMarker);
                          }
                          this.model.setData(this.data);

                          if (this.isChangedMaxVisibleXCount) {
                            this.model.setVisibleXRangeMaximum(Number(this.mLastMaxVisibleXCount));
                            this.model.setVisibleXRangeMinimum(Number(this.mLastMaxVisibleXCount));
                            this.data.notifyDataChanged();
                            this.model.notifyDataSetChanged();
                            this.model.invalidate();
                          }
                        }
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.data_type'))
                Row() {
                  Text('NoData: ')
                  Radio({ value: 'noData', group: 'dataType' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (isChecked) {
                        if (this.model) {
                          this.model.setData(null);
                          this.model.invalidate();
                        }
                      }
                    })
                }

                Row() {
                  Text('NoDataSets: ')
                  Radio({ value: 'gradient', group: 'dataType' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (isChecked) {
                        this.data = new BarData(new JArrayList<IBarDataSet>());
                        if (this.model) {
                          this.model.setData(this.data);
                        }
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.show_y_axis'))
                Row() {
                  Text($r('app.string.left_y_axis'))
                  Checkbox({ name: 'leftYAxis', group: 'yAxis' })
                    .select(true)
                    .onChange((isChecked: boolean) => {
                      if (this.leftAxis) {
                        this.leftAxis.setEnabled(isChecked);
                      }
                    })
                }

                Row() {
                  Text($r('app.string.right_y_axis'))
                  Checkbox({ name: 'rightYAxis', group: 'yAxis' })
                    .onChange((isChecked: boolean) => {
                      if (this.rightAxis) {
                        this.rightAxis.setEnabled(isChecked);
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.left_y_axis_position'))
                Row() {
                  Text($r('app.string.outside'))
                  Radio({ value: 'outSideChart', group: 'leftYAxisLabelPosition' })
                    .checked(true)
                    .onChange((isChecked: boolean) => {
                      if (this.leftAxis && isChecked) {
                        this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
                      }
                    })
                }

                Row() {
                  Text($r('app.string.inside'))
                  Radio({ value: 'inSideChart', group: 'leftYAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.leftAxis && isChecked) {
                        this.leftAxis.setPosition(YAxisLabelPosition.INSIDE_CHART);
                      }
                    })
                }.padding({ left: 20 })
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.right_y_axis_position'))
                Row() {
                  Text($r('app.string.outside'))
                  Radio({ value: 'outSideChart', group: 'rightYAxisLabelPosition' })
                    .checked(true)
                    .onChange((isChecked: boolean) => {
                      if (this.rightAxis && isChecked) {
                        this.rightAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
                      }
                    })
                }

                Row() {
                  Text($r('app.string.inside'))
                  Radio({ value: 'inSideChart', group: 'rightYAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.rightAxis && isChecked) {
                        this.rightAxis.setPosition(YAxisLabelPosition.INSIDE_CHART);
                      }
                    })
                }.padding({ left: 20 })
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.show_x_axis'))
                Row() {
                  Checkbox({ name: 'isShowXAxis', group: 'xAxis' })
                    .select(true)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis) {
                        this.xAxis.setEnabled(isChecked);
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)

              Text($r('app.string.x_axis_position'))
                .alignSelf(ItemAlign.Start)

              Row() {
                Column() {
                  Text('top')
                  Radio({ value: 'top', group: 'xAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis && isChecked) {
                        this.xAxis.setPosition(XAxisPosition.TOP);
                      }
                    })
                }.layoutWeight(1)

                Column() {
                  Text('bottom')
                  Radio({ value: 'bottom', group: 'xAxisLabelPosition' })
                    .checked(true)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis && isChecked) {
                        this.xAxis.setPosition(XAxisPosition.BOTTOM);
                      }
                    })
                }.layoutWeight(1)

                Column() {
                  Text('both side')
                  Radio({ value: 'both', group: 'xAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis && isChecked) {
                        this.xAxis.setPosition(XAxisPosition.BOTH_SIDED);
                      }
                    })
                }.layoutWeight(1)

                Column() {
                  Text('top')
                  Text('InSide')
                  Radio({ value: 'topInSide', group: 'xAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis && isChecked) {
                        this.xAxis.setPosition(XAxisPosition.TOP_INSIDE);
                      }
                    })
                }.layoutWeight(1)

                Column() {
                  Text('bottom')
                  Text('InSide')
                  Radio({ value: 'bottomInSide', group: 'xAxisLabelPosition' })
                    .checked(false)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis && isChecked) {
                        this.xAxis.setPosition(XAxisPosition.BOTTOM_INSIDE);
                      }
                    })
                }.layoutWeight(1)
              }.alignSelf(ItemAlign.Start)
              MpSlider({ name: $r('app.string.width_percentage'), min: 1, max: 100, value: this.widthPercentage!! })


              Text($r('app.string.background'))
              GridRow({
                columns: 6,
                gutter: { x: 5, y: 10 },
                breakpoints: {
                  value: ["100vp", "200vp", "300vp"],
                  reference: BreakpointsReference.WindowSize
                },
                direction: GridRowDirection.Row
              }) {
                ForEach(this.bgColors, (color: Color | string) => {
                  GridCol({ span: { xs: 4, sm: 3, md: 2, lg: 1 } }) {
                    this.colorButton(color)
                  }
                })
              }

              Row() {
                Text($r("app.string.show_mark_view"))
                Checkbox({ name: 'isShowMarkerView', group: 'isShowMarkerView' })
                  .select(true)
                  .onChange((isChecked: boolean) => {
                    if (this.model) {
                      this.model.setDrawMarkerViews(isChecked);
                    }
                  })
              }.alignSelf(ItemAlign.Start)

              MpSlider({ name: $r('app.string.visible_xrange_maximum'), min: 8, max: 20, value: this.maxLabelSize!! })

              Row() {
                Text($r('app.string.limit_line_after_data'))
                Checkbox({ name: 'isShowLimitLineBehindData', group: 'isShowLimitLineBehindData' })
                  .select(false)
                  .onChange((isChecked: boolean) => {
                    if (this.leftAxis) {
                      this.leftAxis.setDrawLimitLinesBehindData(isChecked);
                    }
                  })
              }.alignSelf(ItemAlign.Start)

                MpSelect({
                  name: $r('app.string.left_y_axis_limitLine_position'),
                  selectItem: this.limitLinePosition!!,
                  itemsList: this.limitLinePositionList,
                  valuesList: this.limitLinePositionValueList
                })
            }
          }.align(Alignment.TopEnd);
        }.layoutWeight(1)
        .margin({bottom: 50})
      }
    }
  }
}