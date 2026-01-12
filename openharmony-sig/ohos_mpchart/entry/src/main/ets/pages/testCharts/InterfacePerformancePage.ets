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
  EntryOhos,
  LineDataSet,
  ILineDataSet,
  LineData,
  Mode,
  LineChart,
  LineChartModel,
  YAxis,
  XAxis,
} from '@ohos/mpchart/index';
import { EventControl, EventType } from '@ohos/mpchart/src/main/ets/components/listener/EventControl';
import title, { ChartTitleModel } from '../../title';
import { GridLineConfig } from '@ohos/mpchart/src/main/ets/components/interfaces/LineConfig/CustomGridLineConfig';
import calcFunctionExecTime from '../../utils/calcFunctionExecTime';


@Entry
@ComponentV2
struct performanceTestPage {
  //标题栏标题
  private title: string = 'Performance Test'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: LineChartModel | undefined = undefined;
  private dataSet: LineDataSet | null = null;
  titleSelcetString: string = 'X'


  aboutToAppear() {
    // this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new LineChartModel();

    this.model.setMaxVisibleValueCount(60);

    let lineData: LineData = this.getLineData()
    lineData.setValueTextSize(10);
    this.model.setData(lineData);
    this.model.setVisibleXRangeMaximum(7);
    this.model.moveViewToX(10);
    this.model.invalidate();
  }

  private getLineData(): LineData {

    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 141);

      if (Math.random() * 100 < 25) {
        values.add(new EntryOhos(i, val));
      } else {
        values.add(new EntryOhos(i, val));
      }
    }

    this.dataSet = new LineDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Black);
    this.dataSet.setDrawIcons(false);

    this.dataSet.setMode(Mode.LINEAR);
    this.dataSet.setDrawCircles(true);
    this.dataSet.setDrawCircleHole(false);
    this.dataSet.setColorByColor(Color.Black);

    // 设置数据点的颜色
    this.dataSet.setCircleColor(Color.Blue);

    // 设置数据点的半径
    this.dataSet.setCircleRadius(4);
    this.dataSet.setCircleHoleRadius(2);

    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(this.dataSet);

    let lineData: LineData = new LineData(dataSetList);
    return lineData
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
          Flex({direction: FlexDirection.Column, justifyContent: FlexAlign.Start}) {
            if (this.model) {
              ForEach([
                { thisArg: this.model.eventControl, funcName: 'setEventEnable', params: [EventType.SingleTap] },
                { thisArg: this.model.eventControl, funcName: 'setEventDisable', params: [EventType.SingleTap] },
                { thisArg: this.model.eventControl, funcName: 'eventIsEnable', params: [EventType.SingleTap] },
                { thisArg: this.model.eventControl, funcName: 'eventIsDisable', params: [EventType.SingleTap] },
              ], (item: ESObject) => {
                Button('eventControl.' + item.funcName).width('90%').margin({ top: 10})
                  .onClick(() => {
                    calcFunctionExecTime<EventControl, EventType>(
                      item.thisArg,
                      item.funcName,
                      item.params,
                    )
                  })
              })
              Button('model.setYAxisExtensionLine').width('90%').margin({ top: 10})
                .onClick(() => {
                  if (this.model) {
                    calcFunctionExecTime<LineChartModel, boolean>(
                      this.model,
                      'setYAxisExtensionLine',
                      [true],
                    )
                  }
                })
              Button('model.yAxisExtensionLineIsEnable').width('90%').margin({ top: 10})
                .onClick(() => {
                  if (this.model) {
                    calcFunctionExecTime<LineChartModel, undefined>(
                      this.model,
                      'yAxisExtensionLineIsEnable',
                      [],
                    )
                  }
                })
              Button(`xAxis.addGridLine`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, GridLineConfig>(
                      xAxis,
                      'addGridLine',
                      [{ lineWidth: 1, lineColor: Color.Blue, position: 7}],
                      {
                        endCallBack: () => {
                          xAxis.getGridLines().clear();
                        }
                      }
                    )
                  }
                })
              Button(`xAxis.getGridLines`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, GridLineConfig>(
                      xAxis,
                      'getGridLines',
                      [],
                    )
                  }
                })
              Button(`xAxis.setTextColor`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, number>(
                      xAxis,
                      'setTextColor',
                      [Color.Black],
                    )
                  }
                })
              Button(`xAxis.setTextSize`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, number>(
                      xAxis,
                      'setTextSize',
                      [12],
                    )
                  }
                })
              Button(`xAxis.setAxisLineColor`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, number>(
                      xAxis,
                      'setAxisLineColor',
                      [Color.Blue],
                    )
                  }
                })
              Button(`xAxis.setAxisLineWidth`).width('90%').margin({ top: 10})
                .onClick(() => {
                  const xAxis = this.model?.getXAxis();
                  if (xAxis) {
                    calcFunctionExecTime<XAxis, number>(
                      xAxis,
                      'setAxisLineWidth',
                      [1],
                    )
                  }
                })
            }
          }.margin(10)
        }.height('60%')
      }

    }
  }
}