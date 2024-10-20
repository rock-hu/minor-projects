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
import { promptAction } from '@kit.ArkUI';

@Builder
export function Question7IncorrectBuilder() {
  Question7Incorrect()
}

@Component
export struct Question7Incorrect {
  private windowClass = (getContext(this) as common.UIAbilityContext).windowStage.getMainWindowSync();

  aboutToAppear(): void {
    this.windowClass.setSpecificSystemBarEnabled('status', false);
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
              top: 5
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