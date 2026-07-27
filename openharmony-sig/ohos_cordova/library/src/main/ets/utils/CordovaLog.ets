import hilog from '@ohos.hilog'
import { CordovaContext } from '../CordovaContext'

/**
 * 日志打印工具
 *
 * @author A Shuai
 * @email lis7489@bank-of-china.com
 * @date 2024/5/19 14:24
 */


// 默认Domain
const DEFAULT_DOMAIN: number = 0x7489

/**
 * 省略版打印Debug日志（省略domain参数）
 *
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function d(tag: string, format: string, ...args: ESObject[]): void {
  debug(DEFAULT_DOMAIN, tag, format, args)
}

/**
 * 完整版打印Debug日志
 *
 * @param domain 指示服务域，使用16禁止数字，取值范围为 0x0000 到 0xFFFF
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function debug(domain: number, tag: string, format: string, ...args: ESObject[]): void {
  if (CordovaContext.isDebugMode() && hilog.isLoggable(domain, tag, hilog.LogLevel.DEBUG)) {
    hilog.debug(domain, tag, format, args)
  }
}

/**
 * 省略版打印Info日志（省略domain参数）
 *
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function i(tag: string, format: string, ...args: ESObject[]): void {
  info(DEFAULT_DOMAIN, tag, format, args)
}

/**
 * 完整版打印Info日志
 *
 * @param domain 指示服务域，使用16禁止数字，取值范围为 0x0000 到 0xFFFF
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function info(domain: number, tag: string, format: string, ...args: ESObject[]): void {
  if (CordovaContext.isDebugMode() && hilog.isLoggable(domain, tag, hilog.LogLevel.INFO)) {
    hilog.info(domain, tag, format, args)
  }
}

/**
 * 省略版打印Warn日志（省略domain参数）
 *
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function w(tag: string, format: string, ...args: ESObject[]): void {
  warn(DEFAULT_DOMAIN, tag, format, args)
}

/**
 * 完整版打印Warn日志
 *
 * @param domain 指示服务域，使用16禁止数字，取值范围为 0x0000 到 0xFFFF
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function warn(domain: number, tag: string, format: string, ...args: ESObject[]): void {
  if (CordovaContext.isDebugMode() && hilog.isLoggable(domain, tag, hilog.LogLevel.WARN)) {
    hilog.warn(domain, tag, format, args)
  }
}

/**
 * 省略版打印Error日志（省略domain参数）
 *
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function e(tag: string, format: string, ...args: ESObject[]): void {
  error(DEFAULT_DOMAIN, tag, format, args)
}

/**
 * 完整版打印Error日志
 *
 * @param domain 指示服务域，使用16禁止数字，取值范围为 0x0000 到 0xFFFF
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function error(domain: number, tag: string, format: string, ...args: ESObject[]): void {
  if (CordovaContext.isDebugMode() && hilog.isLoggable(domain, tag, hilog.LogLevel.ERROR)) {
    hilog.error(domain, tag, format, args)
  }
}

/**
 * 省略版打印Fatal日志（省略domain参数）
 *
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function f(tag: string, format: string, ...args: ESObject[]): void {
  fatal(DEFAULT_DOMAIN, tag, format, args)
}

/**
 * 完整版打印Fatal日志
 *
 * @param domain 指示服务域，使用16禁止数字，取值范围为 0x0000 到 0xFFFF
 * @param tag 日志标签指示符，推荐使用全英文字符，不超过32位长
 * @param format 带有格式化字符的字符串，格式化占位符可选
 * @param args 如果有format中有格式化占位符，需要在这里提供对应的占位符变量
 */
export function fatal(domain: number, tag: string, format: string, ...args: ESObject[]): void {
  if (CordovaContext.isDebugMode() && hilog.isLoggable(domain, tag, hilog.LogLevel.FATAL)) {
    hilog.fatal(domain, tag, format, args)
  }
}