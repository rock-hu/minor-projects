/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { CodeView } from '../../../commoncomponents/CodeView';
import { ViewCodeText } from '../../../commoncomponents/ViewCodeText';
import { TitleBarDark } from '../../../commoncomponents/TitleBarDark';
import { TitleBar } from '../../../commoncomponents/TitleBar';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');
let storage: LocalStorage = uiContext?.getSharedLocalStorage()!;

@Entry(storage)
@Component
struct StorageLinkAbilityPage {
  @State title: Resource = $r('app.string.localStorage_pagetitletwo');
  // Synchronize night mode status with home page and Page2.
  @LocalStorageLink('currentModelStatus') currentModelStatus: boolean = true;
  // And home page, Page2 bidirectional synchronization content font size.
  @LocalStorageLink('contentFontSize') contentFontSize: number = 18;

  build() {
    Column() {
      if (this.currentModelStatus) {
        TitleBarDark({ title: $title })
      } else {
        TitleBar({ title: $title })
      }
      CodeView({ title: $title, isShowTitle: false }) {
        Column({ space: 30 }) {
          // View source code.
          ViewCodeText({ webSrc: $rawfile('LocalStorageLinkPage.ets.html') })
          Text($r('app.string.localStorage_both_data_binding'))
            .fontSize(this.contentFontSize)
            .fontColor(this.currentModelStatus ? $r('app.color.color_white') : $r('app.color.tips_font_color'))
            .width('100%')
            .textAlign(TextAlign.Center)
          Text($r('app.string.localStorage_single_data_binding'))
            .fontSize(this.contentFontSize)
            .fontColor(this.currentModelStatus ? $r('app.color.color_white') : $r('app.color.tips_font_color'))
            .width('100%')
            .textAlign(TextAlign.Center)
          Button() {
            Text(this.currentModelStatus ? $r('app.string.close_nightmode') : $r('app.string.start_nightmode'))
              .fontColor(this.currentModelStatus ? $r('app.color.color_white') : $r('app.color.button_text_color'))
              .fontSize(this.contentFontSize)
              .width('60%')
              .textAlign(TextAlign.Center)
          }
          .id('localStorageLinkPage')
          .type(ButtonType.Capsule)
          .backgroundColor($r('app.color.button_background_color'))
          .padding($r('app.float.page_padding'))
          .onClick(() => {
            // Updated variable data that controls the night mode state.
            // The data variables associated with @LocalStorageLink and @StorageProp were refreshed.
            this.currentModelStatus = !this.currentModelStatus;
          })

          Text($r('app.string.localStorage_nigntmode_both_binding'))
            .fontSize(this.contentFontSize)
            .fontColor(this.currentModelStatus ? $r('app.color.color_white') : $r('app.color.tips_font_color'))
            .width('100%')
            .textAlign(TextAlign.Center)
        }
        .height('100%')
        .padding($r('app.float.page_padding'))
      }
    }
    .width('100%')
    .height('100%')
    .backgroundColor(this.currentModelStatus ? $r('app.color.nightnode_color') : $r('app.color.background_shallow_grey'))
  }

  aboutToDisappear(): void {
    AppStorage.setOrCreate('sideBarShow', false);
  }
}