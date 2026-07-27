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
  ChartColorStop,
  ChartGesture,
  ChartPixelMap,
  EntryOhos,
  Highlight,
  IBarDataSet,
  ILineDataSet,
  JArrayList,
  LineChart,
  LineChartModel,
  LineData,
  LineDataSet,
  Mode,
  OnChartGestureListener,
  OnChartValueSelectedListener,
  XAxis,
  XAxisPosition,
  YAxis,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import Constants from '../../constants/Constants';
import Utils from '../../utils/Utils';
import { EventType } from '@ohos/mpchart/src/main/ets/components/listener/EventControl';
import { CustomUiInfo } from '@ohos/mpchart/src/main/ets/components/data/customUiData';
import { LogUtil } from '../../utils/LogUtil';

interface XAxisColorSetter {
  colorText: string
  colorVal: string
  color: number
}

interface uiTriggerType {
  text: string
  type: EventType
}

@Entry
@ComponentV2
struct CustomMarkerViewChartPage {
  @Local private model: BarChartModel | undefined = undefined;
  private leftAxis: YAxis | null = null;
  private rightAxis: YAxis | null = null;
  private xAxis: XAxis | null = null;
  private data: BarData | null = null;

  @Local private lineChartModel: LineChartModel | undefined = undefined;
  private dataSet: LineDataSet | null = null;
  private isDrawIcons: boolean= false;

  // 宽: 90 高: 30
  @Local customUiInfo: CustomUiInfo = new CustomUiInfo(90, 50);

  @Local lineChartCustomUiInfo: CustomUiInfo = new CustomUiInfo(90, 60);

  // 自定义ui事件触发类型
  @Local uiTriggerEvent: EventType = EventType.SingleTap;
  private pixelMap: ChartPixelMap = new ChartPixelMap()

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
  //标题栏标题
  private title: string = 'Event Control & Custom MarkerView'
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

