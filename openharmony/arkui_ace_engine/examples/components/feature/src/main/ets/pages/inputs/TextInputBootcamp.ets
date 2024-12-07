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

import { Drawer } from 'common/src/main/ets/components/Drawer';
import { RadioBlock, ColorBlock, useEnabled } from 'common';
import { LengthMetrics } from '@kit.ArkUI';


@Component
export struct TextInputBootcamp {
  @Require @Prop title: ResourceStr;
  @State showParameters: boolean = false
  @State isShow: boolean = false
  @State message1: string =
    'They can be classified as built-in components–those directly provided \n\n by the ArkUI framework and custom components – those defined by developers ' +
      'The built-in components include buttons radio buttonsprogress indicators and text You can set the rendering effectof thesecomponents in method chaining mode,\n\n' +
      'page components are divided into independent UI units to implementindependent creation development and reuse of different units on pages making pages more engineering-oriented.';
  @State enablebackgroundColor: boolean = false;
  @State ebackgroundColor: ResourceColor = 'rgb(255, 0, 0, 0.9)';
  @State enablecaretColor: boolean = false;
  @State ecaretColor: ResourceColor = 'rgb(255, 0, 0, 0.9)';
  @State enableTextStyle: boolean = false;
  @State textStyle: TextInputStyle = TextInputStyle.Default;
  @State enableTextAlign: boolean = false;
  @State textAlign: TextAlign = TextAlign.Start;

  build() {
    NavDestination() {
      Drawer({
        title: this.title,
        showParameters: $showParameters,
        content: () => {
          this.Content()
        },
        parameters: () => {
          this.Parameters()
        }
      })
    }
    .backgroundColor('#f1f3f5')
    .hideTitleBar(true)
  }

  @Builder
  Content() {
    Row() {
      TextInput({ text: this.message1 })
        .onChange((value: string) => {
          this.message1 = value
        })
        .caretColor(useEnabled(this.enablebackgroundColor, this.ebackgroundColor))
        .selectedBackgroundColor(useEnabled(this.enablecaretColor, this.ecaretColor))
        .style(useEnabled(this.enableTextStyle, this.textStyle))
        .textAlign(useEnabled(this.enableTextAlign, this.textAlign))
        .maxLines(10)
        .margin({
          start: LengthMetrics.vp(24),
          end: LengthMetrics.vp(24)
        })
    }.justifyContent(FlexAlign.Start)
  }

  @Builder
  Parameters() {
    Scroll() {
      Column({ space: 8 }) {
        RadioBlock({
          title: 'textStyle',
          isEnabled: $enableTextStyle,
          value: $textStyle,
          dataSource: [
            { label: 'Default', value: TextInputStyle.Default },
            { label: 'Inline', value: TextInputStyle.Inline }
          ]
        })
        RadioBlock({
          title: 'textAlign',
          isEnabled: $enableTextAlign,
          value: $textAlign,
          dataSource: [
            { label: 'Start', value: TextAlign.Start },
            { label: 'Center', value: TextAlign.Center },
            { label: 'End', value: TextAlign.End },
            { label: 'JUSTIFY', value: TextAlign.JUSTIFY }
          ]
        })
        ColorBlock({
          title: '光标颜色',
          isEnabled: this.enablebackgroundColor,
          color: this.ebackgroundColor
        })
        ColorBlock({
          title: '背景颜色',
          isEnabled: this.enablecaretColor,
          color: this.ecaretColor
        })
      }.width('100%')
      .height('100%')
    }
    .height('52%')
  }
}

@Preview
@Component
struct TextInputBootcampPreviewer {
  build() {
    TextInputBootcamp({
      title: '单行文本框/TextInput'
    })
  }
}