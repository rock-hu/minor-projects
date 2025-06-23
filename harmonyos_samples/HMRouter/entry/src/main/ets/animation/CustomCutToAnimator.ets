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
 * 最佳实践：特定页面设置自定义转场
 */

import {
  HMAnimator,
  HMAnimatorHandle,
  HMRouterMgr,
  IHMAnimator,
  OpacityOption,
  ScaleOption,
  TranslateOption
} from "@hadss/hmrouter";

// [Start custom_cut_to_animator]

@HMAnimator({ animatorName: 'CustomAnimator' })
export class CustomAnimator implements IHMAnimator {
  effect(enterHandle: HMAnimatorHandle, exitHandle: HMAnimatorHandle): void {
    // to animator
    enterHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '100%'
      scaleOption.x = 0.7;
      opacityOption.opacity = 0.3;
    })
    enterHandle.finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '0'
      scaleOption.x = 1;
      opacityOption.opacity = 1;
    })
    enterHandle.duration = 400;
    enterHandle.curve = Curve.Linear;

    // cut animator
    exitHandle.start((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '0'
      scaleOption.x = 1;
      opacityOption.opacity = 1;
    })
    exitHandle.finish((translateOption: TranslateOption, scaleOption: ScaleOption,
      opacityOption: OpacityOption) => {
      translateOption.y = '100%'
      scaleOption.x = 0.7;
      opacityOption.opacity = 0.3;
    })
    exitHandle.duration = 400;
    enterHandle.curve = Curve.Linear;
  }
}

// [End custom_cut_to_animator]

export class CustomAnimatorTransfer {
  animatorTo() {
    // [Start animator_transfer]
    HMRouterMgr.push({ pageUrl: 'ProductContent', animator: new CustomAnimator() })
    // [End animator_transfer]
  }
}