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
export class IgnoreFile {
    public filePath: string;
    public ignoreLines: IgnoreLine[];

    public generateIgnoreLines(config: any): void {

        this.filePath = config["filePath"];
        let ignoreLines = config["disableLine"];
        this.ignoreLines = [];
        if (!ignoreLines) {
            return;
        }
        for (let line of ignoreLines) {
            if (!line.hasOwnProperty("rules")) {
                continue;
            }
            let ignoreLine: IgnoreLine = new IgnoreLine();
            ignoreLine.lineNumber = line.line || 0;
            ignoreLine.ruleIds = line.rules;
            this.ignoreLines.push(ignoreLine);
        }
    }
}

export class IgnoreLine {
    //不需要检查的行号，为0则代表整个文件都不需要检查
    public lineNumber: number;
    //对应行号不需要检查的规则id，为空则代表所有规则都不需要检查
    public ruleIds: string[];
}