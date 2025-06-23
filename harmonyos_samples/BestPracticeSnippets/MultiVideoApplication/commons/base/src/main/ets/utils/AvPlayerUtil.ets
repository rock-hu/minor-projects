/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import { media } from '@kit.MediaKit';
import { common } from '@kit.AbilityKit';
import { resourceManager } from '@kit.LocalizationKit';
import { BusinessError } from '@kit.BasicServicesKit';
import Logger from './Logger';
import { CommonConstants } from '../constants/CommonConstants';

const uiContext: UIContext | undefined = AppStorage.get('uiContext');

export class AvPlayerUtil {
  private avPlayer?: media.AVPlayer;
  private context: Context  = uiContext!.getHostContext() as Context;
  private url: resourceManager.RawFileDescriptor | null = null;
  private surfaceId: string = '';
  private sliderBegin: number = 0;
  private isMoving: boolean = false;
  private onError: (err: BusinessError) => void = (err: BusinessError) => {
    Logger.error(`Invoke avPlayer failed, code is ${err.code}, message is ${err.message}`);
    this.avPlayer?.reset();
  };
  private onTimeUpdateFunction: (updateTime: number) => void = (updateTime: number) => {
    AppStorage.setOrCreate<string>(CommonConstants.AV_PLAYER_CURRENT_TIME, this.formatTime(updateTime));
    AppStorage.setOrCreate<number>(CommonConstants.AV_PLAYER_UPDATE_TIME, updateTime);
    AppStorage.setOrCreate<number>(CommonConstants.AV_PLAYER_PROGRESS, updateTime / this.avPlayer!.duration *
      CommonConstants.PROGRESS_HUNDRED);
  };
  private onStateChange: (state: media.AVPlayerState) => void = async (state: media.AVPlayerState) => {
    if (this.avPlayer === undefined) {
      Logger.error(`AvPlayer is undefined`);
      return;
    }
    switch (state) {
      case CommonConstants.AV_PLAYER_IDLE_STATE:
        this.url = await this.context.resourceManager.getRawFd(CommonConstants.PRODUCT_VIDEO_NAME);
        this.avPlayer.fdSrc = this.url;
        Logger.info('AVPlayer state idle called.');
        break;
      case CommonConstants.AV_PLAYER_INITIALIZED_STATE:
        Logger.info('AVPlayer initialized called.');
        this.avPlayer.surfaceId = this.surfaceId;
        this.avPlayer.prepare().then(() => {
          Logger.info('AVPlayer prepare succeeded.');
        }, (err: BusinessError) => {
          Logger.error(`Invoke prepare failed, code is ${err.code}, message is ${err.message}`);
          this.avPlayer!.reset();
        });
        break;
      case CommonConstants.AV_PLAYER_PREPARED_STATE:
        this.avPlayer.videoScaleType = media.VideoScaleType.VIDEO_SCALE_TYPE_FIT;
        Logger.info('AVPlayer state prepared called.');
        this.avPlayer.play();
        AppStorage.setOrCreate<string>(CommonConstants.AV_PLAYER_TOTAL_TIME, this.formatTime(this.avPlayer.duration));
        break;
      case CommonConstants.AV_PLAYER_PLAYING_STATE:
        Logger.info('AVPlayer state playing called.');
        AppStorage.setOrCreate('avplayerState', this.avPlayer.state);
        break;
      case CommonConstants.AV_PLAYER_PAUSED_STATE:
        Logger.info('AVPlayer state paused called.');
        AppStorage.setOrCreate('avplayerState', this.avPlayer.state);
        break;
      case CommonConstants.AV_PLAYER_COMPLETED_STATE:
        Logger.info('AVPlayer state completed called.');
        AppStorage.setOrCreate('avplayerState', this.avPlayer.state);
        this.avPlayer.stop();
        break;
      case CommonConstants.AV_PLAYER_STOPPED_STATE:
        Logger.info('AVPlayer state stopped called.');
        break;
      case CommonConstants.AV_PLAYER_RELEASE_STATE:
        Logger.info('AVPlayer state released called.');
        break;
      case CommonConstants.AV_PLAYER_ERROR_STATE:
        Logger.error('AVPlayer state error called.');
        break;
      default:
        Logger.info('AVPlayer state unknown called.');
        break;
    }
  };

  static getInstance(): AvPlayerUtil | undefined {
    if (!AppStorage.get<AvPlayerUtil>('avPlayerUtil')) {
      AppStorage.setOrCreate('avPlayerUtil', new AvPlayerUtil());
    } else {
      Logger.info(`AppStorage does not have avPlayerUtil`);
    }
    return AppStorage.get<AvPlayerUtil>('avPlayerUtil');
  }

