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

// [Start ExpandSafeAreaIndex]
@Entry
@Component
struct ExpandSafeAreaIndex {
  build() {
    Column(){
      // Sub-component representing the content area
      // designed to achieve automatic content avoidance around the notch area.
      Column(){

      }
      .width('100%')
      .height(50)
      // Passing an empty array indicates that the sub-component will not extend any safe area.
      // thereby achieving automatic avoidance of the punch-hole/notch area.
      .expandSafeArea([])
      .backgroundColor(Color.Blue)
    }
    .width('100%')
    .height('100%')
    // By default, the root container expands to accommodate system bars, punch-hole area, and keyboard.
    .expandSafeArea()
    .backgroundColor(Color.Red)
  }
}
// [End ExpandSafeAreaIndex]