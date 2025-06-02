/*
* Copyright (C) 2024 Huawei Device Co., Ltd.
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

/*
* 最佳实践: 点击完成时延分析
*/

import { AudioPlayerService } from '../components/AudioPlayerService';
import { AudioPlayerStatus } from '../components/AudioPlayerStatus';
import { ContinueModel } from '../components/ContinueModel';

@Entry
@Component
struct Index {
  @State message: string = 'Hello World';
  @State currentIndex: number = 0;

  build() {
    RelativeContainer() {
      // [Start tabs]
      // products\phone\src\main\ets\pages\MainPage.ets

      Tabs({ index: this.currentIndex }) {
        // ...
      }
      .layoutWeight(1)
      .barHeight(0)
      .scrollable(false)
      .onChange((index) => {
        this.currentIndex = index;
        ContinueModel.getInstance().data.mainTabIndex = index;
        if (AppStorage.get('audioPlayerStatus') !== AudioPlayerStatus.IDLE) {
          AudioPlayerService.getInstance().stop().then(() => {
            AudioPlayerService.destroy();
          });
        }
      })
      // [End tabs]
    }
    .height('100%')
    .width('100%')
  }
}