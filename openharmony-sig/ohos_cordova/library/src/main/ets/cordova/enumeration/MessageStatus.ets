/**
 * Cordova交互的常见消息
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/24 16:59
 */
import HashMap from '@ohos.util.HashMap'

/**
 * Cordova Native发送给H5的消息状态枚举
 */
export enum MessageStatus {
  NO_RESULT = 0,
  OK = 1,
  CLASS_NOT_FOUND_EXCEPTION = 2,
  ILLEGAL_ACCESS_EXCEPTION = 3,
  INSTANTIATION_EXCEPTION = 4,
  MALFORMED_URL_EXCEPTION = 5,
  IO_EXCEPTION = 6,
  INVALID_ACTION = 7,
  JSON_EXCEPTION = 8,
  ERROR = 9
}

/**
 * Cordova Native发送给H5的消息状态枚举描述
 */
export enum MessageStatusDesc {
  NO_RESULT = 'No result',
  OK = 'OK',
  CLASS_NOT_FOUND_EXCEPTION = 'Class not found',
  ILLEGAL_ACCESS_EXCEPTION = 'Illegal access',
  INSTANTIATION_EXCEPTION = 'Instantiation error',
  MALFORMED_URL_EXCEPTION = 'Malformed url',
  IO_EXCEPTION = 'IO error',
  INVALID_ACTION = 'Invalid action',
  JSON_EXCEPTION = 'JSON error',
  ERROR = 'Error'
}

const STATUS_DESC_MAPPER: HashMap<MessageStatus, string> = new HashMap()
STATUS_DESC_MAPPER.set(MessageStatus.NO_RESULT, MessageStatusDesc.NO_RESULT)
STATUS_DESC_MAPPER.set(MessageStatus.OK, MessageStatusDesc.OK)
STATUS_DESC_MAPPER.set(MessageStatus.CLASS_NOT_FOUND_EXCEPTION, MessageStatusDesc.CLASS_NOT_FOUND_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.ILLEGAL_ACCESS_EXCEPTION, MessageStatusDesc.ILLEGAL_ACCESS_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.INSTANTIATION_EXCEPTION, MessageStatusDesc.INSTANTIATION_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.MALFORMED_URL_EXCEPTION, MessageStatusDesc.MALFORMED_URL_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.IO_EXCEPTION, MessageStatusDesc.IO_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.INVALID_ACTION, MessageStatusDesc.INVALID_ACTION)
STATUS_DESC_MAPPER.set(MessageStatus.JSON_EXCEPTION, MessageStatusDesc.JSON_EXCEPTION)
STATUS_DESC_MAPPER.set(MessageStatus.ERROR, MessageStatusDesc.ERROR)

/**
 * 将Cordova消息状态枚举值转换为文字描述
 *
 * @param status 状态枚举
 * @returns 状态描述
 */
export function convertMessageStatusDesc(status: MessageStatus): string {
  return STATUS_DESC_MAPPER.hasKey(status) ? STATUS_DESC_MAPPER.get(status) : MessageStatusDesc.ERROR
}
