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

import { Logger, Level, LoggerFactory, MDC, Configuration } from '@ohos/logback'
import * as entry from '../entryability/EntryAbility'
import { Context } from '@ohos.abilityAccessCtrl'

@Entry
@Component
struct Index {
  scroller: Scroller = new Scroller()
  private config: Configuration = {
    appender: entry.appender,
    logger: entry.logger,
    root: entry.root
  };
  private context: Context = getContext();

  build() {
    Scroll(this.scroller) {
      Column() {
        // 无需配置文件，使用默认
        Text($r('app.string.Without_Configuration'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.hello")
            logger.setTag('logback')
            // print all
            logger.debug("this is debug log.")
            logger.info("this is info log.")
            logger.warn("this is warn log.")
            logger.error("this is error log.")
            logger.fatal("this is fatal log.")
          })
        // 无需配置文件，使用默认
        Text($r('app.string.log_level'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.hello")
            logger.setLevel(Level.INFO)
            logger.setTag('logback')
            // not print debug,print others
            logger.debug("this is debug log.")
            logger.info("this is info log.")
            logger.warn("this is warn log.")
            logger.error("this is error log.")
            logger.fatal("this is fatal log.")
          })
        // 配置文件中不需要配置logger节点，只需要配置root节点
        Text($r('app.string.root'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let config: Configuration = {
              appender: entry.appender,
              root: entry.root
            };
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.hello", config)
            logger.setTag('logback')
            // ROOT level is ERROR,print error、 fatal
            logger.debug("=====debug=====")
            logger.info("=====info=====")
            logger.warn("=====warn=====")
            logger.error("=====error=====")
            logger.fatal("=====fatal=====")
          })
        Text($r('app.string.logger_level'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.logger", this.config)
            logger.setTag('logback')
            // logger level is FATAL,print fatal
            logger.debug("=====debug=====")
            logger.info("=====info=====")
            logger.warn("=====warn=====")
            logger.error("=====error=====")
            logger.fatal("=====fatal=====")
          })
        Text($r('app.string.ConsoleAppender_ThresholdFilter'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.threshold", this.config)
            logger.setTag('logback')
            // threshold level is INFO，print info、warn、error、fatal
            logger.debug("=====debug=====")
            logger.info("=====info=====")
            logger.warn("=====warn=====")
            logger.error("=====error=====")
            logger.fatal("=====fatal=====")
          })
        Text($r('app.string.ConsoleAppender_LevelFilter'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.level", this.config)
            logger.setTag('logback')
            // level level is WARN,but onMismatch is Match.DENY,print warn
            logger.debug("=====debug=====")
            logger.info("=====info=====")
            logger.warn("=====warn=====")
            logger.error("=====error=====")
            logger.fatal("=====fatal=====")
          })
        Text($r('app.string.Write_logs_to_a_file_using_FileAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.file", this.config)
            // file path /data/app/el2/100/base/cn.openharmony.logback/haps/entry/files/111.log
            // file level is ERROR,print error、fatal to file 111.log
            logger.debug("this is debug")
            logger.info("this is info")
            logger.warn("this is warn")
            logger.error("this is error")
            logger.fatal("this is fatal")
          })
        Text($r('app.string.SMTPAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(async () => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.smtp", this.config)
            logger.debug("this is debug")
            logger.info("this is info")
            logger.warn("this is warn")
            logger.error("this is error")
            logger.fatal("this is fatal")
          })
        Text($r('app.string.SiftingAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.sift", this.config)
            // file path /data/app/el2/100/base/cn.openharmony.logback/haps/entry/files/100001.log
            MDC.put("userid", "100001")
            // sift level is INFO,print info、warn、error、fatal to file 100001.log
            logger.debug("=====debug 100001=====")
            logger.info("=====info 100001=====")
            logger.warn("=====warn 100001=====")
            logger.error("=====error 100001=====")
            logger.fatal("=====fatal 100001=====")

            // file path /data/app/el2/100/base/cn.openharmony.logback/haps/entry/files/100002.log
            MDC.put("userid", "100002");
            // sift level is INFO,print info、warn、error、fatal to file 100002.log
            logger.debug("debug 100002")
            logger.info("info 100002")
            logger.warn("warn 100002")
            logger.error("error 100002")
            logger.fatal("fatal 100002")
          })
        Text($r('app.string.DBAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.db", this.config)
            // file path /data/app/el2/100/database/cn.openharmony.logback/entry/rdb/logback_rdb.db
            // db level is INFO,print info、warn、error、fatal to db file
            logger.debug("=====debug=====")
            logger.info("=====info=====")
            logger.warn("=====warn=====")
            logger.error("=====error=====")
            logger.fatal("=====fatal=====")
          })
        Text($r('app.string.RollingFileAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.rolling", this.config)
            // file path /data/app/el2/100/base/cn.openharmony.logback/haps/entry/files/rolling.log
            for (let i = 0;i < 200; i++) {
              // level is WARN,print warn、error、fatal to file rolling.log
              logger.debug("=====debug=====")
              logger.info("=====info=====")
              logger.warn("=====warn=====")
              logger.error("=====error=====")
              logger.fatal("=====fatal=====")
            }
          })
        Text($r('app.string.SocketAppender'))
          .fontSize(20)
          .margin(30)
          .onClick(() => {
            let logger: Logger = LoggerFactory.getLogger(this.context, "com.log.socket", this.config)
            setTimeout(()=>{
              // level is WARN,print warn、error、fatal to socket address ...
              logger.debug("=====debug=====")
              logger.info("=====info=====")
              logger.warn("=====warn=====")
              logger.error("=====error=====")
              logger.fatal("=====fatal=====")
            }, 1000)
          })
      }
      .width('100%')
    }
    .height('100%')
    .scrollable(ScrollDirection.Vertical)
    .scrollBarWidth(0)
    .backgroundColor(Color.White)
  }
}

