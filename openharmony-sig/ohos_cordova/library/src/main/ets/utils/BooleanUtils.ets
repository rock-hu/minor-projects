import { isEmptyString } from './StringUtils'

/**
 * 布尔相关工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/11/20 10:43
 */

const TRUE_SET: Set<string> = new Set(['true', 'y', '是'])

/**
 * 将传入的参数字符串解析为布尔值
 *
 * @param value 待解析的字符串
 * @returns 解析后的结果
 */
export function parseBoolean(value: string): boolean {
  return !isEmptyString(value) && TRUE_SET.has(value.toLowerCase())
}

/**
 * 将一个未知类型转换为boolean类型
 *
 * @param value 未知类型对象
 * @returns 转换后的boolean值
 */
export function convertBoolean(value: string | boolean): boolean {
  if (typeof value === 'boolean') {
    return value
  } else {
    return parseBoolean(value)
  }
}