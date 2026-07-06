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
import { Formatter } from '../formatter/Formatter';
import { FileIO } from '../io/FileIO'
import { PreferencesUtils } from '../util/PreferencesUtils'
import { Appender } from '../config/Appender'
import { DateFormat } from '../format/DateFormat';
import { Context } from '@ohos.abilityAccessCtrl';
import { AppenderEnum } from '../AppenderEnum';
import { Match } from '../config/Match';
import { FilterEnum } from '../FilterEnum';
import { makeMsg } from '../util/PatternUtils';

export class RollingFileAppender {
    private effectiveLevelInt: number
    private pattern: string
    private className: string
    private filter: Filter
    private maxHistory: number
    private fileName: string
    private filePattern: string
    private fileIO: FileIO
    private context: Context

    constructor(context: Context, appender: Appender) {
        this.fileName = appender.rollingPolicy.fileName
        this.filePattern = appender.rollingPolicy.filePattern
        this.maxHistory = appender.rollingPolicy.maxHistory
        this.pattern = appender.encoder.pattern.msg
        this.context = context;
        this.fileIO = new FileIO(this.context)
        this.fileIO.create(this.fileName)
        PreferencesUtils.putData(this.context, PreferencesUtils.CURRENT_TIME, PreferencesUtils.CURRENT_TIME, new Date().getTime())
    }

    async start(loggerContext: LoggerContext, effectiveLevelInt: number, appenderArray: Array<Appender>, level: Level, msg: string) {
        for (let i = 0; i < appenderArray.length; i++) {
            if (appenderArray[i].className === AppenderEnum.ROLLING_FILE && appenderArray[i].encoder) {
                this.pattern = appenderArray[i].encoder.pattern.msg
            }
            if (!!!appenderArray[i].filter) {
                // appender中没有配置filter，使用root或者logger中的level
                this.effectiveLevelInt = effectiveLevelInt
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.writeMsg(makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].filter.className === FilterEnum.THRESHOLD) {
                this.effectiveLevelInt = appenderArray[i].filter.level.getLevelInt()
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.writeMsg(makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].filter.className === FilterEnum.LEVEL &&
                appenderArray[i].filter.onMatch === Match.ACCEPT && appenderArray[i].filter.onMismatch === Match.DENY &&
                level.getLevelStr() === appenderArray[i].filter.level.getLevelStr()) {
                this.writeMsg(makeMsg(loggerContext, this.pattern, level, msg))
            }
        }
    }

    private async writeMsg(msg: string) {
        let value = await PreferencesUtils.getData(this.context, PreferencesUtils.CURRENT_TIME, PreferencesUtils.CURRENT_TIME, 0)
        let dateNow = new Date()
        let timeDiff = dateNow.getTime() - value
        let days: number
        let newFile: string
        if (this.filePattern.match('%d{yyyy-MM-dd HH:mm:ss}') != null) {
            days = Math.floor(timeDiff / 1000)
            newFile = this.filePattern.replace('%d{yyyy-MM-dd HH:mm:ss}', DateFormat.format(new Date(), 'yyyy-MM-dd HH:mm:ss'))
        } else if (this.filePattern.match('%d{yyyy-MM-dd HH:mm}') != null) {
            days = Math.floor(timeDiff / (60 * 1000))
            newFile = this.filePattern.replace('%d{yyyy-MM-dd HH:mm}', DateFormat.format(new Date(), 'yyyy-MM-dd HH:mm'))
        } else if (this.filePattern.match('%d{yyyy-MM-dd HH}') != null) {
            days = Math.floor(timeDiff / (3600 * 1000))
            newFile = this.filePattern.replace('%d{yyyy-MM-dd HH}', DateFormat.format(new Date(), 'yyyy-MM-dd HH'))
        } else if (this.filePattern.match('%d{yyyy-MM-dd}') != null) {
            days = Math.floor(timeDiff / (24 * 3600 * 1000))
            newFile = this.filePattern.replace('%d{yyyy-MM-dd}', DateFormat.format(new Date(), 'yyyy-MM-dd'))
        }
        if (days > this.maxHistory) {
            let isRename = await this.fileIO.rename(newFile)
            if (isRename) {
                this.fileIO = new FileIO(this.context)
                this.fileIO.create(this.fileName)
                PreferencesUtils.putData(this.context, PreferencesUtils.CURRENT_TIME, PreferencesUtils.CURRENT_TIME, new Date().getTime())
            }
        }
        this.fileIO.write(msg)
    }
}