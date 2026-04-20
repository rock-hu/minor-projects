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
import { ArkFile, FileSignature, Scene } from "@ArkAnalyzer/src";
import * as path from "node:path";
import LoggerFactory from "./LoggerFactory";

export class SceneUtils {

    public static getArkFile(filePath: string, scene: Scene): ArkFile[] {
        let logger = LoggerFactory.getLogger("SceneUtils");
        let arkFiles: ArkFile[] = [];
        let fileName = path.relative(scene.getRealProjectDir(), filePath);
        let fileSignature = new FileSignature(scene.getProjectName(), fileName);
        let file = scene.getFile(fileSignature);
        if (file) {
            arkFiles.push(file);
        } else {
            arkFiles = scene.getFiles();
        }
        return arkFiles;
    }
}