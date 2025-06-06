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
import { getStringData } from '../../../../data/ResourceDataHandle';
import { locationTypes, visibileTypes } from '../../UniversialData';

@Extend(Column)
function cardStyle() {
  .backgroundColor(Color.White)
  .borderRadius(24)
  .width('100%')
  .margin({ bottom: 12 })
}

@Extend(Text)
function subTitleStyle() {
  .fontColor($r('app.color.sub_title_color'))
  .fontSize(14)
  .height(22)
}

@Extend(Text)
function leftTitleStyle() {
  .fontSize(18)
  .width('100%')
  .direction(Direction.Ltr)
  .margin({ left: 24, top: 8, bottom: 8 })
  .fontColor($r('app.color.font_color_shallow'))
}

@Entry
@Component
struct Page {
  @State firstZindex: number = 0;
  @State secondZindex: number = 0;
  @State thirdZindex: number = 0;
  @State alignmentLocation: Alignment = Alignment.Center;
  @State offsetX: number = 0;
  @State offsetY: number = 0;
  @State selectIndex: number = 0;
  @State currentVisibility: Visibility = Visibility.Visible;
  @State visibleIndex: number = 0;
  @State contentOpacity: number = 0.5;
  @State currentTabIndex: number = 0;
  private visibilityArray: Visibility[] = [Visibility.Visible, Visibility.Hidden, Visibility.None];
  private alignments: Alignment[] = [Alignment.Center, Alignment.TopStart,
    Alignment.TopEnd, Alignment.Top, Alignment.Start,
    Alignment.End, Alignment.BottomStart,
    Alignment.Bottom, Alignment.BottomEnd];
  private selectArray: ValueInfo[] = locationTypes;
  private visibleArray: ValueInfo[] = visibileTypes;

  @Builder
  tabbarTitle(name: string | Resource, index: number) {
    Stack() {
      Text(name)
        .fontSize(22)
        .fontColor(this.currentTabIndex === index ? $r('app.color.title_colorone') : $r('app.color.title_colortwo'))

      if (this.currentTabIndex === index) {
        Row()
          .backgroundColor($r('app.color.title_colorone'))
          .width('100%')
          .height(2)
          .position({ y: 48 })
      }
    }.width(88)
    .height(56)
  }

