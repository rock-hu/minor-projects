/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import AbilityConstant from '@ohos.app.ability.AbilityConstant';
import hilog from '@ohos.hilog';
import UIAbility from '@ohos.app.ability.UIAbility';
import Want from '@ohos.app.ability.Want';
import window from '@ohos.window';

export default class EmptyAbility extends UIAbility {
    onCreate(want: Want, launchParam: AbilityConstant.LaunchParam): void {
        hilog.info(0, 'VMB', '%{public}s', 'ABILITY ON CREATE');
    }

    onWindowStageCreate(windowStage: window.WindowStage): void {
        hilog.info(0, 'VMB', '%{public}s', 'WINDOW CREATE');
        windowStage.loadContent('pages/Index', (err, data) => {
            if (err.code) {
                hilog.info(0, 'VMB', 'ERROR LOADING PAGE: %{public}s',
                           JSON.stringify(err) ?? '');
            } else {
                hilog.info(0, 'VMB', '%{public}s', 'PAGE LOADED');
            }
        });
        hilog.info(0, 'VMB', '%{public}s', 'Warmup 1:1234 ops, 1234567 ns/op');
        hilog.info(0, 'VMB', '%{public}s', 'Iter 1: 1234 ops, 1234567 ns/op');
        hilog.info(0, 'VMB', '%{public}s', 'Benchmark result: 1234567 ns/op');
    }

}