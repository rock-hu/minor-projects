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

import { HMLifecycle, HMLifecycleContext, IHMLifecycle } from '@hadss/hmrouter';
import { CommonConstants } from '../component/common/constants/CommonConstants'

@HMLifecycle({lifecycleName: 'ExitAppLifecycle'})
export class ExitAppLifecycle implements IHMLifecycle {
  private lastTime: number = 0;
  onBackPressed(ctx: HMLifecycleContext): boolean {
    let time = new Date().getTime();
    if(time - this.lastTime > 1000) {
      this.lastTime = time;
      ctx.uiContext.getPromptAction().showToast({
        message: CommonConstants.EXIT_TOAST,
        duration: 1000
      });
      return true;
    } else {
      return false;
    }
  }
}