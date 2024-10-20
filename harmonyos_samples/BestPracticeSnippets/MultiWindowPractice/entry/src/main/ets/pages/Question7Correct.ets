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
import { promptAction, window } from '@kit.ArkUI';

@Builder
export function Question7CorrectBuilder() {
  Question7Correct()
}

@Component
export struct Question7Correct {
  private windowClass = (getContext(this) as common.UIAbilityContext).windowStage.getMainWindowSync();
  @State topSafeHeight: number = 0;
  @State windowStatus: WindowStatusType = window.WindowStatusType.FULL_SCREEN;

  aboutToAppear(): void {
    this.windowClass.setSpecificSystemBarEnabled('status', false);
    this.windowStatus = this.windowClass.getWindowStatus();

    if (this.windowStatus === window.WindowStatusType.FLOATING) {
      this.topSafeHeight = px2vp(this.windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM).topRect.height);
    }

    this.windowClass.on('windowStatusChange', data => {
      if (data === window.WindowStatusType.FLOATING) {
        this.topSafeHeight =
          px2vp(this.windowClass.getWindowAvoidArea(window.AvoidAreaType.TYPE_SYSTEM).topRect.height);
      } else {
        this.topSafeHeight = 0;
      }
    })
  }

  aboutToDisappear(): void {
    this.windowClass.setSpecificSystemBarEnabled('status', true);
  }

  build() {
    NavDestination() {
      Stack() {
        Image($r('app.media.game_bg'))
          .height('100%')

        Row() {
          Image($r('app.media.icon_pause'))
            .height(40)
            .margin({
              right: 16,
              top: this.topSafeHeight + 5
            })
            .onClick(() => {
              promptAction.showToast({
                message: 'Action success'
              });
            })
        }
        .height('100%')
        .width('100%')
        .justifyContent(FlexAlign.End)
        .alignItems(VerticalAlign.Top)
      }
    }
    .hideTitleBar(true)
  }
}