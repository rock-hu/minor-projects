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

import { hilog, hiTraceMeter } from '@kit.PerformanceAnalysisKit';

class MyDataSource implements IDataSource {
  private dataArray: number[] = [];

  public pushData(data: number): void {
    this.dataArray.push(data);
  }

  // The total amount of data from the data source
  public totalCount(): number {
    return this.dataArray.length;
  }

  // Returns data for the specified index location
  public getData(index: number): number {
    return this.dataArray[index];
  }

  registerDataChangeListener(): void {
  }

  unregisterDataChangeListener(): void {
  }
}

@Entry
@Component
struct NegativeOfLazyForEach {
  // Data sources
  private data: MyDataSource = new MyDataSource();
  private item: number = 0;

  // [Start negative_of_lazy_for_each]
  // [StartExclude negative_of_lazy_for_each]
  // [EndExclude negative_of_lazy_for_each]
  aboutToAppear(): void {
    // [StartExclude negative_of_lazy_for_each]
    for (let i = 1; i < 1000; i++) {
      this.data.pushData(i);
    }
    this.item = this.itemGeneratorFunc(0);
    hilog.info(0x0000, 'Sample', '%{public}s', `item: ${this.item}`);
    // [EndExclude negative_of_lazy_for_each]
  }

  // Simulate time-consuming operations
  itemGeneratorFunc(item: number): number {
    let temp: number = 0;
    for (let index = 0; index < 1000000; index++) {
      temp += 1;
    }
    item += temp;
    return item;
  }

  build() {
    Column() {
      Row({ space: 5 }) {
        Column() {
          Image($r('app.media.chevron_left'))
            .width(16)
            .height(16)
        }
        .width(40)
        .height(40)
        .justifyContent(FlexAlign.Center)
        .backgroundColor('#E8E8E8')
        .borderRadius(40)
        .onClick(() => {
          this.getUIContext().getRouter().back({
            url: 'pages/Index'
          });
        })

        Text('Loop rendering counterexamples')
          .fontSize(20)
          .fontWeight(700)
          .height(26)
      }
      .padding({ left: 16, right: 16 })
      .width('100%')
      .height(56)
      .margin({ top: 36 })

      Column({ space: 5 }) {
        Grid() {
          LazyForEach(this.data, (item: number) => {
            GridItem() {
              // Use reusable custom components
              ReusableChildComponent({ item: item })
            }
          }, (item: number) => item.toString())
        }
        // [StartExclude negative_of_lazy_for_each]
        .cachedCount(2)
        .columnsTemplate('1fr 1fr 1fr 1fr')
        .columnsGap(10)
        .rowsGap(10)

        // [EndExclude negative_of_lazy_for_each]
      }
      .margin({ top: 12 })
    }
    .backgroundColor('#F5F5F5')
  }

  // [End negative_of_lazy_for_each]
}

// Custom components are decorated @Reusable decorators, which signify their ability to be reused by components
@Reusable
@Component
struct ReusableChildComponent {
  @State item: number = 0;

  // Simulate time-consuming operations with loop functions
  count(): number {
    let temp: number = 0;
    for (let index = 0; index < 1000000; index++) {
      temp += 1;
    }
    return temp;
  }

  // [Start about_to_reuse]
  // [StartExclude about_to_reuse]
  // [EndExclude about_to_reuse]
  aboutToReuse(params: Record<string, number>): void {
    hiTraceMeter.startTrace('ReuseOfGrid', 1001);
    this.item = params.item;
    // Simulate time-consuming operations
    this.count();
    hiTraceMeter.finishTrace('ReuseOfGrid', 1001)
  }
  // [StartExclude about_to_reuse]
  // [EndExclude about_to_reuse]
  // [Start about_to_reuse]

  build() {
    Column() {
      Image($r('app.media.app_icon'))
        .width(48)
        .height(48)
        .margin({ bottom: 12 })
      Text(`image${this.item}`)
        .fontSize(12)
        .textAlign(TextAlign.Center)
    }
    .width('100%')
    .height(100)
  }
}