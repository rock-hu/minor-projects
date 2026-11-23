/**
 * Cordova交互的数据类型
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/24 16:57
 */
export enum MessageType {
  /**
   * 字符串
   */
  STRING = 1,
  /**
   * JSON对象，包括JSON对象和JSON数组
   */
  JSON = 2,
  /**
   * 数值类型，包括整形和浮点
   */
  NUMBER = 3,
  /**
   * boolean
   */
  BOOLEAN = 4,
  /**
   * 空值
   */
  NULL = 5,
  /**
   * 字节数组
   */
  ARRAYBUFFER = 6,
  /**
   * 二进制字符串
   */
  BINARYSTRING = 7,
  /**
   * 多媒体类型
   */
  MULTIPART = 8
}