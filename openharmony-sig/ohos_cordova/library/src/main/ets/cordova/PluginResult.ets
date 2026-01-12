import util from '@ohos.util'
import { MessageStatus, convertMessageStatusDesc } from './enumeration/MessageStatus'
import { MessageType } from './enumeration/MessageType'
import { ObjectUtils, StringUtils } from '../utils/Index'

/**
 * Cordova回调给H5的结果
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/24 16:56
 */
export class PluginResult {
  private readonly status: MessageStatus
  private readonly messageType: MessageType
  private keepCallback: boolean = false
  private readonly message: string = ''
  private readonly multipartMessages: PluginResult[] = []

  constructor(status: MessageStatus, type: MessageType, message: string) {
    this.status = status
    this.messageType = type
    this.message = message
  }

  public static create(status: MessageStatus): PluginResult {
    return PluginResult.createByString(status, convertMessageStatusDesc(status))
  }

  public static createByString(status: MessageStatus, message?: string | null): PluginResult {
    return new PluginResult(status, StringUtils.isEmptyString(message) ? MessageType.NULL : MessageType.STRING, JSON.stringify(StringUtils.wrapNullString(message)))
  }

  public static createByJson(status: MessageStatus, message: object | undefined | null): PluginResult {
    return new PluginResult(status, ObjectUtils.isNull(message) ? MessageType.NULL : MessageType.JSON, JSON.stringify(message ?? null))
  }

  public static createByNumber(status: MessageStatus, message: number): PluginResult {
    return new PluginResult(status, MessageType.NUMBER, '' + message)
  }

  public static createByBoolean(status: MessageStatus, message: boolean): PluginResult {
    return new PluginResult(status, MessageType.BOOLEAN, '' + message)
  }

  public static createByArrayBuffer(status: MessageStatus, message: Uint8Array): PluginResult {
    return new PluginResult(status, MessageType.ARRAYBUFFER, new util.Base64Helper().encodeToStringSync(message, util.Type.BASIC))
  }

  public static createByBinaryString(status: MessageStatus, message: Uint8Array): PluginResult {
    return new PluginResult(status, MessageType.BINARYSTRING, new util.Base64Helper().encodeToStringSync(message, util.Type.BASIC))
  }

  public static createByMessageList(status: MessageStatus, message: PluginResult[]): PluginResult {
    const result = new PluginResult(status, MessageType.MULTIPART, '')
    result.multipartMessages.push(...message)
    return result
  }

  getKeepCallback(): boolean {
    return this.keepCallback
  }

  setKeepCallback(keepCallback: boolean): void {
    this.keepCallback = keepCallback
  }

  getStatus(): MessageStatus {
    return this.status
  }

  getMessageType(): MessageType {
    return this.messageType
  }

  getMessage(): string {
    return this.message
  }

  getMultipartMessagesSize(): number {
    return this.multipartMessages.length
  }

  getMultipartMessage(index: number): PluginResult {
    return this.multipartMessages[index]
  }
}