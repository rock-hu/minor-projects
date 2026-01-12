import { CordovaBridge } from '../CordovaBridge'
import { ExposedJsApi } from '../ExposedJsApi'

/**
 * 使用系统标准H5 Bridge API实现的一套Cordova协议操作桥接口
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 10:08
 */
export class SystemExposedJsApi implements ExposedJsApi {
  private readonly bridge: CordovaBridge

  constructor(bridge: CordovaBridge) {
    this.bridge = bridge
  }

  exec = (bridgeSecret: number, service: string, action: string, callbackId: string, arg: string): string | null => {
    return this.bridge.jsExec(bridgeSecret, service, action, callbackId, arg)
  }
  setNativeToJsBridgeMode = (bridgeSecret: number, value: number): void => {
    this.bridge.jsSetNativeToJsBridgeMode(bridgeSecret, value)
  }
  retrieveJsMessages = (bridgeSecret: number, fromOnlineEvent: boolean): string | null => {
    return this.bridge.jsRetrieveJsMessages(bridgeSecret, fromOnlineEvent)
  }
}