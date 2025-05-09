/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

import { IntroductionTitle } from '../../../../common/IntroductionTitle';
import ShowToast from '../../../../util/ShowToast';
import { TitleBar } from '../../../../common/TitleBar';
import { LEFT_PADDING, RIGHT_PADDING } from '../../ComponentData';
import { textAreaControlPanel } from './ChangePanel';

@Extend(Column)
function cardStyle() {
  .borderRadius(16)
  .backgroundColor($r('app.color.white'))
  .width('100%')
  .padding(12)
}

@Entry
@Component
struct TextAreaSample {
  @State textAreaSet: string = '';
  @State placeholderColor: Color = Color.Blue;
  @State textAlign: TextAlign = TextAlign.Start;
  @State caretColor: Color = Color.Blue;
  @State copyOptions: CopyOptions = CopyOptions.None;
  controller: TextAreaController = new TextAreaController();

  build() {
    Column() {
      TitleBar({ title: $r('app.string.textarea_title') })
      Scroll() {
        Column() {
          IntroductionTitle({ introduction: $r('app.string.textarea_show') })
          TextArea({
            text: this.textAreaSet,
            placeholder: 'This textArea can contain more than one line, please input your word.',
            controller: this.controller
          })
            .id(`textarea_001`)
            .placeholderColor(this.placeholderColor)
            .placeholderFont({ size: 20, weight: 400 })
            .backgroundColor($r('app.color.white'))
            .textAlign(this.textAlign)
            .caretColor(this.caretColor)
            .copyOption(this.copyOptions)
            .borderRadius(16)
            .width('100%')
            .height(85)
            .fontSize(20)
            .enableKeyboardOnFocus(false)
            .onChange(() => {
              ShowToast.shortToast('The input have changed.')
            })
            .onCopy(() => {
              ShowToast.shortToast('copy')
            })
            .onCut(() => {
              ShowToast.shortToast('cut')
            })
            .onPaste(() => {
              ShowToast.shortToast('paste')
            })
          IntroductionTitle({ introduction: $r('app.string.textarea_control') })
          Column() {
            textAreaControlPanel({
              placeholderColor: $placeholderColor,
              textAlign: $textAlign,
              caretColor: $caretColor,
              copyOption: $copyOptions
            })
          }.cardStyle()
        }
        .width('100%')
      }
      .padding({ left: LEFT_PADDING, right: RIGHT_PADDING })
      .margin({ bottom: 100 })
    }
    .backgroundColor($r('app.color.background_light_gray'))
    .height('100%')
  }
}