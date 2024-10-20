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

import { media } from '@kit.MediaKit';
import { common } from '@kit.AbilityKit';
import Logger from './Logger';

export class AVPlayerUtil {
  private surfaceId: string = '';
  private avPlayer: media.AVPlayer | undefined = undefined;
  private callBack: Function = () => {
  };

  setSurfaceId(surfaceId: string) {
    this.surfaceId = surfaceId;
  }

  setAVPlayerCallback() {
    if (this.avPlayer) {
      this.avPlayer.on('stateChange', async (state: string) => {
        switch (state) {
          case 'idle':
            Logger.info('AVPlayer state idle called.');
            this.avPlayer?.release();
            break;
          case 'initialized':
            Logger.info('AVPlayer state initialized called.');
            this.avPlayer!.surfaceId = this.surfaceId;
            this.avPlayer?.prepare();
            break;
          case 'prepared':
            Logger.info('AVPlayer state prepared called.');
            this.avPlayer?.play();
            break;
          case 'playing':
            Logger.info('AVPlayer state playing called.');
            break;
          case 'paused':
            Logger.info('AVPlayer state paused called.');
            break;
          case 'completed':
            Logger.info('AVPlayer state completed called.');
            this.avPlayer?.play();
            break;
          case 'stopped':
            Logger.info('AVPlayer state stopped called.');
            this.avPlayer?.reset();
            break;
          case 'released':
            Logger.info('AVPlayer state released called.');
            break;
          default:
            break;
        }
      })

      // get video height and width
      this.avPlayer?.on('videoSizeChange', (width: number, height: number) => {
        Logger.info(`videoSizeChange called,and width is: ${width} , height is : ${height}`);
        this.callBack(height / width);
      })
    }
  }

  async initPlayer(url: string, callBack: Function) {
    this.avPlayer = await media.createAVPlayer();
    this.callBack = callBack;
    this.setAVPlayerCallback();

    let context = getContext(this) as common.UIAbilityContext;
    let fileDescriptor = await context.resourceManager.getRawFd(url);
    let avFileDescriptor: media.AVFileDescriptor =
      { fd: fileDescriptor.fd, offset: fileDescriptor.offset, length: fileDescriptor.length };
    this.avPlayer.fdSrc = avFileDescriptor;
    Logger.info(`fdSrc: ${this.avPlayer.fdSrc}`);
  }

  stopPlay() {
    this.avPlayer?.off('videoSizeChange');
    this.avPlayer?.stop();
  }
}
