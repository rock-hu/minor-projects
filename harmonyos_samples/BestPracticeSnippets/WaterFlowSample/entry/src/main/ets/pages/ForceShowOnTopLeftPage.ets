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
struct ForceShowOnTopLeftPage {
  private data: MyDataSource = new MyDataSource();

  aboutToAppear(): void {
    for(let i = 0; i < 80; i++) {
      this.data.pushData(i);
    }
  }

  build() {
    WaterFlow() {
      FlowItem() {
        Column() {
          ForEach(this.data.getTopMastData(5), (item: number) => {
            Text(`Top Hello ${item}`)
              .fontSize(35)
              .onAppear(() => {
                console.log(`top onAppear: ${item}`);
              })
          })
        }
      }
      .width('100%')
      .alignSelf(ItemAlign.End)
      .backgroundColor(Color.White)

      LazyForEach(this.data, (item: number, index: number) => {
        FlowItem() {
          Row() {
            Text(`Hello ${item}`)
              .fontSize(20)
              .onAppear(() => {
                console.log(`onAppear: ${item}`);
              })
          }
        }
        .width('100%')
        .height(30 + Math.random() * 30)
        .backgroundColor(Color.White)
      }, (item: number) => item.toString())
    }
    .cachedCount(5)
    .columnsTemplate('1fr 1fr')
    .backgroundColor('#efefef')
    .columnsGap(10)
    .rowsGap(5)
  }
}
