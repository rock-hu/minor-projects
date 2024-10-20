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

import { common } from '@kit.AbilityKit';
import { display } from '@kit.ArkUI';
import { AVPlayerUtil } from '../common/AVPlayerUtil';

const context: Context = getContext(this);

@Builder
export function Question2CorrectBuilder() {
  Question2Correct()
}

@Component
export struct Question2Correct {
  @State aspect: number = 9 / 16; // default video height/width ratio value
  @State xComponentWidth: number = px2vp(display.getDefaultDisplaySync().width);
  @State xComponentHeight: number = px2vp(display.getDefaultDisplaySync().width * this.aspect);
  private xComponentController: XComponentController = new XComponentController();
  private player?: AVPlayerUtil;
  private windowClass = (context as common.UIAbilityContext).windowStage.getMainWindowSync();

  aboutToAppear(): void {
    this.windowClass.on('windowSizeChange', () => {
      this.calculateVideoSize();
    });
  }

  aboutToDisappear(): void {
    this.player?.stopPlay();
  }

  calculateVideoSize() {
    let size = this.windowClass.getWindowProperties().windowRect;
    let windowWidth = px2vp(size.width);
    let windowHeight = px2vp(size.height);
    if (windowWidth * this.aspect > windowHeight) {
      this.xComponentHeight = windowHeight;
      this.xComponentWidth = windowHeight / this.aspect;
    } else {
      this.xComponentHeight = windowHeight * this.aspect;
      this.xComponentWidth = windowWidth;
    }
  }

  build() {
    NavDestination() {
      Stack() {
        XComponent({ id: 'video_player_id', type: XComponentType.SURFACE, controller: this.xComponentController })
          .onLoad(() => {
            this.player = new AVPlayerUtil();
            this.player.setSurfaceId(this.xComponentController.getXComponentSurfaceId());
            this.player.initPlayer('testVideo2.mp4', (aspect: number) => {
              this.aspect = aspect;
              this.calculateVideoSize();
            });
          })
          .width(this.xComponentWidth)
          .height(this.xComponentHeight)
      }
      .width('100%')
      .height('100%')
      .backgroundColor(Color.Black)
    }
    .hideTitleBar(true)
  }
}