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
  AxisBase,
  AxisDependency,
  ChartColorStop,
  ColorTemplate,
  DefaultAxisValueFormatter,
  Description,
  EntryOhos,
  Highlight,
  IAxisValueFormatter,
  ILineDataSet,
  JArrayList,
  Legend,
  LegendForm,
  LimitLabelPosition,
  LimitLine,
  LineChart,
  LineChartModel,
  LineData,
  LineDataSet,
  Mode,
  MPPointF,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
  YAxisLabelPosition
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import LongPressMarkerView from '../../customcomponents/LongPressMarkerView';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';
import { LogUtil } from '../../utils/LogUtil';
import MpSlider from '../../customView/MpSlider';
import MpSelect, { ValueObject } from '../../customView/MpSelect';

class customFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    return value % 2 === 0 ? value.toString().repeat(5) : value.toString();
  }
}

class TibetanFormatter implements IAxisValueFormatter {
  getFormattedValue(value: number, axis: AxisBase): string {
    return 'བོད་སྐད།';
  }
}

@Entry
@ComponentV2
struct LineChartPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_VALUES, Constants.TOGGLE_DRAW_ICON, Constants.TOGGLE_FILLED, Constants.TOGGLE_CIRCLES,
  Constants.TOGGLE_CUBIC, Constants.TOGGLE_STEPPED, Constants.TOGGLE_HORIZONTAL_CUBIC, Constants.TOGGLE_PINCHZOOM,
  Constants.TOGGLE_AUTO_SCALE, Constants.TOGGLE_HIGHLIGHT, Constants.ANIMATE_X, Constants.ANIMATE_Y, Constants.ANIMATE_XY, Constants.SAVE_IMAGE];
  //标题栏标题
  private title: string = 'LineChartPage 1'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: LineChartModel = new LineChartModel();
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  @Local xAxisVisible: boolean = true;
  @Local leftAxisVisible: boolean = true;
  @Local rightAxisVisible: boolean = false;
  private limitLine1: LimitLine | null = null;
  private limitLine2: LimitLine | null = null;
  private dataSet: LineDataSet | null = null;
  private lineData: LineData | null = null;
  private isDrawIcons: boolean = false;
  @Local gridLineAlpha: number = 90;
  @Local maxLabelSize: number = 7;
  @Local mode: number = Mode.LINEAR;
  @Local xPosition: number = XAxisPosition.TOP;
  @Local limitLinePosition: number = 0;
  @Local bgColors: Array<Color | string> = ['#C7EDCC', '#FDE6E0', '#FAF9DE', '#DCE2F1', '#E9EBFE', Color.White]
  @Local modeList: number[] = [
    Mode.LINEAR,
    Mode.STEPPED,
    Mode.CUBIC_BEZIER,
    Mode.HORIZONTAL_BEZIER
  ]
  @Local modeValueList: Array<ValueObject> = [
    { value: $r('app.string.linear') },
    { value: $r('app.string.stepped') },
    { value: $r('app.string.cubic') },
    { value: $r('app.string.horizon_cubic') },
  ]
  @Local xPositionList: number[] = [
    XAxisPosition.TOP,
    XAxisPosition.BOTTOM,
    XAxisPosition.BOTH_SIDED,
    XAxisPosition.TOP_INSIDE,
    XAxisPosition.BOTTOM_INSIDE
  ]
  @Local xPositionValueList: Array<ValueObject> = [
    { value: 'TOP' },
    { value: 'BOTTOM' },
    { value: 'BOTH_SIDED' },
    { value: 'TOP_INSIDE' },
    { value: 'BOTTOM_INSIDE' }
  ]
  @Local limitLinePositionList: number[] = [
    0, 1, 2, 3
  ]
  @Local limitLinePositionValueList: Array<ValueObject> = [
    { value: 'none' },
    { value: 'above' },
    { value: 'below' },
    { value: 'both' }
  ]
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      LogUtil.log("LineChartPage onValueSelected: " + e.getX());
    },
    onNothingSelected: () => {
      LogUtil.log("LineChartPage onNothingSelected");
    }
  }

  @Monitor("gridLineAlpha")
  setGridLineAlpha() {
    this.model?.getAxisLeft()?.setGridAlpha(this.gridLineAlpha);
    this.model?.invalidate();
  }

  @Monitor("mode")
  changeMode() {
    if (this.dataSet && this.model) {
      this.dataSet.setMode(this.mode);
      this.model.invalidate();
    }
  }

  @Monitor("xPosition")
  changeXPosition() {
    this.xAxis?.setPosition(this.xPosition);
    this.model.notifyDataSetChanged();
    this.model.invalidate();
  }

  @Monitor("limitLinePosition")
  changeLimitLine() {
    this.leftAxis?.removeAllLimitLines();

    const addLimitLineToAxis = (axis: YAxis | null, limitLine: LimitLine | null) => {
      if (limitLine && axis) {
        axis.addLimitLine(limitLine);
      }
    }

    const limitLineArr: Function[] = [
      () => {},
      () => {
        addLimitLineToAxis(this.leftAxis, this.limitLine1);
      },
      () => {
        addLimitLineToAxis(this.leftAxis, this.limitLine2);
      },
      () => {
        addLimitLineToAxis(this.leftAxis, this.limitLine1);
        addLimitLineToAxis(this.leftAxis, this.limitLine2);
      }
    ]

    const action: Function = limitLineArr[this.limitLinePosition]
    if (action) {
      action();
    }

    this.model.invalidate();
  }

  @Monitor("maxLabelSize")
  setMaxLabelSize() {
    this.model.setVisibleXRangeMaximum(this.maxLabelSize);
    this.model.setVisibleXRangeMinimum(this.maxLabelSize);
    this.model.notifyDataSetChanged();
    this.model.invalidate();
  }

  @Builder
  colorButton(color: string | number) {
    Button().backgroundColor(color).borderWidth(1).aspectRatio(1).onClick(() => {
      this.model.setGridBackgroundColor(color);
      this.model.setDrawGridBackground(true);
      this.model.invalidate();
    })
  }

  toggleValues(data: LineData | null, sets: JArrayList<ILineDataSet> | null) {
    if (!data || !sets) {
      return;
    }
    for (let i = 0; i < sets.size(); i++) {
      let set = sets.get(i) as LineDataSet;
      set.setDrawValues(!set.isDrawValuesEnabled());
    }
    this.model.invalidate();
  }

  togglePinchZoom() {
    this.model.setPinchZoom(!this.model.isPinchZoomEnabled())
    this.model.invalidate();
  }

  toggleAutoScale() {
    this.model.setAutoScaleMinMaxEnabled(!this.model.isAutoScaleMinMaxEnabled());
    this.model.notifyDataSetChanged();
  }

  toggleHighlight(data: LineData | null) {
    if (data !== null) {
      data.setHighlightEnabled(!data.isHighlightEnabled());
      this.model.invalidate();
    }
  }

  toggleFilled(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawFilled(!set.isDrawFilledEnabled())
      }
      this.model.invalidate();
    }
  }

  toggleCircles(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawCircles(!set.isDrawCirclesEnabled());
      }
      this.model.invalidate();
    }
  }

  toggleCubic(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.CUBIC_BEZIER ? Mode.LINEAR : Mode.CUBIC_BEZIER);
      }
      this.model.invalidate();
    }
  }

  toggleStepped(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.STEPPED ? Mode.LINEAR : Mode.STEPPED);
      }
      this.model.invalidate();
    }
  }

  toggleHorizontalCubic(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setMode(set.getMode() === Mode.HORIZONTAL_BEZIER ? Mode.LINEAR : Mode.HORIZONTAL_BEZIER);
      }
      this.model.invalidate();
    }
  }

  toggleDrawIcon(sets: JArrayList<ILineDataSet> | null) {
    if (sets) {
      for (let i = 0; i < sets.size(); i++) {
        let set = sets.get(i) as LineDataSet;
        set.setDrawIcons(!this.isDrawIcons)
      }
      this.isDrawIcons = !this.isDrawIcons
      this.model.invalidate();
    }
  }

  saveImage() {
    Utils.saveImage(this.title, this.model ? this.model.context2d : null);
  }


  //标题栏菜单回调
  @Monitor("titleModel.index")
  menuCallback() {

    if (this.titleModel === null || this.titleModel === undefined) {
      return
    }
    if (this.model === null || this.model === undefined) {
      return
    }
    let index: number = this.titleModel.getIndex()
    if (index === undefined || index === null || index === -1) {
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
        this.model.animateX(2000);
        break;
      case Constants.ANIMATE_Y:
        this.model.animateY(2000);
        break;
      case Constants.ANIMATE_XY:
        this.model.animateXY(2000, 2000);
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

  async aboutToAppear() {
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);

    let description: Description | null = this.model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    this.model.setMaxVisibleValueCount(60);
    this.model.setPinchZoom(false);
    this.model.setDrawGridBackground(false);

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

    this.xAxis = this.model.getXAxis();
    if (this.xAxis) {
      this.xAxis.setPosition(this.xPosition);
      this.xAxis.setDrawGridLines(false);
      this.xAxis.setGranularity(1);
      this.xAxis.setLabelCount(7);
      this.xAxis.setValueFormatter(new DefaultAxisValueFormatter(1));
    }

    this.leftAxis = this.model.getAxisLeft();
    if (this.leftAxis) {
      this.leftAxis.setLabelCount(8, false);
      this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      this.leftAxis.setSpaceTop(15);
      this.leftAxis.setAxisMinimum(0);
      this.leftAxis.setAxisMaximum(200);
    }


    this.rightAxis = this.model.getAxisRight();
    if (this.rightAxis) {
      this.rightAxis.setLabelCount(8, false);
      this.rightAxis.setDrawGridLines(false);
      this.rightAxis.setSpaceTop(15);
      this.rightAxis.setAxisMinimum(0);
      this.rightAxis.setAxisMaximum(200);
      this.rightAxis.setEnabled(false);
    }

    let legend: Legend | null = this.model.getLegend();
    if (legend) {
      legend.setEnabled(true);
      // draw legend entries as lines
      legend.setForm(LegendForm.LINE);
    }

    let lineData: LineData = await this.getLineData()
    lineData.setValueTextSize(10);
    this.model.setLongPressMarker(new LongPressMarkerView())
    this.model.setData(lineData);
    this.model.setDragDecelerationEnabled(true);
    this.model.setVisibleXRangeMaximum(7);
    this.model.invalidate()
  }

  private async getLineData(): Promise<LineData> {
    if (this.lineData) {
      return this.lineData;
    }
    let start: number = -10;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    let pixelMap = await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)

    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 141);
      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val, pixelMap));
      } else {
        values.add(new EntryOhos(i, val, pixelMap));
      }
    }
    this.dataSet = new LineDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Black);
    this.dataSet.setDrawIcons(this.isDrawIcons);
    this.dataSet.setIconsOffset(new MPPointF(0, 0));

    this.dataSet.setMode(Mode.LINEAR);
    this.dataSet.setDrawCircles(true); //折线点画圆圈
    this.dataSet.setDrawCircleHole(false); //设置内部孔
    this.dataSet.setColorByColor(Color.Black); //设置折线颜色

    let gradientFillColor = new JArrayList<ChartColorStop>();
    gradientFillColor.add(["#0C0099CC", 0.2]);
    gradientFillColor.add(["#7F0099CC", 0.4]);
    gradientFillColor.add(["#0099CC", 1.0]);
    this.dataSet.setGradientFillColor(gradientFillColor);
    this.dataSet.setDrawFilled(true);


    // 设置数据点的颜色
    this.dataSet.setCircleColor(Color.Blue); // 可以设置为你想要的颜色

    // 设置数据点的半径
    this.dataSet.setCircleRadius(4); // 设置半径大小
    this.dataSet.setCircleHoleRadius(2); //设置内径

    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(this.dataSet);

    this.lineData = new LineData(dataSetList);
    return this.lineData;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        LineChart({ model: this.model })
          .width('100%')
          .height('30%')
          .backgroundColor(Color.White)

        Divider()
          .width('1px')
          .padding({ top: 5 })

        Scroll() {
          Stack() {
            Column() {
              MpSlider({
                name: 'GridLine Alpha:',
                min: 0,
                max: 255,
                value: this.gridLineAlpha!!
              })
              MpSelect({
                name: $r('app.string.data_type'),
                selectItem: this.mode!!,
                itemsList: this.modeList,
                valuesList: this.modeValueList
              })

              Row() {
                Text($r('app.string.multi_color'))
                Row() {
                  Checkbox()
                    .select(false)
                    .onChange((isChecked: boolean) => {
                      if(isChecked){
                        this.dataSet?.setColorsByArr([Color.Red, Color.Green, Color.Blue])
                        this.model?.invalidate();
                      }else{
                        this.dataSet?.setColorByColor(Color.Black);
                        this.model?.invalidate();
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)


              Row() {
                Text($r('app.string.show_x_axis'))
                  Checkbox({ name: 'isShowXAxis', group: 'xAxis' })
                    .select(true)
                    .onChange((isChecked: boolean) => {
                      if (this.xAxis) {
                        this.xAxisVisible = isChecked;
                        this.xAxis.setEnabled(isChecked);
                        this.model.invalidate();
                      }
                    })
              }.alignSelf(ItemAlign.Start)

              Row() {
                Text($r('app.string.multi_color_data_points'))
                Checkbox()
                  .onChange((isChecked: boolean) => {
                    if (isChecked) {
                      this.dataSet?.setCircleColors(ColorTemplate.COLORFUL_COLORS)
                      this.model.invalidate();
                    } else {
                      this.dataSet?.setCircleColor(Color.Blue);
                      this.model.invalidate();
                    }
                  })
              }.alignSelf(ItemAlign.Start)

              if (this.xAxisVisible) {
                MpSelect({
                  name: $r('app.string.x_axis_position'),
                  selectItem: this.xPosition!!,
                  itemsList: this.xPositionList,
                  valuesList: this.xPositionValueList
                })

                Text($r('app.string.x_axis_label_style'))
                  .alignSelf(ItemAlign.Start)

                Row() {
                  Column() {
                    Text($r('app.string.normal'))
                    Radio({ value: 'top', group: 'xAxisLabelStyle' })
                      .checked(true)
                      .onChange((isChecked: boolean) => {
                        if (this.xAxis && isChecked) {
                          this.xAxis.setLabelRotationAngle(0);
                          this.xAxis.setValueFormatter(new DefaultAxisValueFormatter(1));
                          this.model?.notifyDataSetChanged();
                          this.model?.invalidate();
                        }
                      })
                  }.layoutWeight(1)

                  Column() {
                    Text($r('app.string.extend_and_rotate'))
                    Radio({ value: 'bottom', group: 'xAxisLabelStyle' })
                      .checked(false)
                      .onChange((isChecked: boolean) => {
                        if (this.xAxis && isChecked) {
                          this.xAxis.setLabelRotationAngle(30);
                          this.xAxis.setValueFormatter(new customFormatter());
                          this.model?.notifyDataSetChanged();
                          this.model?.invalidate();
                        }
                      })
                  }.layoutWeight(1)
                  Column() {
                    Text($r('app.string.Tibetan'))
                    Radio({ value: 'bottom', group: 'xAxisLabelStyle' })
                      .checked(false)
                      .onChange((isChecked: boolean) => {
                        if (this.xAxis && isChecked) {
                          this.xAxis.setValueFormatter(new TibetanFormatter());
                          this.model?.notifyDataSetChanged();
                          this.model?.invalidate();
                        }
                      })
                  }.layoutWeight(1)
                }.alignSelf(ItemAlign.Start)
              }

              MpSlider({ name: $r('app.string.visible_xrange_maximum'), min: 3, max: 10, value: this.maxLabelSize!! })

              Button('moveViewTo(6,100)')
                .onClick(()=>{
                  this.model.moveViewTo(6, 100, AxisDependency.RIGHT);
                }).margin(10)
              Button('centerViewTo(6,100)')
                .onClick(()=>{
                  this.model.centerViewTo(6, 100, AxisDependency.RIGHT);
                })
              Button('centerViewToY 100')
                .onClick(()=>{
                  this.model.centerViewToY(100, AxisDependency.RIGHT);
                }).margin(10)

              Row() {
                Text($r('app.string.show_y_axis'))
                Row() {
                  Text($r('app.string.left_y_axis'))
                  Checkbox({ name: 'leftYAxis', group: 'yAxis' })
                    .select(true)
                    .onChange((isChecked: boolean) => {
                      if (this.leftAxis) {
                        this.leftAxisVisible = isChecked;
                        this.leftAxis.setEnabled(isChecked);
                        this.model.notifyDataSetChanged();
                        this.model.invalidate();
                      }
                    })
                }

                Row() {
                  Text($r('app.string.right_y_axis'))
                  Checkbox({ name: 'rightYAxis', group: 'yAxis' })
                    .onChange((isChecked: boolean) => {
                      if (this.rightAxis) {
                        this.rightAxisVisible = isChecked;
                        this.rightAxis.setEnabled(isChecked);
                        this.model.notifyDataSetChanged();
                        this.model.invalidate();
                      }
                    })
                }
              }.alignSelf(ItemAlign.Start)

              if (this.leftAxisVisible) {
                Row() {
                  Text($r('app.string.left_y_axis_position'))
                  Row() {
                    Text($r('app.string.outside'))
                    Radio({ value: 'outSideChart', group: 'leftYAxisLabelPosition' })
                      .checked(true)
                      .onChange((isChecked: boolean) => {
                        if (this.leftAxis && isChecked) {
                          this.leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
                          this.model.notifyDataSetChanged();
                          this.model.invalidate();
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
                          this.model.notifyDataSetChanged();
                          this.model.invalidate();
                        }
                      })
                  }.padding({ left: 20 })
                }.alignSelf(ItemAlign.Start)
              }

              if (this.rightAxisVisible) {
                Row() {
                  Text($r('app.string.right_y_axis_position'))
                  Row() {
                    Text($r('app.string.outside'))
                    Radio({ value: 'outSideChart', group: 'rightYAxisLabelPosition' })
                      .checked(true)
                      .onChange((isChecked: boolean) => {
                        if (this.rightAxis && isChecked) {
                          this.rightAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
                          this.model.notifyDataSetChanged();
                          this.model.invalidate();
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
                          this.model.notifyDataSetChanged();
                          this.model.invalidate();
                        }
                      })
                  }.padding({ left: 20 })
                }.alignSelf(ItemAlign.Start)
              }

              if (this.leftAxisVisible) {
                MpSelect({
                  name: $r('app.string.left_y_axis_limitLine_position'),
                  selectItem: this.limitLinePosition!!,
                  itemsList: this.limitLinePositionList,
                  valuesList: this.limitLinePositionValueList
                })
              }

              if(this.limitLinePosition != 0) {
                Row() {
                  Text('app.string.limit_line_after_data')
                  Checkbox({ name: 'isShowLimitLineBehindData', group: 'isShowLimitLineBehindData' })
                    .select(false)
                    .onChange((isChecked: boolean) => {
                      if (this.leftAxis) {
                        this.leftAxis.setDrawLimitLinesBehindData(isChecked);
                        this.model.invalidate();
                      }
                    })
                }.alignSelf(ItemAlign.Start)
              }

              Row() {
                Text($r('app.string.highlight_dashed_line'))
                Checkbox({ name: 'enableDashedHighlightLine'})
                  .select(false)
                  .onChange((isChecked: boolean) => {
                    if (isChecked) {
                      this.dataSet?.enableDashedHighlightLine(10, 10, 0)
                    } else {
                      this.dataSet?.disableDashedHighlightLine();
                    }
                    this.model.invalidateHighlight();
                  })
              }.alignSelf(ItemAlign.Start)

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
            }.padding({ left: 20, right: 20, bottom: 20 })
          }.align(Alignment.TopEnd);

        }.layoutWeight(1)
        .margin({ bottom: 50 })
      }
    }
  }
}