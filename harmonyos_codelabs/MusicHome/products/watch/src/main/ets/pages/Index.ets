/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import {
  ArcSwiper,
  ArcSwiperAttribute, // The properties of ArcSwiper depend on the ArcSwiperAttribute object for import.
  ArcDotIndicator,
  ArcDirection,
  ArcSwiperController
} from '@kit.ArkUI';
import { Home } from '../view/Home';
import { PlayList } from '../view/PlayList';
import { MediaService } from 'mediacommon';
import { songList } from 'musiclist';
import { StyleConstants } from '../constants/StyleConstants';

@Entry
@Component
struct Index {
  @Provide pageStack: NavPathStack = new NavPathStack();
  innerSelectedIndex: number = 0;
  private wearableSwiperController: ArcSwiperController = new ArcSwiperController();
  private arcDotIndicator: ArcDotIndicator = new ArcDotIndicator();

  aboutToAppear(): void {
    AppStorage.setOrCreate('songList', songList);
    MediaService.getInstance();
  }

  build() {
    Navigation(this.pageStack) {
      // [Start home_swiper]
      Column() {
        Row() {
          ArcSwiper(this.wearableSwiperController) {
            Home()
            PlayList()
          }
          .duration(400)
          .indicator(this.arcDotIndicator
            .arcDirection(ArcDirection.SIX_CLOCK_DIRECTION)
            .selectedItemColor('#FE1B48')
          )
          // [StartExclude home_swiper]
          .onAnimationStart((index: number, targetIndex: number) => {
            this.innerSelectedIndex = targetIndex;
          })
          .onGestureRecognizerJudgeBegin((event: BaseGestureEvent, current: GestureRecognizer,
            others: Array<GestureRecognizer>): GestureJudgeResult => { // When the recognizer is about to succeed, set the recognizer enable status based on the current component state.
            if (current) {
              let target = current.getEventTargetInfo();
              if (target && current.isBuiltIn() && current.getType() == GestureControl.GestureType.PAN_GESTURE) {
                // Here, the condition `swiperTarget.isBegin()` or `innerSelectedIndex === 0` indicates that the ArcSwiper has been swiped to the beginning.
                let swiperTarget = target as ScrollableTargetInfo;
                if (swiperTarget instanceof ScrollableTargetInfo &&
                  (swiperTarget.isBegin() || this.innerSelectedIndex === 0)) {
                  let panEvent = event as PanGestureEvent;
                  if (panEvent && panEvent.offsetX > 0 && (swiperTarget.isBegin() || this.innerSelectedIndex === 0)) {
                    return GestureJudgeResult.REJECT;
                  }
                }
              }
            }
            return GestureJudgeResult.CONTINUE;
          })

          // [EndExclude home_swiper]
        }
        .height(StyleConstants.FULL_HEIGHT)
      }
      .width(StyleConstants.FULL_WIDTH)

      // [End home_swiper]
    }
    .hideTitleBar(true)
  }
}