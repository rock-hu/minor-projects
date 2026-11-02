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
import { Rule } from "../object/Rule";
import { FileUtils } from "../utils/FileUtils";
import { ConfigUtils } from "../utils/ConfigUtils";
import LoggerFactory from "../utils/LoggerFactory";

const logger = LoggerFactory.getLogger('createFileRulesMapForCommunity');

export function createFileRulesMapForCommunity(argument: Argument): Map<string, Rule[]> {
    //配置文件传入的所有文件路径
    const detectFilesPaths = FileUtils.getAllFiles(argument);

    //配置的规则
    const ruleMap = ConfigUtils.getRuleMap(argument);

    const fileRulesMap: Map<string, Rule[]> = new Map();
    for (const filePath of detectFilesPaths) {
        const tempRulesMap = new Map();
        ruleMap.forEach((value, key) => {
            tempRulesMap.set(key, value);
        })
        fileRulesMap.set(filePath, Array.from(tempRulesMap.values()));
    }

    //检查额外规则覆盖
    argument.overrides?.forEach(override => {
        let overrideFileRulesMap = createFileRulesMapForCommunity(override);
        overrideFileRulesMap.forEach((value, key) => {
            //合并
            fileRulesMap.set(key, value);
        });
    });
    return fileRulesMap;
}
