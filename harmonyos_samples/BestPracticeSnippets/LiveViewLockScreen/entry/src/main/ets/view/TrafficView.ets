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

import { Constants } from "../constant/Constant";
import { LaneData } from "../model/RouteDataModel";
import { TrafficInfoView } from "./TrafficInfoView";
import { BreakpointType } from "../utils/BreakpointUtil";

@Component
export struct TrafficView {
  @StorageLink('horizontalBreakpoint') horizontalBreakpoint: string = Constants.BREAK_POINT_SM;
  @StorageLink('verticalBreakpoint') verticalBreakpoint: string = Constants.BREAK_POINT_SM;
  @State roadInfo: LaneData | undefined = undefined;
  @State isShow: boolean = false;

  @Builder
  myBuilder() {
    Row() {
      TrafficInfoView()
    }
    .backgroundColor($r('sys.color.background_secondary'))
    .width('100%')
    .height('100%')
  }

  build() {
    Column() {
      Row() {
        Column() {
          Column() {
            Row() {
              Image($r('app.media.big_arrow'))
                .objectFit(ImageFit.Contain)
                .height(new BreakpointType(80, 70, 70).getValue(this.horizontalBreakpoint))
              Column() {
                Text($r('app.string.traffic_info_meter'))
                  .fontColor(Color.White)
                  .fontWeight(FontWeight.Bold)
                  .alignSelf(ItemAlign.Start)
                  .fontSize(40)
                  .margin({
                    top: $r('sys.float.padding_level8'),
                    left: $r('sys.float.padding_level8'),
                  })
                Text($r('app.string.road_name'))
                  .fontColor(Color.White)
                  .fontWeight(FontWeight.Regular)
                  .alignSelf(ItemAlign.Start)
                  .fontSize(24)
                  .margin({
                    left: $r('sys.float.padding_level8'),
                  })
              }
            }
            .borderRadius({
              topLeft: new BreakpointType(0, 16, 16).getValue(this.horizontalBreakpoint),
              topRight: new BreakpointType(0, 16, 16).getValue(this.horizontalBreakpoint),
            })
            .backgroundColor($r('app.color.Index_background_first'))
            .width(new BreakpointType('100%', '50%', '30%').getValue(this.horizontalBreakpoint))
            .padding({
              bottom: $r('sys.float.padding_level8'),
              top: new BreakpointType(this.verticalBreakpoint === Constants.BREAK_POINT_MD ?
                ('sys.float.padding_level8') : $r('sys.float.padding_level16'), $r('sys.float.padding_level0'),
                $r('sys.float.padding_level0')).getValue(this.horizontalBreakpoint),
              left: new BreakpointType($r('sys.float.padding_level8'), $r('sys.float.padding_level12'),
                $r('sys.float.padding_level16')).getValue(this.horizontalBreakpoint),
            })

            Row() {
              Text($r('app.string.then'))
                .fontColor(Color.White)
                .fontWeight(FontWeight.Regular)
                .alignSelf(ItemAlign.Start)
                .fontSize(22)
                .margin({
                  left: $r('sys.float.padding_level8'),
                  right: $r('sys.float.padding_level2'),
                })
              Image($r('app.media.little_arrow'))
                .objectFit(ImageFit.Contain)
                .height(new BreakpointType(25, 25, 25).getValue(this.horizontalBreakpoint))
            }
            .borderRadius({
              bottomLeft: new BreakpointType(0, 16, 16).getValue(this.horizontalBreakpoint),
              bottomRight: new BreakpointType(0, 16, 16).getValue(this.horizontalBreakpoint),
            })
            .padding({
              bottom: $r('sys.float.padding_level2'),
              top: $r('sys.float.padding_level2'),
            })
            .width(new BreakpointType('100%', '50%', '30%').getValue(this.horizontalBreakpoint))
            .backgroundColor($r('app.color.Index_background_second'))

            if (this.horizontalBreakpoint !== Constants.BREAK_POINT_SM) {
              Image($r('app.media.traffic_info_speed'))
                .objectFit(ImageFit.Contain)
                .height(80)
                .position({
                  top: 0,
                  left: this.horizontalBreakpoint === Constants.BREAK_POINT_MD ? '51%' : '31%',
                })
            }
          }

          Row() {
            if (this.horizontalBreakpoint === Constants.BREAK_POINT_SM) {
              Image($r('app.media.traffic_info_speed'))
                .objectFit(ImageFit.Contain)
                .height(70)
            }
            Image($r('app.media.traffic_info_label'))
              .objectFit(ImageFit.Contain)
              .height(new BreakpointType(70, 80, 80).getValue(this.horizontalBreakpoint))
          }
          .margin({
            left: new BreakpointType($r('sys.float.padding_level36'), $r('sys.float.padding_level12'),
              $r('sys.float.padding_level16')).getValue(this.horizontalBreakpoint),
            top: $r('sys.float.padding_level2'),
          })
          .width(new BreakpointType('100%', '40%', '30%').getValue(this.horizontalBreakpoint))
        }
        .padding({
          left: new BreakpointType($r('sys.float.padding_level0'), $r('sys.float.padding_level12'),
            $r('sys.float.padding_level16')).getValue(this.horizontalBreakpoint),
          top: new BreakpointType($r('sys.float.padding_level0'), $r('sys.float.padding_level16'),
            $r('sys.float.padding_level16')).getValue(this.horizontalBreakpoint),
        })
      }
      .width('100%')
      .justifyContent(FlexAlign.Start)

      Blank()

      Row() {
        Stack({ alignContent: Alignment.Top }) {
          Image($r('app.media.traffic_right'))
            .width(new BreakpointType(this.verticalBreakpoint === Constants.BREAK_POINT_MD ? '35%' : '70%', '50%',
              '30%').getValue(this.horizontalBreakpoint))
          Image($rawfile('traffic_light.svg'))
            .objectFit(ImageFit.Contain)
            .width(new BreakpointType('20%', '20%', '10%').getValue(this.horizontalBreakpoint))
            .position({
              left: '70%',
              top: 30
            })
        }
        .width('100%')
        .onClick(() => {
          if (this.horizontalBreakpoint === Constants.BREAK_POINT_SM &&
            this.verticalBreakpoint === Constants.BREAK_POINT_MD) {
            this.isShow = true;
          } else {
            this.isShow = false;
          }
        })
      }

      Blank()

      if (this.horizontalBreakpoint !== Constants.BREAK_POINT_SM ||
        this.verticalBreakpoint !== Constants.BREAK_POINT_MD) {
        Row() {
          TrafficInfoView()
        }
        .onSizeChange(() => {
          this.isShow = false;
        })
        .margin({
          left: new BreakpointType($r('sys.float.padding_level0'), $r('sys.float.padding_level12'),
            $r('sys.float.padding_level16')).getValue(this.horizontalBreakpoint),
        })
        .backgroundColor($r('sys.color.background_secondary'))
        .width(new BreakpointType('100%', '50%', '30%').getValue(this.horizontalBreakpoint))
        .height(120)
        .borderRadius({
          topLeft: 32,
          topRight: 32,
        })
      }
    }
    .bindSheet($$this.isShow, this.myBuilder(), {
      height: 90,
      width: 440,
      preferType: SheetType.BOTTOM,
      showClose: false,
    })
    .width('100%')
    .height('100%')
    .backgroundImage($r('app.media.traffic_background'))
    .backgroundImageSize(ImageSize.Cover)
    .backgroundImagePosition(Alignment.Center)
  }
}