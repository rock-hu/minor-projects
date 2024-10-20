/**
 * 对象工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/11/6 16:51
 */

/**
 * 判断对象是否为null
 *
 * @param obj 待判断的对象
 * @returns true表示字符串为null，否则非null
 */
export function isNull<T>(obj: T | undefined | null): boolean {
  return obj === null || obj === undefined
}

/**
 * 将一个可能为空的对象包装为非空对象
 *
 * @param obj 待包装的可空对象
 * @param defaultObj 为空时的补偿对象
 * @returns 非空对象
 */
export function wrapNullObj<T>(obj: T | null | undefined, defaultObj: T): T {
  return obj === null || obj === undefined ? defaultObj : obj
}