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

import { router } from '@kit.ArkUI'

@Entry
@Component
struct MainPage {
  build() {
    Column() {
      Text('Network Boost')
        .width('100%')
        .height(48)
        .margin({ top: '7.2%' })
        .fontColor('#000000')
        .fontSize(36)
        .fontWeight(500)
      Divider().color('gray').strokeWidth(1)
      Button({ type: ButtonType.Normal }) {
        Column() {
          Text('网络质量评估')
            .fontSize(24)
            .fontWeight(500)
            .fontColor('#FFFFFFFF')
        }
        .width(190)
        .height('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 20, top: 30 })
      }
      .width('100%')
      .height(80)
      .margin({ top: '11%' })
      .borderRadius(24)
      .onClick(this.getNetworkQos)

      Button({ type: ButtonType.Normal }) {
        Column() {
          Text('网络场景识别')
            .fontSize(24)
            .fontWeight(500)
            .fontColor('#FFFFFFFF')
        }
        .width(190)
        .height('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 20, top: 30 })
      }
      .width('100%')
      .height(80)
      .margin({ top: '11%' })
      .borderRadius(24)
      .onClick(this.getNetworkScene)

      Button({ type: ButtonType.Normal }) {
        Column() {
          Text('传输体验反馈')
            .fontSize(24)
            .fontWeight(500)
            .fontColor('#FFFFFFFF')
        }
        .width(190)
        .height('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 20, top: 30 })
      }
      .width('100%')
      .height(80)
      .margin({ top: '11%' })
      .borderRadius(24)
      .onClick(this.getReportQoe)

      Button({ type: ButtonType.Normal }) {
        Column() {
          Text('连接迁移通知')
            .fontSize(24)
            .fontWeight(500)
            .fontColor('#FFFFFFFF')
        }
        .width(190)
        .height('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 20, top: 30 })
      }
      .width('100%')
      .height(80)
      .margin({ top: '11%' })
      .borderRadius(24)
      .onClick(this.getHandover)

      Button({ type: ButtonType.Normal }) {
        Column() {
          Text('迁移模式设置')
            .fontSize(24)
            .fontWeight(500)
            .fontColor('#FFFFFFFF')
        }
        .width(190)
        .height('100%')
        .alignItems(HorizontalAlign.Start)
        .padding({ left: 20, top: 30 })
      }
      .width('100%')
      .height(80)
      .margin({ top: '11%' })
      .borderRadius(24)
      .onClick(this.getSetHandoverMode)
    }
    .linearGradient({
      direction: GradientDirection.Top,
      repeating: true,
      colors: [['#FFFFFF', 0], ['#FFFFFF', 1]]
    })
    .height('100%')
    .width('100%')
    .padding({ left: '16vp', right: '16vp' })
    .alignItems(HorizontalAlign.Start)
  }

  private getNetworkQos = () => {
    router.pushUrl({ url: 'pages/NetworkQos' });
  }
  private getNetworkScene = () => {
    router.pushUrl({ url: 'pages/NetworkScene' });
  }
  private getReportQoe = () => {
    router.pushUrl({ url: 'pages/ReportQoe' });
  }
  private getHandover = () => {
    router.pushUrl({ url: 'pages/NetworkHandover' });
  }
  private getSetHandoverMode = () => {
    router.pushUrl({ url: 'pages/SetHandoverMode' });
  }
}