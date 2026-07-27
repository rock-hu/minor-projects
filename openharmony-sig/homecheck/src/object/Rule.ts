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
import { EnumUtils } from "../utils/EnumUtils";

export class Rule {
    ruleId: string;
    alert: ALERT_LEVEL;
    allowExpressions: boolean
    ignoreRestArgs: boolean
    otherObject: Object[] = []

    constructor(ruleString: string) {
        let strings = ruleString.split(":");
        this.ruleId = strings[0];
        this.alert = EnumUtils.getEnumValues(Number(strings[1]), ALERT_LEVEL);
        this.allowExpressions = false;
        this.ignoreRestArgs = false;
    }

}

export enum ALERT_LEVEL {
    OFF = 0,
    WARN = 1,
    ERROR = 2,
    SUGGESTION = 3,
}