/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { Base64 } from '@ohos/base64'

@Entry
@Component
struct EncodeTest {
  @State encodeTest: string = '';
  @State decodeTest: string = '';

  onPageShow() {
    let test: number[] = [1, 2, 3, 4, 5, 6, 7, 8, 9]
    let encode = Base64.encode(new Uint8Array(test));
    console.info("base64 escape onPageShow encode:" + Base64.bytesToString(encode));
    console.info("base64 escape onPageShow decode:" + Base64.bytesToString(Base64.decode(encode)));
  }

  build() {
    Column() {
      TextInput({ placeholder: 'input encode content' })
        .type(InputType.Normal)
        .placeholderColor(Color.Gray)
        .placeholderFont({
          size: 15,
          weight: FontWeight.Normal,
          family: "sans-serif",
          style: FontStyle.Normal
        })
        .enterKeyType(EnterKeyType.Done)
        .height(60)
        .fontSize(15)
        .fontStyle(FontStyle.Italic)
        .fontColor(Color.Black)
        .onChange((value: string) => {
          if (value !== "") {
            this.encodeTest = Base64.encodeToString(Base64.stringToBytes(value))
            this.decodeTest = Base64.bytesToString(Base64.decode(this.encodeTest))
          } else {
            this.encodeTest = "";
            this.decodeTest = "";
          }
        })
      Text() {
        Span($r('app.string.encrypted_data'))
        Span(':')
        Span(this.encodeTest)
      }
      .fontSize(15)
      .margin({ left: 10, top: 10 })
      .width('100%')

      Text() {
        Span($r('app.string.decrypt_encrypted_data'))
        Span(':')
        Span(this.encodeTest)
      }
      .fontSize(15)
      .margin({ left: 10, top: 10 })
      .width('100%')
    }
    .width('100%')
    .height('100%')
  }
}