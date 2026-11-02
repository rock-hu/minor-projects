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
import { FileUtils } from "./FileUtils";
import { Rule } from "../object/Rule";
import LoggerFactory from "./LoggerFactory";

const CONFIG_DIR: string = './src/config/';
const logger = LoggerFactory.getLogger('ConfigUtils');

export class ConfigUtils {
    public static getConfig(configPath: string): any | null {
        try {
            return JSON.parse(FileUtils.readFile(configPath));
        } catch (e) {
            logger.error(e);
            return null;
        }
    }

    /**
     * 从配置文件中获取规则
     * @param argument
     * @returns ruleId -- Rule
     */
    public static getRuleMap(argument: Argument): Map<string, Rule> {
        let ruleMap: Map<string, Rule> = new Map();
        argument.ruleSet.forEach(string => {
            let ruleSetMap = ConfigUtils.getRuleSetMap();
            let ruleSet = ruleSetMap.get(string);
            if (ruleSet !== undefined) {
                ruleSet.forEach(ruleString => {
                    let rule = new Rule(ruleString);
                    ruleMap.set(rule.ruleId, rule);
                })
            }
        })
        let ruleArr = argument.rules;
        if (ruleArr) {
            ruleArr.forEach(ruleSting => {
                let rule = new Rule(ruleSting);
                ruleMap.set(rule.ruleId, rule);
            })
        }
        return ruleMap;
    }

    /**
     * 读取RuleSet.json中配置的规则集
     */
    static getRuleSetMap(): Map<string, string[]> {
        let ruleSetMap = new Map<string, string[]>();
        let config: Record<string, string[]> = JSON.parse(FileUtils.readFile(CONFIG_DIR + 'RuleSet.json'));
        for (let [key, value] of Object.entries(config)) {
            ruleSetMap.set(key, value);
        }
        return ruleSetMap;
    }
}