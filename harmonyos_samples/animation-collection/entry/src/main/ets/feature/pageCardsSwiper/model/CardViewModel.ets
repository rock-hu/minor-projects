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

import { display } from '@kit.ArkUI';
import { CardInfo } from '../model/CardModel';
import Constants from '../common/Constants';

@Component
export default struct CardComponent {
  @State cardInfo: CardInfo = {
    src: $r('app.media.img1'),
    width: Constants.CARD_UNIT_LENGTH,
    height: Constants.CARD_UNIT_LENGTH
  };
  @State cardWith: number = this.cardInfo.width;
  @State cardHeight: number = this.cardInfo.height;
  @State isTablet: boolean = false;
  @State screenW: number = px2vp(display.getDefaultDisplaySync().width);
  @State isPhotoShow: boolean = false;
  @Prop cardOffset: number = 0;
  @Prop showingCard: number = 0;
  cardIndex: number = 0;

  aboutToAppear(): void {
    if (canIUse('SystemCapability.Window.SessionManager')) {
      if ((display.isFoldable() && display.getFoldStatus() === display.FoldStatus.FOLD_STATUS_EXPANDED) ||
        this.screenW >= Constants.DEVICE_SIZE) {
        this.isTablet = true;
      } else {
        this.isTablet = false;
      }
    }
    this.cardWith = this.cardInfo.width;
    this.cardHeight = this.cardInfo.height;
  }

  build() {
    Row() {
      Image(this.cardInfo.src)
        .width(this.cardWith)
        .height(this.cardHeight)
        .objectFit(ImageFit.Cover)
        .borderRadius($r('app.float.card_swiper_photo_radius'))
        .shadow({
          radius: $r('app.float.card_swiper_card_shadow_radius'),
          type: ShadowType.COLOR,
          color: $r('app.color.card_shadow_color')
        })
        .geometryTransition(this.cardIndex.toString(), { follow: true })
        .transition(TransitionEffect.OPACITY.animation({ duration: Constants.DURATION, curve: Curve.Friction }))
        .margin({ left: this.cardOffset })
        .animation({
          curve: Curve.Friction,
          duration: Constants.DURATION
        })
        .onClick(() => {
          if (this.cardIndex !== this.showingCard) {
            return;
          }
          this.animateFunc();
        })
        .bindContentCover(
          this.isPhotoShow,
          this.photoShowBuilder(this.cardInfo.src, this.cardIndex.toString()),
          { backgroundColor: $r('app.color.photo_preview_build_background'), modalTransition: ModalTransition.ALPHA }
        )
        .onTouch((event) => {
          if (event) {
            if (event.type === TouchType.Down) {
              this.cardWith = this.cardInfo.width * Constants.CARD_WIDTH;
              this.cardHeight = this.cardInfo.height * Constants.CARD_WIDTH;
            }
            if (event.type === TouchType.Up) {
              this.cardWith = this.cardInfo.width;
              this.cardHeight = this.cardInfo.height;
            }
          }
        })
    }
    .alignItems(VerticalAlign.Center)
  }

  @Builder
  photoShowBuilder(img: Resource, id: string) {
    Column() {
      Image(img)
        .borderRadius($r('app.float.card_swiper_photo_radius'))
        .geometryTransition(id, { follow: true })
        .width(this.isTablet ? $r('app.string.card_swiper_tablet_preview_width') :
        $r('app.string.card_swiper_photo_preview_width'))
        .transition(TransitionEffect.opacity(Constants.OPACITY))
    }
    .width($r('app.string.card_swiper_full_size'))
    .height($r('app.string.card_swiper_full_size'))
    .justifyContent(FlexAlign.Center)
    .onClick(() => {
      this.animateFunc();
    })
    .transition(TransitionEffect.asymmetric(
      TransitionEffect.opacity(Constants.OPACITY),
      TransitionEffect.OPACITY
    ))
  }

  animateFunc() {
    animateTo({
      duration: Constants.DURATION,
      curve: Curve.Friction
    }, () => {
      this.isPhotoShow = !this.isPhotoShow;
    });
  }
}