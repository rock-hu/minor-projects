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

import { BarEntry } from '@ohos/mpchart'
import { JArrayList } from '@ohos/mpchart'
import { BarDataSet } from '@ohos/mpchart'
import { BarData } from '@ohos/mpchart'
import { BarChart, BarChartModel } from '@ohos/mpchart'
import { IBarDataSet } from '@ohos/mpchart'
import { ColorTemplate } from '@ohos/mpchart'
import title, { ChartTitleModel } from '../../title'

interface valueItem {
  index: number;
  data: BarChartModel
}

@Entry
@ComponentV2
struct ManyBarChartsPage {
  minOffset: number = 15; //X轴线偏移量
  private list: Array<BarChartModel> = new Array<BarChartModel>();
  //标题栏标题
  private title: string = 'ManyBarCharts'
  titleModel: ChartTitleModel = new ChartTitleModel()

  aboutToAppear() {
    // 20 items
    for (let i = 0; i < 20; i++) {
      this.setData(i + 1, this.createModel())
    }
  }

  private createModel(): BarChartModel {
    let model: BarChartModel = new BarChartModel();
    model.setDrawBarShadow(false);
    model.setDrawValueAboveBar(true);
    model.getDescription()?.setEnabled(false);
    model.setMaxVisibleValueCount(60);
    return model;
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      List({ space: 20, initialIndex: 0 }) {
        ForEach(this.list.map((data, index) => {
          let value: valueItem = { data: data, index: index };
          return value;
        }), (item: valueItem) => {
          ListItem() {
            Column() {
              BarChart({ model: item.data })
                .width('100%')
                .height('30%')
            }
          }
        }, (item: valueItem) => item.index + "")
      }
      .listDirection(Axis.Vertical) // 排列方向
      .divider({ strokeWidth: 1, color: Color.Gray, startMargin: 0, endMargin: 0 }) // 每行之间的分界线
      .edgeEffect(EdgeEffect.None) // 滑动到边缘无效果
      .chainAnimation(false) // 联动特效关闭
      .margin({bottom: 50})
    }
  }

  private setData(cnt: number, model: BarChartModel) {

    let entries: JArrayList<BarEntry> = new JArrayList<BarEntry>();

    for (let i = 0; i < 12; i++) {
      entries.add(new BarEntry(i, (Math.random() * 70) + 30));
    }

    let d: BarDataSet = new BarDataSet(entries, "New DataSet " + cnt);
    d.setColorsByVariable(ColorTemplate.VORDIPLOM_COLORS);
    d.setDrawIcons(false);

    let sets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    sets.add(d);

    let cd: BarData = new BarData(sets);
    cd.setValueTextSize(10);

    model.setData(cd);

    this.list.push(model);
  }
}
