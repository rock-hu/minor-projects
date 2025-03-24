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

import { SceneConfig } from '@ArkAnalyzer/src/Config';
import { Scene } from '@ArkAnalyzer/src/Scene';
import { createScope } from 'src/processFunction/beforeCheckFuncation/createScope';
import { genSdks } from 'src/processFunction/BuildScene';

export function getScene(projectPath: string, isNeedScope: boolean) {
    let config: SceneConfig = new SceneConfig();
    config.buildConfig('arkCheckTest', projectPath, genSdks());
    config.buildFromProjectDir(projectPath);
    let scene = new Scene();
    scene.buildSceneFromProjectDir(config);
    scene.inferTypes();
    if (isNeedScope) {
        createScope(scene);
    }
    return scene;
}
