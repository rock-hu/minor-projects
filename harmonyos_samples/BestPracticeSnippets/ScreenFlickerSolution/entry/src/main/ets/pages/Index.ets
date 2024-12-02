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

import { router } from '@kit.ArkUI';

@Entry
@Component
struct Index {
  build() {
    Column() {
      Text($r('app.string.screen_flicker_solution'))
        .width('100%')
        .textAlign(TextAlign.Start)
        .fontSize(30)
        .fontWeight(700)
        .lineHeight(40)
        .padding({ left: 12 })
      Column() {
        Button($r('app.string.continuous_click_scene_counterexample'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/ClickError' });
          })
          .margin({ bottom: 12 })

        Button($r('app.string.continuous_click_scene_positive_example'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/ClickRight' });
          })
          .margin({ bottom: 12 })

        Button($r('app.string.tabs_tab_switchover_scenario_counterexample'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/TabsError' });
          })
          .margin({ bottom: 12 })

        Button($r('app.string.tabs_tab_switchover_scenario_positive_example'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/TabsRight' });
          })
          .margin({ bottom: 12 })

        Button($r('app.string.drop_down_refresh_scenario_counterexample'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/PullToRefreshError' });
          })
          .margin({ bottom: 12 })

        Button($r('app.string.drop_down_refresh_scenario_positive_example'))
          .fontSize(16)
          .width('100%')
          .height(40)
          .fontWeight(FontWeight.Bold)
          .onClick(() => {
            router.pushUrl({ url: 'pages/PullToRefreshRight' });
          })
          .margin({ bottom: 12 })
      }
      .padding({ left: 16, right: 16 })
    }
    .padding({ top: 56, bottom: 16 })
    .justifyContent(FlexAlign.SpaceBetween)
    .height('100%')
    .width('100%')
  }
}

