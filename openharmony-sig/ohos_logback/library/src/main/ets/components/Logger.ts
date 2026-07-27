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
import { Configuration } from './config/Configuration'
import { LoggerContext } from './slf4j/LoggerContext'
import { Level } from './Level'
import { AppenderEnum } from './AppenderEnum'
import { Appender } from './config/Appender'
import { ILogger } from './slf4j/ILogger'
import { ConsoleAppender } from './appender/ConsoleAppender'
import { FileAppender } from './appender/FileAppender'
import { RollingFileAppender } from './appender/RollingFileAppender'
import { DBAppender } from './appender/DBAppender'
import { SiftingAppender } from './appender/SiftingAppender'
import { SocketAppender } from './appender/SocketAppender'
import { SMTPAppender } from './appender/SMTPAppender'
import { HiLog } from './log/HiLog';
import { Formatter } from './formatter/Formatter';
import { Context } from '@ohos.abilityAccessCtrl'

const defaultConfigure: Configuration = {
  appender: [{
    name: 'CONSOLE',
    className: AppenderEnum.CONSOLE,
    encoder: {
      pattern: {
        msg: '%date %level %logger %msg'
      }
    }
  }],
  root: {
    level: Level.DEBUG,
    appenderRef: {
      ref: 'CONSOLE'
    }
  }
}

export class Logger implements ILogger {
  private name: string
  private level: Level = Level.TRACE_INT
  static ROOT_LOGGER_NAME = "ROOT"
  private loggerContext: LoggerContext
  private effectiveLevelInt: number
  private configure: Configuration
  private consoleAppender: ConsoleAppender
  private fileAppender: FileAppender
  private siftingAppender: SiftingAppender
  private dBAppender: DBAppender
  private socketAppender: SocketAppender
  private SMTPAppender: SMTPAppender
  private rollingFileAppender: RollingFileAppender
  private tag = ''
  private appenderArray: Array<Appender> = []
  private context: Context

  constructor(loggerContext: LoggerContext, context: Context, name: string, configure?: Configuration) {
    this.name = name
    this.loggerContext = loggerContext
    this.context = context
    this.configure = configure

    if (!!!this.configure) {
      this.appenderArray = defaultConfigure.appender
      this.effectiveLevelInt = defaultConfigure.root.level.getLevelInt()
    } else {
      this.doConfigure()
    }
  }

  public trace(msg: string) {
    this.filterAndLog(Level.TRACE, msg)
  }

  public debug(msg: string) {
    this.filterAndLog(Level.DEBUG, msg)
  }

  public info(msg: string) {
    this.filterAndLog(Level.INFO, msg)
  }

  public warn(msg: string) {
    this.filterAndLog(Level.WARN, msg)
  }

  public error(msg: string) {
    this.filterAndLog(Level.ERROR, msg)
  }

  public fatal(msg: string) {
    this.filterAndLog(Level.FATAL, msg)
  }

  private defaultPrintLog(level: Level, msg: string) {
    this.level = level
    if (this.effectiveLevelInt > this.level.getLevelInt()) {
      return
    }
    msg = Formatter.format(this.loggerContext, defaultConfigure.appender[0].encoder.pattern.msg, this.level.getLevelStr(), msg)
    HiLog.log(this.level.getLevelStr(), msg, this.tag)
  }

  private filterAndLog(level: Level, msg: string) {
    this.level = level
    if (!!!this.configure) {
      this.defaultPrintLog(level, msg);
    } else {
      for (let i = 0; i < this.appenderArray.length; i++) {
        switch (this.appenderArray[i].className) {
          case AppenderEnum.CONSOLE:
            this.consoleAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg, this.tag)
            break
          case AppenderEnum.FILE:
            this.fileAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
          case AppenderEnum.SMTP:
            this.SMTPAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
          case AppenderEnum.SIFT:
            this.siftingAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
          case AppenderEnum.DB:
            this.dBAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
          case AppenderEnum.ROLLING_FILE:
            this.rollingFileAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
          case AppenderEnum.SOCKET:
            this.socketAppender.start(this.loggerContext, this.effectiveLevelInt, this.appenderArray, this.level, msg)
            break
        }
      }

      // 没有配置或者没匹配，默认输出逻辑
      if (!!!this.appenderArray || this.appenderArray.length == 0) {
        this.defaultPrintLog(level, msg);
      }
    }
  }

  private defaultConfig() {
    if (!!!this.configure.root) {
      this.effectiveLevelInt = defaultConfigure.root.level.getLevelInt()
    } else {
      this.effectiveLevelInt = this.configure.root.level.getLevelInt()
    }

    if (this.configure.appender && this.configure.appender.length > 0 && this.configure.root?.appenderRef?.ref) {
      for (let i = 0; i < this.configure.appender.length; i++) {
        if (this.configure.root.appenderRef.ref === this.configure.appender[i].name) {
          this.appenderArray.push(this.configure.appender[i])
        }
      }
    }

    if (!!!this.appenderArray || this.appenderArray.length == 0) {
      this.appenderArray = defaultConfigure.appender
      this.effectiveLevelInt = defaultConfigure.root.level.getLevelInt()
    }
  }

  private doConfigure() {
    if (!!!this.configure.logger) {
      // 当没有配置logger时使用root
      this.defaultConfig();
    } else { //logger
      for (let i=0; i < this.configure.logger.length; i++) {
        if (this.loggerContext.getLoggerName() === this.configure.logger[i].name) {
          this.effectiveLevelInt = this.configure.logger[i].level.getLevelInt()
          for (let j = 0; j < this.configure.appender.length; j++) {
            if (this.configure.logger[i].appenderRef.ref === this.configure.appender[j].name) {
              this.appenderArray.push(this.configure.appender[j])
            }
          }
        }
      }

      // 没有配置或者没匹配，添加默认输出逻辑
      if (!!!this.appenderArray || this.appenderArray.length == 0) {
        this.defaultConfig();
      }
    }

    for (let i = 0; i < this.appenderArray.length; i++) {
      if (this.appenderArray[i].className === AppenderEnum.CONSOLE) {
        this.consoleAppender = new ConsoleAppender()
      }
      if (this.appenderArray[i].className === AppenderEnum.FILE) {
        this.fileAppender = new FileAppender(this.context, this.appenderArray[i])
      }
      if (this.appenderArray[i].className === AppenderEnum.SMTP) {
        this.SMTPAppender = new SMTPAppender()
        this.SMTPAppender.bind(this.appenderArray[i])
      }
      if (this.appenderArray[i].className === AppenderEnum.SIFT) {
        this.siftingAppender = new SiftingAppender(this.context, this.appenderArray[i])
      }
      if (this.appenderArray[i].className === AppenderEnum.DB) {
        this.dBAppender = new DBAppender(this.context, this.appenderArray[i])
      }
      if (this.appenderArray[i].className === AppenderEnum.ROLLING_FILE) {
        this.rollingFileAppender = new RollingFileAppender(this.context, this.appenderArray[i])
      }
      if (this.appenderArray[i].className === AppenderEnum.SOCKET) {
        this.socketAppender = new SocketAppender(this.appenderArray[i])
        this.socketAppender.bind(this.appenderArray[i])
      }
    }
  }

  public setLevel(level: Level) {
    this.level = level
    this.effectiveLevelInt = level.getLevelInt();
  }

  public setTag(tag: string) {
    this.tag = tag
  }

  public getLevel() {
    return this.level;
  }

  public getTag() {
    return this.tag;
  }
}