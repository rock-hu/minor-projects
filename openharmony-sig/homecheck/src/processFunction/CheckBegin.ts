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
import { Rule } from "../object/Rule";
import { Scene } from "@ArkAnalyzer/src";
import { CheckerFactory } from "../checkers/CheckerFactory";

export function checkForCommunity(fileRulesMap: Map<string, Rule[]>, scene: Scene) {
    let map = new Map();
    // 遍历文件规则，根据规则id创建checker，并调用check方法
    for (const [filePath, rules] of fileRulesMap) {
        for (const rule of rules) {
            const checker = CheckerFactory.createChecker(rule.ruleId);
            if (!checker) {
                continue;
            }

            const reports = checker.check(scene, filePath, rule);
            for (const report of reports) {
                if (!map.has(report.getId())) {
                    map.set(report.getId(), report);
                }
            }
        }
    }
    return Array.from(map.values());
}