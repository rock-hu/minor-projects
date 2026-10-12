import { StringUtils } from '../utils/Index'

/**
 * 字符串拼接工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/12/25 15:33
 */
export class StringBuilder {
  private readonly value: string[] = []

  constructor(str?: string) {
    if (!StringUtils.isNullString(str)) {
      this.value.push(str!)
    }
  }

  append(object: string | number | boolean): StringBuilder {
    this.value.push(StringUtils.convertString(object))
    return this
  }

  toString(): string {
    return this.value.join('')
  }
}