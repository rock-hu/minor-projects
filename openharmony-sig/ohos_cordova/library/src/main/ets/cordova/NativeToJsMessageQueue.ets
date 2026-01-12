import { ObjectUtils, Preconditions, StringBuilder, StringUtils } from '../utils/Index'
import { PluginResult } from './PluginResult'
import { MessageType } from './enumeration/MessageType'
import { MessageStatus } from './enumeration/MessageStatus'
import { CordovaWebViewEngine } from './CordovaWebViewEngine'
import { CordovaInterface } from './CordovaInterface'
import LinkedList from '@ohos.util.LinkedList'

/**
 * 原生通知H5的消息队列
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 15:21
 */

class JsMessage {
  readonly jsPayloadOrCallbackId: string
  readonly pluginResult?: PluginResult

  constructor(callbackId: string, pluginResult?: PluginResult) {
    this.jsPayloadOrCallbackId = Preconditions.checkStrNonEmpty(callbackId)
    this.pluginResult = pluginResult
  }

  static calculateEncodedLengthHelper(pluginResult: PluginResult): number {
    switch (pluginResult.getMessageType()) {
      case MessageType.STRING:
        return 1 + pluginResult.getMessage().length
      case MessageType.JSON:
      default:
        return pluginResult.getMessage().length
      case MessageType.NUMBER:
      case MessageType.ARRAYBUFFER:
      case MessageType.BINARYSTRING:
        return 1 + pluginResult.getMessage().length
      case MessageType.BOOLEAN:
      case MessageType.NULL:
        return 1
      case MessageType.MULTIPART:
        let ret = 1
        for (let index = 0; index < pluginResult.getMultipartMessagesSize(); index++) {
          const length = JsMessage.calculateEncodedLengthHelper(pluginResult.getMultipartMessage(index))
          const argLength = `${length}`.length
          ret += argLength + 1 + length
        }
        return ret
    }
  }

  calculateEncodedLength(): number {
    if (ObjectUtils.isNull(this.pluginResult)) {
      return this.jsPayloadOrCallbackId.length + 1
    } else {
      const statusLen = `${this.pluginResult!.getStatus()}`.length
      const ret = 2 + statusLen + 1 + this.jsPayloadOrCallbackId.length + 1
      return ret + JsMessage.calculateEncodedLengthHelper(this.pluginResult!)
    }
  }

  static encodeAsMessageHelper(sb: StringBuilder, pluginResult: PluginResult): void {
    switch (pluginResult.getMessageType()) {
      case MessageType.STRING:
        sb.append('s').append(pluginResult.getMessage())
        break;
      case MessageType.JSON:
      default:
        sb.append(pluginResult.getMessage())
        break;
      case MessageType.NUMBER:
        sb.append('n').append(pluginResult.getMessage())
        break
      case MessageType.BOOLEAN:
        sb.append(pluginResult.getMessage().charAt(0))
        break
      case MessageType.NULL:
        sb.append('N')
        break
      case MessageType.ARRAYBUFFER:
        sb.append('A').append(pluginResult.getMessage())
        break
      case MessageType.BINARYSTRING:
        sb.append('S').append(pluginResult.getMessage())
        break
      case MessageType.MULTIPART:
        sb.append('M')
        for (let index = 0; index < pluginResult.getMultipartMessagesSize(); index++) {
          const multipartMessage = pluginResult.getMultipartMessage(index)
          sb.append(`${JsMessage.calculateEncodedLengthHelper(multipartMessage)}}`).append(' ')
          JsMessage.encodeAsMessageHelper(sb, multipartMessage)
        }
        break
    }
  }

