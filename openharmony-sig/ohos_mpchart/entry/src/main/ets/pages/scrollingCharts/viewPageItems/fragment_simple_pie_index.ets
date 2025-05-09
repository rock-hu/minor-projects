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

import { PieChart, PieChartModel } from '@ohos/mpchart'
import { PieData } from '@ohos/mpchart';
import { PieDataSet } from '@ohos/mpchart';
import { PieEntry } from '@ohos/mpchart';
import { JArrayList } from '@ohos/mpchart';
import { ColorTemplate } from '@ohos/mpchart';
import { MPPointF } from '@ohos/mpchart';
import { Legend, LegendVerticalAlignment, LegendOrientation, LegendHorizontalAlignment } from '@ohos/mpchart';

@ComponentV2
export default struct fragmentSimplePieIndex {
  pieData: PieData | null = null;
  @Param pieModel: PieChartModel = new PieChartModel()
  parties: string[] = [
    "Party A", "Party B", "Party C", "Party D", "Party E", "Party F", "Party G", "Party H",
    "Party I", "Party J", "Party K", "Party L", "Party M", "Party N", "Party O", "Party P",
    "Party Q", "Party R", "Party S", "Party T", "Party U", "Party V", "Party W", "Party X",
    "Party Y", "Party Z"]

  public aboutToAppear(): void {
    this.pieData = this.initPieData(4, 10);
    this.pieModel.setData(this.pieData);

    let l: Legend | null = this.pieModel.getLegend()
    if (l) {
      l.setOrientation(LegendOrientation.VERTICAL)
      l.setVerticalAlignment(LegendVerticalAlignment.TOP);
      l.setHorizontalAlignment(LegendHorizontalAlignment.RIGHT);
      l.setYEntrySpace(10)
      l.setFormToTextSpace(20)
    }
  }

  // 初始化饼状图数据
  private initPieData(count: number, range: number): PieData {
    let entries = new JArrayList<PieEntry>();
    for (let i = 0; i < count; i++) {
      entries.add(new PieEntry(((Math.random() * range) + range / 5), this.parties[i % this.parties.length]))
    }

    let dataSet: PieDataSet = new PieDataSet(entries, "Election Results");
    dataSet.setDrawIcons(false);
    dataSet.setSliceSpace(1);
    dataSet.setIconsOffset(new MPPointF(0, 40));
    dataSet.setSelectionShift(5);

    // add a lot of colors
    let colors: JArrayList<number> = new JArrayList();
    for (let index = 0; index < ColorTemplate.VORDIPLOM_COLORS.length; index++) {
      colors.add(ColorTemplate.VORDIPLOM_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.JOYFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.JOYFUL_COLORS[index]);
    }

    for (let index = 0; index < ColorTemplate.COLORFUL_COLORS.length; index++) {
      colors.add(ColorTemplate.COLORFUL_COLORS[index]);
    }
    for (let index = 0; index < ColorTemplate.LIBERTY_COLORS.length; index++) {
      colors.add(ColorTemplate.LIBERTY_COLORS[index]);
    }
    for (let index = 0; index < ColorTemplate.PASTEL_COLORS.length; index++) {
      colors.add(ColorTemplate.PASTEL_COLORS[index]);
    }
    colors.add(ColorTemplate.getHoloBlue());
    dataSet.setColorsByList(colors);
    return new PieData(dataSet)
  }

  build() {
    Column() {
      PieChart({
        model: this.pieModel
      })
        .width('100%')
        .height('80%')
    }
  }
}