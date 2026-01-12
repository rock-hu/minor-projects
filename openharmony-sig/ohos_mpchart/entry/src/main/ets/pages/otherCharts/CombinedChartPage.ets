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
  ColorTemplate,
  CombinedChart,
  CombinedChartModel,
  CombinedData,
  EntryOhos,
  JArrayList,
  LineData,
  LineDataSet,
  ChartColor,
  Mode,
  AxisDependency,
  ScatterData,
  ScatterDataSet,
  CandleData,
  CandleEntry,
  CandleDataSet,
  BubbleData,
  BubbleEntry,
  BubbleDataSet,
  YAxisLabelPosition,
  XAxisPosition,
  XAxis,
  YAxis,
  LegendHorizontalAlignment,
  LegendVerticalAlignment,
  WaterfallEntry,
  WaterfallDataSet,
  IWaterfallDataSet,
  WaterfallData,
  BarEntry,
  BarDataSet,
  IBarDataSet,
  BarData,
  ChartPixelMap
} from '@ohos/mpchart';
import title, { ChartTitleModel } from '../../title';
import Utils from '../../utils/Utils';

@Entry
@ComponentV2
struct CombinedChartPage {
  @Local private model: CombinedChartModel = new CombinedChartModel();
  combinedData: CombinedData | null = null;
  private count: number = 12;
  private isDrawIcons: boolean= true;
  //标题栏标题
  private title: string = 'Combined Chart'
  titleModel: ChartTitleModel = new ChartTitleModel()
  private  pixelMap:ChartPixelMap=new ChartPixelMap()

  async aboutToAppear() {
    this.model.getDescription()?.setEnabled(false);
    let l = this.model.getLegend();
    if (l) {
      l.setEnabled(true);
      l.setWordWrapEnabled(true);
      l.setHorizontalAlignment(LegendHorizontalAlignment.LEFT);
      l.setVerticalAlignment(LegendVerticalAlignment.BOTTOM);
      l.setDrawInside(false);
    }

    this.model.setMaxVisibleValueCount(60);
    this.model.setPinchZoom(false);
    this.model.setDragDecelerationEnabled(true);
    this.model.setDrawGridBackground(false);

    let xAxis: XAxis | null = this.model.getXAxis();
    if (xAxis) {
      xAxis.setPosition(XAxisPosition.BOTTOM);
      xAxis.setDrawGridLines(false);
      xAxis.setGranularity(1);
      xAxis.setLabelCount(7);
    }

    let leftAxis: YAxis | null = this.model.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(8, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART)
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(0);
    }


    let rightAxis: YAxis | null = this.model.getAxisRight();
    if (rightAxis) {
      rightAxis.setLabelCount(8, false);
      rightAxis.setDrawGridLines(false);
      rightAxis.setSpaceTop(15);
      rightAxis.setAxisMinimum(0);
    }

