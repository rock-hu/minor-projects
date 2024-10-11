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
import util from '@ohos.util';
import { Level } from '../Level';
import connection from '@ohos.net.connection'
import { Appender } from '../config/Appender'


const TAG: string = "SmtpUtils "
const LINEFEED = "\r\n"


/**
 * 发送邮件
 */
export class SmtpUtils {
  private socket
  private localHost: string
  private fromMail: string
  private toMail: string
  private subject: string
  private smtpHost: string
  private smtpPort: number
  private username: string
  private password: string
  private sendMessageEvent: (err?: string) => void
  private authenticationEvent: (err?: string) => void
  private closeEvent: (err?: string) => void
  private expectCode = "220"
  private serverIsReady = false
  private isConnect = false
  private OUT_TIME: number = 6000
  private timerId = -1
  // 响应码
  private responseCode: number = -1
  private mailMsg = ''
  private level: Level
  private msg: string
  private tcp;

  /**
   * 设置连接服务器超时时长
   * @param timeout
   */
  public setTimeOut(timeout) {
    this.OUT_TIME = timeout
  }

  constructor(appender: Appender, level: Level, msg: string) {
    this.localHost = appender.localHost
    this.fromMail = appender.fromMail
    this.toMail = appender.toMail
    this.subject = appender.subject
    this.smtpHost = appender.smtpHost
    this.smtpPort = appender.smtpPort
    this.username = new util.Base64Helper().encodeToStringSync(new util.TextEncoder().encodeInto(appender.username))
    this.password = new util.Base64Helper().encodeToStringSync(new util.TextEncoder().encodeInto(appender.password))
    console.info('test SmtpUtils localHost:'+this.localHost+',fromMail:'+this.fromMail)
    console.info('test SmtpUtils toMail:'+this.toMail+',subject:'+this.subject)
    console.info('test SmtpUtils smtpHost:'+this.smtpHost+',smtpPort:'+this.smtpPort)
    console.info('test SmtpUtils username:'+this.username+',password:'+this.password)
    this.level = level
    this.msg = msg
  }

  public sendMailOpr(){
    this.tcp = socket.constructTCPSocketInstance();
    this.socket = this.tcp;
    this.tcpInit(this.tcp)
    let promise = this.tcp.bind({ address: this.localHost, port: 0, family: 1 });
    promise.then(() => {
      console.info(TAG + 'SMTPAppender bind success');

      connection.getAddressesByName(this.smtpHost).then((netAddress) => {
        let ip
        for (let i = 0; i < netAddress.length; i++) {
          if (this.isIP(netAddress[i].address)) {
            ip = netAddress[i].address
            console.info(TAG + 'connect -- address = ' + ip)
            break
          }
        }

        if (!!!ip) {
          console.error(TAG + 'not available ip')
          return;
        }

        this.tcp.connect({
          address: { address: ip, port: this.smtpPort },
          timeout: this.OUT_TIME
        }).then(() => {
          console.info(TAG + 'connect tcp server success')
          this.tcp.setExtraOptions({
            keepAlive: true,
            OOBInline: true,
            TCPNoDelay: true,
            receiveBufferSize: 1024 * 1024 * 20,
            reuseAddress: true,
            socketTimeout: this.OUT_TIME,
          }, err => {
            if (err) {
              console.info(TAG + 'setExtraOptions fail')
              return;
            }
            console.info(TAG + 'setExtraOptions success')
          });
        }).catch(err => {
          console.error(TAG + 'connect server fail ' + JSON.stringify(err));
        });
      }).catch(err => {
        console.error(TAG + 'getAddressesByName fail ' + JSON.stringify(err));
      });
    }).catch(err => {
      this.tcp.close()
      console.error('bind tcp fail ' + JSON.stringify(err));
      console.error(TAG + 'bind tcp fail ' + JSON.stringify(err));
    });
  }

