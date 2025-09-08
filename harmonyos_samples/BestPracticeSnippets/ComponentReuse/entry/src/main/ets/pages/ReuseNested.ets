/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

// [Start ReuseNested]
import { MyDataSource } from '../common/CommonConstants';

@Entry
@Component
struct ReuseNested {
  private data: MyDataSource = new MyDataSource();

  aboutToAppear(): void {
    for (let i = 0; i < 1000; i++) {
      this.data.pushData(i.toString());
    }
  }

  build() {
    List({ space: 40 }) {
      LazyForEach(this.data, (item: string, index: number) => {
        ListItem() {
          if (index % 3 === 0) {
            ReusableComponentOne({ item: item })
          } else if (index % 5 === 0) {
            ReusableComponentTwo({ item: item })
          } else {
            ReusableComponentThree({ item: item })
          }
        }
        .backgroundColor('#cccccc')
        .width('100%')
        .onAppear(() => {
          console.info(`ListItem ${index} onAppear`);
        })
      })
    }
    .width('100%')
    .height('100%')
    .cachedCount(0)
  }
}

@Builder
function ReusableComponentOne($$: ESObject) {
  Column() {
    ChildComponentA({ item: $$.item })
    ChildComponentB({ item: $$.item })
    ChildComponentC({ item: $$.item })
  }
}

@Builder
function ReusableComponentTwo($$: ESObject) {
  Column() {
    ChildComponentA({ item: $$.item })
    ChildComponentC({ item: $$.item })
    ChildComponentD({ item: $$.item })
  }
}

@Builder
function ReusableComponentThree($$: ESObject) {
  Column() {
    ChildComponentA({ item: $$.item })
    ChildComponentB({ item: $$.item })
    ChildComponentD({ item: $$.item })
  }
}

@Reusable
@Component
struct ChildComponentA {
  @State item: string = '';

  aboutToReuse(params: ESObject): void {
    console.info(`ChildComponentA ${params.item} Reuse ${this.item}`);
    this.item = params.item;
  }

  aboutToRecycle(): void {
    console.info(`ChildComponentA ${this.item} Recycle`);
  }

  build() {
    Column() {
      Text(`Item ${this.item} Child Component A`)
        .fontSize(20)
        .margin({ left: 10 })
        .fontColor(Color.Blue)
      Grid() {
        ForEach((new Array(20)).fill(''), (item: string, index: number) => {
          GridItem() {
            Image($r('app.media.startIcon'))
              .height(20)
          }
        })

      }
    }
    .margin({ left: 10, right: 10 })
    .backgroundColor(0xFAEEE0)
  }
}

@Reusable
@Component
struct ChildComponentB {
  @State item: string = '';

  aboutToReuse(params: ESObject): void {
    this.item = params.item;
  }

  build() {
    Row() {
      Text(`Item ${this.item} Child Component B`)
        .fontSize(20)
        .margin({ left: 10 })
        .fontColor(Color.Red)
    }
    .margin({ left: 10, right: 10 })
  }
}

@Reusable
@Component
struct ChildComponentC {
  @State item: string = '';

  aboutToReuse(params: ESObject): void {
    this.item = params.item;
  }

  build() {
    Row() {
      Text(`Item ${this.item} Child Component C`)
        .fontSize(20)
        .margin({ left: 10 })
        .fontColor(Color.Green)
    }
    .margin({ left: 10, right: 10 })
  }
}

@Reusable
@Component
struct ChildComponentD {
  @State item: string = '';

  aboutToReuse(params: ESObject): void {
    this.item = params.item;
  }

  build() {
    Row() {
      Text(`Item ${this.item} Child Component D`)
        .fontSize(20)
        .margin({ left: 10 })
        .fontColor(Color.Orange)
    }
    .margin({ left: 10, right: 10 })
  }
}

// [End ReuseNested]