  encodeAsMessage(sb: StringBuilder): void {
    if (ObjectUtils.isNull(this.pluginResult)) {
      sb.append('J').append(this.jsPayloadOrCallbackId)
    } else {
      const status: MessageStatus = this.pluginResult!.getStatus()
      const noResult = status === MessageStatus.NO_RESULT
      const resultOk = status === MessageStatus.OK
      const keepCallback = this.pluginResult!.getKeepCallback()
      sb.append(!noResult && !resultOk ? 'F' : 'S')
        .append(keepCallback ? '1' : '0')
        .append(status)
        .append(' ')
        .append(this.jsPayloadOrCallbackId)
        .append(' ')
      JsMessage.encodeAsMessageHelper(sb, this.pluginResult!)
    }
  }

  encodeAsJsMessage(sb: StringBuilder): void {
    if (ObjectUtils.isNull(this.pluginResult)) {
      sb.append(this.jsPayloadOrCallbackId)
    } else {
      const status = this.pluginResult!.getStatus()
      const success = status === MessageStatus.OK || status === MessageStatus.NO_RESULT
      sb.append("cordova.callbackFromNative('")
        .append(this.jsPayloadOrCallbackId)
        .append("',")
        .append(success)
        .append(",")
        .append(status)
        .append(",[")
      if (this.pluginResult!.getMessageType() === MessageType.ARRAYBUFFER) {
        sb.append("cordova.require('cordova/base64').toArrayBuffer('")
          .append(this.pluginResult!.getMessage())
          .append("')")
      } else if (this.pluginResult!.getMessageType() === MessageType.BINARYSTRING) {
        sb.append("atob('")
          .append(this.pluginResult!.getMessage())
          .append("')")
      } else {
        sb.append(this.pluginResult!.getMessage())
      }
      sb.append("],")
        .append(this.pluginResult!.getKeepCallback())
        .append(");")
    }
  }
}

abstract class BridgeMode {
  abstract onNativeToJsMessageAvailable(queue: NativeToJsMessageQueue): void

  notifyOfFlush(queue: NativeToJsMessageQueue, fromOnlineEvent: boolean) {
  }

  reset() {
  }
}

export class EvalBridgeMode extends BridgeMode {
  private readonly engine: CordovaWebViewEngine
  private readonly cordova: CordovaInterface

  constructor(engine: CordovaWebViewEngine, cordova: CordovaInterface) {
    super()
    this.engine = engine
    this.cordova = cordova
  }

  onNativeToJsMessageAvailable(queue: NativeToJsMessageQueue): void {
    const js = queue.popAndEncodeAsJs()
    if (!StringUtils.isEmptyString(js)) {
      this.engine.evaluateJavascript(js!)
    }
  }
}

export class NoOpBridgeMode extends BridgeMode {
  onNativeToJsMessageAvailable(queue: NativeToJsMessageQueue): void {
    // do nothing
  }
}

// 最大的脚本字符串长度
const MAX_PAYLOAD_SIZE: number = 524288000

export class NativeToJsMessageQueue {
  private paused: boolean = false
  private readonly queue: JsMessage[] = []
  private readonly bridgeModes: LinkedList<BridgeMode> = new LinkedList()
  private activeBridgeMode?: BridgeMode | null

  addBridgeMode(bridgeMode: BridgeMode) {
    this.bridgeModes.add(bridgeMode)
  }

  isBridgeEnabled(): boolean {
    return !ObjectUtils.isNull(this.activeBridgeMode)
  }

  isEmpty(): boolean {
    return this.queue.length === 0
  }

  setBridgeMode(value: number) {
    if (value >= -1 && value < this.bridgeModes.length) {
      const newMode: BridgeMode | null | undefined = value < 0 ? null : this.bridgeModes.get(value)
      if (newMode !== this.activeBridgeMode) {
        this.activeBridgeMode = newMode
        if (!ObjectUtils.isNull(newMode)) {
          newMode!.reset()
          if (!this.paused && this.queue.length > 0) {
            newMode!.onNativeToJsMessageAvailable(this)
          }
        }
      }
    } else {
      // todo log
    }
  }

  reset(): void {
    this.queue.length = 0
    this.setBridgeMode(-1)
  }

