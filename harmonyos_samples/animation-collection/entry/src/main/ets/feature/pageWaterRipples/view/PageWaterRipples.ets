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

import { CommonConstants } from '../common/CommonConstants';

@Entry
@Component
export struct WaterRipples {
  @State isListening: boolean = false;

  build() {
    Column() {
      Text($r('app.string.sound_hound'))
        .fontColor(Color.White)
        .fontSize($r('app.float.water_ripples_font_size'))
        .margin({ top: $r('app.integer.margin_large') })

      ButtonWithWaterRipples({ isListening: this.isListening })

      Text(this.isListening ? $r('app.string.is_listening') : $r('app.string.click_to_listen'))
        .fontColor(Color.White)
        .margin({ bottom: $r('app.integer.margin_large') })
    }
    .backgroundColor(Color.Black)
    .justifyContent(FlexAlign.SpaceBetween)
    .width(CommonConstants.ONE_HUNDRED_PERCENT)
    .height(CommonConstants.ONE_HUNDRED_PERCENT)
  }
}

interface Scale {
  x: number;
  y: number;
}

@Component
struct ButtonWithWaterRipples {
  @Link isListening: boolean;
  @State immediatelyOpacity: number = 0.8;
  @State immediatelyScale: Scale = { x: 1, y: 1 };
  @State delayOpacity: number = 0.8;
  @State delayScale: Scale = { x: 1, y: 1 };
  private readonly BUTTON_SIZE: number = 120;
  private readonly BUTTON_CLICK_SCALE: number = 0.8;
  private readonly ANIMATION_DURATION: number = 1300;

  @Styles
  ripplesStyle() {
    .width(this.BUTTON_SIZE * this.BUTTON_CLICK_SCALE)
    .height(this.BUTTON_SIZE * this.BUTTON_CLICK_SCALE)
    .borderRadius(this.BUTTON_SIZE * this.BUTTON_CLICK_SCALE / 2)
    .backgroundColor(Color.White)
  }

  build() {
    Stack() {
      Stack()
        .ripplesStyle()
        .opacity(this.immediatelyOpacity)
        .scale(this.immediatelyScale)
      Stack()
        .ripplesStyle()
        .opacity(this.delayOpacity)
        .scale(this.delayScale)
      Button() {
        Image($r('app.media.ic_public_music_filled'))
          .width($r('app.float.water_ripples_width'))
          .fillColor(Color.White)
      }
      .clickEffect({ level: ClickEffectLevel.HEAVY, scale: this.BUTTON_CLICK_SCALE })
      .backgroundColor($r('app.color.music_icon'))
      .type(ButtonType.Circle)
      .width(this.BUTTON_SIZE)
      .height(this.BUTTON_SIZE)
      .zIndex(1)
      .onClick(() => {
        this.isListening = !this.isListening;
        if (this.isListening) {
          animateTo({
            duration: this.ANIMATION_DURATION,
            iterations: CommonConstants.ITERATIONS,
            curve: Curve.EaseInOut
          }, () => {
            this.immediatelyOpacity = CommonConstants.COMMON_NUMBER_0;
            this.immediatelyScale = {
              x: CommonConstants.COMMON_NUMBER_6,
              y: CommonConstants.COMMON_NUMBER_6
            };
          })
          animateTo({
            duration: this.ANIMATION_DURATION,
            iterations: CommonConstants.ITERATIONS,
            curve: Curve.EaseInOut,
            delay: CommonConstants.DELAY_200
          }, () => {
            this.delayOpacity = CommonConstants.COMMON_NUMBER_0;
            this.delayScale = {
              x: CommonConstants.COMMON_NUMBER_6,
              y: CommonConstants.COMMON_NUMBER_6
            };
          })
        } else {
          // Break the animation by modifying the variable with a closure of duration 0.
          animateTo({ duration: CommonConstants.COMMON_NUMBER_0 }, () => {
            this.immediatelyOpacity = CommonConstants.COMMON_NUMBER;
            this.delayOpacity = CommonConstants.COMMON_NUMBER;
            this.immediatelyScale = {
              x: CommonConstants.COMMON_NUMBER_1,
              y: CommonConstants.COMMON_NUMBER_1
            };
            this.delayScale = {
              x: CommonConstants.COMMON_NUMBER_1,
              y: CommonConstants.COMMON_NUMBER_1
            };
          })
        }
      })
    }
  }
}

@Builder
export function getPageWaterRipples(name: string): void {
  if (name === 'PageWaterRipples') {
    NavDestination() {
      WaterRipples();
    };
  }
}

export const wrappedBuilderWaterRipples: WrappedBuilder<[string, number]> = wrapBuilder(getPageWaterRipples);