  private tcpInit(tcp) {
    let TAG = "test "
    tcp.on('connect', () => {
      console.info(TAG + " on tcp connect success");
    });
    tcp.on('message', value => {
      let decodeNum = new Uint8Array(value.message)

      let asciiToStr = String.fromCharCode(...decodeNum);
      console.info(TAG + 'message response:'+asciiToStr)
      let resArr = asciiToStr?.split(" ")
      this.responseCode = resArr[0]?Number.parseInt(resArr[0]):0
      console.info(TAG + 'message responseCode:'+this.responseCode)
      // console.log(TAG + " ==收到消息:" + JSON.stringify(value.message) + ", remoteInfo:" + JSON.stringify(value.remoteInfo))

      console.info(TAG + 'message expectCode:'+this.expectCode)
      if (!this.serverIsReady && asciiToStr.startsWith(this.expectCode)) {
        console.info(TAG + 'smtp login')
        this.serverIsReady = true
        // //授权登陆
        this.login();
        return
      } else if (asciiToStr.startsWith(this.expectCode)) {
        console.info(TAG + 'smtp other')
        if (this.timerId != -1) {
          console.info(TAG+" OK clear request timeout")
          clearTimeout(this.timerId)
        }
        if (this.expectCode.startsWith("221")) {
          console.info(TAG + 'smtp 221')
          this.socket.off('message');
          this.closeEvent()
          this.serverIsReady = false
          this.isConnect = false
          if (!!this.socket) {
            console.info(TAG + 'smtp close')
            this.socket.close(err => {
              if (err) {
                console.error(TAG+' socket close fail ' + JSON.stringify(err));
                return;
              }
              console.info(TAG+' socket close success');
            })
          }
        } else {
          this.authenticationEvent()
          if (this.expectCode.startsWith("235")) {
            // 鉴权完成
            this.isConnect = true
            console.info(TAG+" auth success --")
            this.sendMail();
          }
        }

      } else {
        console.info(TAG + 'smtp opr 1')
        if (this.timerId != -1) {
          console.info(TAG+" ERR clear request timeout")
          clearTimeout(this.timerId)
        }
        this.authenticationEvent(asciiToStr)
        if (this.isConnect) {
          this.sendMessageEvent(asciiToStr)
        }
        this.serverIsReady = false
        this.isConnect = false
      }
      asciiToStr = ""
    });
    tcp.on('close', () => {
      console.info(TAG + " on tcp close success");
    });
  }

  private intToIP(num) {
    let str;
    let ipArr = new Array();
    ipArr[0] = (num >>> 24) >>> 0;
    ipArr[1] = ((num << 8) >>> 24) >>> 0;
    ipArr[2] = (num << 16) >>> 24;
    ipArr[3] = (num << 24) >>> 24;
    str = String(ipArr[0]) + "." + String(ipArr[1]) + "." + String(ipArr[2]) + "." + String(ipArr[3]);
    return str;
  }

  private isIP(str: string): boolean {
    if (str.length < 7 || str.length > 15) {
      return false;
    }

    let arr: string[] = str.split(".");
    if (arr.length != 4) {
      return false;
    }

    for (let i = 0; i < 4; i++) {
      let s = arr[i];
      for (let j = 0; j < s.length; j++) {
        if (s.charAt(j) < '0' || s.charAt(j) > '9') {
          return false;
        }
      }
    }

    for (let i = 0; i < 4; i++) {
      let temp = Number.parseInt(arr[i]);
      if (temp < 0 || temp > 255) {
        return false;
      }
    }
    return true;
  }

  /**
   * 登录服务器(鉴权)
   */
  private async login() {
    await new Promise((resolve, reject) => {
      console.info(TAG + '-- ehlo do');
      this.sendEHLO((err) => {
        if (!err) {
          resolve("")
          console.info(TAG + '-- ehlo success');
          return
        }
        console.info(TAG + '-- ehlo err:' + err);
        reject(err)
      }, "250")
    }).then((result) => {
      return new Promise((resolve, reject) => {
        console.info(TAG + '-- auth do');
        this.sendAuthLogin((err) => {
          if (!err) {
            resolve("")
            console.info(TAG + '-- auth success');
            return
          }
          console.error(TAG + '-- auth err:' + err);
          reject(err)
        }, "334")
      })
    }).then((result) => {
      return new Promise((resolve, reject) => {
        console.info(TAG + '-- user do');
        this.sendUser((err) => {
          if (!err) {
            resolve("")
            console.info(TAG + '-- user success');
            return
          }
          console.info(TAG + '-- user err:' + err);
          reject(err)
        }, "334")

      })
    }).then((result) => {
      return new Promise((resolve, reject) => {
        console.info(TAG + '-- password do');
        this.sendPassword((err) => {
          if (!err) {
            resolve("")
            console.info(TAG + '-- password success');
            return
          }
          console.info(TAG + '-- password err:' + err);
          reject(err)
        }, "235")
      })
    })
  }

