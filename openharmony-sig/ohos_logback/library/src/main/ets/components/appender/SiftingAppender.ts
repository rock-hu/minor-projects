/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { LoggerContext } from '../slf4j/LoggerContext';
import { Filter } from '../config/Filter'
import { Level } from '../Level';
import { FilterEnum } from '../FilterEnum'
import { Match } from '../config/Match'
import { Formatter } from '../formatter/Formatter';
import { FileIO } from '../io/FileIO'
import { MDC } from '../mdc/MDC'
import { Appender } from '../config/Appender'
import { AppenderEnum } from '../AppenderEnum';
import { Context } from '@ohos.abilityAccessCtrl';
import { makeMsg } from '../util/PatternUtils';

export class SiftingAppender {
    private effectiveLevelInt: number
    private pattern: string
    private className: string
    private filter: Filter
    private fileNamePattern: string
    private appender: Appender
    private map = new Map();
    private context: Context

    constructor(context: Context, appender: Appender) {
        this.context = context
        this.appender = appender
        for (let i = 0; i < appender.sift.mdc.length; i++) {
            let fileIO = new FileIO(this.context)
            let key = this.appender.sift.mdc[i].key
            let value = this.appender.sift.mdc[i].value
            fileIO.create(this.appender.sift.appender.file.replace('${' + key + '}', value))
            this.map.set(value, fileIO)
        }
    }

    start(loggerContext: LoggerContext, effectiveLevelInt: number, appenderArray: Array<Appender>, level: Level, msg: string) {
        for (let i = 0; i < appenderArray.length; i++) {
            if (appenderArray[i].className === AppenderEnum.SIFT && appenderArray[i].sift && appenderArray[i].sift.appender && appenderArray[i].sift.appender.encoder) {
                this.pattern = appenderArray[i].sift.appender.encoder.pattern.msg
            }
            if (!!!appenderArray[i].sift.appender.filter) {
                // appender中没有配置filter，使用root或者logger中的level
                this.effectiveLevelInt = effectiveLevelInt
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.writeFile(loggerContext, level, makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].sift.appender.filter.className === FilterEnum.THRESHOLD) {
                this.effectiveLevelInt = appenderArray[i].sift.appender.filter.level.getLevelInt()
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.writeFile(loggerContext, level, makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].sift.appender.filter.className === FilterEnum.LEVEL &&
                appenderArray[i].sift.appender.filter.onMatch === Match.ACCEPT && appenderArray[i].sift.appender.filter.onMismatch === Match.DENY &&
                level.getLevelStr() === appenderArray[i].sift.appender.filter.level.getLevelStr()) {
                this.writeFile(loggerContext, level, makeMsg(loggerContext, this.pattern, level, msg))
            }
        }
    }

    private writeFile(loggerContext: LoggerContext, level: Level, msg: string) {
        for (let [key, value] of this.map) {
            for (let value2 of MDC.mdcMap.values()) {
                if (key === value2) {
                    value.write(Formatter.format(loggerContext, this.pattern, level.getLevelStr(), msg))
                }
            }
        }
    }
}