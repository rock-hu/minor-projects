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

import { LineChart, LineChartModel } from '@ohos/mpchart'
import { XAxisPosition } from '@ohos/mpchart';
import { LineData } from '@ohos/mpchart';
import { LimitLine } from '@ohos/mpchart'


@ComponentV2
export default struct LineChartItem {
  @Param data: LineData | null = null;
  @Param lineChartModel: LineChartModel = new LineChartModel();

  aboutToAppear() {
    if (this.data) {
      this.lineChartModel.setData(this.data);
    }
  }

  build() {
    Column() {
      LineChart({ model: this.lineChartModel })
        .width('100%')
        .height('30%')
    }
    .width('100%')
    .alignItems(HorizontalAlign.Start)
  }
}
