/*
 * Copyright (c) 2024 - 2025 Huawei Device Co., Ltd.
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
import { BaseChecker } from '../../checker/BaseChecker';
import { Rule } from '../../model/Rule';
import { ProxyChecker } from './CheckerIndex';

export class CheckerFactory {
    static getChecker(rule: Rule): BaseChecker | null {
        const checkerInstance = ProxyChecker.getClass(rule.ruleId);
        if (!checkerInstance) {
            return null;
        }
        checkerInstance.rule = rule;
        return checkerInstance;
    }
}