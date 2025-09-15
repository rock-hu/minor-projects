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

import { MediaService, SongItem } from '@ohos/mediaCommon';
import { StyleConstants } from '../constants/StyleConstants';
import { VolumeSlider } from './VolumeSliderComponent';

@Builder
export function SongPageBuilder() {
  SongPage();
}

@Preview
@Component
struct SongPage {
  @StorageLink('songList') songList: SongItem[] = [];
  @StorageLink('selectIndex') selectIndex: number = 0;
  @StorageLink('isPlay') isPlay: boolean = false;
  @StorageLink('progress') time: number = 0;
  @StorageLink('progressMax') max: number = 0;
  @StorageLink('isFirstLaunch') isFirstLaunch: boolean = true;
  readonly HALF_WIDTH: string = '50%';

  aboutToAppear(): void {
    if (this.isFirstLaunch) {
      MediaService.getInstance().loadAssent(0, true);
    }
    AppStorage.setOrCreate('isFirstLaunch', false);
  }

  build() {
    NavDestination() {
      // [Start song_page]
      Column() {
        Column() {
          Text(this.songList[this.selectIndex].title)
            .fontWeight(FontWeight.Bold)
            .fontColor($r('app.color.font_color'))
          Text(this.songList[this.selectIndex].singer)
            .fontColor($r('app.color.play_singer_color'))
        }

        Row() {
          Column() {
            Image($r('app.media.previous_btn'))
              .width($r('app.float.play_song_img'))
          }
          // [StartExclude song_page]
          .onClick(() => {
            MediaService.getInstance().playPrevious();
          })

          // [EndExclude song_page]
          Stack() {
            Image(this.songList[this.selectIndex].label)
              .width($r('app.float.play_circle_img'))
              .height($r('app.float.play_circle_img'))
              .borderRadius(StyleConstants.CIRCLE_BORDER_RADIUS)

            Progress({ value: this.time, total: this.max, type: ProgressType.Ring })
              .width($r('app.float.play_progress_width'))
              .backgroundColor(Color.Transparent)
              .color($r('app.color.font_color'))

            Image($r('app.media.play_btn'))
              .width($r('app.float.play_song_img'))
              .visibility(this.isPlay === true ? Visibility.None : Visibility.Visible)
              // [StartExclude song_page]
              .onClick(() => {
                if (MediaService.getInstance().getFirst()) {
                  MediaService.getInstance().loadAssent(0);
                } else {
                    this.isPlay ? MediaService.getInstance().pause() : MediaService.getInstance().play();
                }
              })
            // [EndExclude song_page]

            Image($r('app.media.pause_btn'))
              .width($r('app.float.play_song_img'))
              .visibility(this.isPlay === true ? Visibility.Visible : Visibility.None)
              // [StartExclude song_page]
              .onClick(() => {
                MediaService.getInstance().pause();
              })
            // [EndExclude song_page]
          }
          .width(this.HALF_WIDTH)
          .align(Alignment.Center)

          Column() {
            Image($r('app.media.next_btn'))
              .width($r('app.float.play_song_img'))
          }
          // [StartExclude song_page]
          .onClick(() => {
            MediaService.getInstance().playNextAuto(true);
          })

          // [EndExclude song_page]
        }
        .justifyContent(FlexAlign.SpaceAround)
        .width('85%')

        Row() {
          Image($r('app.media.download'))
            .width($r('app.float.play_icon_width'))
          Image($r('app.media.repeat'))
            .width($r('app.float.play_icon_width'))
          Image($r('app.media.full_screen'))
            .width($r('app.float.play_icon_width'))
        }
        .width('60%')
        .justifyContent(FlexAlign.SpaceAround)
      }
      .width(StyleConstants.FULL_WIDTH)
      .height(StyleConstants.FULL_HEIGHT)
      .padding({ top: $r('app.float.play_column_padding'), bottom: $r('app.float.play_column_padding') })
      .justifyContent(FlexAlign.SpaceAround)

      // [End song_page]
      VolumeSlider()
    }
    .hideTitleBar(true)
    .focusable(true)
    .focusOnTouch(true)
    .defaultFocus(true)
    .linearGradient({
      direction: GradientDirection.Bottom,
      colors: [
        ['#5E4C4B', 1.0],
        ['#695951', 1.0]
      ]
    })
  }
}