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

@Component
export struct ViewCodeText {
  @StorageLink('sideBarShow') sideBarShow: boolean = false;
  private webSrc: Resource =  $rawfile('BaseTypeCode.ets.html');

  build() {
    Column(){
      Text($r('app.string.source_code'))
        .fontColor($r('app.color.source_code_font_color'))
        .fontSize($r('app.float.source_code_font_size'))
        .alignSelf(ItemAlign.End)
        .id('viewSourceCode')
        .onClick(() => {
          // Click to view source code.
          AppStorage.setOrCreate('sideBarShow', !this.sideBarShow);
          AppStorage.setOrCreate('webSrc', this.webSrc);
        })
    }
    .width('100%')
    .justifyContent(FlexAlign.End)
  }
}