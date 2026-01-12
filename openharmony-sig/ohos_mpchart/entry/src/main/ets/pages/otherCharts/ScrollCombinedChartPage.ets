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
  ColorTemplate,
  CombinedChart,
  CombinedChartModel,
  CombinedData,
  JArrayList,
  BubbleData,
  BubbleEntry,
  BubbleDataSet,
  YAxisLabelPosition,
  XAxisPosition,
  XAxis,
  YAxis,
  Legend,
  LegendHorizontalAlignment,
  LegendVerticalAlignment,
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';

/** ScrollCombinedChartPage 组件 */
@Entry
@ComponentV2
struct ScrollCombinedChartPage {
  /** combinedChart 图表配置构建类 */
  @Local private combinedChartModel: CombinedChartModel = new CombinedChartModel();
  /** 图表数据 */
  combinedData: CombinedData | null = null;
  /** 模拟数据量大小 */
  private count: number = 12;
  /** 标题 */
  private title: string = 'ScrollCombinedChartPage';
  /** 标题配置构建类 */
  chartTitleModel: ChartTitleModel = new ChartTitleModel();

  /** 图表数据初始化 */
  async aboutToAppear() {
    /** 设置chart基础属性 */
    this.chartTitleModel.title = this.title;
    this.combinedChartModel.getDescription()?.setEnabled(false);
    this.combinedChartModel.setMaxVisibleValueCount(60);
    this.combinedChartModel.setPinchZoom(false);
    this.combinedChartModel.setDragDecelerationEnabled(true);
    this.combinedChartModel.setDrawGridBackground(false);
    this.combinedChartModel.setAutoScaleMinMaxEnabled(true);
    this.combinedChartModel.setVisibleXRangeMaximum(5);
    this.combinedChartModel.invalidate();

    /** 设置图例 */
    let legend: Legend | null = this.combinedChartModel.getLegend();
    if (legend) {
      legend.setEnabled(true);
      legend.setWordWrapEnabled(true);
      legend.setHorizontalAlignment(LegendHorizontalAlignment.LEFT);
      legend.setVerticalAlignment(LegendVerticalAlignment.BOTTOM);
      legend.setDrawInside(false);
    }

    /** 设置图表x轴 */
    let xAxis: XAxis | null = this.combinedChartModel.getXAxis();
    if (xAxis) {
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(false);
      xAxis.setGranularity(1);
      xAxis.setLabelCount(7);
    }

    /** 设置图表左Y轴 */
    let leftAxis: YAxis | null = this.combinedChartModel.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(8, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(0);
    }

    /** 设置图表右Y轴 */
    let rightAxis: YAxis | null = this.combinedChartModel.getAxisRight();
    if (rightAxis) {
      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      rightAxis.setSpaceTop(15);
      rightAxis.setAxisMinimum(0);
    }

    /** 设置图表数据 */
    let combinedData: CombinedData = new CombinedData();
    this.generateBubbleData(combinedData);
    this.combinedChartModel.setData(combinedData);
  }

  /** 生成bubbleData */
  private generateBubbleData(combinedData: CombinedData): void {
    /** 初始化 bubbleData 对象 */
    let bubbleData: BubbleData = new BubbleData();
    /** 创建一个 JArrayList 对象，用于存储 BubbleEntry 类型的数据 */
    let bubbleEntry: JArrayList<BubbleEntry> = new JArrayList();
    /** 循环生成 1 到 count 的随机数据，并添加到 bubbleEntry 中 */
    for (let i = 0; i < this.count; i++) {
      bubbleEntry.add(new BubbleEntry(i, i * 100, 40));
    }
    /** 创建 bubbleDataSet 对象，使用 bubbleEntry 数据，并设置数据集的名称为 'Bubble DataSet' */
    let bubbleDataSet: BubbleDataSet = new BubbleDataSet(bubbleEntry, "Bubble DataSet");
    bubbleDataSet.setColorsByArr(ColorTemplate.VORDIPLOM_COLORS);
    bubbleDataSet.setValueTextColor(Color.White);
    bubbleDataSet.setHighlightCircleWidth(1.5);
    bubbleDataSet.setDrawValues(true);
    bubbleData.addDataSet(bubbleDataSet);
    combinedData.setBubbleData(bubbleData);
  }

  /** 获取随机数 */
  getRandom(range: number, start: number): number {
    return (Math.random() * range) + start;
  }

  build() {
    Column() {
      title({ model: this.chartTitleModel })
      CombinedChart({ model: this.combinedChartModel })
        .width('100%')
        .height('70%')
        .backgroundColor(Color.White)
    }
  }
}