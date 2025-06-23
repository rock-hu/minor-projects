/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
import { MyDataSource } from '../model/MyDataSource'

@Entry
@Component
struct MergeDoubleWaterFlowPage {
  @State minSize: number = 80
  @State maxSize: number = 210;
  @State fontSize: number = 24;
  @State colors: number[] = [0xFFC0CB, 0xDA70D6, 0x6B8E23, 0x6A5ACD, 0x00FFFF, 0x00FF7E];
  scroller: Scroller = new Scroller();
  private data: MyDataSource = new MyDataSource();
  private itemHeightArray: number[] = [];
  @State sections: WaterFlowSections = new WaterFlowSections();
  sectionMargin: Margin = {
    top: 10,
    left: 5,
    bottom: 10,
    right: 5
  }
  oneColumnSection: SectionOptions = {
    itemsCount: 21,
    crossCount: 2,
    columnsGap: '5vp',
    rowsGap: 10,
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }
  twoColumnSection: SectionOptions = {
    itemsCount: 1,
    crossCount: 1,
    columnsGap: '5vp',
    margin: this.sectionMargin,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }
  lastSection: SectionOptions = {
    itemsCount: 78,
    crossCount: 2,
    onGetItemMainSizeByIndex: (index: number) => {
      return this.itemHeightArray[index % 100];
    }
  }

  getSize() {
    let ret = Math.floor(Math.random() * this.maxSize);
    return (ret > this.minSize ? ret : this.minSize);
  }

  setItemSizeArray() {
    for (let i = 0; i < 100; i++) {
      this.itemHeightArray.push(this.getSize());
    }
  }

  aboutToAppear(): void {
    for(let i = 0; i < 100; i++) {
      this.data.pushData(i);
    }
    this.setItemSizeArray();
    let sectionOptions: SectionOptions[] = [];
    sectionOptions.push(this.oneColumnSection);
    sectionOptions.push(this.twoColumnSection);
    sectionOptions.push(this.lastSection);
    this.sections.splice(0, 0, sectionOptions);
  }

  build() {
    Column({ space: 2 }) {
      WaterFlow({ scroller: this.scroller, sections: this.sections }) {
        LazyForEach(this.data, (item: number) => {
          FlowItem() {
            if (item === 21) {
              Column() {
                Text('------推荐商品------')
                  .width('100%')
                  .height(50)
              }
            } else {
              Column() {
                Text('N ' + item)
                  .fontSize(12)
                  .height('16vp')
                Image($rawfile(`sections/${item % 4}.jpg`))
                  .objectFit(ImageFit.Fill)
                  .width('100%')
                  .layoutWeight(1)
              }
            }
          }
          .onAppear(() => {

          })
          .width('100%')
          .height(this.itemHeightArray[item % 100])
          .backgroundColor(this.colors[item % 5])
        }, (item: number) => item.toString())
      }
      .columnsGap(10)
      .rowsGap(5)
      .backgroundColor('#efefef')
      .width('100%')
      .height('100%')
      .onReachStart(() => {
        console.log('Waterflow reach start');
      })
      .onScrollStart(() => {
        console.log('Waterflow scroll start');
      })
      .onScrollStop(() => {
        console.log('Waterflow scroll stop');
      })
    }
  }
}