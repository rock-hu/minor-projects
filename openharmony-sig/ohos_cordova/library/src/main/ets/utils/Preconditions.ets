import { isNull } from './ObjectUtils'
import { isEmptyString } from './StringUtils'

/**
 * 条件检查工具类
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2023/11/6 13:40
 */

function emptyError(): string {
  return ''
}

/**
 * 检查参数
 * 若第一个表达式值为true表示合法，否则表示非法，将抛出异常
 *
 * @param expression 表达式结果
 * @param error 检查不通过时抛出的错误消息，非必填
 */
function checkArgument(expression: boolean, error: () => string = emptyError): void {
  if (!expression) {
    throw new Error(error())
  }
}

/**
 * 检查状态
 * 若第一个表达式值为true表示合法，否则表示非法，将抛出异常
 *
 * @param state 表达式结果
 * @param error 检查不通过时抛出的错误消息，非必填
 */
function checkState(state: boolean, error: () => string = emptyError): void {
  if (!state) {
    throw new Error(error())
  }
}

/**
 * 检查第一个形参所指参数是否非空
 *
 * @param reference 待检查非空的参数
 * @param error 检查不通过时抛出的错误消息，非必填
 * @returns 非空检查通过的参数
 */
function checkNonNull<T>(reference: T | null | undefined, error: () => string = emptyError): T {
  if (reference === null || reference === undefined) {
    throw new Error(error())
  }
  return reference
}

/**
 * 检查第一个形参所指参数是否为空
 *
 * @param reference 待检查为空的参数
 * @param error 检查不通过时抛出的错误消息，非必填
 */
function checkNullable<T>(reference: T | null | undefined, error: () => string = emptyError): void {
  if (reference !== null && reference !== undefined) {
    throw new Error(error())
  }
}

/**
 * 检查字符串是否为空
 *
 * @param str 待检查为空的字符串
 * @param error 检查不通过时抛出的错误消息，非必填
 */
function checkStrEmpty(str: string | null | undefined, error: () => string = emptyError): void {
  if (!isEmptyString(str)) {
    throw new Error(error())
  }
}

/**
 * 检查字符串是否为空
 *
 * @param str 待检查为空的字符串
 * @param error 检查不通过时抛出的错误消息，非必填
 */
function checkStrNonEmpty(str: string | null | undefined, error: () => string = emptyError): string {
  if (isEmptyString(str)) {
    throw new Error(error())
  }
  return str!
}

/**
 * 检查第一个参数对象是否为一个函数
 *
 * @param obj 待检查的对象
 * @param error 检查不通过时抛出的错误消息，非必填
 * @returns 检查通过后转换为一个function对象
 */
function checkFunction<T>(obj: T | null | undefined, error: () => string = emptyError): T {
  if (isNull(obj)) {
    throw new Error(error())
  }
  if (typeof obj !== 'function') {
    throw new Error(error())
  }
  return obj
}

export {
  checkArgument, checkState, checkNonNull, checkNullable, checkStrEmpty, checkStrNonEmpty, checkFunction
}