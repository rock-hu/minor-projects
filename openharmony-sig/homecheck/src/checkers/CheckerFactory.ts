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
import { BaseChecker } from "./BaseChecker";
import { ProxyChecker } from "./CheckerIndex";

export class CheckerFactory {
    static checkerMap: Map<string, BaseChecker> = new Map();

    static createChecker(ruleId: string): BaseChecker | null {
        const checkerName = CheckerFactory.getCheckerName(ruleId)
        const checker = this.checkerMap.get(checkerName);

        if (checker) {
            return checker;
        } else {
            const checkerInstance = ProxyChecker.getClass(checkerName);
            if (!checkerInstance) {
                return null;
            }
            this.checkerMap.set(checkerName, checkerInstance);
            return checkerInstance;
        }
    }

    /**
     * 根据ruleId, 获取checker的名称 todo
     * @param ruleId
     * @private
     */
    private static getCheckerName(ruleId: string) {
        return this.getFileName(ruleId);
    }

    private static getFileName(ruleId: string) {
        // 去掉前缀 @xxx/
        const withoutPrefix = ruleId.replace(/^@[^/]+\//, '');

        // 用连字符分割单词
        const words = withoutPrefix.split('-');

        // 将每个单词的首字母大写，其余字母小写
        const pascalCaseWords = words.map(word => word.charAt(0).toUpperCase() + word.slice(1).toLowerCase());

        // 拼接成 PascalCase 格式
        let result = pascalCaseWords.join('');

        // 如果不以 'Check' 结尾，则添加 'Check'
        if (!result.toLowerCase().endsWith('check')) {
            result += 'Check';
        }
        return result;
    }

    private static getFilePath(ruleId: string) {
        // 使用正则表达式匹配 '@' 和 '/' 之间的部分
        const regex = /^@([^/]+)\//;
        const match = ruleId.match(regex);

        if (!match) {
            // 如果输入字符串不符合预期格式，则返回空字符串
            return '';
        }

        const [, prefix] = match;

        // 返回提取出的前缀部分
        return prefix + '/';
    }
}