  async createAvPlayer(surfaceId: string): Promise<void> {
    if (!this.avPlayer || this.avPlayer.state === CommonConstants.AV_PLAYER_RELEASE_STATE) {
      this.avPlayer = await media.createAVPlayer();
      this.surfaceId = surfaceId;
      Logger.info('Created AvPlayer successfully.');
      // Entering idle state.
      this.url = await this.context.resourceManager.getRawFd(CommonConstants.PRODUCT_VIDEO_NAME);
      this.avPlayer.fdSrc = this.url;
      this.setAVPlayerCallback();
    } else {
      Logger.info(`AvPlayer has been created`);
    }
  }

  setAVPlayerCallback(): void {
    this.avPlayer?.on('error', this.onError);
    this.onTimeUpdate();
    this.setStateChange();
  }

  onTimeUpdate(): void {
    this.avPlayer?.on('timeUpdate', this.onTimeUpdateFunction);
  }

  offTimeUpdate(): void {
    try {
      this.avPlayer?.off('timeUpdate');
    } catch (exception) {
      Logger.error('Failed to unregister callback. Code: ' + JSON.stringify(exception));
    }
  }

  setStateChange(): void {
    this.avPlayer?.on('stateChange', this.onStateChange);
  }

  release(): void {
    if (this.avPlayer && this.avPlayer.state !== CommonConstants.AV_PLAYER_RELEASE_STATE) {
      try {
        this.avPlayer.off('error');
        this.avPlayer.off('stateChange');
      } catch (exception) {
        Logger.error('Failed to unregister callback. Code: ' + JSON.stringify(exception));
      }
      this.avPlayer.release();
    } else {
      Logger.info(`AvPlayer release failed`);
    }
  }

  sliderChange(value: number, mode: SliderChangeMode): void {
    let seekType: media.SeekMode = value > this.sliderBegin ? media.SeekMode.SEEK_PREV_SYNC :
      media.SeekMode.SEEK_NEXT_SYNC;
    switch (mode) {
      case SliderChangeMode.Begin:
        Logger.info(`AvPlayer SliderChangeMode Begin`);
        this.sliderBegin = value;
        this.avPlayer?.pause();
        break;
      case SliderChangeMode.Moving:
        Logger.info(`AvPlayer SliderChangeMode Moving`);
        this.isMoving = true;
        break;
      case SliderChangeMode.End:
        Logger.info(`AvPlayer SliderChangeMode End`);
        if (this.isMoving) {
          this.avPlayer?.seek(value / CommonConstants.PROGRESS_HUNDRED * this.avPlayer.duration, seekType);
          this.isMoving = false;
        }
        this.avPlayer?.play();
        break;
      case SliderChangeMode.Click:
        Logger.info(`AvPlayer SliderChangeMode Click`);
        this.avPlayer?.seek(this.sliderBegin / CommonConstants.PROGRESS_HUNDRED * this.avPlayer.duration, seekType);
        break;
      default:
        break;
    }
  }

  playerStateControl(): void {
    if (this.avPlayer?.state === CommonConstants.AV_PLAYER_STOPPED_STATE) {
      this.avPlayer.prepare();
    } else if (this.avPlayer?.state === CommonConstants.AV_PLAYER_PAUSED_STATE) {
      this.avPlayer.play();
    } else if (this.avPlayer?.state === CommonConstants.AV_PLAYER_PLAYING_STATE) {
      this.avPlayer.pause();
    }
  }

  fastForward(): void {
    if (this.avPlayer?.state === CommonConstants.AV_PLAYER_PLAYING_STATE || this.avPlayer?.state ===
      CommonConstants.AV_PLAYER_PAUSED_STATE) {
      this.avPlayer.seek(this.avPlayer.currentTime + 5000);
    }
  }

  rewind(): void {
    if (this.avPlayer?.state === CommonConstants.AV_PLAYER_PLAYING_STATE || this.avPlayer?.state ===
      CommonConstants.AV_PLAYER_PAUSED_STATE) {
      this.avPlayer.seek(this.avPlayer.currentTime - 5000);
    }
  }

  formatTime(duration: number): string {
    let totalSecond: number = Math.round(duration / CommonConstants.PROGRESS_THOUSAND);
    let hourNum: number = Math.floor(totalSecond / CommonConstants.SECOND_IN_HOUR);
    let minNum: number = Math.floor((totalSecond % CommonConstants.SECOND_IN_HOUR) / CommonConstants.SECOND_IN_MINUTE);
    let secNum: number = (totalSecond % CommonConstants.SECOND_IN_HOUR) % CommonConstants.SECOND_IN_MINUTE;
    return this.formatUnitTime(hourNum) + CommonConstants.COLON + this.formatUnitTime(minNum) + CommonConstants.COLON +
      this.formatUnitTime(secNum);
  }

  formatUnitTime(time: number): string {
    if (time >= CommonConstants.ONE && time < CommonConstants.TEN) {
      let zero: string = CommonConstants.TIME_PREFIX;
      return zero.concat(time.toString());
    } else if (time >= CommonConstants.ZERO && time < CommonConstants.ONE) {
      return CommonConstants.EMPTY_TIME;
    }
    return time.toString();
  }
}