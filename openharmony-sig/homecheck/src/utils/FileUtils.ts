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
import * as fs from "fs";
import * as path from "path";
import { minimatch } from "minimatch";
import { Argument } from "../object/Argument";

export class FileUtils {
    public static readFile(fileName: string): string {
        return fs.readFileSync(fileName, 'utf8').replace(/^\ufeff/u, '');
    }

    /**
     * 获取配置中所有需要检查的文件，包括override中配置的
     * @param argument
     */
    public static getAllFiles(argument: Argument): string[] {
        let result = this.getFiles(argument.projectPath, argument.files, argument.ignored);
        let overrides = argument.overrides;
        if (overrides) {
            for (let override of overrides) {
                result = result.concat(this.getAllFiles(override));
                result = [...new Set(result)];
            }
        }
        return result;
    }

    /**
     * 获取指定目录下的所有文件类型的决定路径
     * 
     * @param dir 需要获取的文件的根目录
     * @param fileTypes 需要获取的文件类型
     * @param ignoreDirs 根目录下的忽略的目录
     */
    public static getFiles(dir: string, fileTypes: string[], ignoreDirs: string[] = []): string[] {
        let result: string[] = [];
        let files = fs.readdirSync(dir, { withFileTypes: true });
        for (let file of files) {
            let filePath = path.join(dir, file.name);
            if (file.isDirectory()) {
                result = result.concat(this.getFiles(filePath, fileTypes, ignoreDirs));
            } else {
                if (fileTypes.some(fileType => minimatch(filePath, fileType)) && !ignoreDirs.some(ignoreDir => minimatch(filePath, ignoreDir))) {
                    result.push(filePath);
                }
            }
        }

        return result;
    }
}