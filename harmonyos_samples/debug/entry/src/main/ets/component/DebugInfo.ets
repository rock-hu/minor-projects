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

@Component
export default struct DebugInfo {
  @Prop debugWindow: boolean;
  @Link pssMemory: number;
  @Link sharedMemory: number;
  @Link privateMemory: number;

  build() {
    Stack({ alignContent: Alignment.Top }) {
      Text($r('app.string.navigation_information'))
        .fontSize(25)
        .width('80%')
        .lineHeight(40)
        .margin({ top: '5%' })
        .fontWeight(FontWeight.Bold)

      if (this.debugWindow) {
        Column() {
          this.showDebug($r('app.string.pss'), this.pssMemory);
          this.showDebug($r('app.string.sharedDirty'), this.sharedMemory);
          this.showDebug($r('app.string.privateDirty'), this.privateMemory);
        }
        .id('debugInfo')
        .width('85%')
        .height('25%')
        .margin({ top: '50%' })
        .padding({ bottom: '5%', top: '5%' })
        .backgroundColor(Color.White)
        .border({ width: 2, radius: 10 })
      }
    }
    .width('100%')
    .height('100%')
  }

  @Builder
  // Debug显示信息复用组件
  showDebug(title: Resource, data: number) {
    Row() {
      Text(title)
        .id('title')
        .fontSize(20)
        .width('60%')
        .margin({ left: 6 })
        .textAlign(TextAlign.End)

      Text(`${data}KB`)
        .id('storageSize')
        .fontSize(20)
    }
    .width('100%')
    .margin({ top: 10 })
  }
}