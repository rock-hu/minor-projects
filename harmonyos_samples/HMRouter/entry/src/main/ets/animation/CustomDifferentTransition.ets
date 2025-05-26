/*
 *   Copyright (c) 2024 Huawei Device Co., Ltd.
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 */

/**
 * 最佳实践：根据条件呈现不同转场动效
 */

import { HMAnimator, HMAnimatorHandle,
  HMRouterMgr,
  IHMAnimator,
  OpacityOption,
  ScaleOption, TranslateOption } from "@hadss/hmrouter";

// [Start my_animator1]
@HMAnimator({ animatorName: 'myAnimator1' })
export class MyAnimator1 implements IHMAnimator {
  effect(enterHandle: HMAnimatorHandle, exitHandle: HMAnimatorHandle): void {
    enterHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '100%';
    }).finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = 0;
    })


    exitHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = 0;
    }).finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '100%';
    })
  }
}
// [End my_animator1]

// [Start my_animator2]
@HMAnimator({ animatorName: 'myAnimator2' })
export class MyAnimator2 implements IHMAnimator {
  effect(enterHandle: HMAnimatorHandle, exitHandle: HMAnimatorHandle): void {
    enterHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.x = '100%';
      translateOption.y = 0;
    }).finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.x = 0;
    })


    enterHandle.duration = 500;
    exitHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.x = 0;
    }).finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.x = '100%';
    })
    exitHandle.duration = 500;
  }
}
// [End my_animator2]

// [Start comment_input_amimator]
@Component
export struct CommentInput {
  @State isLandscape: boolean = true;
  @State videoWidth: string = ''
  @State videoHeight: string = ''

  build() {
    Row() {
      Image($r('app.media.icon_comments'))
        .width(24)
        .height(24)
        .margin({ right: 16 })
        .onClick(() => {
          if (this.isLandscape) {
            HMRouterMgr.push({
              navigationId: this.queryNavigationInfo()?.navigationId,
              pageUrl: 'liveComments',
              param: {
                commentRenderNode: '',
              },
              animator: new MyAnimator2()
            }, {
              onResult: (paramInfo: PopInfo) => {
                this.videoWidth = '100%';
              }
            })
            this.videoWidth = '50%';
          } else {
            HMRouterMgr.push({
              navigationId: this.queryNavigationInfo()?.navigationId,
              pageUrl: 'liveComments',
              param: {
                commentRenderNode: '',
              },
              animator: new MyAnimator1()
            }, {
              onResult: (paramInfo: PopInfo) => {
                this.videoHeight = '100%'
              }
            })
            this.videoHeight = '30%'
          }
        });
    }
  }
}
// [End comment_input_amimator]