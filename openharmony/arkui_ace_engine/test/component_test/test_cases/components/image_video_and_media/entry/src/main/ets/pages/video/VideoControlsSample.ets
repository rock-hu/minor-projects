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
//测试代码路径
import videoControlsTest from '../MyTest/VideoControlsTest'
import { loadComponentTestEntry } from '@ohos.arkui.componentTest'

@Entry
@Component
struct VideoControlsSample {
  @State buttonFalse: boolean = true
  @State videoSrc: Resource = $rawfile('video2.mp4')
  @State preViewUri: Resource = $r('app.media.app_icon')
  @State curRate: PlaybackSpeed = PlaybackSpeed.Speed_Forward_1_00_X
  controller: VideoController = new VideoController()

  aboutToAppear(): void {
    loadComponentTestEntry(videoControlsTest)
  }

  build() {
    Column() {
      Video({
        src: this.videoSrc,
        previewUri: this.preViewUri,
        currentProgressRate: this.curRate,
        controller: this.controller
      }).width('100%').height(500).id('videoControls')
        .controls(this.buttonFalse)
      Button('VideoControls')
        .onClick(() => {
          this.buttonFalse = !this.buttonFalse
        })
    }
  }
}