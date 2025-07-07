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

@Entry
@Component
struct Index {
  @Provide('pageInfos') pageInfos: NavPathStack = new NavPathStack();

  build() {
    Navigation(this.pageInfos) {
      Column({ space: 12 }) {
        Button($r('app.string.side_navigation'))
          .width('100%')
          .onClick(() => {
            this.pageInfos.pushPathByName('TabsPage', null, true);
          })
          .constraintSize({
            maxWidth: 448
          })
        Button($r('app.string.keyboard_keys'))
          .width('100%')
          .onClick(() => {
            this.pageInfos.pushPathByName('KeyboardPage', null, true);
          })
          .constraintSize({
            maxWidth: 448
          })
        Button($r('app.string.menu_listening'))
          .width('100%')
          .onClick(() => {
            this.pageInfos.pushPathByName('VideoPage', null, true);
          })
          .constraintSize({
            maxWidth: 448
          })
      }
      .width('100%')
      .height('100%')
      .padding(16)
      .justifyContent(FlexAlign.End)
    }
    .title($r('app.string.smart_screen_practice'))
    .mode(NavigationMode.Stack)
    .backgroundColor($r('sys.color.container_modal_unfocus_background'))
  }
}