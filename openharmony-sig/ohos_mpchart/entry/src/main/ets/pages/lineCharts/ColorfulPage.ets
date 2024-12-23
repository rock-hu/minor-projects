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
  Mode,
  LineChart,
  LineChartModel,
  OnChartGestureListener,
  ChartGesture,
  ColorTemplate,
  AxisDependency,
  ChartPixelMap
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import Utils from '../../utils/Utils';


@Entry
@ComponentV2
struct ColorfulPage {
  //标题栏标题
  private title: string = 'LineChartPageColored'
  titleModel: ChartTitleModel = new ChartTitleModel()
  @Local lineChartModel1: LineChartModel = new LineChartModel();
  @Local lineChartModel2: LineChartModel = new LineChartModel();
  @Local lineChartModel3: LineChartModel = new LineChartModel();
  @Local lineChartModel4: LineChartModel = new LineChartModel();

  aboutToAppear() {
    this.titleModel.title = this.title

    this.initialLineChartModel(this.lineChartModel1);

    this.initialLineChartModel(this.lineChartModel2);

    this.initialLineChartModel(this.lineChartModel3);

    this.initialLineChartModel(this.lineChartModel4);

  }

  // 初始化折线图数据模型
  private async initialLineChartModel(model: LineChartModel) {
    let description: Description | null = model.getDescription();
    if (description) {
      description.setEnabled(false);
    }

    model.setMaxVisibleValueCount(50);
    model.setPinchZoom(false);
    model.setDrawGridBackground(false);

    let xAxis: XAxis | null = model.getXAxis();
    if (xAxis) {
      xAxis.setEnabled(false)
      xAxis.setXOffset(0);
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(false);
      xAxis.setGranularity(1);
      xAxis.setLabelCount(7);
    }

    let leftAxis: YAxis | null = model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setEnabled(false);
      leftAxis.setLabelCount(8, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(0);
    }


    let rightAxis: YAxis | null = model.getAxisRight();
    if (rightAxis) {
      rightAxis.setEnabled(false);

      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      rightAxis.setSpaceTop(15);
      rightAxis.setAxisMinimum(0);
    }

    let legend: Legend | null = model.getLegend();
    if (legend) {
      legend.setEnabled(false);
    }
    let pixelMap = await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)

    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 40; i++) {
      let val = Number(Math.random() * 41);

      values.add(Math.random() * 100 < 25 ? new EntryOhos(i, val,pixelMap) : new EntryOhos(i, val));
    }


    let dataSet: LineDataSet = new LineDataSet(values, 'DataSet');
    dataSet.setHighlightEnabled(false);
    dataSet.setDrawIcons(false);

    dataSet.setMode(Mode.LINEAR)
    dataSet.setDrawCircles(true);
    dataSet.setDrawIcons(true)
    dataSet.setDrawCircleHole(true); //设置内部孔
    dataSet.setCircleHoleColor(Color.Green);
    dataSet.setDrawValues(false);
    dataSet.setColorByColor(Color.White); //设置折线颜色

    // 设置数据点的颜色
    dataSet.setCircleColor(Color.White); // 可以将圆环设置为你想要的颜色
    // dataSet.enableDashedLine(5, 5, 0);//虚折线

    // 设置数据点的半径
    dataSet.setCircleRadius(5); // 设置半径大小
    dataSet.setCircleHoleRadius(2.5); //设置内径

    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(dataSet);

    let lineData: LineData = new LineData(dataSetList);
    lineData.setValueTextSize(10);
    model.setData(lineData);
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.lineChartModel1 }).height('25%')
            .backgroundColor(ColorTemplate.colorRgb(137, 230, 81))
        }

        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.lineChartModel2 }).height('25%')
            .backgroundColor(ColorTemplate.colorRgb(240, 240, 30))
        }

        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.lineChartModel3 }).height('25%')
            .backgroundColor(ColorTemplate.colorRgb(89, 199, 250))
        }

        Stack({ alignContent: Alignment.TopStart }) {
          LineChart({ model: this.lineChartModel4 }).height('25%')
            .backgroundColor(ColorTemplate.colorRgb(250, 104, 104))
        }
      }.layoutWeight(1)
    }
  }
}