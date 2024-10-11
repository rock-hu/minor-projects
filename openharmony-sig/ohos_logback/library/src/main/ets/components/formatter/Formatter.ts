/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import { DateFormat } from '../format/DateFormat';
import { LoggerContext } from '../slf4j/LoggerContext';

export class Formatter {
    private static FORMAT = 'yyyy-MM-dd HH:mm:ss'

    static format(loggerContext: LoggerContext, pattern: string, level: string, msg: string): string {
        if (pattern.match('%date') != null) {
            pattern = pattern.replace('%date', DateFormat.format(new Date(), this.FORMAT))
        }
        if (pattern.match('%level') != null) {
            pattern = pattern.replace('%level', level)
        }
        if (pattern.match('%logger') != null) {
            pattern = pattern.replace('%logger', loggerContext.getLoggerName())
        }
        if (pattern.match('%msg') != null) {
            pattern = pattern.replace('%msg', msg)
        }
        return pattern
    }
}