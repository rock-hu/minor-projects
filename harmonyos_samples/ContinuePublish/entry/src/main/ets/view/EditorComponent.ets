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

import { CommonConstants } from '../constants/CommonConstants';

@Component
export struct EditorComponent {
  @StorageLink('mainTitle') mainTitle: string = '';
  @StorageLink('textContent') textContent: string = '';
  // Whether to display the bottom toolbar.
  @Link isShowToolbar: boolean;
  // Whether to display the teletext title
  @Link isKeyboard: boolean;
  @Link isShowLocalInfo: boolean;

  build() {
    Flex({ direction: FlexDirection.Column }) {
      TextInput({ text: this.mainTitle, placeholder: $r('app.string.text_input_placeholder') })
        .onChange((mainTitle: string) => {
          this.mainTitle = mainTitle;
          AppStorage.set('mainTitle', mainTitle);
        })
        .onFocus(() => {
          this.isShowToolbar = false;
        })
        .width(CommonConstants.FULL_PERCENT)
        .height($r('app.integer.text_input_height'))
        .fontSize($r('app.integer.text_size_body1'))
        .backgroundColor(Color.White)
        .constraintSize({ minHeight: $r('app.integer.text_input_height') })
        .margin({
          top: $r('app.integer.text_input_margin')
        })

      TextArea({ text: this.textContent, placeholder: $r('app.string.richEditor_placeholder') })
        .width(CommonConstants.FULL_PERCENT)
        .height(CommonConstants.FULL_PERCENT)
        .id(CommonConstants.TITLE_ID)
        .fontSize($r('app.integer.text_size_body1'))
        .backgroundColor(Color.White)
        .constraintSize({ minHeight: $r('app.integer.text_input_height') })
        .margin({
          top: $r('app.integer.text_input_margin')
        })
        .onFocus(() => {
          this.isShowToolbar = true;
          this.isKeyboard = true;
          this.isShowLocalInfo = false;
        })
        .onChange((textContent: string) => {
          this.textContent = textContent;
          AppStorage.set('textContent', textContent);
        })
    }
    .width(CommonConstants.FULL_PERCENT)
    .height($r('app.integer.flex_input_height'))
    .margin({ bottom: $r('app.integer.flex_input_margin') })
    .layoutWeight(CommonConstants.DEFAULT_LAYOUT_WEIGHT)
    .expandSafeArea([SafeAreaType.KEYBOARD])
  }
}