  build() {
    Column() {
      TitleBar({ title: $r('app.string.components_display') })
      Tabs() {
        TabContent() {
          Column() {
            Stack() {
              Row()
                .offset({ x: -30, y: -30 })
                .width(150)
                .height(150)
                .backgroundColor(Color.Red)
                .zIndex(this.firstZindex)
              Row()
                .width(150)
                .height(150)
                .backgroundColor(Color.Blue)
                .zIndex(this.secondZindex)
              Row()
                .width(150)
                .height(150)
                .offset({ x: 30, y: 30 })
                .backgroundColor(Color.Yellow)
                .zIndex(this.thirdZindex)
            }
            .height(300)
            .width('100%')
            .backgroundColor(Color.White)
            .borderRadius(24)

            Text($r('app.string.z_index_display'))
              .subTitleStyle()
            Column() {
              Row() {
                Blank()
                Column() {
                  Text($r('app.string.Red'))
                    .fontSize(12)
                  Select([{ value: '1' }, { value: '2' }, { value: '3' }])
                    .value(this.firstZindex + "")
                    .onSelect((index: number) => {
                      this.firstZindex = index + 1;
                    }).id('test_select1')
                }

                Blank()
                Column() {
                  Text($r('app.string.Blue'))
                    .fontSize(12)
                  Select([{ value: '1' }, { value: '2' }, { value: '3' }])
                    .value(this.secondZindex + "")
                    .onSelect((index: number) => {
                      this.secondZindex = index + 1;
                    }).id('test_select2')
                }

                Blank()
                Column() {
                  Text($r('app.string.Yellow'))
                    .fontSize(12)
                  Select([{ value: '1' }, { value: '2' }, { value: '3' }])
                    .value(this.thirdZindex + "")
                    .onSelect((index: number) => {
                      this.thirdZindex = index + 1;
                    }).id('test_select3')
                }

                Blank()
              }
              .width('100%')
            }.cardStyle()
            .margin({ top: 20 })
          }
          .padding(12)
        }.tabBar(this.tabbarTitle($r('app.string.z_index'), 0))

        TabContent() {
          Column() {
            Column() {
              Column() {
                Row()
                  .width(200)
                  .height(200)
                  .backgroundColor($r('app.color.background_grey'))
                  .overlay(getStringData($r('app.string.components_display_content')), {
                    align: this.alignmentLocation,
                    offset: {
                      x: this.offsetX,
                      y: this.offsetY
                    }
                  })
              }.cardStyle()

              Text($r('app.string.effects_display'))
                .subTitleStyle()

              Text($r('app.string.positon_modify'))
                .leftTitleStyle()
              Column() {
                Select(this.selectArray).onSelect((index: number) => {
                  this.alignmentLocation = this.alignments[index];
                  this.selectIndex = index;;
                }).id('test_selectPosition')
                  .value(getStringData(this.selectArray[this.selectIndex].value))
              }.cardStyle()

              Text($r('app.string.offset_modify'))
                .leftTitleStyle()
              Column() {
                Row({ space: 20 }) {
                  Text($r('app.string.x_offset'))
                    .fontSize(15)
                  Slider({ min: -20, max: 20, step: 1 })
                    .onChange((value: number) => {
                      this.offsetX = value;
                    })
                    .layoutWeight(1)
                }.width('90%')

                Row({ space: 20 }) {
                  Text($r('app.string.y_offset'))
                    .fontSize(15)
                  Slider({ min: -20, max: 20, step: 1 })
                    .onChange((value: number) => {
                      this.offsetY = value;
                    })
                    .layoutWeight(1)
                }.width('90%')
              }.cardStyle()
            }.padding(12)
          }
        }.tabBar(this.tabbarTitle($r('app.string.overlay'), 1))

        TabContent() {
          Column() {
            Column({ space: 10 }) {
              Row()
                .width(100)
                .height(100)
                .backgroundColor(Color.Red)
              Row()
                .width(100)
                .height(100)
                .backgroundColor(Color.Blue)
                .visibility(this.currentVisibility)
                .animation({
                  duration: 2000
                })
              Row()
                .width(100)
                .height(100)
                .backgroundColor(Color.Yellow)
            }.cardStyle()

            Text($r('app.string.effects_display'))
              .subTitleStyle()
            Text($r('app.string.visibility_change'))
              .leftTitleStyle()
            Column() {
              Select(this.visibleArray)
                .value(getStringData(this.visibleArray[this.visibleIndex].value))
                .onSelect((index: number) => {
                  this.currentVisibility = this.visibilityArray[index];
                  this.visibleIndex = index;
                }).id('test_selectVisible')
                .selected(this.visibleIndex)
            }.cardStyle()
          }.width('100%')
          .padding(12)
        }.tabBar(this.tabbarTitle($r('app.string.visibility'), 2))

        TabContent() {
          Column() {
            Column() {
              Row()
                .width(200)
                .height(200)
                .backgroundColor(Color.Green)
                .opacity(this.contentOpacity)
            }.cardStyle()

            Text($r('app.string.opacity_display'))
              .subTitleStyle()

            Text($r('app.string.opacity_change'))
              .leftTitleStyle()
            Column() {
              Row() {
                Text('0')
                  .fontSize(15)
                Slider({ value: this.contentOpacity, min: 0, max: 1.0, step: 0.05 })
                  .onChange((value: number) => {
                    this.contentOpacity = value;
                  })
                Text('1')
                  .fontSize(15)
              }.width('90%')
              .padding({ left: 5, right: 15 })
            }.cardStyle()
          }.width('100%')
          .padding(12)
        }.tabBar(this.tabbarTitle($r('app.string.opacity'), 3))
      }.layoutWeight(1)
      .onChange((index) => {
        this.currentTabIndex = index
      })
      .barMode(BarMode.Scrollable)
    }
    .height('100%')
    .backgroundColor($r('app.color.background_light_gray'))
  }
}

class ValueInfo {
  value: Resource = $r('app.string.components_display');
}