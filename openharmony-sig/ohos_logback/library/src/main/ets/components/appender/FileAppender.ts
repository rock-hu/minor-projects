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
import { Appender } from '../config/Appender'
import { Formatter } from '../formatter/Formatter';
import { AppenderEnum } from '../AppenderEnum';
import { FileIO } from '../io/FileIO';
import { Context } from '@ohos.abilityAccessCtrl';
import { makeMsg } from '../util/PatternUtils';

export class FileAppender {
    private effectiveLevelInt: number
    private pattern: string
    private className: string
    private filter: Filter
    private fileNamePattern: string
    private fileIO: FileIO
    private context: Context

    constructor(context: Context, appender: Appender) {
        this.context = context
        this.fileIO = new FileIO(this.context)
        this.fileIO.create(appender.file)
    }

    start(loggerContext: LoggerContext, effectiveLevelInt: number, appenderArray: Array<Appender>, level: Level, msg: string) {
        for (let i = 0; i < appenderArray.length; i++) {
            if (appenderArray[i].className === AppenderEnum.FILE && appenderArray[i].encoder) {
                this.pattern = appenderArray[i].encoder.pattern.msg
            }
            if (!!!appenderArray[i].filter) {
                // appender中没有配置filter，使用root或者logger中的level
                this.effectiveLevelInt = effectiveLevelInt
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.fileIO.write(makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].filter.className === FilterEnum.THRESHOLD) {
                this.effectiveLevelInt = appenderArray[i].filter.level.getLevelInt()
                if (this.effectiveLevelInt > level.getLevelInt()) return;
                this.fileIO.write(makeMsg(loggerContext, this.pattern, level, msg))
            } else if (appenderArray[i].filter.className === FilterEnum.LEVEL) {
                if (appenderArray[i].filter.onMatch === Match.ACCEPT && appenderArray[i].filter.onMismatch === Match.DENY) {
                    if (level.getLevelStr() === appenderArray[i].filter.level.getLevelStr()) {
                        this.fileIO.write(makeMsg(loggerContext, this.pattern, level, msg))
                    }
                }
            }
        }
    }
}