    this.pixelMap=await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let data: CombinedData = new CombinedData();
    this.generateLineData(data);
    this.generateWaterfallData(data);
    this.generateBubbleData(data);
    this.generateScatterData(data);
    this.generateCandleData(data);
    this.generateBarData(data);
    this.model.setData(data);
    this.model.invalidate();

  }

  private async generateLineData(data: CombinedData): Promise<void> {
    let d: LineData = new LineData();

    let entries: JArrayList<EntryOhos> = new JArrayList();

    for (let index = 0; index < this.count; index++) {

      entries.add(new EntryOhos(index + 0.5, this.getRandom(15, 5),this.pixelMap));
    }

    let set: LineDataSet = new LineDataSet(entries, "Line DataSet");
    set.setColorByColor(ChartColor.rgb(240, 238, 70));
    set.setLineWidth(2.5);
    set.setCircleColor(ChartColor.rgb(240, 238, 70));
    set.setCircleRadius(5);
    set.setDrawIcons(this.isDrawIcons);
    set.setFillColor(ChartColor.rgb(240, 238, 70));
    set.setMode(Mode.CUBIC_BEZIER);
    set.setDrawValues(true);
    set.setValueTextSize(10);
    set.setValueTextColor(ChartColor.rgb(240, 238, 70));

    set.setAxisDependency(AxisDependency.LEFT);
    d.addDataSet(set);
    data.setLineData(d);

  }

  private async generateWaterfallData(data: CombinedData): Promise<void> {
    let values: JArrayList<WaterfallEntry> = new JArrayList<WaterfallEntry>();

    for (let i = 0; i < this.count - 1; i++) {
      values.add(new WaterfallEntry(i + 0.5, 35, 50,this.pixelMap));
    }

    let dataSet = new WaterfallDataSet(values, 'DataSet');
    dataSet.setDrawIcons(this.isDrawIcons);
    dataSet.setColorByColor(ColorTemplate.COLORFUL_COLORS[0]);
    dataSet.setValueTextSize(10)

    let dataSetList: JArrayList<IWaterfallDataSet> = new JArrayList<IWaterfallDataSet>();
    dataSetList.add(dataSet);

    let waterfallData: WaterfallData = new WaterfallData(dataSetList);
    waterfallData.setBarWidth(0.6);
    data.setWaterfallData(waterfallData);

  }

  private async generateBarData(data: CombinedData): Promise<void> {
    let entries1: JArrayList<BarEntry> = new JArrayList();
    let entries2: JArrayList<BarEntry> = new JArrayList();
    for (let index = 0; index < this.count; index++) {
      entries1.add(new BarEntry(0, this.getRandom(10, 6),this.pixelMap));

      // stacked
      entries2.add(new BarEntry(0, [this.getRandom(10, 6), this.getRandom(10, 6)],this.pixelMap));
    }

    let set1: BarDataSet = new BarDataSet(entries1, "Bar 1");
    set1.setColorByColor(ChartColor.rgb(60, 220, 78));
    set1.setValueTextColor(ChartColor.rgb(60, 220, 78));
    set1.setValueTextSize(10);
    set1.setDrawIcons(this.isDrawIcons);
    set1.setAxisDependency(AxisDependency.LEFT);

    let set2: BarDataSet = new BarDataSet(entries2, "");
    set2.setStackLabels(["Stack 1", "Stack 2"]);
    set2.setColorsByArr([ChartColor.rgb(61, 165, 255), ChartColor.rgb(23, 197, 255)]);
    set2.setValueTextColor(ChartColor.rgb(61, 165, 255));
    set2.setValueTextSize(10);
    set2.setDrawIcons(this.isDrawIcons);
    set2.setAxisDependency(AxisDependency.LEFT);

    let groupSpace = 0.06;
    let barSpace = 0.02; // x2 dataset
    let barWidth = 0.45; // x2 dataset
    // (0.45 + 0.02) * 2 + 0.06 = 1.00 -> interval per "group"

    let set: JArrayList<IBarDataSet> = new JArrayList();
    set.add(set1);
    set.add(set2);
    let d: BarData = new BarData(set);
    d.setBarWidth(barWidth);

    // make this BarData object grouped
    d.groupBars(0, groupSpace, barSpace); // start at x = 0
    data.setBarData(d);
  }



  private async generateScatterData(data: CombinedData): Promise<void> {

    let d: ScatterData = new ScatterData();

    let entries: JArrayList<EntryOhos> = new JArrayList();

    for (let index = 0; index < this.count; index += 0.5)
      entries.add(new EntryOhos(index + 0.25, this.getRandom(10, 55),this.pixelMap));

    let set: ScatterDataSet = new ScatterDataSet(entries, "Scatter DataSet");
    set.setColorsByArr(ColorTemplate.MATERIAL_COLORS);
    set.setScatterShapeSize(7.5);
    set.setDrawValues(false);
    set.setDrawIcons(this.isDrawIcons);
    set.setValueTextSize(10);
    d.addDataSet(set);
    data.setScatterData(d);
  }

  private generateCandleData(data: CombinedData): void {

    let d: CandleData = new CandleData();

    let entries: JArrayList<CandleEntry> = new JArrayList();

    for (let index = 0; index < this.count; index += 2) {

      entries.add(new CandleEntry(index + 1, 90, 70, 85, 75,this.pixelMap));
    }

    let set: CandleDataSet = new CandleDataSet(entries, "Candle DataSet");
    set.setDecreasingColor(ChartColor.rgb(142, 150, 175));
    set.setShadowColor(ColorTemplate.DKGRAY);
    set.setDrawIcons(this.isDrawIcons);
    set.setBarSpace(0.3);
    set.setValueTextSize(10);
    set.setDrawValues(false);
    d.addDataSet(set);
    data.setCandleData(d);
  }

  private generateBubbleData(data: CombinedData): void {

    let bd: BubbleData = new BubbleData();

    let entries: JArrayList<BubbleEntry> = new JArrayList();

    for (let index = 0; index < this.count; index++) {
      let y = this.getRandom(10, 105);
      let size = this.getRandom(20, 30);
      entries.add(new BubbleEntry(index + 0.5, y, size,this.pixelMap));
    }

    let set: BubbleDataSet = new BubbleDataSet(entries, "Bubble DataSet");
    set.setColorsByArr(ColorTemplate.VORDIPLOM_COLORS);
    set.setValueTextSize(10);
    set.setDrawIcons(this.isDrawIcons);
    set.setValueTextColor(ChartColor.rgb(255, 255, 255));
    set.setHighlightCircleWidth(1.5);
    set.setDrawValues(true);
    bd.addDataSet(set);
    data.setBubbleData(bd);
  }

  getRandom(range: number, start: number): number {
    return (Math.random() * range) + start;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      CombinedChart({ model: this.model })
        .width('100%')
        .height('70%')
        .backgroundColor(ChartColor.rgb(255, 255, 255))

    }
  }
}