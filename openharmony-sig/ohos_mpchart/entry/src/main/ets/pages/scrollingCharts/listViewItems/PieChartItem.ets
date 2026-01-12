/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

import { PieChartModel, PieData } from '@ohos/mpchart';
import { PieChart } from '@ohos/mpchart'
import { Legend, LegendVerticalAlignment, LegendOrientation, LegendHorizontalAlignment } from '@ohos/mpchart';
import { MPPointF } from '@ohos/mpchart';

@ComponentV2
export default struct PieChartItem {
  @Param data: PieData = new PieData()
  @Param pieModel: PieChartModel = new PieChartModel()

  aboutToAppear() {
    this.pieModel.setHoleRadius(95);
    this.pieModel.setData(this.data);
    this.pieModel.setUsePercentValues(true);
    this.pieModel.setHoleRadius(10);
    this.pieModel.setTransparentCircleRadius(15);
    this.pieModel.setExtraOffsets(-20, 0, 0, 0);
    this.pieModel.setEntryLabelTextSize(5);
    this.pieModel.setDrawHoleEnabled(false);

    let l: Legend | null = this.pieModel.getLegend()
    if (l) {
      l.setOrientation(LegendOrientation.VERTICAL)
      l.setVerticalAlignment(LegendVerticalAlignment.TOP);
      l.setHorizontalAlignment(LegendHorizontalAlignment.RIGHT);
    }
  }

  build() {
    Column() {
      PieChart({
        model: this.pieModel
      })
        .width('100%')
        .height('30%')
    }.alignItems(HorizontalAlign.Start)
  }
}
