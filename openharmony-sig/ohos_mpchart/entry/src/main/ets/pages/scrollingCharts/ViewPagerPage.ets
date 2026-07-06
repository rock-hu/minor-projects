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
import fragmentSimplePieIndex from './viewPageItems/fragment_simple_pie_index'
import fragSimpleBarIndex from './viewPageItems/frag_simple_bar_index'
import fragSimpleLineCircleIndex from './viewPageItems/frag_simple_line_circle_index'
import fragSimpleLineIndex from './viewPageItems/frag_simple_line_index'
import fragSimpleScatterIndex from './viewPageItems/frag_simple_scatter_index'
import title, { ChartTitleModel } from '../../title'
import { LogUtil } from '../../utils/LogUtil'

@Entry
@ComponentV2
struct ViewPager {
  private swiperController: SwiperController = new SwiperController()
  private data: MyDataSource = new MyDataSource([])
  //标题栏标题
  private title: string = 'View Pager'
  titleModel: ChartTitleModel = new ChartTitleModel()

  build() {
    Column({ space: 5 }) {
      title({ model: this.titleModel })
      Swiper(this.swiperController) {
        LazyForEach(this.data, (item: string) => {
          Column() {
            if ('0' == item) {
              fragSimpleLineIndex();
            } else if ('1' == item) {
              fragSimpleLineCircleIndex();
            } else if ('2' == item) {
              fragSimpleBarIndex();
            } else if ('3' == item) {
              fragSimpleScatterIndex();
            } else if ('4' == item) {
              fragmentSimplePieIndex();
            }
          }
        }, (item: string) => item)
      }
      .index(0)
      .indicator(false) // 默认开启指示点
      .loop(false) // 默认开启循环播放
      //.duration(1000)
      .vertical(false) // 默认横向切换
      .itemSpace(0)
      .width('100%')
      .height('100%')
      .cachedCount(0)
      .onChange((index: number) => {
        LogUtil.log("index:" + index.toString())
      })

    }
  }

  public aboutToAppear(): void {
    let list: number[] = []
    for (let i: number = 0; i < 5; i++) {
      list.push(i);
    }
    this.data = new MyDataSource(list)
  }
}

class MyDataSource implements IDataSource {
  private list: number[] = []
  private listener: DataChangeListener | null = null;

  constructor(list: number[]) {
    this.list = list
  }

  totalCount(): number {
    return this.list.length
  }

  getData(index: number): number {
    return this.list[index]
  }

  registerDataChangeListener(listener: DataChangeListener): void {
    this.listener = listener
  }

  unregisterDataChangeListener() {
  }
}