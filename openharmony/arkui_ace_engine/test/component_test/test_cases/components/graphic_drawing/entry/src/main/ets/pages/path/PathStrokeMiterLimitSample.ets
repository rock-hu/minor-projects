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
// 测试代码路径
import pathStrokeMiterLimitTest from '../MyTest/PathStrokeMiterLimitTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct PathStrokeMiterLimitSample {
  @State PathStrokeMiterLimitValue: number | string = 10

  aboutToAppear(): void {
    loadComponentTestEntry(pathStrokeMiterLimitTest)
  }

  build() {
    Column({ space: 10 }) {
      Path()
        .width('210px')
        .height('310px')
        .commands('M100 0 L200 240 L0 240 Z')
        .stroke(Color.Blue)
        .strokeWidth(3)
        .strokeLineJoin(LineJoinStyle.Miter)
        .strokeMiterLimit(this.PathStrokeMiterLimitValue)
        .id('pathStrokeMiterLimit')
      Button('PathStrokeMiterLimit')
        .onClick(() => {
          this.PathStrokeMiterLimitValue = '15'
        })
    }
  }
}