    if (!this.lineChartModel || index == undefined || index == -1) {
      return
    }
    let lineData = this.lineChartModel.getLineData();
    if (!lineData) return
    let lineSets: JArrayList<ILineDataSet> | null = null;
    lineSets = lineData.getDataSets();

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
        }

        if (lineSets) {
          for (let i = 0; i < lineSets.size(); i++) {
            let set = lineSets.get(i) as LineDataSet;
            set.setDrawIcons(!this.isDrawIcons)
          }
        }
        this.isDrawIcons=!this.isDrawIcons
        this.lineChartModel.invalidate();
        this.model.invalidate();
        break;
      default:

    }
    this.titleModel.setIndex(-1)
  }

  getResourceString(res:Resource){
    return getContext().resourceManager.getStringSync(res.id)
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
      this.eventText = 'LongPressed!';
      LogUtil.log("-----------------chartGestureListener onChartLongPressed");
    },
    onChartDoubleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      this.eventText = 'DoubleTapped!';
      LogUtil.log("-----------------chartGestureListener onChartDoubleTapped");
    },
    onChartSingleTapped: (isTouchEvent: boolean, me: TouchEvent | GestureEvent) => {
      this.eventText = 'SingleTapped!';
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

  async  aboutToAppear() {

    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new BarChartModel();


    this.model.setOnChartValueSelectedListener(this.valueSelectedListener);
    this.model.setOnChartGestureListener(this.chartGestureListener);

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

    // line chart 设置
    this.lineChartModel = new LineChartModel();
    this.lineChartModel.setOnChartGestureListener(this.chartGestureListener);

    this.pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)

    this.data = this.getNormalData();
    this.model.setData(this.data);
    this.model.setVisibleXRangeMaximum(20);


    let lineData: LineData = this.getLineData()
    lineData.setValueTextSize(10);
    this.lineChartModel.setData(lineData);
    this.lineChartModel.setDragDecelerationEnabled(true);
  }

  private  getLineData(): LineData {
    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 141);

      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val,this.pixelMap));
      } else {
        values.add(new EntryOhos(i, val, this.pixelMap));
      }
    }

    this.dataSet = new LineDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Black);
    this.dataSet.setDrawIcons(this.isDrawIcons);

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

    let lineData: LineData = new LineData(dataSetList);
    return lineData
  }

  private  getNormalData(): BarData {
    let values: JArrayList<BarEntry> = new JArrayList<BarEntry>();
    values.add(new BarEntry(1, 73.3));
    values.add(new BarEntry(2, 5.4));
    values.add(new BarEntry(3, 73.9));
    values.add(new BarEntry(4, 79.9,this.pixelMap));
    values.add(new BarEntry(5, 69.3,this.pixelMap));
    values.add(new BarEntry(6, 70.7,this.pixelMap));
    values.add(new BarEntry(7, 81.2,this.pixelMap));
    values.add(new BarEntry(8, 13.1,this.pixelMap));
    values.add(new BarEntry(9, 34.2,this.pixelMap));
    values.add(new BarEntry(10, 58.4));
    values.add(new BarEntry(11, 44.7));
    values.add(new BarEntry(12, 10.5,this.pixelMap));
    values.add(new BarEntry(13, 15.6,this.pixelMap));
    values.add(new BarEntry(14, 95.8,this.pixelMap));
    values.add(new BarEntry(15, 57.4,this.pixelMap));
    values.add(new BarEntry(16, 64.5));
    values.add(new BarEntry(17, 21.4,this.pixelMap));
    values.add(new BarEntry(18, 33.2,this.pixelMap));
    values.add(new BarEntry(19, 96.9));

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

  private setEventStatusByType(evType: EventType, isChecked: Boolean) {
    if (this.model && this.lineChartModel) {
      if (isChecked) {
        this.model.eventControl.setEventEnable(evType)
        this.lineChartModel.eventControl.setEventEnable(evType)
      } else {
        this.model.eventControl.setEventDisable(evType);
        this.lineChartModel.eventControl.setEventDisable(evType);
      }
    }
  }


  @Builder eventSettings() {
    Row() {
      Text(`${this.getResourceString($r('app.string.custom_click'))}${this.eventText}`)
    }
    Row() {
      Text($r('app.string.enable_all_events'))
      Checkbox({ name: 'disable' })
        .select(this.model?.getTouchEnabled())
        .onChange((isChecked: boolean) => {
          this.model?.setTouchEnabled(isChecked);
          this.lineChartModel?.setTouchEnabled(isChecked);
        })
    }
    Column() {
      Text($r('app.string.event_activated'))
      Row() {
        Text($r('app.string.single_tap'))
        Checkbox({ name: 'leftYAxis', group: 'yAxis' })
          .select(this.model?.eventControl.eventIsEnable(EventType.SingleTap))
          .onChange((isChecked: boolean) => {
            this.setEventStatusByType(EventType.SingleTap, isChecked)
          })
      }

      Row() {
        Text($r('app.string.double_tap'))
        Checkbox({ name: 'rightYAxis', group: 'yAxis' })
          .select(this.model?.eventControl.eventIsEnable(EventType.DoubleTap))
          .onChange((isChecked: boolean) => {
            this.setEventStatusByType(EventType.DoubleTap, isChecked)
          })
      }

      Row() {
        Text($r("app.string.long_press"))
        Checkbox({ name: 'rightYAxis', group: 'yAxis' })
          .select(this.model?.eventControl.eventIsEnable(EventType.LongPress))
          .onChange((isChecked: boolean) => {
            this.setEventStatusByType(EventType.LongPress, isChecked)
          })
      }
    }.alignSelf(ItemAlign.Start)

    Column() {
      Text($r('app.string.event_trigger'))
      ForEach([
        { text: $r('app.string.single_tap'), type: EventType.SingleTap },
        { text: $r('app.string.double_tap'), type: EventType.DoubleTap },
        { text: $r('app.string.long_press'), type: EventType.LongPress },
      ], (item: uiTriggerType) => {
        Row() {
          Text(item.text)
          Radio({ value: String(item.type), group: 'eV'})
            .checked(this.uiTriggerEvent === item.type)
            .onChange((isChecked: boolean) => {
              if (isChecked) {
                this.uiTriggerEvent = item.type;
              }
            })
        }
      })
    }.alignSelf(ItemAlign.Start)
  }

  @Builder customUi() {
    // 是否在图表content内
    if (this.customUiInfo.isInbounds && this.customUiInfo.data) {
      Column() {
        Text(`2023-12-15`).fontColor(Color.Gray).fontSize(12).fontWeight(FontWeight.Bold)
        Text(`X: ${this.customUiInfo.data.getX()}`).fontColor(Color.Gray).fontSize(12)
        Text(`Y: ${this.customUiInfo.data.getY()}`).fontColor(Color.Gray).fontSize(12)
      }
      .padding(4)
      .borderRadius(6)
      .border({ width: 1, color: Color.Orange})
      .backgroundColor(0xf0f0f0)
      .width(this.customUiInfo.width)
      .height(this.customUiInfo.height)
      .margin({ left: this.customUiInfo.x, top: this.customUiInfo.y })
      .alignItems(HorizontalAlign.Start)
      .onClick(ev => {
        this.customUiInfo.showUi = false;
      })
    }
  }

  @Builder lineChartCustomUi() {
    // 是否在图表content内
    if (this.customUiInfo.isInbounds && this.customUiInfo?.data) {
      Column() {
        Text(`2023-12-15`).fontColor(Color.Gray).fontSize(12).fontWeight(FontWeight.Bold)
        Text(`X: ${this.customUiInfo.data.getX()}`).fontColor(Color.Gray).fontSize(12)
        Text(`Y: ${Math.round(this.customUiInfo.data.getY())}`).fontColor(Color.Gray).fontSize(12)
      }
      .padding(4)
      .borderRadius(8)
      .border({ width: 1, color: Color.Blue})
      .backgroundColor(0xf0f0f0)
      .width(this.customUiInfo.width)
      .height(this.customUiInfo.height)
      .margin({ left: this.customUiInfo.x, top: this.customUiInfo.y })
      .alignItems(HorizontalAlign.Start)
      .onClick(ev => {
        this.customUiInfo.showUi = false;
      })
    }
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        BarChart({ model: this.model,
          // 自定义 ui: 传入 builder
          customUiBuilder: this.customUi,
          // 通过什么事件触发
          customUiTriggerEvent: this.uiTriggerEvent,
          // 自定义ui的位置信息
          customUiInfo: this.customUiInfo,
        })
          .width('100%')
          .height('30%')

        Divider()
          .width('1px')
          .padding({ top: 5 })

        LineChart({ model: this.lineChartModel,
          // 自定义 ui: 传入 builder
          customUiBuilder: this.lineChartCustomUi,
          // 通过什么事件触发
          customUiTriggerEvent: this.uiTriggerEvent,
          // 自定义ui的位置信息
          customUiInfo: this.lineChartCustomUiInfo,
        })
          .width('100%')
          .height('30%')
          .backgroundColor(Color.White)

        Divider()
          .width('1px')
          .padding({ top: 5 })

        Scroll() {
          Stack() {
            Column() {
              this.eventSettings()
            }
          }.align(Alignment.TopEnd)
        }.layoutWeight(1)
        .margin({bottom: 50})
      }
    }
  }
}