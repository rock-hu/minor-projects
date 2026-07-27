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

import { Level } from '../Level';
import { Logger } from '../Logger';
import { ILoggerFactory } from './ILoggerFactory'
import { Context } from '@ohos.abilityAccessCtrl';
import { Configuration } from '../config/Configuration';

export class LoggerContext implements ILoggerFactory {
    private root: Logger
    private name: string
    private context: Context
    private config: Configuration

    constructor(context: Context, config?: Configuration) {
        this.context = context
        this.config = config
    }

    getLogger(name: string): Logger{
        if (name == null) {
            throw new Error("name argument cannot be null")
        }
        if (Logger.ROOT_LOGGER_NAME === name) {
            this.root = new Logger(this, this.context, Logger.ROOT_LOGGER_NAME, this.config)
            this.root.setLevel(Level.INFO);
            return this.root;
        }
        this.name = name
        this.root = new Logger(this, this.context, this.name, this.config)
        return this.root;
    }

    getLoggerName(): string{
        return this.name
    }
}
