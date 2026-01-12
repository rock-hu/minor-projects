/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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
  ILineDataSet,
  LineData,
  LineChart,
  LineChartModel,
  Mode,
  LineDataSet,
  XAxisPosition,
  OnChartValueSelectedListener,
  Highlight,
  AxisDependency,
} from '@ohos/mpchart';

/** CursorPage组件 */
@Entry
@ComponentV2
struct CursorPage {
  /** LineChart 图表配置构建类 */
  @Local private lineChartModel: LineChartModel = new LineChartModel();
  /** 选中点位置信息 */
  @Local positionX: number = 0;
  @Local positionY: number = 0;
  /** 选中点X轴数据信息*/
  @Local dataX: number = 0;
  /** 选中点Y轴数据信息 */
  @Local dataY: number = 0;
  /** 是否展示UI */
  @Local showUI: boolean = false;
  /** UI宽度 */
  @Local uiWidth: number = 0;
  /** UI高度 */
  @Local uiHeight: number = 0;
  /** 是否展示气泡 */
  @Local handlePopup: boolean = false;
  /** 数据选择监听 */
  private valueSelectedListener: OnChartValueSelectedListener = {
    onValueSelected: (e: EntryOhos, h: Highlight) => {
      /** 数据选中时显示气泡UI */
      this.showUI = true;
      /** 设置选中点的位置信息和数据信息 */
      this.dataX = Number(e.getX().toFixed(1));
      this.dataY = Number(e.getY().toFixed(1));
      this.positionX =
        this.lineChartModel.getTransformer(AxisDependency.LEFT)?.getPixelForValues(e.getX(), e.getY()).x ?? 0;
      this.positionY =
        this.lineChartModel.getTransformer(AxisDependency.LEFT)?.getPixelForValues(e.getX(), e.getY()).y ?? 0;
      this.handlePopup = true;
    },
    onNothingSelected: () => {
      /** 去除气泡UI */
      this.showUI = false;
      this.handlePopup = false;
    }
  }
  /** 数据初始化 */
  aboutToAppear() {
    /** 创建一个 JArrayList 对象，用于存储 EntryOhos 类型的数据 */
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    /** 循环生成 1 到 20 的随机数据，并添加到 values 中 */
    for (let i = 1; i <= 20; i++) {
      values.add(new EntryOhos(i, Math.random() * 100));
    }
    /** 创建 LineDataSet 对象，使用 values 数据，并设置数据集的名称为 'DataSet' */
    let lineDataSet = new LineDataSet(values, 'DataSet');
    lineDataSet.setMode(Mode.CUBIC_BEZIER);
    lineDataSet.setDrawCircles(false);
    let lineDataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    lineDataSetList.add(lineDataSet);
    /** 创建 LineData 对象，使用 lineDataSetList 数据，并将其传递给 lineChartModel */
    let lineData: LineData = new LineData(lineDataSetList);
    this.lineChartModel?.setData(lineData);
    this.lineChartModel.setOnChartValueSelectedListener(this.valueSelectedListener);
    this.lineChartModel.getAxisLeft()?.setAxisLineWidth(2);
    this.lineChartModel.getXAxis()?.setPosition(XAxisPosition.BOTTOM);
    this.lineChartModel.getAxisRight()?.setEnabled(false);
    this.lineChartModel.setLongPressCursorEnabled(false);
    this.lineChartModel.getDescription()?.setEnabled(false);
  }

  build() {
    Stack() {
      LineChart({ model: this.lineChartModel })
        .width('100%')
        .height('50%')
        .backgroundColor(Color.White)
      if (this.showUI) {
        Text()
          .width(1)
          .height(1)
          .position({ x: this.positionX, y: this.positionY })
          .bindPopup(this.handlePopup, {
            message: `x=${this.dataX}, y=${this.dataY}`,
            placementOnTop: true,
            showInSubWindow: false,
            onStateChange: (e) => {
              console.info(JSON.stringify(e.isVisible))
              if (!e.isVisible) {
                this.handlePopup = false
                this.lineChartModel.highlightValueForObject(null);
              }
            }
          })
      }
    }
  }
}