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

export interface  NavListItem {
  name: string;
  path: string;
}

@Component
export struct NavList {
  pages: NavListItem[] = []
  onPathChange: (item: NavListItem) => void = () => {};

  build() {
    Column() {
      ForEach(this.pages, (item: NavListItem) => {
        Button(item.name)
          .stateEffect(true)
          .type(ButtonType.Capsule)
          .width('80%')
          .height(40)
          .margin(20)
          .onClick(() =>  this.onPathChange(item))
      })
    }
  }
}