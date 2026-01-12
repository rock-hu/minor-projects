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

// @ts-nocheck
import { Logger } from '../Logger';
import { LoggerContext } from './LoggerContext';
import { ILoggerFactory } from './ILoggerFactory'
import { Configuration } from '../config/Configuration'
import { Context } from '@ohos.abilityAccessCtrl'

export class LoggerFactory {
    public static getLogger(context: Context, name: string, config?: Configuration): Logger{
        if (!!!context) {
            throw new Error('Error,context is null')
        }
        let iLoggerFactory = LoggerFactory.getILoggerFactory(context, config)
        return iLoggerFactory.getLogger(name)
    }

    public static getILoggerFactory(context: Context, config?: Configuration): ILoggerFactory{
        return new LoggerContext(context, config)
    }
}