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

import socket from '@ohos.net.socket';
import { Level } from '../Level';
import { Appender } from '../config/Appender'
import wifi from '@ohos.wifiManager';
import { LoggerContext } from '../slf4j/LoggerContext';
import { AppenderEnum } from '../AppenderEnum';
import { Match } from '../config/Match';
import { FilterEnum } from '../FilterEnum';
import { Formatter } from '../formatter/Formatter';
import { makeMsg } from '../util/PatternUtils';


const TAG: string = 'SocketAppender'

export class SocketAppender {
  private remoteHost: string
  private localHost: string
  private port: number
  private tcp: socket.TCPSocket;
  private FAMILY = 1
  private OUT_TIME = 6000
  private pattern: string
  private effectiveLevelInt: number

  private tcpInit(tcp) {
    tcp.on('connect', () => {
      console.info(TAG + " on tcp connect success");
    });
    tcp.on('message', value => {
      console.log(TAG + " ==收到消息:" + JSON.stringify(value.message) + ", remoteInfo:" + JSON.stringify(value.remoteInfo))
    });
    tcp.on('close', () => {
      console.info(TAG + " on tcp close success");
    });
  }

  async bind(appender: Appender) {
    this.localHost = appender.localHost
    this.remoteHost = appender.remoteHost
    this.port = appender.port
    this.tcp = socket.constructTCPSocketInstance();

    this.tcpInit(this.tcp);

    await this.tcp.bind({ address: this.localHost, port: this.port, family: this.FAMILY }).then(() => {
      console.info(TAG + " SocketAppender bind success")
    }).catch(e => {
      console.info(TAG + " SocketAppender bind failure e:" + JSON.stringify(e))
    });
    await this.tcp.connect({
      address: { address: this.remoteHost, port: this.port, family: this.FAMILY },
      timeout: this.OUT_TIME
    }).then(() => {
      console.info(TAG + " SocketAppender connect success")
    }).catch(e => {
      console.info(TAG + " SocketAppender connect failure e:" + JSON.stringify(e))
    });
  }

  start(loggerContext: LoggerContext, effectiveLevelInt: number, appenderArray: Array<Appender>, level: Level, msg: string) {
    for (let i = 0; i < appenderArray.length; i++) {
      if (appenderArray[i].className === AppenderEnum.SOCKET && appenderArray[i].encoder) {
        this.pattern = appenderArray[i].encoder.pattern.msg
      }
      if (!!!appenderArray[i].filter) {
        // appender中没有配置filter，使用root或者logger中的level
        this.effectiveLevelInt = effectiveLevelInt
        if (this.effectiveLevelInt > level.getLevelInt()) return;
        this.sendMessage(level, makeMsg(loggerContext, this.pattern, level, msg));
      } else if (appenderArray[i].filter.className === FilterEnum.THRESHOLD) {
        this.effectiveLevelInt = appenderArray[i].filter.level.getLevelInt()
        if (this.effectiveLevelInt > level.getLevelInt()) return;
        this.sendMessage(level, makeMsg(loggerContext, this.pattern, level, msg));
      } else if (appenderArray[i].filter.className === FilterEnum.LEVEL &&
        appenderArray[i].filter.onMatch === Match.ACCEPT && appenderArray[i].filter.onMismatch === Match.DENY &&
        level.getLevelStr() === appenderArray[i].filter.level.getLevelStr()) {
        this.sendMessage(level, makeMsg(loggerContext, this.pattern, level, msg));
      }
    }
  }

  private sendMessage(level: Level, msg: string) {
    if (level && msg) {
      this.tcp.send({
        data: `${level} ${msg}`
      }).then(() => {
        console.info(TAG + 'send success');
      }).catch(err => {
        console.error(TAG + 'send fail' + JSON.stringify(err));
      })
    }
  }
}