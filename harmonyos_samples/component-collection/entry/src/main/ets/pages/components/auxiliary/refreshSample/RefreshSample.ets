/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { TitleBar } from '../../../../common/TitleBar';

@Extend(Text)
function leftTitleStyle() {
  .fontColor($r('app.color.sub_title_color'))
  .fontSize(14)
  .height(22)
}

@Extend(Text)
function subTitleStyle() {
  .fontColor($r('app.color.sub_title_color'))
  .fontSize(14)
  .height(22)
}

@Extend(Column)
function cardStyle() {
  .backgroundColor(Color.White)
  .borderRadius(24)
  .width('100%')
  .padding(15)
}

@Extend(ListItem)
function itemCardStyle() {
  .backgroundColor(Color.White)
  .borderRadius(12)
  .width('100%')
  .padding(10)
}

@Entry
@Component
struct RefreshSample {
  @State counter: number = 0;
  @State isRefreshing: boolean = false;
  @State mOffset: number = 0;
  @State mFriction: number = 1;
  private dataArray: number[] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];

  @Builder
  refreshBuilder() {
    Column() {
      Text($r('app.string.auxiliary_refresh_refreshing'))
        .fontSize(16)
    }
    .alignItems(HorizontalAlign.Center)
    .justifyContent(FlexAlign.Center)
    .padding(16)
    .borderRadius(12)
  }

  build() {
    Column() {
      TitleBar({ title: $r('app.string.auxiliary_refresh') })
      Column() {
        Refresh({
          refreshing: $$this.isRefreshing,
          builder: this.refreshBuilder
        }) {
          Scroll() {
            Column() {
              ForEach(this.dataArray, (item: number) => {
                Stack() {
                  Text((item + this.counter).toString())
                    .fontSize(20)
                }.width('100%')
                .height(60)
                .backgroundColor($r('app.color.white'))
                .borderRadius(10)

                Blank(6)
              })
            }.width('100%')
          }.width('100%')
          .height(300)
        }
        .width('100%')
        .height(300)
        .onRefreshing(() => {
          setTimeout(() => {
            this.counter++;
            this.isRefreshing = false;
          }, 1000)
        })
        .refreshOffset(this.mOffset)
        .pullDownRatio(this.mFriction)

        Blank(20)
        Column() {
          Text($r('app.string.auxiliary_refresh_offset'))
            .leftTitleStyle()
            .alignSelf(ItemAlign.Start)
          Row() {
            Text($r('app.string.auxiliary_refresh_minvalue'))
              .leftTitleStyle()
            Slider({ value: this.mOffset, min: 0, max: 100 })
              .onChange((value) => {
                this.mOffset = value;
              })
              .layoutWeight(1)
            Text($r('app.string.auxiliary_refresh_maxvalue'))
              .leftTitleStyle()
          }.width('100%')
          .padding(12)
        }.cardStyle()

        Blank(20)
        Column() {
          Text($r('app.string.auxiliary_refresh_friction'))
            .leftTitleStyle()
            .alignSelf(ItemAlign.Start)
          Row() {
            Text($r('app.string.auxiliary_refresh_minvalue'))
              .leftTitleStyle()
            Slider({ value: this.mFriction, min: 0, max: 1, step: 0.01 })
              .onChange((value) => {
                this.mFriction = value;
              })
              .layoutWeight(1)
            Text($r('app.string.auxiliary_refresh_friction_maxvalue'))
              .leftTitleStyle()
          }.width('100%')
          .padding(12)
        }
      }.padding(12)
    }.backgroundColor($r('app.color.background_light_gray'))
    .height('100%')
  }
}