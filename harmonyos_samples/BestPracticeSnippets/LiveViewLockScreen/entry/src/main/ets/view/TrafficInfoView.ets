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
import { LiveView } from "../utils/LiveView"

@Component
export struct TrafficInfoView {
  @State private isStart: boolean = true;

  build() {
    Row() {
      Column() {
        Text($r('app.string.traffic_info_first'))
          .fontWeight(FontWeight.Bold)
          .alignSelf(ItemAlign.Start)
          .fontSize($r('sys.float.padding_level12'))
          .margin({
            top: $r('sys.float.padding_level8'),
            left: $r('sys.float.padding_level12'),
          })
        Row() {
          Text($r('app.string.traffic_info_second'))
            .fontColor(Color.Grey)
            .fontWeight(FontWeight.Bold)
            .alignSelf(ItemAlign.Start)
            .fontSize($r('sys.float.padding_level8'))
          Image($r('app.media.traffic_light'))
            .height($r('sys.float.padding_level8'))
          Text($r('app.string.traffic_info_third'))
            .fontColor(Color.Grey)
            .fontWeight(FontWeight.Bold)
            .alignSelf(ItemAlign.Start)
            .fontSize($r('sys.float.padding_level8'))
        }
        .padding({
          top: $r('sys.float.padding_level4'),
          left: $r('sys.float.padding_level12'),
          bottom: $r('sys.float.padding_level16'),
        })
      }
      .alignItems(HorizontalAlign.Start)

      Column() {
        Button('开始导航')
          .fontColor(this.isStart ? $r('sys.color.comp_background_emphasize') : Color.White)
          .backgroundColor(this.isStart ? $r('sys.color.comp_background_secondary') :
          $r('sys.color.comp_background_emphasize'))
          .fontSize(12)
          .width(80)
          .height(30)
          .onClick(() => {
            this.isStart = true;
            LiveView.getInstance().createLiveView();
          })
        Button('结束导航')
          .fontColor(!this.isStart ? $r('sys.color.comp_background_emphasize') : Color.White)
          .backgroundColor(!this.isStart ? $r('sys.color.comp_background_secondary') :
          $r('sys.color.comp_background_emphasize'))
          .fontSize(12)
          .width(80)
          .height(30)
          .onClick(() => {
            this.isStart = false;
            LiveView.getInstance().finishLiveView();
          })
          .margin({
            top: $r('sys.float.padding_level4')
          })
      }
      .position({
        top: 10,
        right: 16,
      })
    }
    .width('100%')
  }
}