  /**
   * 向服务器打招呼
   * @param event
   */
  private sendEHLO(event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let cmd = {
      data: "EHLO " + this.smtpHost + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("ehlo response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send EHLO success ');
    }).catch(err => {
      console.error(TAG + '-- send EHLO fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   *
   * 发送登陆鉴权请求
   */
  private sendAuthLogin(event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let cmd = {
      data: "AUTH LOGIN" + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("AUTH LOGIN response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send AUTH LOGIN success');
    }).catch(err => {
      console.error(TAG + '-- send  AUTH LOGIN fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   * 发送用户名
   */
  private sendUser(event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let user = this.username  + LINEFEED
    let cmd = {
      data: user,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("user response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send user success');
    }).catch(err => {
      console.error(TAG + '-- send  fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   * 发送授权码
   */
  private sendPassword(event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let pass = this.password + LINEFEED
    let cmd = {
      data: pass,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("pass response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send pass success');
    }).catch(err => {
      console.error(TAG + '-- send  fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   * 发送邮件
   * @param level
   * @param msg
   */
  public sendMail() {
    this.mailMsg += (this.level.getLevelStr() + ' ' + this.msg + '\n')

    // 发件人，要和前面的一致
    let data: string = "From:" + this.fromMail + "\r\n"
      // 收件人，要和前面的一致
      + "To:" + this.toMail + "\r\n"
      + "Subject:" + this.subject + "\r\n\r\n"
      // 邮件正文内容
      + "\r\n" + this.mailMsg
      // 结束标志
      + "\r\n.\r\n"

    this.sendMessage(this.fromMail, this.toMail, data, (err) => {
      console.info(TAG + 'sendMessage err:'+JSON.stringify(err))
      if(!!!err) {
        console.info(TAG + 'smtp close do')
      }

      this.close((err) => {
        console.info(TAG + 'smtp closed')
      });
    })
  }

  /**
   * 发送送邮件
   * @param mimeMessage
   * @param callback
   */
  private  async sendMessage(from: string, to: string, data: string, event: (err) => void) {
    this.sendMessageEvent = event
    await new Promise((resolve, reject) => {
      this.sendFrom(from, (err) => {
        if (!err) {
          resolve("")
          return
        }
        console.info(TAG + '-- from err:' + err);
        event(err)
        reject(err)
      }, "250")
    }).then((result) => {
      return new Promise((resolve, reject) => {
        this.sendRCPT(to, (err) => {
          if (!err) {
            resolve("")
            return
          }
          console.info(TAG + '-- rcpts err:' + err);
          event(err)
          reject(err)
        }, "250")
      })
    }).then((result) => {
      return new Promise((resolve, reject) => {
        this.sendDATA((err) => {
          if (!err) {
            resolve("")
            return
          }
          console.info(TAG + '--  data err:' + err);
          event(err)
          reject(err)
        }, "354")
      })
    }).then((result) => {
      return new Promise((resolve, reject) => {
        this.sendMsgData(data, (err) => {
          if (!err) {
            this.sendMessageEvent()
            resolve("")
            return
          }
          console.info(TAG + '--  msg data  err:' + err);
          event(err)
          reject(err)
        }, "250")
      })
    })
  }

  private sendFrom(from: string, event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let cmd = {
      data: "MAIL FROM: <" + from + ">" + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("MAIL FROM response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send MAIL FROM success');
    }).catch(err => {
      console.error(TAG + '-- send  fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  private sendRCPT(address, event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let cmd = {
      data: "RCPT TO:<" + address + ">" + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("RCPT response timeout : " + address)
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send RCPT success');
    }).catch(err => {
      console.error(TAG + '-- send RCPT fail: ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  private sendDATA(dataEvent: (err?) => void, expectCode) {
    this.authenticationEvent = dataEvent
    let cmd = {
      data: "DATA" + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("DATA response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send DATA success');
    }).catch(err => {
      console.error(TAG + '-- send DATA fail: ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   * 发送邮件内容
   */
  private sendMsgData(message: string, event: (err?) => void, expectCode) {
    this.authenticationEvent = event
    let cmd = {
      data: message,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.authenticationEvent("msg body response timeout")
      }, this.OUT_TIME)
      this.expectCode = expectCode
      console.info(TAG + '-- send msg body success');
    }).catch(err => {
      console.error(TAG + '-- send msg body fail: ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }

  /**
   * 退出当前登录
   */
  public close(event: (err) => void) {
    this.closeEvent = event
    let cmd = {
      data: "QUIT" + LINEFEED,
      encoding: "UTF-8"
    }

    this.socket.send(cmd).then(() => {
      if (this.timerId != -1) {
        clearTimeout(this.timerId)
      }
      this.timerId = setTimeout(() => {
        this.closeEvent("quit response timeout")
        this.socket.close()
      }, this.OUT_TIME)
      this.expectCode = "221"
      console.info(TAG + '-- send quit success');
    }).catch(err => {
      console.error(TAG + '-- send quit fail ' + JSON.stringify(err));
      this.authenticationEvent(JSON.stringify(err))
    })
  }
}