  private calculatePackedMessageLength(message: JsMessage): number {
    const messageLen = message.calculateEncodedLength()
    return `${messageLen}}`.length + messageLen + 1
  }

  private packMessage(message: JsMessage, sb: StringBuilder): void {
    const len = message.calculateEncodedLength()
    sb.append(len).append(' ')
    message.encodeAsMessage(sb)
  }

  popAndEncode(fromOnlineEvent: boolean): string | null {
    if (ObjectUtils.isNull(this.activeBridgeMode)) {
      return null
    } else {
      this.activeBridgeMode!.notifyOfFlush(this, fromOnlineEvent)
      if (this.queue.length === 0) {
        return null
      } else {
        let totalPayloadLen: number = 0
        let numMessagesToSend: number = 0

        for (; numMessagesToSend < this.queue.length; numMessagesToSend++) {
          const messageSize = this.calculatePackedMessageLength(this.queue[numMessagesToSend])
          if (numMessagesToSend > 0 && totalPayloadLen + messageSize > MAX_PAYLOAD_SIZE && MAX_PAYLOAD_SIZE > 0) {
            break
          }
          totalPayloadLen += messageSize
        }

        const sb: StringBuilder = new StringBuilder()
        for (let index = 0; index < numMessagesToSend; index++) {
          this.packMessage(this.queue.shift()!, sb)
        }

        if (this.queue.length > 0) {
          sb.append('*')
        }
        return sb.toString()
      }
    }

  }

  popAndEncodeAsJs(): string | null {
    if (this.queue.length === 0) {
      return null
    }

    let totalPayloadLen: number = 0
    let numMessagesToSend: number = 0

    for (; numMessagesToSend < this.queue.length; numMessagesToSend++) {
      const messageSize = this.queue[numMessagesToSend].calculateEncodedLength() + 50
      if (numMessagesToSend > 0 && totalPayloadLen + messageSize > MAX_PAYLOAD_SIZE && MAX_PAYLOAD_SIZE > 0) {
        break
      }
      totalPayloadLen += messageSize
    }

    const willSendAllMessages = numMessagesToSend === this.queue.length
    const sb: StringBuilder = new StringBuilder()
    for (let index = 0; index < numMessagesToSend; index++) {
      if (willSendAllMessages && index + 1 === numMessagesToSend) {
        this.queue.shift()!.encodeAsJsMessage(sb)
      } else {
        sb.append("try{")
        this.queue.shift()!.encodeAsJsMessage(sb)
        sb.append("}finally{")
      }
    }

    if (!willSendAllMessages) {
      sb.append("window.setTimeout(function(){cordova.require('cordova/plugin/android/polling').pollOnce();},0);");
    }

    for (let i = willSendAllMessages ? 1 : 0; i < numMessagesToSend; ++i) {
      sb.append('}');
    }

    return sb.toString()
  }

  addJavaScript(statement: string): void {
    this.enqueueMessage(new JsMessage(statement));
  }

  addPluginResult(result: PluginResult, callbackId: string) {
    if (StringUtils.isEmptyString(callbackId)) {
      // todo log
    } else {
      if (result.getStatus() !== MessageStatus.NO_RESULT || !result.getKeepCallback()) {
        this.enqueueMessage(new JsMessage(callbackId, result))
      }
    }
  }

  private enqueueMessage(message: JsMessage): void {
    if (ObjectUtils.isNull(this.activeBridgeMode)) {
      // todo log
    } else {
      this.queue.push(message)
      if (!this.paused) {
        this.activeBridgeMode!.onNativeToJsMessageAvailable(this)
      }
    }
  }

  setPaused(value: boolean): void {
    if (this.queue && value) {
      // todo log
    }
    this.paused = value
    if (!value) {
      if (this.queue.length > 0 && !ObjectUtils.isNull(this.activeBridgeMode)) {
        this.activeBridgeMode!.onNativeToJsMessageAvailable(this)
      }
    }
  }
}