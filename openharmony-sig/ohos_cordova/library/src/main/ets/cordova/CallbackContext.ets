import { CordovaWebView } from './CordovaWebView'
import { MessageStatus } from './enumeration/MessageStatus'
import { PluginResult } from './PluginResult'

/**
 * Cordova Native给H5的的回调
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 10:23
 */
export class CallbackContext {
  private readonly callbackId: string
  private readonly webView: CordovaWebView
  // 是否标志该Cordova回调已完成
  private finished: boolean = false

  constructor(callbackId: string, webView: CordovaWebView) {
    this.callbackId = callbackId
    this.webView = webView
  }

  getCallbackId(): string {
    return this.callbackId
  }

  sendPluginResult(pluginResult: PluginResult): void {
    if (this.finished) {
      return
    }
    this.finished = !pluginResult.getKeepCallback()
    this.webView.sendPluginResult(pluginResult, this.callbackId)
  }

  successByJson(message: object | undefined | null): void {
    this.sendPluginResult(PluginResult.createByJson(MessageStatus.OK, message))
  }

  successByString(message: string | undefined | null): void {
    this.sendPluginResult(PluginResult.createByString(MessageStatus.OK, message))
  }

  successByArrayBuffer(message: Uint8Array): void {
    this.sendPluginResult(PluginResult.createByArrayBuffer(MessageStatus.OK, message))
  }

  successByNumber(message: number): void {
    this.sendPluginResult(PluginResult.createByNumber(MessageStatus.OK, message))
  }

  successByBoolean(message: boolean): void {
    this.sendPluginResult(PluginResult.createByBoolean(MessageStatus.OK, message))
  }

  success(): void {
    this.sendPluginResult(PluginResult.create(MessageStatus.OK))
  }

  errorByJson(message: object): void {
    this.sendPluginResult(PluginResult.createByJson(MessageStatus.ERROR, message))
  }

  errorByString(message: string): void {
    this.sendPluginResult(PluginResult.createByString(MessageStatus.ERROR, message))
  }

  errorByNumber(message: number): void {
    this.sendPluginResult(PluginResult.createByNumber(MessageStatus.ERROR, message))
  }
}