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
  EntryOhos,
  LineDataSet,
  ILineDataSet,
  LineData,
  Mode,
  LineChart,
  LineChartModel,
  ChartPixelMap
} from '@ohos/mpchart';
import Constants from '../../constants/Constants';
import title, { ChartTitleModel } from '../../title';
import Utils from '../../utils/Utils';


@Entry
@ComponentV2
struct LineChartNotAxisConfigPage {
  //标题栏菜单文本
  private menuItemArr: Array<string> = [Constants.TOGGLE_DRAW_ICON,'Animate X', 'Animate Y', 'Animate XY'];
  //标题栏标题
  private title: string = 'No Axis settings'
  @Local titleModel: ChartTitleModel = new ChartTitleModel()
  @Local private model: LineChartModel | undefined = undefined;
  private dataSet: LineDataSet | null = null;
  titleSelectString: string = 'X'
  private isDrawIcons: boolean= false;

  //标题栏菜单回调
  @Monitor("titleModel.index")
  menuCallback() {

    if (this.titleModel === null || this.titleModel === undefined) {
      return
    }
    let index: number = this.titleModel.getIndex()
    if (index === undefined || index === null || index === -1) {
      return
    }

    if (this.model === null || this.model === undefined) {
      return
    }

    let data = this.model.getData();
    let sets: JArrayList<ILineDataSet> | null = null;
    if (data) {
      sets = data.getDataSets();
    }

    const animateFun = (s: string) => {
      this.titleSelectString = s
      this.animate()
    }

    const toggleDrawIcon = (sets: JArrayList<ILineDataSet> | null) => {
      if (sets) {
        for (let i = 0; i < sets.size(); i++) {
          let set = sets.get(i) as LineDataSet;
          set.setDrawIcons(!this.isDrawIcons)
        }
        this.isDrawIcons = !this.isDrawIcons
        this.model?.invalidate();
      }
    }

    switch (this.menuItemArr[index]) {
      case 'Animate X':
        animateFun('X')
        break;
      case 'Animate Y':
        animateFun('Y')
        break;
      case 'Animate XY':
        animateFun('XY')
        break;
      case Constants.TOGGLE_DRAW_ICON:
        toggleDrawIcon(sets)
        break;

    }
    this.titleModel.setIndex(-1)
  }

  public animate() {
    if (this.model) {
      if (this.titleSelectString === 'X') {
        this.model.animateX(2000);
      } else if (this.titleSelectString === 'Y') {
        this.model.animateY(2000);
      } else if (this.titleSelectString === 'XY') {
        this.model.animateXY(2000, 2000);
      }
    }

  }

  async aboutToAppear() {
    this.titleModel.menuItemArr = this.menuItemArr
    this.titleModel.title = this.title

    this.model = new LineChartModel();

    this.model.setMaxVisibleValueCount(60);

    let lineData: LineData = await this.getLineData()
    lineData.setValueTextSize(10);
    this.model.setData(lineData);
    this.model.setDragDecelerationEnabled(true);
    this.model.setVisibleXRangeMaximum(7);
    this.model.moveViewToX(10);
    this.model.invalidate();
  }

  private async getLineData(): Promise<LineData> {
    let pixelMap = await Utils.getPixelMap($r('app.media.star').id, getContext(this).resourceManager)
    let start: number = 1;
    let values: JArrayList<EntryOhos> = new JArrayList<EntryOhos>();
    for (let i = start; i < 20; i++) {
      let val = Number(Math.random() * 141);

      values.add(Math.random() * 100 < 25 ? new EntryOhos(i, val, pixelMap) : new EntryOhos(i, val));
    }

    this.dataSet = new LineDataSet(values, 'DataSet');
    this.dataSet.setHighLightColor(Color.Black);
    this.dataSet.setDrawIcons(this.isDrawIcons);

    this.dataSet.setMode(Mode.LINEAR);
    this.dataSet.setDrawCircles(true); //折线点画圆圈
    this.dataSet.setDrawCircleHole(false); //设置内部孔
    this.dataSet.setColorByColor(Color.Black); //设置折现颜色

    // 设置数据点的颜色
    this.dataSet.setCircleColor(Color.Blue); // 可以设置为你想要的颜色

    // 设置数据点的半径
    this.dataSet.setCircleRadius(4); // 设置半径大小
    this.dataSet.setCircleHoleRadius(2); //设置内径

    let dataSetList: JArrayList<ILineDataSet> = new JArrayList<ILineDataSet>();
    dataSetList.add(this.dataSet);

    let lineData: LineData = new LineData(dataSetList);
    return lineData
  }

  build() {
    Column() {
      title({ model: this.titleModel })
      Column() {
        LineChart({ model: this.model })
          .width('100%')
          .height('30%')
          .backgroundColor(Color.White)

        Divider()
          .width('1px')
          .padding({ top: 5 })

        Flex({justifyContent: FlexAlign.Center, direction: FlexDirection.Column}) {
          Text($r('app.string.display_no_axis'))
            .fontSize(14).fontWeight(FontWeight.Medium).fontColor(Color.Orange)
        }.margin(10)

      }
    }
  }
}