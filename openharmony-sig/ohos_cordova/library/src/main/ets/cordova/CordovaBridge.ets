import cryptoFramework from '@ohos.security.cryptoFramework'
import { NativeToJsMessageQueue } from './NativeToJsMessageQueue'
import { PluginManager } from './PluginManager'
import { NumberUtils, StringUtils } from '../utils/Index'

/**
 * Cordova原生和H5的通信桥
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/26 09:09
 */
export class CordovaBridge {
  private readonly pluginManager: PluginManager
  private readonly jsMessageQueue: NativeToJsMessageQueue
  private expectedBridgeSecret: number = -1

  constructor(pluginManager: PluginManager, jsMessageQueue: NativeToJsMessageQueue) {
    this.pluginManager = pluginManager
    this.jsMessageQueue = jsMessageQueue
  }

  jsExec(bridgeSecret: number, service: string, action: string, callbackId: string, arg: string): string | null {
    if (!this.verifySecret('exec()', bridgeSecret)) {
      return null
    } else if (StringUtils.isNullString(arg)) {
      return '@Null arguments.'
    } else {
      this.jsMessageQueue.setPaused(true)
      try {
        this.pluginManager.exec(service, action, callbackId, arg)
        return this.jsMessageQueue.popAndEncode(false)
      } catch (e) {
        return ''
      } finally {
        this.jsMessageQueue.setPaused(false)
      }
    }
  }

  jsSetNativeToJsBridgeMode(bridgeSecret: number, value: number): void {
    if (this.verifySecret('setNativeToJsBridgeMode()', bridgeSecret)) {
      this.jsMessageQueue.setBridgeMode(value)
    }
  }

  jsRetrieveJsMessages(bridgeSecret: number, fromOnlineEvent: boolean): string | null {
    return!this.verifySecret('retrieveJsMessages()', bridgeSecret) ? null : this.jsMessageQueue.popAndEncode(fromOnlineEvent)
  }

  private verifySecret(action: string, bridgeSecret: number): boolean {
    if (!this.jsMessageQueue.isBridgeEnabled()) {
      if (bridgeSecret === -1) {
        // todo log
      } else {
        // todo log
      }
      return false
    } else if (this.expectedBridgeSecret >= 0 && bridgeSecret === this.expectedBridgeSecret) {
      return true
    } else {
      // todo log
      this.clearBridgeSecret()
      throw new Error()
    }
  }

  clearBridgeSecret() {
    this.expectedBridgeSecret = -1
  }

  isSecretEstablished(): boolean {
    return this.expectedBridgeSecret !== -1
  }

  generateBridgeSecret(): number {
    const intArray = new Uint32Array(cryptoFramework.createRandom().generateRandomSync(4).data)
    this.expectedBridgeSecret = Math.floor(Math.abs(intArray[0]))
    return this.expectedBridgeSecret
  }

  reset(): void {
    this.jsMessageQueue.reset()
    this.clearBridgeSecret()
  }

  promptOnJsPrompt(origin: string, message: string, defaultValue: string): string | null {
    if (StringUtils.isEmptyString(defaultValue)) {
      return null
    } else if (defaultValue.startsWith("gap:")) {
      try {
        const array: string[] = JSON.parse(defaultValue.substring(4))
        const secret = NumberUtils.parseInt(array[0] as string, 0)
        const service = array[1]
        const action = array[2]
        const callbackId = array[3]
        return this.jsExec(secret, service, action, callbackId, message)
      } catch (e) {
        return ''
      }
    } else if (defaultValue.startsWith('gap_bridge_mode:')) {
      try {
        const bridgeMode = Number.parseInt(defaultValue.substring(16))
        this.jsSetNativeToJsBridgeMode(bridgeMode, Number.parseInt(message))
      } catch (e) {
        // nothing to do
      }
      return ''
    } else if (defaultValue.startsWith('gap_poll:')) {
      try {
        const bridgeMode = Number.parseInt(defaultValue.substring(9))
        return StringUtils.wrapNullString(this.jsRetrieveJsMessages(bridgeMode, '1' === message))
      } catch (e) {
        return ''
      }
    } else if (defaultValue.startsWith('gap_init:')) {
      if (this.pluginManager.shouldAllowBridgeAccess(origin)) {
        const bridgeMode = Number.parseInt(defaultValue.substring(9))
        this.jsMessageQueue.setBridgeMode(bridgeMode)
        return this.generateBridgeSecret() + ''
      } else {
        // todo log
        return ''
      }
    } else {
      return null
    }
  }
}