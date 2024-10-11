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

import LoggerFactory from "src/utils/LoggerFactory";
import ForeachArgsCheck from "./performance/ForEachArgsCheck";
import TimezoneInterfaceCheck from "./performance/TimezoneInterfaceCheck";

const logger = LoggerFactory.getLogger('ProxyChecker');

const Checkers: any = {
    TimezoneInterfaceCheck,
    ForeachArgsCheck,
};

export class ProxyChecker {
    static getClass(checkerName: string) {
        if (!Checkers[checkerName]) {
            logger.debug(`checker ${checkerName} is not found`);
            return null;
        }
        return new (Checkers[checkerName])();
    }
}