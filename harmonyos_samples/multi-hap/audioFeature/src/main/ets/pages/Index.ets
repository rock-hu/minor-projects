/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

import { media } from '@kit.MediaKit'
import Logger from '../util/Logger'
import { common } from '@kit.AbilityKit';
import { BusinessError } from '@kit.BasicServicesKit'

const TAG: string = 'Index'
const FD_PREFIX: string = 'fd://'

let player: media.AVPlayer;
media.createAVPlayer().then((video: media.AVPlayer) => {
  if (video != null) {
    player = video;
    console.info('createAVPlayer success');
  } else {
    console.error('createAVPlayer fail');
  }
}).catch((error: BusinessError) => {
  console.error(`AVPlayer catchCallback, error message:${error.message}`);
});

@Entry
@Component
struct Index {
  @State hasInit: boolean = false;
  private context?: common.UIAbilityContext;
  stateChangeCallback: Function | undefined = undefined;

  build() {
    Row() {
      Button({type: ButtonType.Circle, stateEffect: true}) {
        Image($r("app.media.play")).width(35).height(35)
      }
      .onClick((event) => {
        Logger.info(TAG, "play button clicked, hasInit: " + this.hasInit)
        console.log("this.hasInit",this.hasInit)
        if (this.hasInit) {
          player.play()
        } else {
          this.initAudioPlayer()
        }
      })
      .id('btnAudioPlay')
      .backgroundColor(0xFFFFFF)
      .margin({right: 15})

      Button({type: ButtonType.Circle, stateEffect: true}) {
        Image($r("app.media.pause")).width(35).height(35)
      }
      .onClick((event) => {
        player.pause()
      })
      .id('btnAudioStop')
      .backgroundColor(0xFFFFFF)
    }
    .width('100%')
    .height('100%')
    .justifyContent(FlexAlign.Center)
  }

  initAudioPlayer() {
    if(this.context){
      this.setCallback(player)
      this.context.resourceManager.getRawFd("1.mp3").then(value => {
        player.fdSrc = value
      })
    }
  }

  setCallback(player: media.AVPlayer) {
    player.on('stateChange', this.stateChangeCallback = async (state: string, reason: media.StateChangeReason) => {
      switch (state) {
        case 'idle':
          console.info('state idle called');
          break;
        case 'initialized':
          Logger.info(TAG, 'initialized prepared called')
          this.hasInit = true;
          player.prepare().then(() => {
            player.play()
          })
          break;
        case 'playing':
          Logger.info(TAG, 'audio play success')
          break;
        case 'paused':
          Logger.info(TAG, 'audio paused success')
          break;
        case 'error':
          Logger.error('state error called');
          break;
        default:
          Logger.info('unkown state :' + state);
          break;
      }
    })
  }

  aboutToAppear() {
    this.context = this.getUIContext().getHostContext() as common.UIAbilityContext
  }
}