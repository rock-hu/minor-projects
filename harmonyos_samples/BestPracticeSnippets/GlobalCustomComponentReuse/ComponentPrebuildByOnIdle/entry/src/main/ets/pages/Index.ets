﻿/*
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

import { IdleCallback } from '@ohos/customreusablepool/src/main/ets/pages/BuilderNodePoolDemo';
import { furnitureData, natureData } from '@ohos/customreusablepool/src/main/ets/data/MockData';
import { ViewItem } from '@ohos/customreusablepool/src/main/ets/data/ViewItem';

// [Start appear_router]
@Entry
@Component
struct Index {
  // [StartExclude appear_router]
  @State timer: number = 2;

  // [EndExclude appear_router]
  aboutToAppear(): void {
    // [StartExclude appear_router]
    let intervalId = setInterval(() => {
      if (this.timer === 1) {
        let uiContext = this.getUIContext();
        let router = uiContext.getRouter();
        router.pushUrl({ url: 'pages/OnIdlePage' });
        router.clear();
        clearInterval(intervalId)
      } else {
        this.timer--;
      }
    }, 1000)
    // [EndExclude appear_router]
    // Obtain analog data
    let viewItems: ViewItem[] = [];
    viewItems.push(...furnitureData());
    viewItems.push(...natureData());
    let context = this.getUIContext();
    // Open frame callback
    context.postFrameCallback(new IdleCallback(context, viewItems));
  }

  // [StartExclude appear_router]
  build() {
    RelativeContainer() {
      Image($r('app.media.ad_image'))
        .height('100%')
        .width('100%')
        .id('image')
        .alignRules({
          left: { anchor: '__container__', align: HorizontalAlign.Start },
          right: { anchor: '__container__', align: HorizontalAlign.End },
          top: { anchor: '__container__', align: VerticalAlign.Top },
          bottom: { anchor: '__container__', align: VerticalAlign.Bottom }
        })
      Text(this.timer + 's')
        .id('text')
        .textAlign(TextAlign.Center)
        .fontSize(16)
        .fontColor(Color.White)
        .padding(5)
        .alignRules({
          right: { anchor: '__container__', align: HorizontalAlign.End },
          top: { anchor: '__container__', align: VerticalAlign.Top }
        })
        .margin({
          right: 20,
          top: 30
        })
        .width(60)
        .backgroundColor('#B3CFFF')
        .borderRadius(25)
    }
    .width('100%')
    .height('100%')
  }

  // [EndExclude appear_router]
}
// [End  appear_router]