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
@Entry
@Component
struct Index {
  build() {
    Column() {
      Button('BuildParamPage')
        .width(120).margin({ top: 16 })
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/BuilderParamPage' })
        })
      Button('ControllerCallPage')
        .width(120).margin({ top: 16 })
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/ControllerCallPage' })
        })
      Button('EmitterCallPage')
        .width(120).margin({ top: 16 })
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/EmitterCallPage' })
        })
      Button('ParentCallPage')
        .width(120).margin({ top: 16 })
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/ParentCallPage' })
        })
      Button('WatchCallPage')
        .width(120).margin({ top: 16 })
        .onClick(() => {
          this.getUIContext().getRouter().pushUrl({ url: 'pages/WatchCallPage' })
        })
    }
    .height('100%')
    .width('100%')
  }
}