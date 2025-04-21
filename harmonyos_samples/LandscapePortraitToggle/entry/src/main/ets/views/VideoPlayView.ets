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
import { display, window } from '@kit.ArkUI';
import { BusinessError, settings } from '@kit.BasicServicesKit';
import { AVPlayerUtil } from '../utils/AVPlayerUtil';
import Logger from '../utils/Logger';
import { common } from '@kit.AbilityKit';

const context: Context = getContext(this);

@Component
export struct VideoPlayView {
  @State aspect: number = 9 / 16; // default video height/width ratio value
  @State xComponentWidth: number = px2vp(display.getDefaultDisplaySync().width);
  @State xComponentHeight: number = px2vp(display.getDefaultDisplaySync().width * this.aspect);
  @State isLandscape: boolean = false; // Whether it is in full-screen playback state
  @State isVideoLock: boolean = false; // Whether the video playback is locked
  @State orientationLockState: string = '1'; // control center switch rotation ,value 1:enable 0：disable
  private xComponentController: XComponentController = new XComponentController();
  private player?: AVPlayerUtil;
  private windowClass = (context as common.UIAbilityContext).windowStage.getMainWindowSync();

  aboutToAppear(): void {
    // Get the rotation lock on state
    this.orientationLockState = settings.getValueSync(getContext(this), settings.general.ACCELEROMETER_ROTATION_STATUS,
      settings.domainName.DEVICE_SHARED);

    this.windowClass.on('windowSizeChange', (size) => {
      let viewWidth = px2vp(size.width);
      let viewHeight = px2vp(size.height);

      if (this.isExpandedOrHalfFolded()) {
        this.xComponentWidth = viewWidth;
        this.xComponentHeight = viewWidth * this.aspect;
      } else {
        if (viewWidth > viewHeight) {
          this.xComponentWidth = viewHeight / this.aspect;
          this.xComponentHeight = viewHeight;
          this.isLandscape = true;
          this.windowClass.setSpecificSystemBarEnabled('navigationIndicator', false); // hide bottom navigation bar
        } else {
          this.xComponentHeight = viewWidth * this.aspect;
          this.xComponentWidth = viewWidth;
          this.windowClass.setSpecificSystemBarEnabled('navigationIndicator', true); // show bottom navigation bar
          this.isLandscape = false;
        }
      }
    });

    // Listen to get the rotary lock switch chapter table
    settings.registerKeyObserver(context, settings.general.ACCELEROMETER_ROTATION_STATUS,
      settings.domainName.DEVICE_SHARED, () => {
        this.orientationLockState =
          settings.getValueSync(getContext(this), settings.general.ACCELEROMETER_ROTATION_STATUS,
            settings.domainName.DEVICE_SHARED);
      });
  }

  isExpandedOrHalfFolded(): boolean {
    return display.getFoldStatus() === display.FoldStatus.FOLD_STATUS_EXPANDED ||
      display.getFoldStatus() == display.FoldStatus.FOLD_STATUS_HALF_FOLDED
  }

  setOrientation(orientation: number) {
    this.windowClass.setPreferredOrientation(orientation).then(() => {
      Logger.info('setWindowOrientation: ' + orientation + ' Succeeded.');
    }).catch((err: BusinessError) => {
      Logger.info('setWindowOrientation: ' + orientation + ' Failed. Cause: ' + JSON.stringify(err));
    });
  }

  aboutToDisappear(): void {
    settings.unregisterKeyObserver(context, settings.general.ACCELEROMETER_ROTATION_STATUS,
      settings.domainName.DEVICE_SHARED);

    this.windowClass.off('windowSizeChange');
  }

  build() {
    Stack() {
      Column() {
        XComponent({ id: 'video_player_id', type: XComponentType.SURFACE, controller: this.xComponentController })
          .onLoad(() => {
            this.player = new AVPlayerUtil();
            this.player.setSurfaceId(this.xComponentController.getXComponentSurfaceId());
            this.player.initPlayer('videoTest.mp4', (aspect: number) => {
              this.aspect = aspect;
              this.xComponentHeight = px2vp(display.getDefaultDisplaySync().width * aspect);
              this.xComponentWidth = px2vp(display.getDefaultDisplaySync().width);
            });
          })
          .width(this.xComponentWidth)
          .height(this.xComponentHeight)
      }

      RelativeContainer() {
        if (!this.isVideoLock) {
          Image($r('app.media.icon_back'))
            .height(24)
            .width(24)
            .margin({
              left: 16,
              top: this.isLandscape ? (AppStorage.get<number>('statusBarHeight') || 0) : 12
            })
            .onClick(() => {
              if (this.isExpandedOrHalfFolded()) {
                this.isLandscape = false;
              } else {
                this.setOrientation(window.Orientation.USER_ROTATION_PORTRAIT);
              }
            })
        }

        if (this.isLandscape) {
          Image(this.isVideoLock ? $r('app.media.icon_lock') : $r('app.media.icon_lock_open'))
            .height(24)
            .width(24)
            .fillColor(Color.White)
            .alignRules({
              top: { anchor: '__container__', align: VerticalAlign.Center },
              left: { anchor: '__container__', align: HorizontalAlign.Start }
            })
            .margin({ left: (AppStorage.get<number>('statusBarHeight') || 0) })
            .offset({ y: -12 })
            .onClick(() => {
              this.isVideoLock = !this.isVideoLock;

              if (this.isExpandedOrHalfFolded() || this.orientationLockState === '0') {
                return;
              }

              if (this.isVideoLock) {
                this.setOrientation(window.Orientation.AUTO_ROTATION_LANDSCAPE);
              } else {
                this.setOrientation(window.Orientation.AUTO_ROTATION_UNSPECIFIED);
              }
            })
        }

        if (!this.isLandscape) {
          Image($r('app.media.icon_zoom_in'))
            .height(24)
            .width(24)
            .alignRules({
              bottom: { anchor: '__container__', align: VerticalAlign.Bottom },
              right: { anchor: '__container__', align: HorizontalAlign.End }
            })
            .margin({
              right: 16,
              bottom: 8
            })
            .onClick(() => {
              if (this.isExpandedOrHalfFolded()) {
                this.isLandscape = true;
              } else {
                this.setOrientation(window.Orientation.USER_ROTATION_LANDSCAPE);
              }
            })
        }
      }
      .width('100%')
      .height('100%')
    }
    .width('100%')
    .height(this.isLandscape ? '100%' : this.xComponentHeight + (AppStorage.get<number>('statusBarHeight') ?? 0))
    .backgroundColor(Color.Black)
    .padding({ top: this.isLandscape ? 0 : (AppStorage.get<number>('statusBarHeight') || 0) })
  }
}