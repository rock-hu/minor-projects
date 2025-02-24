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

import { BarData } from '@ohos/mpchart'
import { LegendEntry } from '@ohos/mpchart'
import { BarChart, BarChartModel } from '@ohos/mpchart'
import { XAxis, XAxisPosition } from '@ohos/mpchart'
import { YAxis, AxisDependency, YAxisLabelPosition } from '@ohos/mpchart'

@Preview
@ComponentV2
export default struct BarChartItem {
  @Param data: BarData | null = null;
  @Param barModel: BarChartModel = new BarChartModel()
  mChartData: BarData | null = null;
  aboutToAppear() {
    let leftAxis = this.barModel.getAxisLeft();
    if (leftAxis) {
      leftAxis.setLabelCount(7, false);
      leftAxis?.setAxisMinimum(0);
      leftAxis?.setAxisMaximum(100);
      leftAxis?.enableGridDashedLine(10, 10, 0);
    }

    if (this.data) {
      this.barModel.setData(this.data!);
    }
    this.barModel.setDrawBarShadow(false);
    this.barModel.setDrawValueAboveBar(true);
    this.barModel.getDescription()?.setEnabled(false);
    this.barModel.setMaxVisibleValueCount(60);
  }

  build() {
    Column() {
      Stack() {
        BarChart({ model: this.barModel })
          .width('100%')
          .height('30%')
      }
    }.alignItems(HorizontalAlign.Start)
  }
}

interface valueItem {
  index: number,
  legendItem: LegendEntry
}

