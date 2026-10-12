/**
 * 字符串工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/11/3 17:27
 */

/**
 * 判断字符串是否为null
 *
 * @param str 待判断的字符串
 * @returns true表示字符串为null，否则非null
 */
export function isNullString(str: string | null | undefined): boolean {
  return str === null || str === undefined
}

/**
 * 判断字符串是否为空，包括长度为0的字符串
 *
 * @param str 待判断的字符串
 * @returns true表示字符串为空，否则非空
 */
export function isEmptyString(str: string | null | undefined): boolean {
  return isNullString(str) || str!.length === 0
}

/**
 * 使用第二个参数包装null或undefined字符串
 *
 * @param str 待包装的字符串
 * @param defaultStr 若第一个字符串为null或undefined，则使用本参数包装，非必填
 * @returns 包装后的字符串
 */
export function wrapNullString(str: string | null | undefined, defaultStr: string = ''): string {
  return isNullString(str) ? defaultStr : str!
}

/**
 * 使用第二个参数包装空字符串
 *
 * @param str 待包装的字符串
 * @param defaultStr 若第一个字符串为空，则使用本参数包装，非必填
 * @returns 包装后的字符串
 */
export function wrapEmptyString(str: string | null | undefined, defaultStr: string = ''): string {
  return isEmptyString(str) ? defaultStr : str!
}

/**
 * 将一个未知类型对象转换为字符串
 *
 * @param obj 待转换为字符串的未知类型的对象
 * @param defaultStr 如果obj为null时的补偿字符串
 * @returns 转换为字符串的结果
 */
export function convertString<T>(obj: T | null | undefined, defaultStr: string = ''): string {
  if (obj === null || obj === undefined) {
    return defaultStr
  } else if (typeof obj === 'string') {
    return obj
  } else if (typeof obj === 'object') {
    return JSON.stringify(obj)
  } else if (typeof obj === 'boolean' || typeof obj === 'number') {
    return '' + obj
  } else {
    return defaultStr
  }
}
