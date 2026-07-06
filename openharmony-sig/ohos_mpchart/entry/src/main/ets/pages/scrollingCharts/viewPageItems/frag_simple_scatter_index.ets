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

import { ScatterChartModel, ScatterData } from '@ohos/mpchart';
import { ScatterDataSet } from '@ohos/mpchart';
import { IScatterDataSet } from '@ohos/mpchart';
import { LegendEntry } from '@ohos/mpchart';
import { XAxisPosition } from '@ohos/mpchart';
import { YAxis, AxisDependency, YAxisLabelPosition } from '@ohos/mpchart'
import { EntryOhos } from '@ohos/mpchart';
import { JArrayList } from '@ohos/mpchart';
import { MPPointF } from '@ohos/mpchart';
import { ScatterChart } from '@ohos/mpchart';
import { LimitLine, ChartShape } from '@ohos/mpchart';

@ComponentV2
export default struct fragSimpleScatterIndex {
  minOffset: number = 15;
  XLimtLine: LimitLine = new LimitLine(35, "Index 10");
  legendScatter: LegendEntry = new LegendEntry();
  legendColorWidth: number = 10;
  legendColorHeight: number = 10;
  legendArr: LegendEntry[][] = [[], [], []]
  dataScatter: ScatterData | null = null;
  @Param scatterChartModel: ScatterChartModel = new ScatterChartModel();

  build() {
    Column() {
      Column() {
        Stack({ alignContent: Alignment.TopStart }) {
          ScatterChart({
            model: this.scatterChartModel
          })
            .width('100%')
            .height('70%')
        }
      }
    }
    .alignItems(HorizontalAlign.Start)
  }

  aboutToAppear() {
    this.dataScatter = this.generateScatterData(6, 10000, 20);
    this.scatterChartModel.setData(this.dataScatter);
    let xAxis = this.scatterChartModel.getXAxis();
    if (xAxis) {
      xAxis.setLabelCount(7, false);
      xAxis.setPosition(XAxisPosition.TOP);
      xAxis.setAxisMinimum(0);
      xAxis.setAxisMaximum(this.dataScatter.getXMax());
      xAxis.enableGridDashedLine(10, 10, 0)
      xAxis.setDrawAxisLine(false);
      xAxis.setDrawLabels(false)
      xAxis.setDrawGridLines(true);
    }

    let leftAxis = this.scatterChartModel.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(6, false);
      leftAxis.setPosition(YAxisLabelPosition.OUTSIDE_CHART);
      leftAxis.setSpaceTop(15);
      leftAxis.setAxisMinimum(this.dataScatter.getYMin() - 500);
      leftAxis.setAxisMaximum(this.dataScatter.getYMax() + 1000);
      leftAxis.setDrawGridLines(true);
      leftAxis.enableGridDashedLine(10, 10, 0)
      leftAxis.setDrawLabels(true)
    }

    let rightAxis = this.scatterChartModel.getAxisRight();
    if (rightAxis) {
      rightAxis = new YAxis(AxisDependency.RIGHT);
      rightAxis.setDrawGridLines(false);
      rightAxis.setLabelCount(6, false);
      rightAxis.setSpaceTop(15);
      rightAxis.setAxisMinimum(this.dataScatter.getYMin() - 500); // this replaces setStartAtZero(true)
      rightAxis.setAxisMaximum(this.dataScatter.getYMax() + 1000);
      rightAxis.setDrawAxisLine(false);
      rightAxis.setDrawLabels(false);
    }
  }

  /**
   * 初始化数据
   * @param count  曲线图点的个数
   * @param range  y轴范围
   */
  private generateScatterData(dataSets: number, range: number, count: number): ScatterData {
    let sets: JArrayList<IScatterDataSet> = new JArrayList<IScatterDataSet>();
    let shapes = [ChartShape.SQUARE, ChartShape.CIRCLE, ChartShape.TRIANGLE, ChartShape.CROSS, ChartShape.X, ChartShape.CHEVRON_UP, ChartShape.CHEVRON_DOWN];

    for (let i = 0; i < dataSets; i++) {

      let entries: JArrayList<EntryOhos> = new JArrayList<EntryOhos>()

      for (let j = 0; j < count; j++) {
        entries.add(new EntryOhos(j, Math.random() * range + range / 5));
      }
      let ds: ScatterDataSet = new ScatterDataSet(entries, this.mLabels[i]);
      ds.setScatterShape(shapes[i % shapes.length]);
      ds.setColorsByArr([0xc12552, 0xff6600, 0xf5c700, 0x6a961f, 0xb36435]);
      ds.setScatterShapeSize(9);
      ds.setDrawValues(false);
      ds.setIconsOffset(new MPPointF(0, 0));
      sets.add(ds);
    }
    return new ScatterData(sets);
  }

  private mLabels: string[] = ["Company A", "Company B", "Company C", "Company D", "Company E", "Company F"];
}