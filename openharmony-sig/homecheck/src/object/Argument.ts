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
import { IgnoreFile } from "./IgnoreFile";


export class Argument {
    public projectName: string;
    public ohosSdkPath: string;
    public hmsSdkPath: string;
    public projectPath: string;
    public product: string;

    public files: string[];
    public ignored: string[];
    public ignoreFiles: IgnoreFile[];
    public rules: string[];
    public ruleSet: string[];
    public overrides: Argument[];

    public generateArgument(config: any): void {
        if (config.projectName) {
            this.projectName = config.projectName;
        } else {
            this.projectName = "";
        }
        if (config.ohosSdkPath) {
            this.ohosSdkPath = config.ohosSdkPath;
        } else {
            this.ohosSdkPath = "";
        }
        if (config.hmsSdkPath) {
            this.hmsSdkPath = config.hmsSdkPath;
        } else {
            this.hmsSdkPath = "";
        }
        if (config.projectPath) {
            this.projectPath = config.projectPath;
        } else {
            this.projectPath = "";
        }
        if (config.product) {
            this.product = config.product;
        } else {
            this.product = "";
        }
        if (config.files) {
            this.files = config.files;
        } else {
            this.files = ['**/*.*'];
        }
        if (config.ignored) {
            this.ignored = config.ignored;
        } else {
            this.ignored = [];
        }
        if (config.ignoreFiles) {
            let ignoreFiles: IgnoreFile[] = [];
            let ignoreFileList = config.ignoreFiles;
            ignoreFileList.forEach((file: any) => {
                let tempIgnoreFile = new IgnoreFile();
                tempIgnoreFile.generateIgnoreLines(file)
                ignoreFiles.push(tempIgnoreFile);
            });
            this.ignoreFiles = ignoreFiles;
        }
        if (config.rules) {
            this.rules = config.rules;
        } else {
            this.rules = [];
        }
        if (config.ruleSet) {
            this.ruleSet = config.ruleSet;
        } else {
            this.ruleSet = [];
        }
        if (config.overrides) {
            let overrides: Argument[] = [];
            let overArguments = config.overrides;
            overArguments.forEach((overArgument: any) => {
                let tempArg = new Argument();
                tempArg.generateArgument(overArgument);
                overrides.push(tempArg);
            });
            this.overrides = overrides;
        } else {
            this.overrides = [];
        }
    }

    /**
     * 判断传入路径是否匹配exclude中的元素
     * @param path
     * @param isOverride
     * @param isIde
     */
    // public isShouldBeChecked(path: string, isOverride: boolean, isIde: boolean): boolean {
    //     if (isIde && !isOverride) {
    //         for (let pattern of this.excluded) {
    //             if(minimatch(path,pattern)){
    //                 return false;
    //             }
    //         }
    //         return true;
    //     }else {
    //         if (this.files.length == 0) {
    //             return false;
    //         }
    //         for (let pattern of this.excluded) {
    //             if(minimatch(path,pattern)){
    //                 return false;
    //             }
    //         }
    //         for (let pattern of this.files) {
    //             if(minimatch(path,pattern)){
    //                 return true;
    //             }
    //         }
    //         return false;
    //     }
    // }
}
