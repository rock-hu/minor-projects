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
import type { Logger } from 'log4js';
import { configure, getLogger } from 'log4js';


export default class LoggerFactory {
    private static FILE_PATTERN: string = '';
    private static CONSOLE_PATTERN: string = '';
    public static getLogger(tag: string): Logger {
        let config = require('../config/log4js.json');

        let filePattern = this.getFilePattern(config);
        filePattern = filePattern.replace('tag', tag);
        config.appenders.default.layout.pattern = filePattern;

        let consolePattern = this.getConsolePattern(config);
        consolePattern = consolePattern.replace('tag', tag);
        config.appenders.console.layout.pattern = consolePattern;

        configure(config);
        return getLogger("default");
    }

    private static getFilePattern(config: any) {
        if (this.FILE_PATTERN != '') {
            return this.FILE_PATTERN;
        } else {
            this.FILE_PATTERN = config.appenders.default.layout.pattern;
        }
        return this.FILE_PATTERN;
    }
    private static getConsolePattern(config: any) {
        if (this.CONSOLE_PATTERN != '') {
            return this.CONSOLE_PATTERN;
        } else {
            this.CONSOLE_PATTERN = config.appenders.console.layout.pattern;
        }
        return this.CONSOLE_PATTERN;
    }
}