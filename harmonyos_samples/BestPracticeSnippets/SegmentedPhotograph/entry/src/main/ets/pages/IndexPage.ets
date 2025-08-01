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

import { Constants } from '../common/Constants';

@Entry
@Component
struct IndexPage {
  build() {
    Column() {
      Text($r('app.string.camera_segment_photography'))
        .width('100%')
        .textAlign(TextAlign.Start)
        .fontSize(32)
        .lineHeight(40)
        .fontWeight(700)
        .margin({ top: 106 })
      Blank()
      Column() {
        Button($r('app.string.single_segment_photography'))
          .width('100%')
          .fontSize(16)
          .height(42)
          .onClick(() => {
            AppStorage.setOrCreate('photoMode', Constants.SINGLE_STAGE_MODE);
            this.getUIContext().getRouter().pushUrl({ url: 'pages/PhotoPage' });
          })
        Button($r('app.string.segmented_photography'))
          .height(42)
          .fontSize(16)
          .margin({ top: 16 })
          .width('100%')
          .onClick(() => {
            AppStorage.setOrCreate('photoMode', Constants.SUBSECTION_MODE);
            this.getUIContext().getRouter().pushUrl({ url: 'pages/PhotoPage' });
          })

      }.margin({ bottom: 46 })
    }
    .width('100%')
    .height('100%')
    .padding({ left: 16, right: 16 })
    .backgroundColor('rgb(241, 243, 245)')
    .alignItems(HorizontalAlign.Center)
  }
}
