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

// [Start tabs_negative_example]
@Entry
@Component
struct TabsNegativeExample {
  // ...
  private controller: TabsController = new TabsController();


  // ...


  build() {
    Column() {
      // ...


      Tabs({ barPosition: BarPosition.Start, controller: this.controller }) {
        // ...


      }
      .barHeight(0)
      .layoutWeight(1)
      .animationDuration(1000)
      .scrollable(false)
    }
    .width('100%')
  }
}
// [End tabs_negative_example]