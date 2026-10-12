/**
 * 数字相关工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/11/16 14:02
 */


/**
 * 将一个未知对象转换为一个整型数
 *
 * @param num 待转换的未知对象
 * @param defaultNum 转换失败时的默认值
 * @returns 转换好的整型数
 */
export function parseInt<T>(num: T, defaultNum: number): number {
  if (typeof num === 'string') {
    return Number.parseInt(num)
  } else if (typeof num === 'number') {
    return num
  } else {
    return defaultNum
  }
}

/**
 * 将一个未知对象转换为一个浮点数
 *
 * @param num 待转换的未知对象
 * @param defaultNum 转换失败时的默认值
 * @returns 转换好的浮点数
 */
export function parseFloat<T>(num: T, defaultNum: number): number {
  if (typeof num === 'string') {
    return Number.parseFloat(num)
  } else if (typeof num === 'number') {
    return num
  } else {
    return defaultNum
  }
}


/**
 * 获得在范围内的有效数字，如果传入数字小于最小值，则返回最小值；如果传入数字大于最大值，则返回最大值；在范围内则返回传入数字
 *
 * @param num 带转换的数字
 * @param min 范围内最小值
 * @param max 范围内最大值
 * @returns
 */
export function parseInMinMax(num: number, min: number, max: number): number {
  if (num <= min) {
    return min
  } else if (num >= max) {
    return max
  } else {
    return num
  }
}