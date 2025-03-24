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
import { Argument } from "../object/Argument";
import { Scene, SceneConfig } from "@ArkAnalyzer/src";
import * as path from "node:path";
import * as fs from "node:fs";
import { Sdk } from "@ArkAnalyzer/src/Config";
import LoggerFactory from "../utils/LoggerFactory";

const logger = LoggerFactory.getLogger('buildScene');

export function buildSceneForCommunity(argument: Argument): Scene {
    const sdks = genSdks(argument);
    const sceneConfig = new SceneConfig();
    sceneConfig.buildConfig(argument.projectName, argument.projectPath, sdks);
    const scene = new Scene();
    scene.buildBasicInfo(sceneConfig);
    scene.buildScene4HarmonyProject();
    scene.inferTypes();
    return scene;
}

export function genSdks(argument?: Argument): Sdk[] {
    let sdks: Sdk[] = [];
    const sdkConfigPath = path.resolve('./resources/sdkConfig.json');
    if (fs.existsSync(sdkConfigPath)) {
        const configurations = JSON.parse(fs.readFileSync(sdkConfigPath, 'utf-8'));
        sdks = configurations.sdks ?? [];
    }
    if (argument) {
        sdks.forEach(sdk => {
            if (sdk.name == 'ohosSdk') {
                sdk.path = argument.ohosSdkPath
            } else if (sdk.name === 'hmsSdk') {
                sdk.path = argument.hmsSdkPath
            } else {
                sdk.path = path.resolve(sdk.path);
            }
        });
    }
    return sdks;
}