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

import { IntroductionTitle } from '../../../../common/IntroductionTitle';
import ShowToast from '../../../../util/ShowToast';
import { TitleBar } from '../../../../common/TitleBar';
import { LEFT_PADDING, RIGHT_PADDING } from '../../ComponentData';
import { AutoPlaySwitchPanel, VideoFitControlPanel, CurrentRateControlPanel } from '../videoSample/VideoControlPanel';
import { window } from '@kit.ArkUI';

@Extend(Column)
function cardStyle() {
  .backgroundColor($r('app.color.white'))
  .borderRadius(16)
  .width('100%')
  .padding(12)
}

@Entry
@Component
struct VideoSample {
  @State message: string = 'Hello World';
  @State videoCurrent: number = 1;
  @State autoPlay: boolean = false;
  @State controlsShow: boolean = true;
  @State mutedShow: boolean = false;
  @State videoFit: ImageFit = ImageFit.Auto;
  @State videoLoop: boolean = false;
  controller: VideoController = new VideoController();
  private context: Context = this.getUIContext().getHostContext()!;

  build() {
    Column() {
      TitleBar({ title: $r('app.string.video_title') })
      Column() {
        IntroductionTitle({ introduction: $r('app.string.video_show') })
        Video({ src: $r('app.media.video_test'),
          currentProgressRate: this.videoCurrent,
          previewUri: $r('app.media.ic_image_alt'),
          controller: this.controller })
          .height(300)
          .width('100%')
          .autoPlay(this.autoPlay)
          .controls(this.controlsShow)
          .muted(this.mutedShow)
          .objectFit(this.videoFit)
          .loop(this.videoLoop)
          .onStart(() => {
            ShowToast.shortToast('start');
          })
          .onPause(() => {
            ShowToast.shortToast('Pause');
          })
          .onFinish(() => {
            ShowToast.shortToast('Finish');
          })
          .onError(() => {
            ShowToast.shortToast('Error');
          })
          .onPrepared((video: DurationObject) => {
            ShowToast.longToast('Video is Prepared and video duration is ' + video.duration);
          })
          .onSeeking((video: TimeObject) => {
            ShowToast.shortToast('' + video.time);
          })
          .onSeeked((video: TimeObject) => {
            ShowToast.shortToast('' + video.time);
          })
          .onUpdate((video) => {
          })
          .onFullscreenChange((event: FullScreenObject) => {
            if (event.fullscreen) {
              let windowClass: window.Window | null = null;
              try {
                let promise = window.getLastWindow(this.context);
                promise.then((data) => {
                  windowClass = data;
                  console.info('Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
                  let names: Array<'status' | 'navigation'> = [];
                  try {
                    let promise = windowClass.setWindowSystemBarEnable(names);
                    promise.then(() => {
                      console.info('Succeeded in setting the system bar to be invisible.');
                    }).catch((err: Error) => {
                      console.error('Failed to set the system bar to be invisible. Cause:' + JSON.stringify(err));
                    });
                  } catch (exception) {
                    console.error('Failed to set the system bar to be invisible. Cause:' + JSON.stringify(exception));
                  }
                }).catch((err: Error) => {
                  console.error('Failed to obtain the top window. Cause: ' + JSON.stringify(err));
                });
              } catch (exception) {
                console.error('Failed to obtain the top window. Cause: ' + JSON.stringify(exception));
              }
              ShowToast.shortToast('FullScreen')
            }
            else {
              let windowClass: window.Window | null = null;
              try {
                let promise = window.getLastWindow(this.context);
                promise.then((data) => {
                  windowClass = data;
                  console.info('Succeeded in obtaining the top window. Data: ' + JSON.stringify(data));
                  let names: Array<'status' | 'navigation'> = ['status', 'navigation'];
                  try {
                    let promise = windowClass.setWindowSystemBarEnable(names);
                    promise.then(() => {
                      console.info('Succeeded in setting the system bar to be invisible.');
                    }).catch((err: Error) => {
                      console.error('Failed to set the system bar to be invisible. Cause:' + JSON.stringify(err));
                    });
                  } catch (exception) {
                    console.error('Failed to set the system bar to be invisible. Cause:' + JSON.stringify(exception));
                  }
                }).catch((err: Error) => {
                  console.error('Failed to obtain the top window. Cause: ' + JSON.stringify(err));
                });
              } catch (exception) {
                console.error('Failed to obtain the top window. Cause: ' + JSON.stringify(exception));
              }
              ShowToast.shortToast('NotFullScreen')
            }
          })
        Scroll() {
          Column() {
            IntroductionTitle({ introduction: $r('app.string.video_play_switch') })
            Column() {
              AutoPlaySwitchPanel({
                AutoPlay: $autoPlay,
                controlsShow: $controlsShow,
                muted: $mutedShow,
                loop: $videoLoop
              })
            }.cardStyle()

            IntroductionTitle({ introduction: $r('app.string.video_play_mode') })
            Column() {
              VideoFitControlPanel({ VideoObjectFit: $videoFit })
              CurrentRateControlPanel({ currentRate: $videoCurrent })
            }.cardStyle()
          }
        }.layoutWeight(1)
      }.layoutWeight(1)
      .padding({ left: LEFT_PADDING, right: RIGHT_PADDING })
      .margin({ bottom: 5 })
    }
    .backgroundColor($r('app.color.background_grey'))
    .height('100%')
  }
}

interface DurationObject {
  duration: number;
}

interface TimeObject {
  time: number;
}

interface FullScreenObject {
  fullscreen: boolean;
}