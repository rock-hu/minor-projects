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

import { LineData } from '@ohos/mpchart'
import { PieData } from '@ohos/mpchart'
import { LineDataSet, Mode } from '@ohos/mpchart'
import { PieDataSet } from '@ohos/mpchart'
import { EntryOhos } from '@ohos/mpchart'
import { PieEntry } from '@ohos/mpchart'
import { JArrayList } from '@ohos/mpchart'
import { ILineDataSet } from '@ohos/mpchart'
import { IBarDataSet } from '@ohos/mpchart'
import { ColorTemplate } from '@ohos/mpchart'
import PieChartItem from './listViewItems/PieChartItem'
import LineChartItem from './listViewItems/LineChartItem'
import BarChartItem from './listViewItems/BarChartItem'
import { BarDataSet } from '@ohos/mpchart'
import { BarData } from '@ohos/mpchart'
import { BarEntry } from '@ohos/mpchart'
import title, { ChartTitleModel } from '../../title'

interface valueItem {
  index: number;
  itemData: object;
}

@Entry
@Preview
@ComponentV2
struct ListViewMultiChartPage {
  list: Array<object> = new Array()
  private title: string = 'ListViewMultiChartPage'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  private menuItemArr: Array<string> = ['View on GitHub']

  aboutToAppear() {
    // 30 items
    for (let i = 0; i < 12; i++) {
      if (i % 3 == 0) {
        this.list.push(this.generateDataLine(i + 1))
      } else if (i % 3 == 1) {
        this.list.push(this.generateDataBar(i + 1))
      } else if (i % 3 == 2) {
        this.list.push(this.generateDataPie())
      }
    }
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title
    this.titleModel.isShowMenu = false
  }

  @Monitor("titleModel.index")
  menuCallback() {
    if (this.titleModel == null || this.titleModel == undefined) {
      return
    }
    let index: number = this.titleModel.getIndex()
    switch (index) {
      case 0:
        break;
    }
    this.titleModel.setIndex(-1)
  }

  private generateDataPie(): PieData {
    let entries: JArrayList<PieEntry> = new JArrayList<PieEntry>();
    for (let i = 0; i < 4; i++) {
      entries.add(new PieEntry(30, "Quarter " + (i + 1)));
    }
    let d: PieDataSet = new PieDataSet(entries, "");
    // space between slices
    d.setSliceSpace(1);
    d.setColorsByVariable(ColorTemplate.COLORFUL_COLORS);
    d.setValueTextColor(Color.White);

    return new PieData(d);
  }

  private generateDataLine(cnt: number): LineData {
    let values1: JArrayList<EntryOhos> = new JArrayList<EntryOhos>()
    for (let i = 0; i < 12; i++) {
      values1.add(new EntryOhos(i, (Math.random() * 65) + 40));
    }

    let d1: LineDataSet = new LineDataSet(values1, "New DataSet " + cnt + ", (1)");
    d1.setLineWidth(2.5);
    d1.setCircleRadius(4.5);
    d1.setHighLightColor(0xF47575);
    d1.setDrawValues(false);
    d1.setCircleColor(0x8CEAFF);

    let values2: JArrayList<EntryOhos> = new JArrayList <EntryOhos>();

    for (let i = 0; i < 12; i++) {
      values2.add(new EntryOhos(i, values1.get(i).getY() - 30));
    }

    let d2: LineDataSet = new LineDataSet(values2, "New DataSet " + cnt + ", (2)");
    d2.setLineWidth(2.5);
    d2.setCircleRadius(4.5);
    d2.setHighLightColor(0xF47575);
    d2.setColorByColor(ColorTemplate.VORDIPLOM_COLORS[0]);
    d2.setCircleColor(ColorTemplate.VORDIPLOM_COLORS[0]);
    d2.setDrawValues(false);

    let sets: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    sets.add(d1);
    sets.add(d2);

    return new LineData(sets);
  }

  private generateDataBar(cnt: number): BarData {
    let entries: JArrayList<BarEntry> = new JArrayList<BarEntry>()

    for (let i = 0; i < 12; i++) {
      entries.add(new BarEntry(i, (Math.random() * 70) + 30));
    }
    let d: BarDataSet = new BarDataSet(entries, "New DataSet " + cnt);
    d.setColorsByVariable(ColorTemplate.VORDIPLOM_COLORS);
    d.setHighLightAlpha(255);
    d.setValueTextSize(8)
    let dataSets: JArrayList<IBarDataSet> = new JArrayList<IBarDataSet>();
    dataSets.add(d);

    let cd: BarData = new BarData(dataSets);
    return cd
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      List({ space: 20, initialIndex: 0 }) {
        ForEach(this.list.map((data, index) => {
          let value: valueItem = { index: index, itemData: data };
          return value;
        }), (item: valueItem) => {
          ListItem() {
            Column() {
              if (item.itemData instanceof PieData) {
                PieChartItem({ data: item.itemData })
              }
              if (item.itemData instanceof LineData) {
                LineChartItem({ data: item.itemData })
              }
              if (item.itemData instanceof BarData) {
                BarChartItem({ data: item.itemData })
              }
            }
          }.editable(false)
        }, (item: valueItem) => item.index + "")
      }
      .margin({ bottom: 100 })
      .listDirection(Axis.Vertical) // 排列方向
      .divider({ strokeWidth: 1, color: Color.Gray, startMargin: 0, endMargin: 0 }) // 每行之间的分界线
      .edgeEffect(EdgeEffect.None) // 滑动到边缘无效果
      .chainAnimation(false) // 联动特效关闭

    }
  }
}